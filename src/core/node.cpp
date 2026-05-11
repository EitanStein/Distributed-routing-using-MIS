
#include "MISDistributedRouting/core/node.h"
#include "MISDistributedRouting/utils/log_macros.h"

#include <ranges>

void Node::AddEdge(Node* other)
{
    neighbors[other->GetID()] = other;
}

Node* Node::GetNeighbor(node_id_t id) const
{
    auto target = neighbors.find(id);
    if(target == neighbors.end())
    {
        LOG_ERROR("node {} does not have a neighbor {}", this->id, id);
        return nullptr;
    }

    return target->second;
}


MessagerNode* MessagerNode::GetNeighbor(node_id_t id) const
{
    return static_cast<MessagerNode*>(Node::GetNeighbor(id));
}


void MessagerNode::AddOutboxMsg(node_id_t target_id, Message msg)
{
    outbox.AddMsg(target_id, std::move(msg));
}

void MessagerNode::AddInboxMsg(node_id_t src_id, Message msg)
{
    inbox.AddMsg(src_id, std::move(msg));
}

void MessagerNode::Broadcast(Message msg)
{
    for(auto target : std::views::keys(neighbors))
    {
        AddOutboxMsg(target, msg);
    }
}


std::optional<std::pair<node_id_t, Message>> MessagerNode::ReadMsgFromInbox()
{
    return inbox.PopMsg();
}


void MessagerNode::HandleAllInboxMessages()
{
    while(std::optional<std::pair<node_id_t, Message>> optional_msg = inbox.PopMsg()) // TODO check that move works here
    {
        auto [src, msg] = std::move(optional_msg.value());

        HandleMsg(src, std::move(msg));
    }
}


void MessagerNode::SendAllOutboxMessages()
{
    while(std::optional<std::pair<node_id_t, Message>> optional_msg = outbox.PopMsg())
    {
        auto [target, msg] = std::move(*optional_msg);
        MessagerNode* target_ptr = GetNeighbor(target);
        if(target_ptr == nullptr)
        {
            LOG_ERROR("Outbox message from {} to nonexistent neighbor {}", id, target);
            continue;
        }

        target_ptr->AddInboxMsg(this->id, std::move(msg));
    } 
}


bool MessagerNode::IsOutboxEmpty()
{
    return outbox.IsEmpty();
}

using CycleFunc = void(*)(MessagerNode*);
constexpr std::array<CycleFunc, static_cast<size_t>(MessagerNodeTask::Task::NumTasks)> CycleFuncDispatch{
    [](MessagerNode* node){node->PreCycle();},
    [](MessagerNode* node){node->SendAllOutboxMessages();},
    [](MessagerNode* node){node->PostCycle();}
};


void MessagerNode::PerformTask(MessagerNodeTask::Task task){
    if(task == MessagerNodeTask::Task::NumTasks)
    {
        LOG_ERROR("Can't perform task of type 'NumTasks'");
        return;
    }

    CycleFuncDispatch[static_cast<size_t>(task)](
        this
    );

}

#include "MISDistributedRouting/core/node.h"

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
        // TODO logger didnt find neighbor
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
        thread_pool->AddTask([this, target, msg](){
            AddOutboxMsg(target, std::move(msg));
        });
    }
}


std::optional<std::pair<node_id_t, Message>> MessagerNode::ReadMsgFromInbox()
{
    return inbox.PopMsg();
}

void MessagerNode::HandleAllInboxMessages(std::function<void(node_id_t, Message)> func)
{
    while(std::optional<std::pair<node_id_t, Message>> optional_msg = inbox.PopMsg()) // TODO check that move works here
    {
        auto [src, msg] = std::move(optional_msg.value());

        func(src, std::move(msg));
    }
}


void MessagerNode::SendAllOutboxMessages()
{
    while(std::optional<std::pair<node_id_t, Message>> optional_msg = outbox.PopMsg()) // TODO check that move works here
    {
        auto [target, msg] = std::move(*optional_msg);
        MessagerNode* target_ptr = GetNeighbor(target);
        thread_pool->AddTask([this, target_ptr, message = std::move(msg)](){
            target_ptr->AddInboxMsg(this->id, std::move(message));
        });
    }
    
}

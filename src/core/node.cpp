
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


void MessagerNode::SendMsg(node_id_t target_id, Message msg) const
{
    MessagerNode* target_ptr = GetNeighbor(target_id);
    thread_pool->AddTask([this, target_ptr, message = std::move(msg)](){
        target_ptr->ReceiveMsg(this->id, std::move(message));
    });
}

void MessagerNode::ReceiveMsg(node_id_t src_id, Message msg)
{
    inbox.ReceiveMsg(src_id ,std::move(msg));
}

void MessagerNode::Broadcast(Message msg) const
{
    for(auto target : std::views::values(neighbors))
    {
        thread_pool->AddTask([this, target, msg](){
            static_cast<MessagerNode*>(target)->ReceiveMsg(this->id, std::move(msg));
        });
    }
}


std::optional<std::pair<node_id_t, Message>> MessagerNode::ReadMsgFromInbox()
{
    return inbox.ReadMsg();
}

void MessagerNode::HandleAllInboxMessages(std::function<void(node_id_t, Message)> func)
{
    auto optional_msg = std::move(ReadMsgFromInbox());

    while(optional_msg.has_value())
    {
        auto [src, msg] = std::move(optional_msg.value());

        func(src, std::move(msg));

        optional_msg = std::move(ReadMsgFromInbox());
    }
}

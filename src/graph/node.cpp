
#include "MISDistributedRouting/graph/node.h"
#include <ranges>

void Node::AddEdge(Node* other)
{
    neighbors[other->GetID()] = other;
}


void Node::SendMsg(node_id_t target_id, Message msg) const
{
    auto target = neighbors.find(target_id);
    if(target == neighbors.end())
    {
        // TODO logger
        return;
    }
    thread_pool->AddTask([this, target_ptr = target->second, message = std::move(msg)](){
        target_ptr->ReceiveMsg(this->id, std::move(message));
    });
}

void Node::ReceiveMsg(node_id_t src_id, Message msg)
{
    inbox.ReceiveMsg(src_id ,std::move(msg));
}

void Node::Broadcast(Message msg) const
{
    for(auto target : std::views::values(neighbors))
    {
        thread_pool->AddTask([this, target, msg](){
            target->ReceiveMsg(this->id, std::move(msg));
        });
    }
}


std::optional<std::pair<node_id_t, Message>> Node::ReadMsgFromInbox()
{
    return inbox.ReadMsg();
}

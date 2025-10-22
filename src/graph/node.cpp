
#include "MISDistributedRouting/graph/node.h"
#include <ranges>

void Node::AddEdge(node_id_t id, Node* neighbor)
{
    neighbors[id] = neighbor;
}

void Node::ReceiveMsg(node_id_t src_id, std::string_view msg)
{
    // TODO handle msg
}

void Node::SendMsg(node_id_t target_id, std::string_view msg) const
{
    auto target = neighbors.find(target_id);
    if(target == neighbors.end())
    {
        // TODO logger
        return;
    }
    thread_pool->AddTask([this, target_ptr = target->second, msg](){
        target_ptr->ReceiveMsg(this->id, msg);
    });
}

void Node::Broadcast(std::string_view msg) const
{
    for(auto target : std::views::values(neighbors))
    {
        thread_pool->AddTask([this, target, msg](){
            target->ReceiveMsg(this->id, msg);
        });
    }
}




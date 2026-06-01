
#include "MISDistributedRouting/core/node.h"
#include "MISDistributedRouting/utils/log_macros.h"

#include <ranges>


Node::~Node() = default;

void Node::AddEdge(Node* other)
{
    if(id_based_neighbors_map.contains(other->GetID()))
        return;

    neighbors.push_back(other);
    id_based_neighbors_map[other->GetID()] = neighbors.size()-1;
}

std::optional<node_id_t> Node::GetNeighborIdxFromId(node_id_t id) const{
    auto target = id_based_neighbors_map.find(id);
    if(target == id_based_neighbors_map.end())
    {
        LOG_ERROR("node {} does not have a neighbor {}", this->id, id);
        return std::nullopt;
    }

    return target->second;
}

Node* Node::GetNeighbor(node_id_t id) const
{
    auto opt_idx = GetNeighborIdxFromId(id);

    if(opt_idx)
        return neighbors[opt_idx.value()];
    else
        return nullptr;
}


MessagerNode::~MessagerNode() = default;


MessagerNode* MessagerNode::GetNeighbor(node_id_t id) const
{
    return static_cast<MessagerNode*>(Node::GetNeighbor(id));
}

void MessagerNode::AddEdge(Node* other)
{
    Node::AddEdge(other);
}

void MessagerNode::InitInbox(){
    inbox.ReserveInbox(neighbors.size());
}

void MessagerNode::HandleMsg(node_id_t, Message) {}

void MessagerNode::SendMsg(MessagerNode* dest, Message msg)
{
    if(dest != nullptr)
        dest->AddInboxMsg(id, std::move(msg));
}

void MessagerNode::SendMsg(node_id_t target_id, Message msg)
{
    SendMsg(GetNeighbor(target_id), std::move(msg));
}

void MessagerNode::AddInboxMsg(node_id_t src_id, Message msg)
{
    auto opt_idx = GetNeighborIdxFromId(src_id);
    if(opt_idx)
        inbox.AddMsg(opt_idx.value(), std::move(msg));
}

void MessagerNode::Broadcast(Message msg)
{
    for(auto target : neighbors)
    {
        SendMsg(static_cast<MessagerNode*>(target), msg);
    }
}


std::optional<std::pair<node_id_t, Message>> MessagerNode::ReadMsgFromInbox()
{
    if(std::optional<std::pair<node_id_t, Message>> optional_msg = inbox.PopMsg())
    {
        auto [src, msg] = std::move(optional_msg.value());
        // TODO going through hoops to return the id instead of index
        // TODO check lifetime and make sure no excess construction happens
        return std::pair<node_id_t, Message>{neighbors[src]->GetID(), std::move(msg)}; 
    }
    else
        return std::nullopt;
}


void MessagerNode::HandleAllInboxMessages()
{
    while(auto optional_msg = ReadMsgFromInbox())
    {
        std::apply([this](auto&&... args) {
            this->HandleMsg(std::forward<decltype(args)>(args)...);
        },std::move(*optional_msg));
    }
}

void MessagerNode::HandleSendingNewMessages() {}

bool MessagerNode::IsInboxEmpty() const{
    return inbox.IsEmpty();
}

void MessagerNode::RunPhase() {
    HandleAllInboxMessages();

    HandleSendingNewMessages();
}

void MessagerNode::UpdatePhase() {
    inbox.ChangePhase();
}
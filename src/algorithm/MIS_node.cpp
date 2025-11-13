#include "MISDistributedRouting/algorithm/MIS_node.h"

#include <ranges>


void MIS_Node::AddEdge(Node* other)
{
    Node::AddEdge(other);
    active_MIS_building_neighbors[other->GetID()] = static_cast<MIS_Node*>(other);
}


MIS_Node* MIS_Node::GetNeighbor(node_id_t id) const
{
    return static_cast<MIS_Node*>(Node::GetNeighbor(id));
}

void MIS_Node::MISBuildingBroadcast(Message msg)
{
    for(auto target : std::views::keys(active_MIS_building_neighbors))
    {
        thread_pool->AddTask([this, target, msg](){
            AddOutboxMsg(target, std::move(msg));
        });
    }
}

void MIS_Node::HandleMISBuildingMsg(node_id_t sender, Message msg)
{
    if (my_MIS != nullptr)
        return;
    if (std::holds_alternative<double>(msg.msg))
    {
        if (std::get<double>(msg.msg) > rand_num)
        {
            is_MIS = false;
        }
    }
    else if(std::holds_alternative<bool>(msg.msg))
    {
        if (std::get<bool>(msg.msg) == true)
        {
            my_MIS = GetNeighbor(sender);
            MISBuildingBroadcast(Message(false));
        }
        else
        {
            active_MIS_building_neighbors.erase(sender);
        }
    }
}

std::optional<Message> MIS_Node::HandleRegularMsg(Message msg)
{
    if(!msg.recipient.has_value())
    {
        // TODO log error - recipient is unllopt
        return std::nullopt;
    }

    node_id_t msg_recipient = msg.recipient.value();
    if(msg_recipient == id)
        return msg;

    if(!msg.router_to_recipient.has_value())
    {
        // TODO log error - router_to_recipient is nullopt
        return std::nullopt;
    }

    node_id_t recipient_MIS_node = msg.router_to_recipient.value();
    if(recipient_MIS_node == id)
    {
        AddOutboxMsg(msg_recipient, std::move(msg));
        return std::nullopt;
    }

    if(path_table_to_MIS_nodes.contains(recipient_MIS_node))
    {
        MIS_Node* mis_node_ptr = path_table_to_MIS_nodes.at(recipient_MIS_node);
        AddOutboxMsg(mis_node_ptr->GetID(), std::move(msg));
    }
    else
    {
        // TODO log error - target node not in path_table_to_MIS_nodes
    }
    
    return std::nullopt;
}

double MIS_Node::GetRandNumber(double min_val, double max_val)
{
    std::uniform_real_distribution<> distrib(min_val, max_val); 

    return distrib(rng);
}

void MIS_Node::MISBroadcast()
{
    if (my_MIS != nullptr)
    {
        rand_num = 0;
        return;
    }

    rand_num = GetRandNumber();    

    MISBuildingBroadcast(Message(rand_num));
}

void MIS_Node::PostMISBroadacst()
{
    if(my_MIS != nullptr)
    {
        inbox.Clear();
        return;
    }

    is_MIS = true;

    HandleAllInboxMessages([this](node_id_t sender, Message msg) {
        this->HandleMISBuildingMsg(sender, std::move(msg));
    });
}

void MIS_Node::BroadcastMISStatus()
{
    if(my_MIS != nullptr)
        return;

    if(is_MIS == true)
    {
        MISBuildingBroadcast(Message(true));
        my_MIS = this;
        active_MIS_building_neighbors.clear();
    }
}

void MIS_Node::PostMISStatusBroadacst()
{
    if(my_MIS != nullptr)
    {
        inbox.Clear();
        return;
    }

    HandleAllInboxMessages([this](node_id_t sender, Message msg) {
        this->HandleMISBuildingMsg(sender, std::move(msg));
    });
}



void MIS_Node::BuildPathTableBroadacstStart()
{
    if(is_MIS == false)
        return;

    Broadcast(Message(id));
}


void MIS_Node::BuildPathTableBroadacst()
{
    for(node_id_t MIS_id : new_entries_to_path_table)
        Broadcast(Message(MIS_id));
}

void MIS_Node::PostPathTableBroadacst()
{
    new_entries_to_path_table.clear();

    HandleAllInboxMessages([this](node_id_t sender, Message msg) {
        node_id_t MIS_id = std::get<node_id_t>(msg.msg);
        if(!this->path_table_to_MIS_nodes.contains(MIS_id))
        {
            path_table_to_MIS_nodes[MIS_id] = GetNeighbor(sender);
            new_entries_to_path_table.insert(MIS_id);
        }
    });
}

bool MIS_Node::IsNewPathTableEntriesEmpty() const
{
    return new_entries_to_path_table.empty();
}
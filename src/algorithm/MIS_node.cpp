#include "MISDistributedRouting/algorithm/MIS_node.h"

#include <ranges>


void MIS_Node::AddEdge(Node* other)
{
    neighbors[other->GetID()] = other;
    active_MIS_building_neighbors[other->GetID()] = static_cast<MIS_Node*>(other);
}

void MIS_Node::MISBuildingBroadcast(Message msg) const
{
    for(auto target : std::views::values(active_MIS_building_neighbors))
    {
        thread_pool->AddTask([this, target, msg](){
            target->ReceiveMsg(id, std::move(msg));
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
            my_MIS = dynamic_cast<MIS_Node*>(neighbors[sender]);
            MISBuildingBroadcast(Message(false));
        }
        else
        {
            active_MIS_building_neighbors.erase(sender);
        }
    }
}

// TODO fix HandleRegularMsg + ForwardMsg
// std::optional<Message> MIS_Node::HandleRegularMsg(Message msg) const
// {
//     if(!msg.recipient.has_value())
//         return std::nullopt;

//     if(id == msg.recipient.value())
//         return msg;
//     else
//         path_table_to_MIS_nodes[msg.router_to_recipient.value()]->ReceiveMsg(id, std::move(msg));
    
//     return std::nullopt;
// }

// void MIS_Node::ForwardMsg(Message msg) const
// {
//     if(!msg.recipient.has_value())
//         return; // TODO logging

//     if(id == msg.router_to_recipient.value())
//         neighbors[msg.recipient.value()]->ReceiveMsg(id, std::move(msg));
//     else
//         path_table_to_MIS_nodes[msg.router_to_recipient.value()]->ReceiveMsg(id, std::move(msg));
// }

void MIS_Node::MISBroadcast()
{
    if (my_MIS != nullptr)
    {
        rand_num = 0;
        return;
    }

    std::uniform_real_distribution<> distrib(0, 1); 

    rand_num = distrib(rng);

    MISBuildingBroadcast(Message(rand_num));
}

void MIS_Node::PostMISBroadacst()
{
    if(my_MIS != nullptr)
    {
        inbox.EmptyInbox();
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
        inbox.EmptyInbox();
        return;
    }

    HandleAllInboxMessages([this](node_id_t sender, Message msg) {
        this->HandleMISBuildingMsg(sender, std::move(msg));
    });
}



void MIS_Node::BuildPathTableBroadacstStart() const
{
    if(is_MIS == false)
        return;

    Broadcast(Message(id));
}


void MIS_Node::BuildPathTableBroadacst() const
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
            path_table_to_MIS_nodes[MIS_id] = static_cast<MIS_Node*>(neighbors[sender]);
            new_entries_to_path_table.insert(MIS_id);
        }
    });
}

bool MIS_Node::IsNewPathTableEntriesEmpty() const
{
    return new_entries_to_path_table.empty();
}
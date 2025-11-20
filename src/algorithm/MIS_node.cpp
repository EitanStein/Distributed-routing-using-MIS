#include "MISDistributedRouting/algorithm/MIS_node.h"
#include "MISDistributedRouting/utils/log_macros.h"

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

void MIS_Node::HandleMsg(node_id_t sender, Message msg)
{
    if(!msg.recipient.has_value())
    {
        LOG_ERROR("regular message recipient is nullopt");
        return;
    }

    node_id_t msg_recipient = msg.recipient.value();
    if(msg_recipient == id)
    {
        LOG_INFO("Node {} received from Node {} the msg:\n{}", id, msg.author.value(), std::get<std::string>(msg.msg));
        return;
    }

    if(!msg.router_to_recipient.has_value())
    {
        LOG_ERROR("regular message router-to-recipient is nullopt");
        return;
    }

    node_id_t recipient_MIS_node = msg.router_to_recipient.value();
    if(recipient_MIS_node == id)
    {
        AddOutboxMsg(msg_recipient, std::move(msg));
        return;
    }

    if(path_table_to_MIS_nodes.contains(recipient_MIS_node))
    {
        MIS_Node* mis_node_ptr = path_table_to_MIS_nodes.at(recipient_MIS_node);
        AddOutboxMsg(mis_node_ptr->GetID(), std::move(msg));
    }
    else
    {
        LOG_ERROR("regular message - target ({}) MIS node ({}) is not in path table", recipient_MIS_node, msg_recipient);
    }

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

    // setting to true - might change to false on post cycle when reading messages
    is_MIS = true;
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
        new_entries_to_path_table.insert(id);
    }
}

void MIS_Node::PostMISBroadacst()
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


void MIS_Node::PreCycle()
{
    if(stage == INIT)
        return;

    // regular msg
    if(stage == COMPLETE)
    {
        return;
    }

    if(stage == MIS_BUILDING)
    {
        if (my_MIS != nullptr)
        {
            rand_num = 0;
            return;
        }

        if(isRandNumMISCycle)
            MISBroadcast();
        else
            BroadcastMISStatus();
        return;
    }

    if(stage == PATH_BUILDING)
    {
        BuildPathTableBroadacst();
        return;
    }

}

void MIS_Node::PostCycle()
{
    if(stage == INIT)
        return;

    // regular msg
    if(stage == COMPLETE)
    {
        HandleAllInboxMessages();
        return;
    }

    if(stage == MIS_BUILDING)
    {
        PostMISBroadacst();

        isRandNumMISCycle = !isRandNumMISCycle;
        return;
    }

    if(stage == PATH_BUILDING)
    {
        PostPathTableBroadacst();
        return;
    }
}
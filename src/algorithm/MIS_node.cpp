#include "MISDistributedRouting/algorithm/MIS_node.h"
#include "MISDistributedRouting/utils/log_macros.h"

#include <ranges>


MIS_Node::MIS_Node(node_id_t id, ThreadPool* pool): MessagerNode(id, pool), rng(std::random_device{}()), rand_num(0), is_MIS(false), 
                                                 my_MIS(nullptr), stage(INIT), isRandNumMISCycle(false) {}
MIS_Node::~MIS_Node() = default;

void MIS_Node::AddEdge(Node* other)
{
    MessagerNode::AddEdge(other);
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
        SendMsg(target, msg);
    }
}

void MIS_Node::HandleMISBuildingMsg(node_id_t sender, Message msg)
{
    if (my_MIS != nullptr)
        return;

    std::visit([this, sender](auto&& arg){
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, double>){
            if (arg > rand_num)
            {
                is_MIS = false;
            }
        }
        else if constexpr (std::is_same_v<T, bool>){
            if (arg == true)
            {
                my_MIS = GetNeighbor(sender);
                MISBuildingBroadcast(Message(false));
            }
            else
            {
                active_MIS_building_neighbors.erase(sender);
            }
        }
    }, msg.msg);
}

void MIS_Node::HandleMsg([[maybe_unused]] node_id_t sender, Message msg)
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
        LOG_DEBUG("final message routing - from ({}) to ({})", id, msg_recipient);
        SendMsg(msg_recipient, std::move(msg));
        return;
    }

    if(path_table_to_MIS_nodes.contains(recipient_MIS_node))
    {
        MIS_Node* mis_node_ptr = path_table_to_MIS_nodes.at(recipient_MIS_node);
        LOG_DEBUG("regular message routing - from ({}) to ({})", id, mis_node_ptr->GetID());
        SendMsg(mis_node_ptr->GetID(), std::move(msg));
    }
    else
    {
        LOG_ERROR("regular message - target's ({}) MIS node ({}) is not in node {} path table", recipient_MIS_node, msg_recipient, id);
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

void MIS_Node::HandleMISBuildingMessages()
{
    if(my_MIS != nullptr)
    {
        inbox.Clear();
        return;
    }  

    while(auto optional_msg = ReadMsgFromInbox())
    {
        std::apply([this](auto&&... args) {
            this->HandleMISBuildingMsg(std::forward<decltype(args)>(args)...);
        },std::move(*optional_msg));
    }
}


void MIS_Node::BuildPathTableBroadacst()
{
    for(node_id_t MIS_id : new_entries_to_path_table)
        Broadcast(Message(MIS_id));

    new_entries_to_path_table.clear();
}

void MIS_Node::HandlePathBuildingMessages()
{
    while(std::optional<std::pair<node_id_t, Message>> optional_msg = ReadMsgFromInbox())
    {
        auto [src, msg] = std::move(optional_msg.value());
        node_id_t MIS_id = std::get<node_id_t>(msg.msg);

        if(!this->path_table_to_MIS_nodes.contains(MIS_id))
        {
            path_table_to_MIS_nodes[MIS_id] = GetNeighbor(src);
            new_entries_to_path_table.insert(MIS_id);
        }
    }
}

void MIS_Node::HandleAllInboxMessages(){
    switch(stage){
        case INIT:
            return;
        case COMPLETE:
            MessagerNode::HandleAllInboxMessages();
            return;
        case MIS_BUILDING:
            HandleMISBuildingMessages();
            return;
        case PATH_BUILDING:
            HandlePathBuildingMessages();
            return;
        default:
            LOG_ERROR("invalid MIS node stage: {}", int(stage));
    }
}

void MIS_Node::HandleSendingNewMessages(){
    switch(stage){
        case INIT:
        case COMPLETE:
            return;
        case MIS_BUILDING:
            isRandNumMISCycle = !isRandNumMISCycle;
            if(isRandNumMISCycle)
                MISBroadcast();
            else
                BroadcastMISStatus();
            return;
        case PATH_BUILDING:
            BuildPathTableBroadacst();
            return;
        default:
            LOG_ERROR("invalid MIS node stage: {}", int(stage));
    }
} 


void MIS_Node::AdvanceStage() {
    if(stage == INIT) 
        InitInbox();
    if (stage < COMPLETE) 
        stage = static_cast<MIS_Stage>(static_cast<int>(stage) + 1);
}

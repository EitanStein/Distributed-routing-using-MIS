#include "MISDistributedRouting/algorithm/MIS_node.h"
#include "MISDistributedRouting/utils/log_macros.h"

#include <ranges>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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

void MIS_Node::HandleMISBuildingMsg(node_id_t sender, MsgType::MISBuildingMessage&& msg)
{
    if (stage != MIS_BUILDING) [[unlikely]]{
        LOG_WARN("Handling a MIS building message while in stage {}", static_cast<size_t>(stage));
        return;
    }

    
    if (my_MIS != nullptr){
        inbox.Clear(); // TODO is it needed?
        return;
    }
        

    std::visit(overloaded{
        [this, sender](rand_num_t input_rand_num){
            is_MIS = is_MIS && input_rand_num <= rand_num;
        },
        [this, sender](is_MIS_t is_neighbor_MIS){
            if(is_neighbor_MIS){
                my_MIS = GetNeighbor(sender);
                MISBuildingBroadcast(MsgType::MISBuildingMessage{false});
            }
            else
                active_MIS_building_neighbors.erase(sender);
        }
    }, msg.MIS_data);
}

void MIS_Node::HandlePathBuildingMsg(node_id_t sender, MsgType::PathBuildingMessage&& msg){
    if (stage != PATH_BUILDING) [[unlikely]]{
        LOG_WARN("Handling a path building message while in stage {}", static_cast<size_t>(stage));
        return;
    }

    if(!this->path_table_to_MIS_nodes.contains(msg.MIS_node_id))
    {
        path_table_to_MIS_nodes[msg.MIS_node_id] = GetNeighbor(sender);
        new_entries_to_path_table.insert(msg.MIS_node_id);
    }
}
void MIS_Node::HandleRegularMessage([[maybe_unused]] node_id_t sender, MsgType::RoutedMessage&& msg){
    if (stage != COMPLETE) [[unlikely]]{
        LOG_WARN("Handling a regular message while in stage {}", static_cast<size_t>(stage));
        return;
    }

    if(msg.recipient == id)
    {
        LOG_INFO("Node {} received from Node {} the msg:\n{}", id, msg.author, msg.data);
        return;
    }

    if(msg.router_to_recipient == id)
    {
        LOG_DEBUG("final message routing - from ({}) to ({})", id, msg.recipient);
        SendMsg(msg.recipient, std::move(msg));
        return;
    }

    if(path_table_to_MIS_nodes.contains(msg.router_to_recipient))
    {
        MIS_Node* mis_node_ptr = path_table_to_MIS_nodes.at(msg.router_to_recipient);
        LOG_DEBUG("regular message routing - from ({}) to ({})", id, mis_node_ptr->GetID());
        SendMsg(mis_node_ptr->GetID(), std::move(msg));
    }
    else
    {
        LOG_ERROR("regular message - target's ({}) MIS node ({}) is not in node {} path table", msg.recipient, msg.router_to_recipient, id);
    }

}



void MIS_Node::HandleMsg(node_id_t sender, Message&& msg)
{
    std::visit(overloaded{
        [this, sender](MsgType::MISBuildingMessage&& msg) {HandleMISBuildingMsg(sender, std::move(msg));},
        [this, sender](MsgType::PathBuildingMessage&& msg) {HandlePathBuildingMsg(sender, std::move(msg));},
        [this, sender](MsgType::RoutedMessage&& msg) {HandleRegularMessage(sender, std::move(msg));}
    }, std::move(msg.data));
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

    MISBuildingBroadcast(MsgType::MISBuildingMessage{rand_num});

    // setting to true - might change to false on post cycle when reading messages
    is_MIS = true;
}

void MIS_Node::BroadcastMISStatus()
{
    if(my_MIS != nullptr)
        return;

    if(is_MIS == true)
    {
        MISBuildingBroadcast(MsgType::MISBuildingMessage{true});
        my_MIS = this;
        active_MIS_building_neighbors.clear();
        new_entries_to_path_table.insert(id);
    }
}

void MIS_Node::BuildPathTableBroadacst()
{
    for(node_id_t MIS_id : new_entries_to_path_table)
        Broadcast(MsgType::PathBuildingMessage{MIS_id});

    new_entries_to_path_table.clear();
}

void MIS_Node::HandleSendingNewMessages(){
    switch(stage){
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
        case INIT:
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

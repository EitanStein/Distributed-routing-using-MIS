#include "MISDistributedRouting/core/message_box.h"


void Inbox::ReserveInbox(node_id_t num_neighbors){
    per_neighbor_inbox.reserve(num_neighbors);
    for(node_id_t i = 0 ; i < num_neighbors ; ++i){
        per_neighbor_inbox.emplace_back();
        per_neighbor_inbox[i].reserve(2);
    }
}

void Inbox::AddNeighborInbox(){
    per_neighbor_inbox.emplace_back();
    per_neighbor_inbox.back().reserve(2);
}

void Inbox::AddMsg(node_id_t node_idx, Message msg){
    per_neighbor_inbox[node_idx].emplace_back(std::move(msg));
}

void Inbox::ResetIndexes(){
    cur_inbox_idx = 0;
    inbox_msg_idx = 0;
}


void Inbox::Clear(){
    for(auto& neighbor_inbox : per_neighbor_inbox){
        neighbor_inbox.clear();
    }
}


std::optional<std::pair<node_id_t, Message>> Inbox::PopMsg(){
    while(cur_inbox_idx < per_neighbor_inbox.size()){
        if(per_neighbor_inbox[cur_inbox_idx].empty()){
            ++cur_inbox_idx;
            continue;
        }

        std::vector<Message>& cur_neighbor_inbox = per_neighbor_inbox[cur_inbox_idx];
        if(inbox_msg_idx < cur_neighbor_inbox.size()){
            ++inbox_msg_idx;
            return std::pair<node_id_t, Message>{cur_inbox_idx, std::move(cur_neighbor_inbox[inbox_msg_idx - 1])};
        }
        
        inbox_msg_idx = 0;
        cur_neighbor_inbox.clear();
        ++cur_inbox_idx;
    }

    return std::nullopt;
}

bool Inbox::IsEmpty() const{
    for(const auto& neighbor_inbox : per_neighbor_inbox){
        if(!neighbor_inbox.empty())
            return false;
    }

    return true;
}
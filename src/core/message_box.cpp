#include "MISDistributedRouting/core/message_box.h"

using per_neighbor_buffer = std::vector<std::vector<Message>>;

void Inbox::ReserveInbox(node_id_t num_neighbors){
    for(auto& per_neighbor_inbox_buffer : buffers){
        per_neighbor_inbox_buffer.reserve(num_neighbors);
        for(node_id_t i = 0 ; i < num_neighbors ; ++i){
            per_neighbor_inbox_buffer.emplace_back();
            per_neighbor_inbox_buffer[i].reserve(2);
        }
    }
    
}

void Inbox::AddNeighborInbox(){
    buffers[0].emplace_back();
    buffers[0].back().reserve(2);
    buffers[1].emplace_back();
    buffers[1].back().reserve(2);
}

void Inbox::AddMsg(node_id_t node_idx, Message msg){
    buffers[write_buffer_index][node_idx].emplace_back(std::move(msg));
}

void Inbox::ChangePhase(){
    std::swap(read_buffer_index, write_buffer_index);
    cur_inbox_idx = 0;
    inbox_msg_idx = 0;
}


void Inbox::Clear(){
    for(auto& per_neighbor_inbox : buffers){
        for(auto& neighbor_inbox : per_neighbor_inbox)
            neighbor_inbox.clear();
    }
}


std::optional<std::pair<node_id_t, Message>> Inbox::PopMsg(){
    per_neighbor_buffer& per_neighbor_read_buffer = buffers[read_buffer_index];
    while(cur_inbox_idx < per_neighbor_read_buffer.size()){
        if(per_neighbor_read_buffer[cur_inbox_idx].empty()){
            ++cur_inbox_idx;
            continue;
        }

        std::vector<Message>& cur_neighbor_inbox = per_neighbor_read_buffer[cur_inbox_idx];
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
    for(const auto& neighbor_inbox : buffers[read_buffer_index]){
        if(!neighbor_inbox.empty())
            return false;
    }

    return true;
}
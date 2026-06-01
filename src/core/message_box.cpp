#include "MISDistributedRouting/core/message_box.h"
#include <utility>

using per_neighbor_buffer = std::vector<NeighborInbox>;

NeighborInbox::NeighborInbox() : size(0), front_msg_index(0) {
    inbox.reserve(2);
    inbox.emplace_back(0);
    inbox.emplace_back(0);
}

void NeighborInbox::AddMsg(Message&& msg){
    if(Empty())
        Clear();

    if(size != inbox.size())
        inbox[size] = std::move(msg);
    else
        inbox.emplace_back(std::move(msg));

    ++size;

}
Message NeighborInbox::PopMsg(){
    ++front_msg_index;  
    [[assume(front_msg_index-1 < inbox.size())]];
    return inbox[front_msg_index-1];
}
bool NeighborInbox::Empty() const {
    return front_msg_index == size;
}
void NeighborInbox::Clear(){
    size = 0;
    front_msg_index = 0;
}


void Inbox::ReserveInbox(node_id_t num_neighbors){
    for(auto& per_neighbor_inbox_buffer : buffers){
        per_neighbor_inbox_buffer.reserve(num_neighbors);
        for(node_id_t i = 0 ; i < num_neighbors ; ++i){
            per_neighbor_inbox_buffer.emplace_back();
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
    buffers[write_buffer_index][node_idx].AddMsg(std::move(msg));
}

void Inbox::ChangePhase(){
    std::swap(read_buffer_index, write_buffer_index);
    cur_inbox_idx = 0;
}

// TODO consider clearing only one buffer or have a clear function per buffer
void Inbox::Clear(){
    for(auto& per_neighbor_inbox : buffers){
        for(auto& neighbor_inbox : per_neighbor_inbox)
            neighbor_inbox.Clear();
    }
}


std::optional<std::pair<node_id_t, Message>> Inbox::PopMsg(){
    per_neighbor_buffer& per_neighbor_read_buffer = buffers[read_buffer_index];

    while(cur_inbox_idx < per_neighbor_read_buffer.size()){
        NeighborInbox& cur_neighbor_inbox = per_neighbor_read_buffer[cur_inbox_idx];

        if(cur_neighbor_inbox.Empty()){
            ++cur_inbox_idx;
            continue;
        }

        return std::optional<std::pair<node_id_t, Message>>{std::in_place, 
                                                            cur_inbox_idx, 
                                                            std::move(cur_neighbor_inbox.PopMsg())};
    }

    return std::nullopt;
}

bool Inbox::IsEmpty() const{
    for(const auto& neighbor_inbox : buffers[read_buffer_index]){
        if(!neighbor_inbox.Empty())
            return false;
    }

    return true;
}
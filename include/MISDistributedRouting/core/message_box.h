#pragma once

#include <optional>
#include <array>
#include <vector>

#include "message.h"

// TODO consider a bitset per buffer that marks where messages were accepted - potentially faster to check if empty
struct Inbox{
    std::array<std::vector<std::vector<Message>>, 2> buffers{};

    int read_buffer_index{0};
    int write_buffer_index{1};

    node_id_t cur_inbox_idx{};
    node_id_t inbox_msg_idx{};

    void ReserveInbox(node_id_t num_neighbors);
    void AddNeighborInbox();
    void AddMsg(node_id_t node_idx, Message msg);
    void ChangePhase();
    void Clear();
    std::optional<std::pair<node_id_t, Message>> PopMsg();
    bool IsEmpty() const;
};

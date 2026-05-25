#pragma once

#include <thread>
#include <queue>
#include <optional>
#include <mutex>

#include "message.h"


struct Inbox{
    std::vector<std::vector<Message>> per_neighbor_inbox{};
    node_id_t cur_inbox_idx{};
    node_id_t inbox_msg_idx{};

    void ReserveInbox(node_id_t num_neighbors);
    void AddNeighborInbox();
    void AddMsg(node_id_t node_idx, Message msg);
    void ResetIndexes();
    void Clear();
    std::optional<std::pair<node_id_t, Message>> PopMsg();
    bool IsEmpty() const;
};

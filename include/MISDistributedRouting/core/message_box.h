#pragma once

#include <thread>
#include <queue>
#include <optional>
#include <mutex>

#include "message.h"


class MessageBox
{
private:
    std::mutex queue_mutex;
    std::queue<std::pair<node_id_t, Message>> queue;
public:
    void AddMsg(node_id_t node_id, Message msg);
    std::optional<std::pair<node_id_t, Message>> PopMsg();
    void Clear();
    bool IsEmpty();
    ~MessageBox() = default;
};

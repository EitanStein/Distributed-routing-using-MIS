#pragma once

#include <thread>
#include <queue>
#include <optional>
#include <mutex>

#include "message.h"

class Inbox
{
private:
    std::mutex queue_mutex;
    std::queue<std::pair<node_id_t, Message>> queue;
public:
    void ReceiveMsg(node_id_t src, Message msg);
    std::optional<std::pair<node_id_t, Message>> ReadMsg();
    void EmptyInbox();
    bool IsEmpty();
    ~Inbox() = default;
};
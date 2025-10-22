#pragma once

#include <unordered_map>
#include <optional>

#include "MISDistributedRouting/utils/types.h"
#include "MISDistributedRouting/utils/thread_pool.h"
#include "MISDistributedRouting/messaging/inbox.h"


class Node
{
private:
    node_id_t id;
    std::unordered_map<node_id_t, Node*> neighbors;
    ThreadPool* thread_pool;
    Inbox inbox;

public:
    Node(node_id_t id, ThreadPool* pool) : id(id), thread_pool(pool) {}
    void AddEdge(node_id_t id, Node* neighbor);

    void SendMsg(node_id_t dest_id, Message msg) const;
    void ReceiveMsg(node_id_t from_id, Message msg);
    std::optional<std::pair<node_id_t, Message>> ReadMsgFromInbox();
    void Broadcast(Message msg) const;

    ~Node() = default;

};
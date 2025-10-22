#pragma once

#include <unordered_map>
#include <optional>

#include "MISDistributedRouting/utils/thread_pool.h"

typedef size_t node_id_t;

// TODO add Node Inbox class for handling receieved msgs
class Node
{
private:
    node_id_t id;
    std::unordered_map<node_id_t, Node*> neighbors;

    ThreadPool* thread_pool;

public:
    Node(node_id_t id, ThreadPool* pool) : id(id), thread_pool(pool) {}
    void AddEdge(node_id_t id, Node* neighbor);

    // TODO more complex msgs including far target and original sender
    void ReceiveMsg(node_id_t from_id, std::string_view msg);
    void SendMsg(node_id_t dest_id, std::string_view msg) const;
    void Broadcast(std::string_view msg) const;

    ~Node() = default;

};
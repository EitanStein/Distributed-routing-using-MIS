#pragma once

#include <unordered_map>
#include <optional>
#include <functional>

#include "types.h"
#include "MISDistributedRouting/utils/thread_pool.h"
#include "inbox.h"


class Node
{
protected:
    node_id_t id;
    std::unordered_map<node_id_t, Node*> neighbors;
    ThreadPool* thread_pool;
    Inbox inbox;

public:
    Node(node_id_t id, ThreadPool* pool) : id(id), thread_pool(pool) {}
    virtual void AddEdge(Node* neighbor);

    void SendMsg(node_id_t dest_id, Message msg) const;
    void ReceiveMsg(node_id_t from_id, Message msg);
    std::optional<std::pair<node_id_t, Message>> ReadMsgFromInbox();
    void HandleAllInboxMessages(std::function<void(node_id_t, Message)>);
    void Broadcast(Message msg) const;

    node_id_t GetID() const { return id; }

    virtual ~Node() = default;

};
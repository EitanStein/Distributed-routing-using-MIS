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
public:
    Node(node_id_t id) : id(id) {}
    virtual ~Node() = default;

    virtual void AddEdge(Node* neighbor);
    virtual Node* GetNeighbor(node_id_t id) const;

    node_id_t GetID() const { return id; }
};


class MessagerNode : public Node
{
protected:
    ThreadPool* thread_pool;
    Inbox inbox;
public:
    MessagerNode(node_id_t id, ThreadPool* pool) : Node(id), thread_pool(pool) {}
    ~MessagerNode() = default;

    MessagerNode* GetNeighbor(node_id_t id) const override;

    void SendMsg(node_id_t dest_id, Message msg) const;
    void ReceiveMsg(node_id_t from_id, Message msg);
    std::optional<std::pair<node_id_t, Message>> ReadMsgFromInbox();
    void HandleAllInboxMessages(std::function<void(node_id_t, Message)>);
    void Broadcast(Message msg) const;
};
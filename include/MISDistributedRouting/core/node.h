#pragma once

#include <unordered_map>
#include <optional>
#include <functional>

#include "types.h"
#include "MISDistributedRouting/utils/thread_pool.h"
#include "message_box.h"


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
    MessageBox inbox;
    MessageBox outbox;
public:
    MessagerNode(node_id_t id, ThreadPool* pool) : Node(id), thread_pool(pool) {}
    ~MessagerNode() = default;

    MessagerNode* GetNeighbor(node_id_t id) const override;

    virtual void HandleMsg(node_id_t sender, Message msg) {}
    
    void AddInboxMsg(node_id_t from_id, Message msg);
    std::optional<std::pair<node_id_t, Message>> ReadMsgFromInbox();
    void HandleAllInboxMessages();
    void HandleAllInboxMessages(std::function<void(node_id_t, Message)>);

    void AddOutboxMsg(node_id_t dest_id, Message msg);
    void Broadcast(Message msg);

    bool IsOutboxEmpty();

    virtual void PreCycle() {}
    virtual void SendAllOutboxMessages();
    virtual void PostCycle() { HandleAllInboxMessages(); }
};
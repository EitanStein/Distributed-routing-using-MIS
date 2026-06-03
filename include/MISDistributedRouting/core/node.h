#pragma once

#include <unordered_map>
#include <optional>
#include <functional>

#include "types.h"
#include "thread_pool.h"
#include "message_box.h"


class Node
{
protected:
    node_id_t id;
    std::vector<Node*> neighbors;
    std::unordered_map<node_id_t, node_id_t> id_based_neighbors_map;

    std::optional<node_id_t> GetNeighborIdxFromId(node_id_t id) const;
public:
    Node(node_id_t id) : id(id) {}
    virtual ~Node();

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
    ~MessagerNode();

    void AddEdge(Node* neighbor) override;
    MessagerNode* GetNeighbor(node_id_t id) const override;

    virtual void HandleMsg(node_id_t, Message);
    
    void AddInboxMsg(node_id_t from_id, Message msg);
    std::optional<std::pair<node_id_t, Message>> ReadMsgFromInbox();
    bool IsInboxEmpty() const;

    virtual void HandleAllInboxMessages();
    virtual void HandleSendingNewMessages();

    void SendMsg(node_id_t dest_id, Message msg);
    virtual void SendMsg(MessagerNode* dest, Message msg);
    void Broadcast(Message msg);

    // TODO change RunPhase, UpdatePhase, InitInbox somehow
    // either different names or different way to use or both
    void RunPhase();
    void UpdatePhase();

    void InitInbox();
};


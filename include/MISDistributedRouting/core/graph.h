#pragma once

#include <vector>
#include <memory>

#include "node.h"

const size_t DEFAULT_POOL_SIZE = std::thread::hardware_concurrency();

class Graph
{
protected:
    std::vector<std::unique_ptr<Node>> nodes;
public:
    Graph() = default;
    virtual ~Graph() = default;

    virtual void InitGraphNodes(size_t graph_size);

    size_t GetGraphSize() const { return nodes.size(); }

    virtual void AddNode();
    virtual void AddEdge(node_id_t node1, node_id_t node2);
};



class SyncedGraph : public Graph
{
protected:
    ThreadPool thread_pool;

    void RunTaskOnAllNodes(std::function<void(node_id_t)> task, bool wait=true);
public:
    SyncedGraph(size_t thread_pool_size=DEFAULT_POOL_SIZE) : thread_pool(thread_pool_size) {};
    ~SyncedGraph() = default;

    void AddNode() override;

    void WaitForInactiveThreadPool();
};
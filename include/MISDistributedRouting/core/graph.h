#pragma once

#include <vector>
#include <memory>

#include "node.h"

const size_t DEFAULT_POOL_SIZE = std::thread::hardware_concurrency();

class Graph
{
protected:
    size_t graph_size;
    std::vector<std::unique_ptr<Node>> nodes;
    ThreadPool thread_pool;

    void RunTaskOnAllNodes(std::function<void(node_id_t)> task, bool wait=true);
public:
    Graph(size_t thread_pool_size=DEFAULT_POOL_SIZE) : thread_pool(thread_pool_size), graph_size(0) {};

    void InitGraphNodes(size_t graph_size);
    void InitRandEdges(size_t num_edges);

    virtual void AddNode();
    virtual void AddEdge(node_id_t node1, node_id_t node2);

    virtual ~Graph() = default;

};
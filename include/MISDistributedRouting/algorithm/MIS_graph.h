#pragma once

#include "MIS_node.h"
#include "MISDistributedRouting/core/graph.h"


class MIS_Graph : public Graph
{
private:
    bool IsPathTableDone() const;
public:
    MIS_Graph(size_t thread_pool_size=DEFAULT_POOL_SIZE) : Graph(thread_pool_size) {};
    MIS_Graph(size_t graph_size, size_t num_edges, size_t thread_pool_size=DEFAULT_POOL_SIZE);
    void AddNode() override;
    void AddEdge(node_id_t node1, node_id_t node2) override;

    void BuildMIS();
    void BuildPathTable();
};
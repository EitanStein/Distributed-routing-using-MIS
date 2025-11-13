#pragma once

#include "MIS_node.h"
#include "MISDistributedRouting/core/graph.h"


class MIS_Graph : public SyncedGraph
{
protected:
    bool IsPathTableDone() const;

    void BuildMIS();
    void BuildPathTable();
public:
    MIS_Graph(size_t thread_pool_size=DEFAULT_POOL_SIZE) : SyncedGraph(thread_pool_size) {};

    void AddNode() override;

    void InitMIS();

    MIS_Node* GetNode(node_id_t) const override;
};
#pragma once

#include "MIS_node.h"
#include "MISDistributedRouting/core/graph.h"


class MIS_Graph : public SyncedGraph
{
protected:
    MIS_Node::MIS_Stage stage;
public:
    MIS_Graph(size_t thread_pool_size=DEFAULT_POOL_SIZE) : SyncedGraph(thread_pool_size), stage(MIS_Node::INIT) {};

    void InitGraph(size_t graph_size) override;
    
    void AddNode() override;

    MIS_Node* GetNode(node_id_t) const override;
    bool RunCycle() override;
    void AdvanceStatus();

    MIS_Node::MIS_Stage GetGraphStage() const {return stage;}
};
#include "MISDistributedRouting/algorithm/MIS_graph.h"

#include <ranges>

void MIS_Graph::InitGraph(size_t graph_size)
{
    stage = MIS_Node::INIT;
    SyncedGraph::InitGraph(graph_size);
}

void MIS_Graph::AddNode()
{
    nodes.emplace_back(std::make_unique<MIS_Node>(nodes.size(), &thread_pool));
}

MIS_Node* MIS_Graph::GetNode(node_id_t node_id) const
{
    return static_cast<MIS_Node*>(Graph::GetNode(node_id));
}

bool MIS_Graph::RunCycle()
{
    if(stage == MIS_Node::INIT)
        return false;

    bool ret = SyncedGraph::RunCycle();

    // MIS building has 2 steps of broadcasting and retreiving msgs
    if(stage == MIS_Node::MIS_BUILDING)
        ret = SyncedGraph::RunCycle();


    if(ret)
        AdvanceStatus();

    return ret; 
}


void MIS_Graph::AdvanceStatus()
{
    if(stage == MIS_Node::COMPLETE)
        return;

    stage = static_cast<MIS_Node::MIS_Stage>(static_cast<int>(stage) + 1);
    for(node_id_t id=0; id < nodes.size() ; ++id)
        GetNode(id)->AdvanceStage();
}
#include "MISDistributedRouting/simulation/simulation_graph.h"

void SimulationGraph::AddNode()
{
    nodes.emplace_back(std::make_unique<SimulationNode>(nodes.size(), &thread_pool, 0, graph_width));
    
    ConnectNewNode();
}


void SimulationGraph::ConnectNewNode()
{
    size_t graph_size = GetGraphSize();

    auto new_node_ptr = static_cast<SimulationNode*>(nodes[graph_size-1].get());
    for(size_t i = 0 ; i < graph_size - 1 ; ++i)
    {
        auto old_node_ptr = static_cast<SimulationNode*>(nodes[i].get());
        if(connection_checker.AreConnected(*old_node_ptr, *new_node_ptr))
            AddEdge(old_node_ptr->GetID(), new_node_ptr->GetID());
    }
}


SimulationNode* SimulationGraph::GetNode(node_id_t id) const
{
    return static_cast<SimulationNode*>(Graph::GetNode(id));
}


void SimulationGraph::AddEdge(node_id_t node1, node_id_t node2)
{
    MIS_Graph::AddEdge(node1, node2);
    edges.emplace_back(std::make_unique<SimulationEdge>(node1, node2));
}
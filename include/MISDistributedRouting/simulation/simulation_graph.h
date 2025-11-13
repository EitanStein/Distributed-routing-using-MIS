#pragma once

#include "MISDistributedRouting/algorithm/MIS_graph.h"
#include "simulation_node.h"
#include "simulation_edge.h"
#include "unit_connectivity.h"


constexpr double DEFAULT_GRAPH_LENGTH = 10.0;


class SimulationGraph : public MIS_Graph
{
protected:
    double graph_width;
    UnitConnectivity connection_checker;
    std::vector<std::unique_ptr<SimulationEdge>> edges; // TODO change edges - cannot get specific pointer easily atm

    void ConnectNewNode();
public:
    SimulationGraph(size_t thread_pool_size=DEFAULT_POOL_SIZE, double graph_width=DEFAULT_GRAPH_LENGTH) : MIS_Graph(thread_pool_size), graph_width(graph_width) {};
    ~SimulationGraph() = default;

    void AddNode() override;
    void AddNode(double x, double y);
    void AddEdge(node_id_t node1, node_id_t node2) override;

    SimulationNode* GetNode(node_id_t) const override;
};
#pragma once

#include "MISDistributedRouting/algorithm/MIS_graph.h"
#include "simulation_node.h"
#include "simulation_edge.h"
#include "unit_connectivity.h"


constexpr double DEFAULT_GRAPH_WIDTH = 10.0;
constexpr double DEFAULT_GRAPH_HEIGHT = 10.0;


class SimulationGraph : public MIS_Graph
{
protected:
    double graph_width;
    double graph_height;
    UnitConnectivity connection_checker;
    std::vector<std::unique_ptr<SimulationEdge>> edges; // TODO change edges - cannot get specific pointer easily atm

    void ConnectNewNode();
public:
    SimulationGraph(double graph_width=DEFAULT_GRAPH_WIDTH, double graph_height=DEFAULT_GRAPH_HEIGHT, size_t thread_pool_size=DEFAULT_POOL_SIZE) : MIS_Graph(thread_pool_size), graph_width(graph_width), graph_height(graph_height) {};
    ~SimulationGraph() = default;

    void AddNode() override;
    void AddNode(double x, double y);
    void AddEdge(node_id_t node1, node_id_t node2) override;

    SimulationNode* GetNode(node_id_t) const override;
};
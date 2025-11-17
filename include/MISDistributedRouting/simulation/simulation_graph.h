#pragma once

#include "MISDistributedRouting/algorithm/MIS_graph.h"
#include "simulation_node.h"
#include "simulation_utils.h"
#include "MISDistributedRouting/config.h"





class SimulationGraph : public MIS_Graph
{
protected:
    double graph_width;
    double graph_height;
    double unit_radius;
    std::unordered_map<node_id_t, std::unordered_map<node_id_t, sf::VertexArray>> edges; // TODO change edges - cannot get specific pointer easily atm

    bool AreNodesConnected(SimulationNode*, SimulationNode*) const;
    void ConnectNewNode();
public:
    SimulationGraph(double graph_width=DEFAULT_GRAPH_WIDTH, double graph_height=DEFAULT_GRAPH_HEIGHT, double unit_dist=DEFAULT_UNIT_DIST, size_t thread_pool_size=DEFAULT_POOL_SIZE) : 
                    MIS_Graph(thread_pool_size), graph_width(graph_width - NODE_RADIUS*2), graph_height(graph_height - NODE_RADIUS*2), unit_radius(unit_dist) {};
    ~SimulationGraph() = default;

    void AddNode() override;
    virtual void AddNode(float x, float y);
    void AddEdge(node_id_t node1, node_id_t node2) override;

    void InitGraphNodes(node_id_t num_nodes) override;

    SimulationNode* GetNode(node_id_t) const override;
    SimulationNode* GetNode(const sf::Vector2f& loc) const;

    void Draw(sf::RenderWindow& window);
};
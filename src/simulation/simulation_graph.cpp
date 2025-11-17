#include "MISDistributedRouting/simulation/simulation_graph.h"
#include <cmath>
#include <ranges>
#include <random>

void SimulationGraph::AddNode()
{
    nodes.emplace_back(std::make_unique<SimulationNode>(nodes.size(), &thread_pool));
    
    ConnectNewNode();
}

void SimulationGraph::AddNode(float x, float y)
{
    nodes.emplace_back(std::make_unique<SimulationNode>(nodes.size(), &thread_pool, sf::Vector2f({x, y})));
    
    ConnectNewNode();
}

bool SimulationGraph::AreNodesConnected(SimulationNode* node1, SimulationNode* node2) const
{
    if(node1 == nullptr || node2 == nullptr)
        return false;

    const sf::Vector2f node1_pos = node1->GetPosition();
    const sf::Vector2f node2_pos = node2->GetPosition();

    if(std::sqrt(std::pow(node1_pos.x - node2_pos.x, 2) + std::pow(node1_pos.y - node2_pos.y, 2)) <= unit_radius)
        return true;

    return false;
}

void SimulationGraph::ConnectNewNode()
{
    size_t graph_size = GetGraphSize();

    auto new_node_ptr = static_cast<SimulationNode*>(nodes[graph_size-1].get());
    for(size_t i = 0 ; i < graph_size - 1 ; ++i)
    {
        auto old_node_ptr = static_cast<SimulationNode*>(nodes[i].get());
        if(AreNodesConnected(old_node_ptr, new_node_ptr))
            AddEdge(old_node_ptr->GetID(), new_node_ptr->GetID());
    }
}


SimulationNode* SimulationGraph::GetNode(node_id_t id) const
{
    return static_cast<SimulationNode*>(Graph::GetNode(id));
}

SimulationNode* SimulationGraph::GetNode(const sf::Vector2f& loc) const
{
    for (node_id_t node_id=0 ; node_id < nodes.size() ; ++node_id)
    {
        SimulationNode* node_ptr = GetNode(node_id);
        if(node_ptr->Contains(loc))
            return node_ptr;
    }
    return nullptr;
}


void SimulationGraph::AddEdge(node_id_t node1, node_id_t node2)
{
    MIS_Graph::AddEdge(node1, node2);
    if(node1 < node2)
    {
        sf::VertexArray edge(sf::PrimitiveType::Lines, 2);
        edge[0].position = GetNode(node1)->GetPosition();
        edge[1].position = GetNode(node2)->GetPosition();

        edge[0].color = COLORS::INACTIVE_EDGE;
        edge[1].color = COLORS::INACTIVE_EDGE;
        
        edges[node1][node2] = std::move(edge);
    }
    else
    {
        sf::VertexArray edge(sf::PrimitiveType::Lines, 2);
        edge[0].position = GetNode(node2)->GetPosition();
        edge[1].position = GetNode(node1)->GetPosition();
        
        edge[0].color = COLORS::INACTIVE_EDGE;
        edge[1].color = COLORS::INACTIVE_EDGE;

        edges[node2][node1] = std::move(edge);
    }
    
}


void SimulationGraph::InitGraphNodes(node_id_t num_nodes)
{
    nodes.clear();
    edges.clear();

    nodes.reserve(num_nodes);

    std::mt19937 rng(std::random_device{}());

    std::uniform_real_distribution<> distrib_x(0, graph_width);
    std::uniform_real_distribution<> distrib_y(0, graph_height); 


    for(auto i : std::views::iota(size_t{0}, num_nodes))
        AddNode(float(distrib_x(rng)), float(distrib_y(rng)));

}


void SimulationGraph::Draw(sf::RenderWindow& window)
{
    for (node_id_t node_id=0 ; node_id < nodes.size() ; ++node_id)
    {
        GetNode(node_id)->Draw(window);
    }

    for(auto& node_neighbors : std::views::values(edges))
    {
        for(auto& edge : std::views::values(node_neighbors))
        {
            window.draw(edge);
            edge[0].color = COLORS::INACTIVE_EDGE;
            edge[1].color = COLORS::INACTIVE_EDGE;
        }
    }
}
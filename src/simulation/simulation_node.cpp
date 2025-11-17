#include "MISDistributedRouting/simulation/simulation_node.h"
#include "MISDistributedRouting/simulation/simulation_utils.h"
#include "MISDistributedRouting/config.h"



SimulationNode::SimulationNode(node_id_t id, ThreadPool* pool, const sf::Vector2f& point) : MIS_Node(id, pool), is_sending_msg()
{
    SetPosition(point);
}

void SimulationNode::SetPosition(const sf::Vector2f& point)
{
    shape.setPosition(point);
    shape.setRadius(NODE_RADIUS);
}

sf::Vector2f SimulationNode::GetPosition() const
{
    return shape.getPosition() + shape.getGeometricCenter();
}


bool SimulationNode::Contains(const sf::Vector2f& point)
{
    return shape.getGlobalBounds().contains(point);
}

void SimulationNode::Draw(sf::RenderWindow& window)
{
    if(!IsOutboxEmpty())
        shape.setFillColor(COLORS::SENDING_MSG_NODE);
    else if(is_MIS)
        shape.setFillColor(COLORS::MIS_NODE);
    else
        shape.setFillColor(COLORS::REGULAR_NODE);
    
    window.draw(shape);
}
#include "MISDistributedRouting/simulation/simulation_node.h"
#include "MISDistributedRouting/simulation/simulation_utils.h"
#include "MISDistributedRouting/config.h"
#include "MISDistributedRouting/utils/log_macros.h"



SimulationNode::SimulationNode(node_id_t id, ThreadPool* pool, const sf::Vector2f& point) : MIS_Node(id, pool), is_sending_msg(false)
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


void SimulationNode::SendAllOutboxMessages()
{
    is_sending_msg = false;
    while(std::optional<std::pair<node_id_t, Message>> optional_msg = outbox.PopMsg())
    {
        auto [target, msg] = std::move(*optional_msg);
        MessagerNode* target_ptr = GetNeighbor(target);
        if(target_ptr == nullptr)
        {
            LOG_ERROR("Outbox message from {} to nonexistent neighbor {}", id, target);
            continue;
        }

        is_sending_msg = true;
        thread_pool->AddTask([this, target_ptr, message = std::move(msg)](){
            target_ptr->AddInboxMsg(this->id, std::move(message));
        });
    } 
}


void SimulationNode::Draw(sf::RenderWindow& window)
{
    if(is_sending_msg)
        shape.setFillColor(COLORS::SENDING_MSG_NODE);
    else if(is_MIS)
        shape.setFillColor(COLORS::MIS_NODE);
    else
        shape.setFillColor(COLORS::REGULAR_NODE);
    
    window.draw(shape);
}
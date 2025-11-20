#pragma once

#include "MISDistributedRouting/algorithm/MIS_node.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class SimulationNode : public MIS_Node
{
private:
    sf::CircleShape shape;
    bool is_sending_msg;
public:
    SimulationNode(node_id_t id, ThreadPool* pool) : MIS_Node(id, pool), is_sending_msg(false) {}
    SimulationNode(node_id_t id, ThreadPool* pool, const sf::Vector2f& point);
    ~SimulationNode() = default;

    void SetPosition(const sf::Vector2f& point);
    sf::Vector2f GetPosition() const;
    bool Contains(const sf::Vector2f& point);

    void Draw(sf::RenderWindow& window);

    void SendAllOutboxMessages() override;
};
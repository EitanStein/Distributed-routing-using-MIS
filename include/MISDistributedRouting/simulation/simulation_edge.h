#pragma once

#include "simulation_node.h"

class SimulationEdge
{
private:
    node_id_t node1;
    node_id_t node2;
    bool active;
public:
    SimulationEdge(node_id_t id1, node_id_t id2) : node1(id2 > id1 ? id2 : id1), node2(id2 > id1 ? id1 : id2), active(false) {}
    std::pair<node_id_t, node_id_t> GetNodes() const { return std::pair<node_id_t, node_id_t>(node1, node2); }
    bool IsActive() const { return active; }
    void ReverseActiveState() { active = !active; }
};
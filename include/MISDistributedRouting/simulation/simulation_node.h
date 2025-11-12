#pragma once

#include "MISDistributedRouting/algorithm/MIS_node.h"
#include "eucledian_trait.h"

class SimulationNode : public MIS_Node, public EucledianTrait
{
public:
    SimulationNode(node_id_t id, ThreadPool* pool, double min_val, double max_val) : MIS_Node(id, pool), EucledianTrait(GetRandNumber(min_val, max_val), GetRandNumber(min_val, max_val)) {}
    SimulationNode(node_id_t id, ThreadPool* pool, std::pair<double, double> point) : MIS_Node(id, pool), EucledianTrait(point.first, point.second) {}
    ~SimulationNode() = default;

};
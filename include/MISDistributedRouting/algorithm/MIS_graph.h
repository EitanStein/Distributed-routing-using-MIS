#pragma once

#include "MIS_node.h"
#include "MISDistributedRouting/core/graph.h"


class MIS_Graph : public Graph
{
protected:
    bool IsPathTableDone() const;

    void BuildMIS();
    void BuildPathTable();
public:
    MIS_Graph(size_t thread_pool_size=DEFAULT_POOL_SIZE) : Graph(thread_pool_size) {};

    void AddNode() override;

    void InitMIS();
};
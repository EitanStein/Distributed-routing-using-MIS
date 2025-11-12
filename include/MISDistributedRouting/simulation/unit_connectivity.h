#pragma once

#include "eucledian_trait.h"

constexpr double UNIT_DIST = 1.0;

class UnitConnectivity
{
public:
    bool AreConnected(const EucledianTrait& a, const EucledianTrait& b) { return a.distance(b) <= UNIT_DIST;}
};
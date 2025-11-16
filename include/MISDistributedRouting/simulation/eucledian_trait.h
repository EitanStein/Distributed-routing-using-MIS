#pragma once

#include <cmath>

class EucledianTrait
{
private:
    double x;
    double y;
public:
    EucledianTrait(double x, double y): x(x), y(y) {}
    ~EucledianTrait() = default;

    std::pair<double, double> GetCooard() const {return {x, y};}

    virtual double distance(const EucledianTrait& other) const {return std::sqrt(std::pow(other.x - x, 2) + std::pow(other.y - y, 2));}
};
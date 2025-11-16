#pragma once

#include "simulation_graph.h"
#include "simulation_window.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>



class Simulator
{
private:
    SimulationWindow window;
    double window_graph_ratio;

    SimulationGraph graph;
public:
    Simulator(double ratio=10) : window("Simulator"), window_graph_ratio(ratio), 
                                graph(window.GetGraphWindowSize().first/ratio, window.GetGraphWindowSize().second/ratio) {}
    ~Simulator() = default;

    void Run();
};
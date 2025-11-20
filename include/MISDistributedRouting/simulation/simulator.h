#pragma once

#include "simulation_graph.h"
#include "simulation_window.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "MISDistributedRouting/config.h"



class Simulator
{
private:
    SimulationWindow window;

    SimulationGraph graph;
    GraphStatus graph_status;

    SimulationNode* sender;
    SimulationNode* recipient;

    void HandleMouseClick(const sf::Event::MouseButtonReleased* key_pressed);
    void HandleKeyboardInput(const sf::Event::KeyPressed* key_pressed);
    bool IsMouseClickOnPanel(const sf::Vector2i& mouse_pos) const;

    void CheckMIS_CalcStatus();
    void UpdateStatus(GraphStatus status);

    void ResetMsgNodes();
public:
    Simulator() : window("Simulator"), graph(window.GetGraphWindowSize().first, window.GetGraphWindowSize().second), 
                    graph_status(EMPTY), sender(nullptr), recipient(nullptr) {}
    ~Simulator() = default;

    void Run();
    
};
#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>

constexpr double DEFAULT_GRAPH_WIDTH = 10.0;
constexpr double DEFAULT_GRAPH_HEIGHT = 10.0;

constexpr uint32_t DEFAULT_WINDOW_WIDTH = 800;
constexpr uint32_t DEFAULT_CONTROL_PANEL_WIDTH = 250;
constexpr uint32_t DEFAULT_WINDOW_HEIGHT = 600;
const sf::Font DEFAULT_FONT("assets/fonts/Arimo/Arimo-Regular.ttf");

namespace COLORS
{
    const sf::Color BACKGROUND(40, 40, 40);
    const sf::Color TEXT_FILL(80, 80, 80);
    const sf::Color BUTTON_FILL(50, 50, 50);

    const sf::Color INACTIVE_EDGE(sf::Color::Blue);
    const sf::Color ACTIVE_EDGE(sf::Color::Red);

    const sf::Color REGULAR_NODE(sf::Color::Blue);
    const sf::Color MIS_NODE(sf::Color::Red);
    const sf::Color SENDING_MSG_NODE(sf::Color::Green);
}

enum GraphStatus{EMPTY, INITIALIZED, CALCULATING_MIS, CALCULATED_MIS}; // TODO remove dupliacte with MIS_Node status

const std::unordered_map<GraphStatus, std::string> INSTRUCTIONS = {
    {EMPTY, "Enter graph size\nand create graph"},
    {INITIALIZED, "Build MIS"},
    {CALCULATING_MIS, "Calculating MIS\nPlease wait"},
    {CALCULATED_MIS, "Choose sender (1st)\nand recipient (2nd)\ntype message and send"} 
};

#include "MISDistributedRouting/simulation/simulation_window.h"
#include "MISDistributedRouting/config.h"


SimulationWindow::SimulationWindow(std::string_view title, uint32_t width, uint32_t height) : window(sf::VideoMode({width, height}), title.data()), control_panel(height, width-DEFAULT_CONTROL_PANEL_WIDTH)
{
    window.setFramerateLimit(FRAME_RATE);
}

void SimulationWindow::ClearWindow()
{
    window.clear(sf::Color::Black);
}

void SimulationWindow::DrawPanel()
{
    control_panel.Draw(window);
}

void SimulationWindow::DisplayWindow()
{
    window.display();
}
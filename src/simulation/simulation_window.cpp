#include "MISDistributedRouting/simulation/simulation_window.h"



SimulationWindow::SimulationWindow(std::string_view title, uint32_t width, uint32_t height) : window(sf::VideoMode({width, height}), title.data()), control_panel(height)
{

}

void SimulationWindow::RunWindow()
{
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);
        control_panel.Draw(window);

        window.display();
    }
}
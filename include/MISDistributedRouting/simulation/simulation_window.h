#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

constexpr uint32_t DEFAULT_WINDOW_WIDTH = 800;
constexpr uint32_t DEFAULT_CONTROL_PANEL_WIDTH = 250;
constexpr uint32_t DEFAULT_WINDOW_HEIGHT = 600;
const sf::Font DEFAULT_FONT("assets/fonts/Arimo/Arimo-Regular.ttf");

const sf::Color BACKGROUND_COLOR(40, 40, 40);
const sf::Color TEXT_FILL_COLOR(80, 80, 80);
const sf::Color BUTTON_FILL_COLOR(50, 50, 50);



struct Button {
    sf::RectangleShape shape;
    sf::Text text;

    bool enabled = true;

    Button() : text(DEFAULT_FONT)  {}

    void Init(float pos_x, float pos_y, const sf::String& title)
    {
        shape.setSize({200, 40});
        shape.setPosition({pos_x, pos_y});
        shape.setFillColor(BUTTON_FILL_COLOR);
        text.setString(title);
        text.setCharacterSize(18);
        text.setPosition({pos_x+10, pos_y+8});
    }

    bool isHovered(const sf::Vector2f& m) {
        return enabled && shape.getGlobalBounds().contains(m);
    }

    void Draw(sf::RenderWindow& window)
    {
        if(!enabled)
            return;

        window.draw(shape);
        window.draw(text);
    }
};

struct TextBox {
    sf::RectangleShape box;
    sf::Text text;

    bool active = false;
    bool enabled = true;

    TextBox() : text(DEFAULT_FONT) {}

    void Init(float pos_x, float pos_y)
    {
        box.setSize({200, 40});
        box.setPosition({pos_x, pos_y});
        box.setFillColor(TEXT_FILL_COLOR);
        text.setCharacterSize(18);
        text.setPosition({pos_x+5, pos_y+8});
    }

    bool isHovered(const sf::Vector2f& m) {
        return enabled && box.getGlobalBounds().contains(m);
    }

    void Draw(sf::RenderWindow& window)
    {
        if(!enabled)
            return;

        window.draw(box);
        window.draw(text);
    }
};

struct ControlPanel{
    sf::RectangleShape shape;

    TextBox num_nodes_description_textbox;
    TextBox num_nodes_interactive_textbox;
    Button create_graph_button;

    Button build_MIS_button;

    Button send_message_button;
    TextBox message_textbox;

    TextBox intructions_textbox;

    ControlPanel(size_t height) : shape(sf::Vector2f(float(DEFAULT_CONTROL_PANEL_WIDTH), height)){
        shape.setFillColor(BACKGROUND_COLOR);

        num_nodes_description_textbox.Init(25, 10);
        num_nodes_description_textbox.text.setString("Graph Size:");
        num_nodes_interactive_textbox.Init(25, 60);
        num_nodes_interactive_textbox.text.setString("100");

        create_graph_button.Init(25, 110, "Create Graph");

        build_MIS_button.Init(25, 200, "Build MIS");
        build_MIS_button.enabled = false;

        send_message_button.Init(25, 300, "Send Message");
        send_message_button.enabled = false;

        message_textbox.Init(25, 350);
        message_textbox.enabled = false;

        intructions_textbox.Init(25, 450);
        intructions_textbox.text.setString("100");
        intructions_textbox.box.setFillColor(BACKGROUND_COLOR);
    }

    void Draw(sf::RenderWindow& window)
    {
        window.draw(shape);

        num_nodes_description_textbox.Draw(window);
        num_nodes_interactive_textbox.Draw(window);
        create_graph_button.Draw(window);

        build_MIS_button.Draw(window);

        send_message_button.Draw(window);
        message_textbox.Draw(window);

        intructions_textbox.Draw(window);
    }

};

struct WindowNode { sf::CircleShape shape; };
struct WindowEdge { sf::Vertex line[2]; };

class SimulationWindow
{
private:
    sf::RenderWindow window;

    ControlPanel control_panel;
    
    sf::Font font;
public:
    SimulationWindow(std::string_view title, uint32_t width=DEFAULT_WINDOW_WIDTH, uint32_t height=DEFAULT_WINDOW_HEIGHT);
    ~SimulationWindow() = default;

    std::pair<size_t, size_t> GetGraphWindowSize() const {return {window.getSize().x - DEFAULT_CONTROL_PANEL_WIDTH, window.getSize().y};}

    void RunWindow();
};


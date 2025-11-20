#pragma once

#include <vector>
#include <string>
#include "simulation_utils.h"
#include "MISDistributedRouting/core/types.h"



struct Button {
    sf::RectangleShape shape;
    sf::Text text;

    bool enabled = true;

    Button() : text(DEFAULT_FONT)  {}

    void Init(float pos_x, float pos_y, const sf::String& title)
    {
        shape.setSize({200, 40});
        shape.setPosition({pos_x, pos_y});
        shape.setFillColor(COLORS::BUTTON_FILL);
        text.setString(title);
        text.setCharacterSize(18);
        text.setPosition({pos_x+10, pos_y+8});
    }

    bool IsHovered(const sf::Vector2f& m) {
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

    bool numbers_only = false;
    bool active = false;
    bool enabled = true;

    TextBox() : text(DEFAULT_FONT) {}

    void Init(float pos_x, float pos_y)
    {
        box.setSize({200, 40});
        box.setPosition({pos_x, pos_y});
        box.setFillColor(COLORS::TEXT_FILL);
        text.setCharacterSize(18);
        text.setPosition({pos_x+5, pos_y+8});
    }

    bool IsHovered(const sf::Vector2f& m) {
        return enabled && box.getGlobalBounds().contains(m);
    }

    void UpdateText(const sf::Keyboard::Key& key)
    {
        if(!active)
            return;
        
        std::string txt = text.getString();
        if(key == sf::Keyboard::Key::Backspace)
        {
            if(txt.empty())
                return;

            txt.pop_back();
        }
        // TODO better conversion from keyboard::key to char/string
        // TODO handle shifts
        else if(key <= sf::Keyboard::Key::Num9 && key >= sf::Keyboard::Key::Num0)
        {
            txt.push_back(static_cast<int>(key) - static_cast<int>(sf::Keyboard::Key::Num0) +'0');
        }
        else if(key <= sf::Keyboard::Key::Z && key >= sf::Keyboard::Key::A && !numbers_only)
        {
            txt.push_back(static_cast<int>(key) - static_cast<int>(sf::Keyboard::Key::A) +'a');
        }
        
        text.setString(txt);
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

    ControlPanel(size_t height, float x_shift=0) : shape(sf::Vector2f(float(DEFAULT_CONTROL_PANEL_WIDTH), float(height))){
        shape.setFillColor(COLORS::BACKGROUND);
        shape.setPosition(sf::Vector2f({x_shift, 0}));

        num_nodes_description_textbox.Init(25 + x_shift, 10);
        num_nodes_description_textbox.text.setString("Graph Size:");
        num_nodes_interactive_textbox.Init(25 + x_shift, 60);
        num_nodes_interactive_textbox.text.setString("100");
        num_nodes_interactive_textbox.numbers_only = true;

        create_graph_button.Init(25 + x_shift, 110, "Create Graph");

        build_MIS_button.Init(25 + x_shift, 200, "Build MIS");
        build_MIS_button.enabled = false;

        send_message_button.Init(25 + x_shift, 300, "Send Message");
        send_message_button.enabled = false;

        message_textbox.Init(25 + x_shift, 350);
        message_textbox.enabled = false;

        intructions_textbox.Init(25 + x_shift, 450);
        intructions_textbox.text.setString("100");
        intructions_textbox.box.setFillColor(COLORS::BACKGROUND);
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


struct SimulationWindow
{

    sf::RenderWindow window;

    ControlPanel control_panel;

    SimulationWindow(std::string_view title, uint32_t width=DEFAULT_WINDOW_WIDTH, uint32_t height=DEFAULT_WINDOW_HEIGHT);
    ~SimulationWindow() = default;

    std::pair<size_t, size_t> GetGraphWindowSize() const {return {window.getSize().x - DEFAULT_CONTROL_PANEL_WIDTH, window.getSize().y};}

    const std::optional<sf::Event> GetWindowPollEvent() { return window.pollEvent(); }
    bool IsWindowOpen() { return window.isOpen(); }
    void Close()  {window.close(); }

    void ClearWindow();
    void DrawPanel();
    void DisplayWindow();


    bool IsHoveringCreateGraphButton(sf::Vector2f pos) {return control_panel.create_graph_button.IsHovered(pos);}
    bool IsHoveringNumNodesText(sf::Vector2f pos) {return control_panel.num_nodes_interactive_textbox.IsHovered(pos);}
    bool IsHoveringBuildMISButton(sf::Vector2f pos) {return control_panel.build_MIS_button.IsHovered(pos);}

    bool IsHoveringSendMessageButton(sf::Vector2f pos) {return control_panel.send_message_button.IsHovered(pos);}
    bool IsHoveringMessageText(sf::Vector2f pos) {return control_panel.message_textbox.IsHovered(pos);}

    void Disable_MIS_Options() {control_panel.build_MIS_button.enabled = false;}
    void DisableSendMessageOptions() {control_panel.send_message_button.enabled = false; control_panel.message_textbox.enabled = false;}

    void Enable_MIS_Options() {control_panel.build_MIS_button.enabled = true;}
    void EnableSendMessageOptions() {control_panel.send_message_button.enabled = true; control_panel.message_textbox.enabled = true;}

    void ActivateMessageText() {control_panel.message_textbox.active = true;}
    void DeactivateMessageText() {control_panel.message_textbox.active = false;}

    void ActivateNumNodesText() {control_panel.num_nodes_interactive_textbox.active = true;}
    void DeactivateNumNodesText() {control_panel.num_nodes_interactive_textbox.active = false;}

    void UpdateMessageText(const sf::Keyboard::Key& key) { control_panel.message_textbox.UpdateText(key); }
    void UpdateNumNodesText(const sf::Keyboard::Key& key) { control_panel.num_nodes_interactive_textbox.UpdateText(key); }


    void UpdateInstructions(GraphStatus stage) {control_panel.intructions_textbox.text.setString(INSTRUCTIONS.at(stage));}

    node_id_t GetNumNodes() const {
        std::string num_nodes_text = control_panel.num_nodes_interactive_textbox.text.getString();
        node_id_t ret_val = 0;
        try
        {
            ret_val = std::stoi(num_nodes_text);
        }
        catch(const std::invalid_argument& e)
        {
            ret_val = 0;
        }
        catch(const std::out_of_range& e)
        {
            ret_val = 0;
        }

        return ret_val;
    }

    std::string GetMessage() const {return control_panel.message_textbox.text.getString();}
};


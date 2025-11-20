#include "MISDistributedRouting/simulation/simulator.h"


void Simulator::Run()
{
    UpdateStatus(EMPTY);

    while(window.IsWindowOpen())
    {
        while(const std::optional event = window.GetWindowPollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.Close();
            }
            else if(const auto* key_pressed = event->getIf<sf::Event::MouseButtonReleased>())
            {
                HandleMouseClick(key_pressed);
            }
            else if(const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
            {
                HandleKeyboardInput(key_pressed);
            }
        }

        CheckMIS_CalcStatus();

        window.ClearWindow();
        window.DrawPanel();
        if(graph_status != EMPTY)
            graph.Draw(window.window);

        window.DisplayWindow();
    }
}

void Simulator::UpdateStatus(GraphStatus status)
{
    graph_status = status;
    window.UpdateInstructions(status);
}


void Simulator::CheckMIS_CalcStatus()
{
    if(graph.RunCycle() && graph_status == CALCULATING_MIS)
    {
        UpdateStatus(CALCULATING_PATH_TABLE);
    }

    if(graph.RunCycle() && graph_status == CALCULATING_PATH_TABLE)
    {
        UpdateStatus(DONE_CALCULATING);
        window.EnableSendMessageOptions();
    }
}

void Simulator::ResetMsgNodes()
{
    sender = nullptr;
    recipient = nullptr;
}

bool Simulator::IsMouseClickOnPanel(const sf::Vector2i& mouse_pos) const
{
    return mouse_pos.x > window.GetGraphWindowSize().first;
}

void Simulator::HandleMouseClick(const sf::Event::MouseButtonReleased* key_pressed)
{
    if(key_pressed->button != sf::Mouse::Button::Left)
        return;

    window.DeactivateMessageText();
    window.DeactivateNumNodesText();

    auto mouse_pos = key_pressed->position; //sf::Mouse::getPosition(window.window);
    if(IsMouseClickOnPanel(mouse_pos))
    {
        if(window.IsHoveringCreateGraphButton(sf::Vector2f(mouse_pos)))
        {
            node_id_t num_nodes = window.GetNumNodes();
            if(num_nodes == 0)
                return;

            UpdateStatus(EMPTY);
            window.DisableSendMessageOptions();
            window.Disable_MIS_Options();

            graph.InitGraph(num_nodes);

            window.Enable_MIS_Options();
            UpdateStatus(INITIALIZED);
        }
        else if(window.IsHoveringNumNodesText(sf::Vector2f(mouse_pos)))
        {
            window.ActivateNumNodesText();
        }
        else if(window.IsHoveringBuildMISButton(sf::Vector2f(mouse_pos)))
        {
            graph.AdvanceStatus();

            window.Disable_MIS_Options();
            
            UpdateStatus(CALCULATING_MIS);
        }
        else if(window.IsHoveringSendMessageButton(sf::Vector2f(mouse_pos)))
        {
            if(sender == nullptr || recipient == nullptr)
                return;

            std::string msg = window.GetMessage();

            sender->HandleMsg(sender->GetID(), Message(sender->GetID(), recipient->GetID(), recipient->GetMyMISID(), msg));
        }
        else if(window.IsHoveringMessageText(sf::Vector2f(mouse_pos)))
        {
            window.ActivateMessageText();
            return;
        }

        ResetMsgNodes();
    }
    else
    {
        if(graph_status != DONE_CALCULATING)
            return;

        SimulationNode* node_ptr = graph.GetNode(sf::Vector2f(mouse_pos));
        if(node_ptr == nullptr)
        {
            ResetMsgNodes();
            return;
        }

        if(sender == nullptr)
            sender = node_ptr;
        else
            recipient = node_ptr;
    }
}


void Simulator::HandleKeyboardInput(const sf::Event::KeyPressed* key_pressed)
{
    window.UpdateMessageText(key_pressed->code);
    window.UpdateNumNodesText(key_pressed->code);
}
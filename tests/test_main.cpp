#define CATCH_CONFIG_MAIN

#include "MISDistributedRouting/simulation/simulation_graph.h"
#include "MISDistributedRouting/utils/log_macros.h"

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <ranges>


class TestNode : public SimulationNode
{
public:
    TestNode(node_id_t id, ThreadPool* pool) : SimulationNode(id, pool) {}
    TestNode(node_id_t id, ThreadPool* pool, const sf::Vector2f& point) : SimulationNode(id, pool, point) {}
    TestNode* GetNeighbor(node_id_t node_id) {return static_cast<TestNode*>(Node::GetNeighbor(node_id));}
    node_id_t GetMyMisID() const {return my_MIS->GetID();}

    bool IsMISNode() {return is_MIS;}

    bool IsMISConsistent()
    {
        if(is_MIS)
        {
            for(auto neigbor_id : std::views::keys(neighbors))
            {
                if (GetNeighbor(neigbor_id)->IsMISNode())
                {
                    LOG_DEBUG("MIS node {} has MIS node neighbor {}", id, neigbor_id);
                    return false;
                }
            }
            return true;
        }
        else
        {
            for(auto neigbor_id : std::views::keys(neighbors))
            {
                if (GetNeighbor(neigbor_id)->IsMISNode())
                    return true;
            }
            LOG_DEBUG("not MIS node {} has no MIS node neighbors", id);
            return false;
        }
    }

    bool IsSelfNeighbor()
    {
        if(neighbors.contains(id))
            return true;

        return false;
    }
};


class TestGraph : public SimulationGraph
{
public:
    TestGraph(double graph_width=DEFAULT_GRAPH_WIDTH, double graph_height=DEFAULT_GRAPH_HEIGHT, size_t thread_pool_size=DEFAULT_POOL_SIZE) : SimulationGraph(graph_width, graph_height, thread_pool_size) {};

    void AddNode() override { 
        nodes.emplace_back(std::make_unique<TestNode>(nodes.size(), &thread_pool));
    
        ConnectNewNode();
    }

    void AddNode(float x, float y) override
    {
        nodes.emplace_back(std::make_unique<TestNode>(nodes.size(), &thread_pool, sf::Vector2f({x, y})));
    
        ConnectNewNode();
    }

    TestNode* GetNode(node_id_t id) const override {
        return static_cast<TestNode*>(Graph::GetNode(id));
    }

    void SendMessage(node_id_t sender, node_id_t receiver, std::string msg)
    {
        auto receiver_router_id = GetNode(receiver)->GetMyMisID();
        GetNode(sender)->HandleMsg(sender, Message(sender, receiver, receiver_router_id, msg));
    }

    std::optional<std::pair<node_id_t, Message>> GetMessageFromNode(node_id_t node_id)
    {
        return GetNode(node_id)->ReadMsgFromInbox();
    }

    void ReadMsgFromInboxOnAll()
    {
        RunTaskOnAllNodes([this](node_id_t id){
            std::optional<std::pair<node_id_t, Message>> msg_from_inbox = GetNode(id)->ReadMsgFromInbox();
            if(msg_from_inbox == std::nullopt)
                return;

            GetNode(id)->HandleMsg(msg_from_inbox.value().first, std::move(msg_from_inbox.value().second));
        });

        WaitForInactiveThreadPool();
    }

    bool IsMISConsistent()
    {
        for(node_id_t id = 0 ; id < GetGraphSize() ; ++id)
        {
            if (!GetNode(id)->IsMISConsistent())
                return false;
        }
        return true;
    }

    bool AreThereSelfNeighbors()
    {
        for(node_id_t id = 0 ; id < GetGraphSize() ; ++id)
        {
            if (GetNode(id)->IsSelfNeighbor())
                return true;
        }
        return false;
    }

    void InitMIS()
    {
        AdvanceStatus();
        while(stage!= MIS_Node::COMPLETE)
        {
            RunCycle();
        }
    }
};

TEST_CASE("Graph creation no self neighbors check", "")
{
    INIT_LOGGER();

    TestGraph graph;
    graph.InitGraph(100);

    graph.InitMIS();

    REQUIRE(!graph.AreThereSelfNeighbors());
}


TEST_CASE("MIS Creation check", "")
{
    TestGraph graph;
    graph.InitGraph(100);

    graph.InitMIS();

    REQUIRE(graph.IsMISConsistent());
}



TEST_CASE("Check sending message", "")
{
    TestGraph graph;
    
    graph.AddNode(1, 1);
    graph.AddNode(1, 2);
    graph.AddNode(2, 1);
    graph.AddNode(2, 2);
    graph.AddNode(3, 2);
    graph.AddNode(2, 3);
    graph.AddNode(4, 3);
    graph.AddNode(3, 3);
    graph.AddNode(5, 3);
    graph.AddNode(0, 1);
    graph.AddNode(5, 4);
    graph.AddNode(4, 4);

    graph.InitMIS();
    
    std::string msg = "hello";
    graph.SendMessage(2, 5, msg);
    std::optional<std::pair<node_id_t, Message>> final_msg = graph.GetMessageFromNode(5);
    while(final_msg == std::nullopt)
    {  
        graph.ReadMsgFromInboxOnAll();
        graph.TransferPendingMessages();
        final_msg = graph.GetMessageFromNode(5);
    }

    REQUIRE(std::get<std::string>(final_msg.value().second.msg)==msg);
}


TEST_CASE("Check sending message on random graph", "")
{
    const double GRAPH_WIDTH = 10;
    TestGraph graph(std::thread::hardware_concurrency(), GRAPH_WIDTH);
    graph.InitGraph(300);

    graph.InitMIS();
    
    std::string msg = "hello";
    graph.SendMessage(2, 5, msg);
    std::optional<std::pair<node_id_t, Message>> final_msg = graph.GetMessageFromNode(5);
    while(final_msg == std::nullopt)
    {  
        graph.ReadMsgFromInboxOnAll();
        if(!graph.AreMessagesPending())
            break;
        graph.TransferPendingMessages();
        final_msg = graph.GetMessageFromNode(5);
    }

    if(final_msg.has_value())
        REQUIRE(std::get<std::string>(final_msg.value().second.msg)==msg);
}



TEST_CASE("Check sending message to self", "")
{
    TestGraph graph;
    
    graph.AddNode(1, 1);

    graph.InitMIS();
    
    std::string msg = "hello";
    graph.SendMessage(0, 0, msg);
    std::optional<std::pair<node_id_t, Message>> final_msg = graph.GetMessageFromNode(0);
    while(final_msg == std::nullopt)
    {  
        graph.ReadMsgFromInboxOnAll();
        if(!graph.AreMessagesPending())
            break;
        graph.TransferPendingMessages();
        final_msg = graph.GetMessageFromNode(0);
    }

    if(final_msg.has_value())
        REQUIRE(std::get<std::string>(final_msg.value().second.msg)==msg);
}
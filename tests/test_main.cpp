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
    ~TestNode() = default;
    TestNode* GetNeighbor(node_id_t node_id) const override {return static_cast<TestNode*>(Node::GetNeighbor(node_id));}
    node_id_t GetMyMisID() const {return my_MIS->GetID();}

    bool IsMISNode() {return is_MIS;}

    bool IsMISConsistent()
    {
        if(is_MIS)
        {
            for(auto neighbor_ptr : neighbors)
            {
                if (static_cast<TestNode*>(neighbor_ptr)->IsMISNode())
                {
                    LOG_DEBUG("MIS node {} has MIS node neighbor {}", id, neighbor_ptr->GetID());
                    return false;
                }
            }
            return true;
        }
        else
        {
            for(auto neighbor_ptr : neighbors)
            {
                if (static_cast<TestNode*>(neighbor_ptr)->IsMISNode())
                    return true;
            }
            LOG_DEBUG("not MIS node {} has no MIS node neighbors", id);
            return false;
        }
    }

    bool IsMISNodeInPathTable(node_id_t id) const{
        return path_table_to_MIS_nodes.contains(id);
    }

    bool IsSelfNeighbor()
    {
        if(id_based_neighbors_map.contains(id))
            return true;

        return false;
    }

    void updateInboxPhase(){
        inbox.ChangePhase();
    }

    bool IsConnectedToTarget(TestNode* target){
        auto receiver_router_id = target->GetMyMisID();
        return path_table_to_MIS_nodes.contains(receiver_router_id);
    }
};


class TestGraph : public SimulationGraph
{
public:
    TestGraph(double graph_width=DEFAULT_GRAPH_WIDTH, double graph_height=DEFAULT_GRAPH_HEIGHT, double unit_dist=DEFAULT_UNIT_DIST, size_t thread_pool_size=DEFAULT_POOL_SIZE) : SimulationGraph(graph_width, graph_height, unit_dist, thread_pool_size) {};
    ~TestGraph() = default;

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

    // assumes only a single message in the graph
    void HandleOneMessage(){
        size_t graph_size = GetGraphSize();
        for(node_id_t id=0; id < graph_size ; ++id)
        {
            if(GetNode(id)->IsInboxEmpty())
                continue;
            std::optional<std::pair<node_id_t, Message>> msg_from_inbox = GetNode(id)->ReadMsgFromInbox();

            GetNode(id)->HandleMsg(msg_from_inbox.value().first, std::move(msg_from_inbox.value().second));
            GetNode(id)->ReadMsgFromInbox();
            break;
        }

        RunAllNodesPostCycle();
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

    // assumes connected graph
    bool IsPathTableConsistent(){
        std::vector<node_id_t> mis_nodes;
        for(node_id_t id = 0 ; id < GetGraphSize() ; ++id)
        {
            if (GetNode(id)->IsMISNode())
                mis_nodes.push_back(id);
        }

        for(node_id_t id = 0 ; id < GetGraphSize() ; ++id)
        {
            for(auto mis_id : mis_nodes){
                if(id == mis_id)
                    continue;
                if(!GetNode(id)->IsMISNodeInPathTable(mis_id))
                    return false;
            }
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

    REQUIRE(!graph.AreThereSelfNeighbors());
}


TEST_CASE("Larger Graph MIS test", "")
{
    INIT_LOGGER();

    TestGraph graph;
    graph.InitGraph(500);
    graph.InitMIS();
    REQUIRE(graph.IsMISConsistent());
    REQUIRE(graph.IsPathTableConsistent());
}


TEST_CASE("MIS Creation check1", "")
{
    INIT_LOGGER();
    TestGraph graph;

    graph.AddNode(1, 1);
    graph.AddNode(1, 2);
    graph.AddEdge(0, 1);
    
    graph.InitMIS();

    REQUIRE(graph.IsMISConsistent());
    REQUIRE(graph.IsPathTableConsistent());
}


TEST_CASE("MIS Creation check2", "")
{
    INIT_LOGGER();
    TestGraph graph;

    graph.AddNode(0, 0);
    graph.AddNode(DEFAULT_UNIT_DIST-1, 0);
    graph.AddNode(DEFAULT_UNIT_DIST*2-1, 0);
    graph.AddNode(DEFAULT_UNIT_DIST*3-1, 0);
    graph.AddNode(DEFAULT_UNIT_DIST*4-1, 0);

    graph.InitMIS();

    REQUIRE(graph.IsMISConsistent());
    REQUIRE(graph.IsPathTableConsistent());
}


TEST_CASE("MIS Creation check3", "")
{
    INIT_LOGGER();
    TestGraph graph;
    graph.InitGraph(100);

    graph.InitMIS();

    REQUIRE(graph.IsMISConsistent());
}


TEST_CASE("Check sending message1", "")
{
    INIT_LOGGER();
    TestGraph graph;
    
    graph.AddNode(0, 0);
    graph.AddNode(DEFAULT_UNIT_DIST-1, 0);
    graph.AddNode(DEFAULT_UNIT_DIST*2-1, 0);
    graph.AddNode(DEFAULT_UNIT_DIST*3-1, 0);
    graph.AddNode(DEFAULT_UNIT_DIST*4-1, 0);

    graph.InitMIS();

    REQUIRE(graph.IsPathTableConsistent());

    graph.RunCycle();
    
    std::string msg = "hello";
    graph.SendMessage(0, 4, msg);
    std::optional<std::pair<node_id_t, Message>> final_msg = graph.GetMessageFromNode(4);
    while(final_msg == std::nullopt)
    {  
        graph.HandleOneMessage();
        if(!graph.AreMessagesPending())
            break;
        final_msg = graph.GetMessageFromNode(4);
    }

    REQUIRE(std::get<std::string>(final_msg.value().second.msg)==msg);
}



TEST_CASE("Check sending message2", "")
{
    INIT_LOGGER();
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
    graph.RunCycle();
    
    std::string msg = "hello";
    graph.SendMessage(2, 5, msg);
    std::optional<std::pair<node_id_t, Message>> final_msg = graph.GetMessageFromNode(5);
    while(final_msg == std::nullopt)
    {  
        graph.HandleOneMessage();
        if(!graph.AreMessagesPending())
            break;
        final_msg = graph.GetMessageFromNode(5);
    }

    REQUIRE(std::get<std::string>(final_msg.value().second.msg)==msg);
}


TEST_CASE("Check sending message on random graph", "")
{
    INIT_LOGGER();
    const double GRAPH_WIDTH = 10;
    TestGraph graph(std::thread::hardware_concurrency(), GRAPH_WIDTH);
    graph.InitGraph(300);

    graph.InitMIS();
    graph.RunCycle();
    
    std::string msg = "hello";
    graph.SendMessage(2, 5, msg);
    std::optional<std::pair<node_id_t, Message>> final_msg = graph.GetMessageFromNode(5);
    while(final_msg == std::nullopt)
    {  
        graph.HandleOneMessage();
        if(!graph.AreMessagesPending())
            break;
        final_msg = graph.GetMessageFromNode(5);
    }

    if(final_msg.has_value())
        REQUIRE(std::get<std::string>(final_msg.value().second.msg)==msg);
}



TEST_CASE("Check sending message to self", "")
{
    INIT_LOGGER();
    TestGraph graph;
    
    graph.AddNode(1, 1);

    graph.InitMIS();
    
    std::string msg = "hello";
    graph.SendMessage(0, 0, msg);
    std::optional<std::pair<node_id_t, Message>> final_msg = graph.GetMessageFromNode(0);
    while(final_msg == std::nullopt)
    {  
        graph.HandleOneMessage();
        if(!graph.AreMessagesPending())
            break;
        final_msg = graph.GetMessageFromNode(0);
    }

    if(final_msg.has_value())
        REQUIRE(std::get<std::string>(final_msg.value().second.msg)==msg);
}
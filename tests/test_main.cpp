#define CATCH_CONFIG_MAIN

#include "MISDistributedRouting/simulation/simulation_graph.h"
#include "MISDistributedRouting/utils/log_macros.h"

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>


class TestNode : public SimulationNode
{
public:
    TestNode(node_id_t id, ThreadPool* pool, double min_val, double max_val) : SimulationNode(id, pool, min_val, max_val) {}
    node_id_t GetMyMisID() const {return my_MIS->GetID();}
};


class TestGraph : public SimulationGraph
{
public:
    void AddNode() override {
        nodes.emplace_back(std::make_unique<TestNode>(nodes.size(), &thread_pool, 0, graph_width));
    
        size_t graph_size = GetGraphSize();

        auto new_node_ptr = static_cast<TestNode*>(nodes[graph_size-1].get());
        for(size_t i = 0 ; i < graph_size - 1 ; ++i)
        {
            auto old_node_ptr = static_cast<TestNode*>(nodes[i].get());
            if(connection_checker.AreConnected(*old_node_ptr, *new_node_ptr))
                AddEdge(old_node_ptr->GetID(), new_node_ptr->GetID());
        }
    }

    void SendMessage(node_id_t sender, node_id_t receiver, std::string msg)
    {
        auto receiver_router = static_cast<TestNode*>(nodes[receiver].get())->GetMyMisID();
        static_cast<TestNode*>(nodes[sender].get())->HandleRegularMsg(Message(sender, receiver, receiver_router, msg));
    }

    std::optional<std::pair<node_id_t, Message>> GetMessage(node_id_t node_id)
    {
        return static_cast<TestNode*>(nodes[node_id].get())->ReadMsgFromInbox();
    }

    void ReadMsgFromInboxOnAll()
    {
        RunTaskOnAllNodes([this](node_id_t id){
            std::optional<std::pair<node_id_t, Message>> msg_from_inbox = static_cast<MIS_Node*>(nodes[id].get())->ReadMsgFromInbox();
            if(msg_from_inbox == std::nullopt)
                return;

            static_cast<MIS_Node*>(nodes[id].get())->HandleRegularMsg(msg_from_inbox.value().second);
        });
    }
};

TEST_CASE("MIS Creation sanity check", "")
{
    INIT_LOGGER();

    TestGraph graph;
    graph.InitGraphNodes(100);

    graph.InitMIS();
    REQUIRE(1==1);
}


// TEST_CASE("Check sending message", "")
// {
//     INIT_LOGGER();
//     TestGraph graph;
//     graph.InitGraphNodes(100);

//     graph.InitMIS();
    
//     std::string msg = "hello";
//     graph.SendMessage(2, 5, msg);
//     std::optional<std::pair<node_id_t, Message>> final_msg = std::nullopt;
//     while(final_msg == std::nullopt)
//     {
//         graph.WaitForInactiveThreadPool();
//         final_msg = graph.GetMessage(5);
//         graph.ReadMsgFromInboxOnAll();
//     }

//     REQUIRE(std::get<std::string>(final_msg.value().second.msg)==msg);
// }

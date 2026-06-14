#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "MISDistributedRouting/utils/log_macros.h"

#include "MISDistributedRouting/algorithm/MIS_node.h"

// TODO node testing is too complicated - need to refactor it

TEST_CASE("adding neighbors", ""){
    INIT_LOGGER();
    Node n1(0);
    Node n2(1);

    REQUIRE(n1.GetNeighbor(1) == nullptr);

    n1.AddEdge(&n2);

    REQUIRE(n1.GetNeighbor(1) == &n2);
}

TEST_CASE("ID sanity check", ""){
    INIT_LOGGER();
    Node n1(0);
    Node n2(1);

    REQUIRE(n1.GetID() == 0);
    REQUIRE(n2.GetID() == 1);
}


TEST_CASE("checking inbox status"){
    INIT_LOGGER();
    ThreadPool pool(1);
    MessagerNode n1(0, &pool);
    MessagerNode n2(1, &pool);

    n1.AddEdge(&n2);
    n2.AddEdge(&n1);


    n1.InitInbox();
    n2.InitInbox();

    n1.AddInboxMsg(1, MsgType::MISBuildingMessage{false});
    REQUIRE(n1.IsInboxEmpty() == true);
    REQUIRE(n2.IsInboxEmpty() == true);

    n1.UpdatePhase();
    REQUIRE(n1.IsInboxEmpty() == false);
    REQUIRE(n2.IsInboxEmpty() == true);

    n1.HandleAllInboxMessages();
    REQUIRE(n1.IsInboxEmpty() == true);
    REQUIRE(n2.IsInboxEmpty() == true);
}


TEST_CASE("sending messages"){
    INIT_LOGGER();
    ThreadPool pool(1);
    MessagerNode n1(0, &pool);
    MessagerNode n2(1, &pool);

    n1.AddEdge(&n2);
    n2.AddEdge(&n1);

    n1.InitInbox();
    n2.InitInbox();

    n2.SendMsg(&n1, MsgType::MISBuildingMessage{false});
    REQUIRE(n1.IsInboxEmpty() == true);
    REQUIRE(n2.IsInboxEmpty() == true);

    n1.UpdatePhase();
    REQUIRE(n1.IsInboxEmpty() == false);
    REQUIRE(n2.IsInboxEmpty() == true);

    n1.HandleAllInboxMessages();
    REQUIRE(n1.IsInboxEmpty() == true);
    REQUIRE(n2.IsInboxEmpty() == true);

    n2.Broadcast(MsgType::MISBuildingMessage{true});
    REQUIRE(n1.IsInboxEmpty() == true);
    REQUIRE(n2.IsInboxEmpty() == true);

    n1.UpdatePhase();
    REQUIRE(n1.IsInboxEmpty() == false);
    REQUIRE(n2.IsInboxEmpty() == true);

    n1.HandleAllInboxMessages();
    REQUIRE(n1.IsInboxEmpty() == true);
    REQUIRE(n2.IsInboxEmpty() == true);
}
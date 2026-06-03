#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "MISDistributedRouting/utils/log_macros.h"

#include "MISDistributedRouting/core/message_box.h"

TEST_CASE("reading from empty message_box", ""){
    INIT_LOGGER();
    Inbox inbox;
    REQUIRE(inbox.PopMsg() == std::nullopt);

    inbox.ReserveInbox(5);
    REQUIRE(inbox.PopMsg() == std::nullopt);
}


TEST_CASE("reading message box with empty read buffer", ""){
    INIT_LOGGER();
    Inbox inbox;
    inbox.ReserveInbox(5);

    inbox.AddMsg(1, Message{0});
    REQUIRE(inbox.PopMsg() == std::nullopt);
}


TEST_CASE("reading messges", ""){
    INIT_LOGGER();
    Inbox inbox;
    inbox.ReserveInbox(5);

    inbox.AddMsg(1, Message{0});
    inbox.ChangePhase();
    std::optional<std::pair<node_id_t, Message>> msg = inbox.PopMsg();
    REQUIRE(msg != std::nullopt);
    REQUIRE(msg.value().first == (node_id_t)1);
    REQUIRE(msg.value().second.msg == Message{0}.msg);
}


TEST_CASE("reading messges from the correct buffer", ""){
    INIT_LOGGER();
    Inbox inbox;
    inbox.ReserveInbox(5);

    inbox.AddMsg(1, Message{0});
    inbox.AddMsg(2, Message{0});
    inbox.ChangePhase();
    inbox.AddMsg(0, Message{0});
    
    std::optional<std::pair<node_id_t, Message>> msg = inbox.PopMsg();
    REQUIRE(msg != std::nullopt);
    REQUIRE(msg.value().first == (node_id_t)1);
    REQUIRE(msg.value().second.msg == Message{0}.msg);

    msg = inbox.PopMsg();
    REQUIRE(msg != std::nullopt);
    REQUIRE(msg.value().first == (node_id_t)2);
    REQUIRE(msg.value().second.msg == Message{0}.msg);

    REQUIRE(inbox.PopMsg() == std::nullopt);
}






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

    inbox.AddMsg(1, MsgType::MISBuildingMessage{false});
    REQUIRE(inbox.PopMsg() == std::nullopt);
}


TEST_CASE("reading messges", ""){
    INIT_LOGGER();
    Inbox inbox;
    inbox.ReserveInbox(5);

    inbox.AddMsg(1, MsgType::MISBuildingMessage{false});
    inbox.ChangePhase();
    std::optional<std::pair<node_id_t, Message>> msg = inbox.PopMsg();
    REQUIRE(msg != std::nullopt);
    REQUIRE(msg.value().first == (node_id_t)1);
    REQUIRE(std::get<MsgType::MISBuildingMessage>(msg.value().second.data).MIS_data == MsgType::MISBuildingMessage{false}.MIS_data);
}


TEST_CASE("reading messges from the correct buffer", ""){
    INIT_LOGGER();
    Inbox inbox;
    inbox.ReserveInbox(5);

    inbox.AddMsg(1, MsgType::MISBuildingMessage{false});
    inbox.AddMsg(2, MsgType::MISBuildingMessage{false});
    inbox.ChangePhase();
    inbox.AddMsg(0, MsgType::MISBuildingMessage{false});
    
    std::optional<std::pair<node_id_t, Message>> msg = inbox.PopMsg();
    REQUIRE(msg != std::nullopt);
    REQUIRE(msg.value().first == (node_id_t)1);
    REQUIRE(std::get<MsgType::MISBuildingMessage>(msg.value().second.data).MIS_data == MsgType::MISBuildingMessage{false}.MIS_data);

    msg = inbox.PopMsg();
    REQUIRE(msg != std::nullopt);
    REQUIRE(msg.value().first == (node_id_t)2);
    REQUIRE(std::get<MsgType::MISBuildingMessage>(msg.value().second.data).MIS_data == MsgType::MISBuildingMessage{false}.MIS_data);

    REQUIRE(inbox.PopMsg() == std::nullopt);
}






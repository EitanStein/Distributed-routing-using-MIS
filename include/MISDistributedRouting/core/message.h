#pragma once

#include <string>
#include <variant>
#include <concepts>
#include <type_traits>
#include <optional>

#include "types.h"

namespace MsgType{
    struct MISBuildingMessage{
        std::variant<is_MIS_t, rand_num_t> MIS_data;
    };

    struct PathBuildingMessage{
        node_id_t MIS_node_id;
    };

    struct RoutedMessage{
        node_id_t author;
        node_id_t recipient;
        node_id_t router_to_recipient;
        std::string data;
    };
};



typedef std::variant<MsgType::MISBuildingMessage, MsgType::PathBuildingMessage, MsgType::RoutedMessage> msg_t;

struct Message{
    msg_t data;

    template<typename T>
    requires(!std::is_same_v<std::decay_t<T>, Message> )
            && (std::is_same_v<std::decay_t<T>, MsgType::MISBuildingMessage> ||
                std::is_same_v<std::decay_t<T>, MsgType::PathBuildingMessage> ||
                std::is_same_v<std::decay_t<T>, MsgType::RoutedMessage>)
    Message(T&& data) : data(std::forward<T>(data)) {}
};


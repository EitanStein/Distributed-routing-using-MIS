#pragma once

#include <string>
#include <variant>
#include <concepts>
#include <type_traits>
#include <optional>

#include "types.h"

typedef std::variant<int32_t, double, bool, node_id_t, std::string> msg_t;

template<typename T>
concept IsMsgType = std::is_convertible_v<T, std::string> || std::same_as<std::remove_cvref_t<T>, int32_t> || std::same_as<std::remove_cvref_t<T>, double> || std::same_as<std::remove_cvref_t<T>, bool> || std::same_as<std::remove_cvref_t<T>, node_id_t>;


struct Message{
    std::optional<node_id_t> author;
    std::optional<node_id_t> recipient;
    std::optional<node_id_t> router_to_recipient;
    msg_t msg;

    template<IsMsgType T>
    Message(node_id_t from, node_id_t to, node_id_t router_to, T&& message) : author(from), recipient(to), router_to_recipient(router_to){
        if constexpr (std::is_convertible_v<T, std::string>)
            msg = std::string(std::forward<T>(message));
        else
            msg = std::forward<T>(message);
    }
    template<IsMsgType T>
    Message(T&& message) : author({}), recipient({}), router_to_recipient({}) {
        if constexpr (std::is_convertible_v<T, std::string>)
            msg = std::string(std::forward<T>(message));
        else
            msg = std::forward<T>(message);
    }
};
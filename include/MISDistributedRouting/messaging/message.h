#pragma once

#include <string>
#include <variant>
#include <concepts>
#include <type_traits>

#include "MISDistributedRouting/utils/types.h"

typedef std::variant<int32_t, std::string> msg_t;

template<typename T>
concept IsMsgType = std::is_convertible_v<T, std::string> || std::same_as<T, int32_t>;


struct Message{
    std::optional<node_id_t> author;
    std::optional<node_id_t> recipient;
    msg_t msg;

    template<IsMsgType T>
    Message(node_id_t from, node_id_t to, T&& message) : author(from), recipient(to){
        if constexpr (std::is_convertible_v<T, std::string>)
            msg = std::string(std::forward<T>(message));
        else
            msg = std::forward<T>(message);
    }
    template<IsMsgType T>
    Message(T&& message) : author({}), recipient({}) {
        if constexpr (std::is_convertible_v<T, std::string>)
            msg = std::string(std::forward<T>(message));
        else
            msg = std::forward<T>(message);
    }
};
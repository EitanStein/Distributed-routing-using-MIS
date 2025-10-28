#pragma once

#include "MISDistributedRouting/utils/log_macros.h"
#include "MISDistributedRouting/core/message.h"

inline void LogInfoMessage(const Message& msg)
{
    if (std::holds_alternative<int32_t>(msg.msg))
        LOG_INFO("Message is: {}", std::get<int32_t>(msg.msg));
    else
        LOG_INFO("Message is: {}", std::get<std::string>(msg.msg));
}
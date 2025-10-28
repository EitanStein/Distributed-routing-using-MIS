#pragma once

#include "MISDistributedRouting/utils/log_macros.h"
#include <chrono>


void TimerWrapper(std::function<void()> func, std::string_view func_name)
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    LOG_INFO("runtime for func {} took {}", func_name, std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
}
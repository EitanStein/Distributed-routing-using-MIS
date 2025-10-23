#pragma once

#include "logger.h"

#define INIT_LOGGER() Logger::Init()

#define LOG_DEBUG(...) Logger::Get()->debug(__VA_ARGS__)
#define LOG_INFO(...)  Logger::Get()->info(__VA_ARGS__)
#define LOG_WARN(...)  Logger::Get()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::Get()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::Get()->critical(__VA_ARGS__)


// #define LOG_DEBUG(...) (void)0
// #define LOG_INFO(...)  (void)0
// #define LOG_WARN(...)  (void)0
// #define LOG_ERROR(...) (void)0
// #define LOG_CRITICAL(...) (void)0
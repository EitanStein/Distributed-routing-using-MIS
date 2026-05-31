#include "MISDistributedRouting/utils/logger.h"

std::shared_ptr<spdlog::logger> Logger::s_logger;

void INIT_LOGGER(){
    [[maybe_unused]] static bool initialized = []() {
        Logger::Init();
        return true;
    }();
}
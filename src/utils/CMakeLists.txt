add_library(utils_module
    thread_pool.cpp
)

add_library(logging_module
    logger.cpp
)

target_link_libraries(logging_module
    PRIVATE
        project_options
        spdlog::spdlog
)

target_link_libraries(utils_module
    PRIVATE
        project_options
        spdlog::spdlog
        Threads::Threads

)

if(TRACY_ENABLE)
    target_link_libraries(utils_module PRIVATE Tracy::TracyClient)
    target_compile_definitions(utils_module PRIVATE TRACY_ENABLE)
endif()

target_include_directories(utils_module
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_include_directories(logging_module
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
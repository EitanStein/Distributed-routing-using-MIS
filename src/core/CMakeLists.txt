add_library(core_module
    graph.cpp
    node.cpp
    message_box.cpp
    thread_pool.cpp
)

target_link_libraries(core_module
    PRIVATE
        project_options
        logging_module
        Threads::Threads
)

if(TRACY_ENABLE)
    target_link_libraries(core_module PRIVATE Tracy::TracyClient)
    target_compile_definitions(core_module PRIVATE TRACY_ENABLE)
endif()

target_include_directories(core_module
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
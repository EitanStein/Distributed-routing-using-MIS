add_library(simulation_module
    simulation_graph.cpp
    simulation_window.cpp
    simulation_node.cpp
    simulator.cpp
)

target_compile_definitions(simulation_module PRIVATE ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets/")


target_link_libraries(simulation_module
    PUBLIC
        MIS_algo_module
    PRIVATE
        project_options
        spdlog::spdlog
        SFML::Graphics
        SFML::Window
)

target_include_directories(simulation_module
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
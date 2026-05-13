add_subdirectory(core)
add_subdirectory(algorithm)
add_subdirectory(utils)
add_subdirectory(simulation)



add_executable(MISDistributedRouting main.cpp)

target_compile_definitions(MISDistributedRouting PRIVATE ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets/")

target_link_libraries(MISDistributedRouting
    PRIVATE
        project_options
        core_module
        utils_module
        logging_module
        MIS_algo_module
        simulation_module
        spdlog::spdlog
        SFML::Graphics 
        SFML::Window    
)

if(TRACY_ENABLE)
    target_link_libraries(MISDistributedRouting PRIVATE Tracy::TracyClient)
    target_compile_definitions(MISDistributedRouting PRIVATE TRACY_ENABLE)
endif()
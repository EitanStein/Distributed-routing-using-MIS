

add_executable(tests test_main.cpp)

target_compile_definitions(tests PRIVATE ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets/")


target_link_libraries(tests
    PRIVATE
        project_options
        core_module
        utils_module
        logging_module
        MIS_algo_module
        simulation_module
        spdlog::spdlog
        Catch2::Catch2WithMain
        SFML::Graphics 
        SFML::Window
)

add_test(NAME MIS_Graph_Checks COMMAND $<TARGET_FILE:tests>)


add_executable(tests test_main.cpp)
add_executable(node_tests test_node.cpp)
add_executable(msg_tests test_msg.cpp)

target_compile_definitions(tests PRIVATE ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets/")


target_link_libraries(tests
    PRIVATE
        project_options
        core_module
        logging_module
        MIS_algo_module
        simulation_module
        Catch2::Catch2WithMain
        SFML::Graphics 
        SFML::Window
)

target_link_libraries(node_tests
    PRIVATE
        project_options
        core_module
        logging_module
        MIS_algo_module
        simulation_module
        Catch2::Catch2WithMain
)

target_link_libraries(msg_tests
    PRIVATE
        project_options
        core_module
        logging_module
        MIS_algo_module
        simulation_module
        Catch2::Catch2WithMain
)

add_test(NAME msg_testing COMMAND $<TARGET_FILE:msg_tests>)
add_test(NAME node_testing COMMAND $<TARGET_FILE:node_tests>)
add_test(NAME MIS_Graph_Checks COMMAND $<TARGET_FILE:tests>)
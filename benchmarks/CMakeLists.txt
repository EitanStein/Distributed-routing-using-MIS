

add_executable(benchmarks bench_main.cpp)

target_compile_definitions(benchmarks PRIVATE ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets/")


target_link_libraries(benchmarks
    PRIVATE
        project_options
        core_module
        logging_module
        MIS_algo_module
        simulation_module
        Catch2::Catch2WithMain
        SFML::Graphics 
        SFML::Window
        benchmark::benchmark_main
)
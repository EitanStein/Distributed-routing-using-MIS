#include <benchmark/benchmark.h>
#include "MISDistributedRouting/simulation/simulation_graph.h"
#include "MISDistributedRouting/utils/log_macros.h"

static void BM_MIS_building(benchmark::State& state){
    INIT_LOGGER();

    SimulationGraph graph(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    

    for(auto _ : state){
        state.PauseTiming();
        graph.InitGraph(state.range(0));
        state.ResumeTiming();

        graph.AdvanceStatus();
        while(graph.GetGraphStage() != MIS_Node::MIS_Stage::COMPLETE){
            graph.RunCycle();
        }

        benchmark::DoNotOptimize(graph);
        benchmark::ClobberMemory();
    }
    
}


BENCHMARK(BM_MIS_building)->Arg(100)->Arg(500)->Arg(1000)->Arg(2000);

BENCHMARK_MAIN();
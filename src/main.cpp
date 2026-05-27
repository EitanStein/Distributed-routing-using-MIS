#include "MISDistributedRouting/utils/log_macros.h"
#include "MISDistributedRouting/simulation/simulator.h"
#include "MISDistributedRouting/utils/profiling.hpp"

int main()
{
    INIT_LOGGER();

    Simulator sim;

    sim.Run();
    return 0;
}
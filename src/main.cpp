#include "MISDistributedRouting/utils/log_macros.h"
#include "MISDistributedRouting/simulation/simulator.h"
#include <tracy/Tracy.hpp>

int main()
{
    INIT_LOGGER();
    ZoneScoped;
    Simulator sim;

    sim.Run();
    return 0;
}
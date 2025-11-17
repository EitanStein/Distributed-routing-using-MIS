#include "MISDistributedRouting/utils/log_macros.h"
#include "MISDistributedRouting/simulation/simulator.h"

int main()
{
    INIT_LOGGER();
    
    Simulator sim;

    sim.Run();
    return 0;
}
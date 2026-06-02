# MIS Routing simulator

A custom c++ synchronized distributed implementation that constructs a maximal independent set and uses it to route messages between nodes in the graph
It uses a random algorithm described in the book Distributed Graph Algorithms For Computer Networks by K. Erciyes (Rand2_MIS in the book)

## Installation
### Requirements
- C++20-compatible compiler
- CMake 3.28+
- runs on linux (planned to extend to windows as well)

- SFML/spdlog/Catch2 are fetched automatically via CMake’s FetchContent – no manual installation required.

### Build & Usage
```bash
./cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
./cmake --build build
build/bin/MISDistributedRouting.exe
```
default values (node size in the gui, max distance between two nodes, frame rate) can be found in `include/config.h`
at the moment changing them will require recompliation of the program (future goal to change)

### Algorithm and Complexity

each node sends a random value in the range [0,1) to its neighbors
if its own random value is larger than its neighbors - its added to the MIS and notifies its neighbors

after the MIS finishes building - each MIS node braodcasts so that each node will know the shortest route to each MIS node

the construction of the MIS takes O(log n) cycles and uses O(mlog n) messages
the construction of the path table takes O(diam(G)) cycles and uses O(m*size(MIS)) messages

## Design choices
#### Node MessageBox
Each message box has a dedicated inbox per neighbor - that way there are no races when different neighbors send messages to the same node
It manages 2 buffers of dedicated inboxes - one for writing (recieving messages) and one for reading (reading messages)
that way in a single round a node can both read from the reading buffer and recieve new messages without races
every cycle - the buffers are switched and the 'new' reading buffer is cleaned up

to avoid repeated allocations for pushing messages
the dedicated inbox of each neighbor overrides existing allocated memory that was already read
it uses an additional size and current index variables to track the current state of the inbox

#### Thread Pool
Instead of repeatedly creating and destroying threads per task
we have a predetermined amount of threads active at all times and waiting for new tasks to be added to a task queue
in our implementation the only possible task are only the process of reading available messages and sending new ones to neighbors
i.e they help simulate autonomous node processing

we use a task queue and mutexes to avoid races and track how many tasks are active at each point in time
I considered using a custom queue using a ring buffer that would mean less memory allocations/deallocations but it proved to be slower than a regular std::queue (I plan to test this more in the future - the custom queue is saved in an archive branch)

## Profiling
I used tracy, googlebenchmarks, valgrind, ASan and UBSan to attempt and find bottlenecks and places that could be optimized further

profiling was done on an ubuntu 24.04 virtual machine with 2 cpus and limited memory - so truly large graphs could not be tested
![Benchmark Results](./docs/benchmark.png)

## future work
1) further memory optimizations in nodes
there is redundancy in node's data about its neighbors

2) compatibility on windows

3) improving the GUI

## Project status
This is a learning project and is under devlopment (currently attempting to optimize it).

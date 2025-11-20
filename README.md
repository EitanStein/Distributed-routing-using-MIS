# MIS Routing simulator

A custom gui in c++ that visualizes routing messages between unit graph nodes using a maximal independent sets as routing points.

## Installation

### Requirements

- C++20-compatible compiler
- CMake 3.28+

- SFML/spdlog/Catch2 are fetched automatically via CMake’s FetchContent – no manual installation required.

### Build
```bash
./cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
./cmake --build build
```


## Usage
```bash
build/bin/Release/MISDistributedRouting.exe
```


- Create a graph (can change the number of nodes by clicking the text box and changing the number)
- Click the Build MIS button to start the MIS building process and router path table
- Send a message from any node to any node by clicking them (1st click the sender, then the recipient) and then sending a message (can click the text box to insert a string for the message)
Clicking anywhere on the screen where there is no node or on message sending related controls will reset the chosen nodes.

Project configuration can be changed via `include/config.h`
values that can be changed are

- NODE_RADIUS -> radius of graph nodes in the simulator (in pixels), smaller nodes are harder to see but take less space on the screen
- DEFAULT_UNIT_DIST -> Max distance between neighboring nodes (in pixels)
- FRAME_RATE -> Simulator frame rate, higher frame rate means faster simulator but its harder to track the simulation




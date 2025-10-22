cmake_minimum_required(VERSION 3.16) 
project(MISDistributedRouting LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)

# TODO
# add_subdirectory(external/imgui)
# add_subdirectory(external/implot)
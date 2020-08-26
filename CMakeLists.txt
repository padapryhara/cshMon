project(cshMonitor)
cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#server
add_executable(hmonsrv hMonSrv.cpp)
target_link_libraries(hmonsrv zmq zmqpp pistache sodium)

#client
add_executable(hmoncli hMonCli.cpp)
target_link_libraries(hmoncli zmq zmqpp sodium)

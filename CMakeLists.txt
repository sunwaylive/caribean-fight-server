cmake_minimum_required (VERSION 2.6)
project(Server)
set (Server_VERSION_MAJOR 1)
set (Server_VERSION_MINOR 0)

include_directoies("/home/sunwayliving/boost_1_60_0")
add_executable(Server server.cpp)

cmake_minimum_required (VERSION 2.6)
set (Server_VERSION_MAJOR 1)
set (Server_VERSION_MINOR 0)

#This dertermins the PEOJCET_SOURCE_DIR
project(server)

#debug message
message("project source dir: " ${PROJECT_SOURCE_DIR})

set (BOOST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/3rd/boost_1_60_0/include)
set (BOOST_LIB_DIR ${PROJECT_SOURCE_DIR}/3rd/boost_1_60_0/lib)

set (SERVER_LIBS
                libboost_system.a
                libboost_thread.a)

set (CMAKE_CXX_FLAGS "-Wall -g -std=c++11 -Werror")
set (CMAKE_EXE_LINKER_FLAGS "-pthread -lrt")

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

message("####### collecting files ######")
#cool! collect files in ./src directory
include (src/CMakeLists.txt)
foreach (FILE ${FILES})
    set (SRCFILES ${SRCFILES} src/${FILE})
    message("FILE: " ${FILE})
    message("SRCFILES: " "${SRCFILES}")
endforeach(FILE)

message("####### collecting files done! ######")

include_directories(${BOOST_INCLUDE_DIR})
link_directories(${BOOST_LIB_DIR})

add_executable(server ${SRCFILES})
target_link_libraries (server ${SERVER_LIBS})

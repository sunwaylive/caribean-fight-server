BOOST_INCLUDE_DIR = "/home/sunwayliving/data/boost_1_60_0"
#BOOST_LIB_DIR = "/Users/sunwei/SoftwareInstallDir/boost_1_60_0/libs/"
BOOST_LIB_DIR = /home/sunwayliving/data/lib

CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG) -std=c++11
LFLAGS = -L$(BOOST_LIB_DIR) -lboost_system

all: echo_client echo_server

#echo_client 
echo_client: echo_client.o
	$(CC) echo_client.o -o echo_client $(LFLAGS)

echo_client.o: echo_client.cpp
	$(CC) $(CFLAGS) -I $(BOOST_INCLUDE_DIR) -I ./ -c echo_client.cpp

#echo_server
echo_server: echo_server.o
	$(CC) echo_server.o -o echo_server $(LFLAGS)

echo_server.o: echo_server.cpp
	$(CC) $(CFLAGS) -I $(BOOST_INCLUDE_DIR) -I ./ -c echo_server.cpp

clean:
	\rm *.o *~ echo_client echo_server

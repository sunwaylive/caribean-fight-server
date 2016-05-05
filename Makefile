BOOST_INCLUDE_DIR = "/home/sunwayliving/boost_1_60_0"
BOOST_LIB_DIR = /home/sunwayliving/lib

ALL_INCLUDE = -I $(BOOST_INCLUDE_DIR) -I ./ -I /usr/include/ -I /usr/include/c++/4.9.3/

CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG) -std=c++11
LFLAGS = -pthread -L$(BOOST_LIB_DIR) -lboost_thread -lboost_system -lrt

all: server

server: server.o
	$(CC) server.o -o server $(LFLAGS)

server.o: server.cpp
	$(CC) $(CFLAGS) $(ALL_INCLUDE) -c server.cpp

clean:
	\rm *.o *~ server

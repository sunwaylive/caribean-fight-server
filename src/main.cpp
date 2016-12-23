#include <cstdlib>
#include <cstring>
#include <thread>
#include <utility>
#include <iostream>
#include <string.h>
#include <map>
#include <vector>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>
#include "common_def.h"
#include "session_mgr.h"
#include "room_mgr.h"
#include "server.hpp"

int main(int argc, char* argv[])
{
  try
  {
    //if (argc != 2)
    //{
    //  std::cerr << "Usage: async_tcp_echo_server <port>\n";
    //  return 1;
    //}

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    Server s(io_service, 3008, 3009);

    while (true)
    {
        io_service.run_one();

        GameMgrSin::instance().Tick();
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}


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
#include "server.h"


//class Server
//{
//public:
//    Server(boost::asio::io_service& io_service, short port)
//        : m_io_service(io_service),
//        m_acceptor(io_service, tcp::endpoint(tcp::v4(), port))
//    {
//        Session* new_session = new Session(m_io_service);
//
//        m_acceptor.async_accept(new_session->Socket(),
//                               boost::bind(&Server::HandleAccept, this, new_session,
//                                           boost::asio::placeholders::error));
//    }
//
//    void HandleAccept(Session* new_session,
//                       const boost::system::error_code& error)
//    {
//        if (!error)
//        {
//            if(new_session != NULL)
//            {
//                new_session->SetId(new_session->Socket().remote_endpoint().address().to_string());
//                SessionMgrSin::instance().AddSession(new_session);
//
//                new_session->Start();
//            }
//
//            Session *new_session_2 = new Session(m_io_service);
//            m_acceptor.async_accept(new_session_2->Socket(),
//                                   boost::bind(&Server::HandleAccept, this, new_session_2,
//                                               boost::asio::placeholders::error));
//        }
//        else
//        {
//            delete new_session;
//        }
//    }
//
//private:
//  boost::asio::io_service& m_io_service;
//  tcp::acceptor m_acceptor;
//};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    Server s(io_service, atoi(argv[1]), 3009);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}


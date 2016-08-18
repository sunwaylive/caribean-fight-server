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

/*************************************************************************/
//const std::string SERVER_IP = "119.29.25.185";
const std::string SERVER_IP = "127.0.0.1";
const unsigned short PORT = 3008;
const unsigned short FSP_PORT = 3009;

boost::asio::io_service io_service;
tcp::acceptor acp(io_service, tcp::endpoint(tcp::v4(), PORT));
tcp::acceptor fsp_acp(io_service, tcp::endpoint(tcp::v4(), FSP_PORT));

/*************************************************************************/
void start_accept(SocketPtr sock_ptr);
void handle_accept(SocketPtr sock_ptr, const boost::system::error_code &err);

/*************************************************************************/
void server(boost::asio::io_service &io_service, unsigned short port, unsigned short fsp_port)
{
    SocketPtr sock_ptr(new tcp::socket(io_service));
    start_accept(sock_ptr);
}

void start_accept(SocketPtr sock_ptr)
{
    printf("start accept\n");
    acp.async_accept(*sock_ptr, boost::bind(handle_accept, sock_ptr, _1));
}

void handle_accept(SocketPtr sock_ptr, const boost::system::error_code &err)
{
    printf("handle accept\n");
    //when a new player connected, add a new session
    //std::string sock_ip = boost::lexical_cast<std::string>(sock_ptr->remote_endpoint());
    std::string sock_ip = sock_ptr->remote_endpoint().address().to_string();

    Session *sess = SessionMgrSin::instance().AddSession(sock_ip, sock_ptr);
    if(sess != NULL)
    {
        sess->Start(); //bug:: async_read/write should be here
    }
    std::cout<<" one client has connected to me!" <<std::endl;

    SocketPtr new_sock(new tcp::socket(io_service));
    start_accept(new_sock);
}

int main(int argc, char* argv[])
{
    try
    {
        unsigned short server_port = PORT;
        unsigned short fsp_server_port = FSP_PORT;
        if(argc >= 3)
        {
            server_port = std::atoi(argv[1]);
            fsp_server_port = std::atoi(argv[2]);
        }
        else
        {
            std::cout<<"Use default port: " << PORT <<" " << FSP_PORT <<std::endl;
        }

        server(io_service, server_port, fsp_server_port);
        io_service.run();
    }
    catch(std::exception &e)
    {
        std::cerr<< "Exception: " <<e.what() <<"\n";
    }

    return 0;
}

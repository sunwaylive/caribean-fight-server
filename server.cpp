#include <cstdlib>
#include <cstring>
#include <thread>
#include <utility>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
const std::string SERVER_IP = "119.29.25.185";
const std::string PORT = "3008";
const int max_length = 1024;

void session(tcp::socket sock)
{
    try
    {
        for(;;)
        {
            char data[max_length] = "";

            boost::system::error_code error;
            size_t length = sock.read_some(boost::asio::buffer(data), error);
            if(error == boost::asio::error::eof)
                break; //connection closed cleanly by peer
            else if(error)
                throw boost::system::system_error(error); //some other error

            std::cout<<"I have received msg: " <<std::string(data) <<std::endl;
            boost::asio::write(sock, boost::asio::buffer(data, length));
        }
    }
    catch(std::exception &e)
    {
        std::cerr <<"Exception in thread: "<<e.what() <<std::endl;
    }
}

void server(boost::asio::io_service &io_service, unsigned short port)
{
    tcp::acceptor acp(io_service, tcp::endpoint(tcp::v4(), port));
    for(;;)
    {
        tcp::socket sock(io_service);
        acp.accept(sock);
        //after std::move the sock will be empty
        std::thread(session, std::move(sock)).detach();
    }
}

int main(int argc, char* argv[])
{
    try
    {
        std::string server_port = PORT;
        if(argc >= 2)
        {
             server_port = argv[1];
        }else
        {
            std::cout<<"Use default port: " << PORT <<std::endl;
        }

        boost::asio::io_service io_service;
        server(io_service, std::atoi(server_port.c_str()));
    }
    catch(std::exception &e)
    {
        std::cerr<< "Exception: " <<e.what() <<"\n";
    }

    return 0;
}

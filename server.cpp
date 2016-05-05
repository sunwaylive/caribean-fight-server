#include <cstdlib>
#include <cstring>
#include <thread>
#include <utility>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <string.h>
#include <map>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using boost::asio::ip::tcp;
using namespace std;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

struct Room
{
    int room_id;
    int max_player_num;
    std::vector<socket_ptr> player_list;

    Room(int rid, int maxPlayerNum)
    : room_id(rid), max_player_num(maxPlayerNum)
    {
    }
};

const std::string SERVER_IP = "119.29.25.185";
const std::string PORT = "3008";
const int max_length = 1024;
int gRoomId = 1;
vector<Room*> gRoomList;

void create_room(socket_ptr sock, int playerNum)
{
    std::cout<<"***************************************" <<std::endl;
    std::cout<<"create room for " << playerNum <<" players!" <<std::endl;
    Room *new_room = new Room(gRoomId, playerNum);
    new_room->player_list.push_back(sock);
    //std::cout<<"In sub thread" <<sock-> <<endl;
    //printf("%x", sock);
    gRoomList.push_back(new_room);
    gRoomId++; //increase the global room id

    //tell the client that room is created successfully
    string toSend = "CREATEROOM@" + std::to_string(new_room->room_id) + "\n"; 
    boost::asio::write(*sock, boost::asio::buffer(toSend, toSend.length()));
    std::cout<<"***************************************" <<std::endl;
}

void list_room(socket_ptr sock)
{
   string toSend = "LISTROOM@";
   for(int i = 0; i < (int)gRoomList.size(); ++i)
   {
        Room *r = gRoomList[i]; 
        toSend += std::to_string(r->room_id) + " " 
                  + std::to_string(r->max_player_num) + " "
                  + std::to_string(r->player_list.size()) + "|"; 
   }

   toSend += "\n";
   std::cout<<"list_room msg: " <<toSend <<std::endl;

   boost::asio::write(*sock, boost::asio::buffer(toSend, toSend.length()));
}

void handle_message(socket_ptr sock, std::string msg)
{
    std::cout<<"I have received msg: " <<msg <<std::endl;
    if(msg.find("CREATEROOM") != std::string::npos)
    {
       int playerNum = std::stoi(msg.substr(strlen("CREATEROOM") + 1));
       create_room(sock, playerNum);
    }else if(msg.find("LISTROOM") != std::string::npos)
    {
       list_room(sock); 
    }
}

void client_session(socket_ptr sock)
{
    std::cout<<"A client has connected to me!" <<std::endl;
    try
    {
        for(;;)
        {
            char data[max_length] = "";

            boost::system::error_code error;
            size_t length = sock->read_some(boost::asio::buffer(data), error);
            if(error == boost::asio::error::eof)
                break; //connection closed cleanly by peer
            else if(error)
                throw boost::system::system_error(error); //some other error

            handle_message(sock, std::string(data));
            std::cout<<"recv from client length: " << length <<std::endl;
            //boost::asio::write(sock, boost::asio::buffer(data, length));
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
        //tcp::socket sock(io_service);
        socket_ptr sock(new tcp::socket(io_service));
        acp.accept(*sock);
        //printf("%x", sock);

        boost::thread(boost::bind(client_session, sock));

        //after std::move the sock will be empty
        //std::thread(functionPtr, args)
        
        //std::thread(session, std::move(sock)).detach();
        //after seperate the socket into a new thread, the main thread will go on listening to clients
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

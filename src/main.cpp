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

/*************************************************************************/
struct Room
{
    int room_id;
    int max_player_num;
    std::vector<SocketPtr> player_list;

    Room(int rid, int maxPlayerNum)
    : room_id(rid), max_player_num(maxPlayerNum)
    {
    }
};

/*************************************************************************/
const std::string SERVER_IP = "119.29.25.185";
const std::string PORT = "3008";

//if two players, then put them in different camp, A and B
const std::string TWO_PLAYER_START_GAME_STRING = "0,A#1,B";

//if four players, then first 2 are in camp A, second 2 are in camp B
const std::string FOUR_PLAYER_START_GAME_STRING = "0,A#1,A#2,B#3,B";

const int max_length = 1024;
int gRoomId = 1;
vector<Room*> gRoomList;

/*************************************************************************/
//Here we begin to design the architecture


/*************************************************************************/
Room* find_room(int room_id)
{
    Room *r = NULL;
    for(auto iter = gRoomList.begin(); iter != gRoomList.end(); ++iter)
    {
        if((*iter)->room_id == room_id)
        {
            r = *iter;
        }
    }

    return r;
}

void create_room(SocketPtr sock, int playerNum)
{
    std::cout<<"**************create room*************************" <<std::endl;
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
    std::cout<<"send to client: " <<toSend <<std::endl;
    std::cout<<"***************************************" <<std::endl;
}

void list_room(SocketPtr sock)
{
   std::cout<<"*******list room********************************" <<std::endl;
   string toSend = "LISTROOM@";
   for(int i = 0; i < (int)gRoomList.size(); ++i)
   {
        Room *r = gRoomList[i];
        toSend += std::to_string(r->room_id) + " "
                  + std::to_string(r->max_player_num) + " "
                  + std::to_string(r->player_list.size()) + "|";
   }

   toSend += "\n";

   boost::asio::write(*sock, boost::asio::buffer(toSend, toSend.length()));
   std::cout<<"send to client: " <<toSend <<std::endl;
   std::cout<<"***************************************" <<std::endl;
}

void join_room(SocketPtr sock, int roomId)
{
    std::cout<<"************join room***************************" <<std::endl;
    string toSend = "JOINTROOM\n";
    for(int i =  0; i < (int)gRoomList.size(); ++i)
    {
        Room *r = gRoomList[i];
        if(r->room_id == roomId)
        {
            if((int)r->player_list.size() >= r->max_player_num)
            {
                std::cerr<<"Room is full!" <<std::endl;
                return;
            }

            r->player_list.push_back(sock);
        }
    }

    boost::asio::write(*sock, boost::asio::buffer(toSend, toSend.length()));
    std::cout<<"send to client: " <<toSend <<std::endl;
    std::cout<<"***************************************" <<std::endl;
}

void start_game(SocketPtr sock, int roomId)
{
    std::cout<<"************start game***************************" <<std::endl;
    Room *room = find_room(roomId);
    if(room == NULL)
    {
        std::cerr<<"Error: wrong room id: " <<roomId <<std::endl;
        return;
    }

    string toSend;
    string toSendEnd;

    auto& player_list = room->player_list;
    if(player_list.size() == 2)
    {
        toSendEnd = TWO_PLAYER_START_GAME_STRING;
    }
    else if(player_list.size() == 4)
    {
        toSendEnd = FOUR_PLAYER_START_GAME_STRING;
    }

    int player_idx = 0;
    //each player
    for(auto player_iter = player_list.begin(); player_iter != player_list.end(); ++player_iter)
    {
        //STARTGAME#max_players_num#my_idx#0,A#1,A#2,B#3,B\n
        toSend = "STARTGAME#" + std::to_string(player_list.size()) + "#"
                 + std::to_string(player_idx) +"#" + toSendEnd + "\n";

        boost::asio::write(**player_iter, boost::asio::buffer(toSend, toSend.length()));
        std::cout<<"send to client "<<player_idx++<<": " <<toSend <<std::endl;
    }

    std::cout<<"***************************************" <<std::endl;
}

void update_game(SocketPtr sock, int roomId, std::string msg)
{
    //std::cout<<"************update game***************************" <<std::endl;
    Room *room = find_room(roomId);
    if(room == NULL)
    {
        std::cerr<<"Error: wrong room id: " <<roomId <<std::endl;
        return;
    }

    //broadcast to all other players in the room
    for(auto iter = room->player_list.begin(); iter != room->player_list.end(); ++iter)
    {
        if(sock == *iter)
        {
            //self
            continue;
        }

        boost::asio::write(**iter, boost::asio::buffer(msg, msg.length()));
    }

    //std::cout<<"***************************************" <<std::endl;
}

void end_game(SocketPtr sock, int roomId)
{

}

void handle_message(SocketPtr sock, std::string msg)
{
    std::cout<<"I have received msg: " <<msg <<std::endl;
    if(msg.find("CREATEROOM") != std::string::npos)
    {
       int playerNum = std::stoi(msg.substr(strlen("CREATEROOM") + 1));
       create_room(sock, playerNum);
    }
    else if(msg.find("LISTROOM") != std::string::npos)
    {
       list_room(sock);
    }
    else if(msg.find("JOINROOM") != std::string::npos)
    {
        int roomId = std::stoi(msg.substr(strlen("JOINROOM") + 1));
        join_room(sock, roomId);
    }
    else if(msg.find("STARTGAME") != std::string::npos)
    {
        int roomId = std::stoi(msg.substr(strlen("STARTGAME") + 1));
        start_game(sock, roomId);
    }
    else if(msg.find("UPDATEGAME") != std::string::npos)
    {
        //send message to other clients
        int roomId = std::stoi(msg.substr(strlen("UPDATEGAME") + 1));
        update_game(sock, roomId, msg);
    }
}

//when a new player connected, a new thread will be launched running this function
void client_session(Session *sess)
{
    std::cout<<" one client has connected to me!" <<std::endl;
    SocketPtr sock = sess->GetSocketPtr();
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

            std::cout<<"recv from client length: " << length <<std::endl;
            handle_message(sock, std::string(data));
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
        printf("1111\n");
        SocketPtr sock_ptr(new tcp::socket(io_service));
        acp.accept(*sock_ptr);
        printf("2222\n");

        //when a new player connected, add a new session
        std::string sock_ip = boost::lexical_cast<std::string>(sock_ptr->remote_endpoint());
        std::cout<<"sock_ip: " <<sock_ip <<std::endl;

        Session *sess = SessionMgrSin::instance().AddSession(sock_ip, sock_ptr);
        if(sess == NULL)
        {
            continue;
        }

        boost::thread(boost::bind(client_session, sess));

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

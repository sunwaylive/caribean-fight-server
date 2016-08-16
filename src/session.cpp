#include "session.h"
#include <boost/asio.hpp>

/**************************************************************/
string Session::HandlePkg(std::string pkg)
{
    std::cout<<"I have received pkg: " <<pkg <<std::endl;

    if(pkg.find("CREATEROOM") != std::string::npos)
    {
        if(m_rid != 0)
        {
            printf("ERROR: already in a room!\n");
            return "";
        }

        int playerNum = std::stoi(pkg.substr(strlen("CREATEROOM") + 1));
        Room *new_room = RoomMgrSin::instance().CreateRoom(this, playerNum);
        m_rid = new_room->GetId();

        std::string to_send = "CREATEROOM@" + std::to_string(new_room->GetId()) + "\n";
        return to_send;
    }
    else if(pkg.find("LISTROOM") != std::string::npos)
    {
        const unordered_map<unsigned, Room*>* r_map = RoomMgrSin::instance().GetAllRoomMap();
        std::string to_send = "LISTROOM@";
        for(auto iter = r_map->begin(); iter != r_map->end(); ++iter)
        {
            to_send += std::to_string(iter->second->GetId()) + " "
                      + std::to_string(iter->second->GetMaxPlayerNum()) + " "
                      + std::to_string(iter->second->GetCurPlayerNum()) + "|";
        }
        to_send += "\n";
        return to_send;
    }
    else if(pkg.find("JOINROOM") != std::string::npos)
    {
        int roomId = std::stoi(pkg.substr(strlen("JOINROOM") + 1));
        Room *r = RoomMgrSin::instance().JoinRoom(this, roomId);
        if( r == NULL)
        {
            //BUG: use protobuf to wrap the network cmd, and use error code
        }

        std::string to_send = "JOINTROOM\n";
        return to_send;
    }
    else if(pkg.find("STARTGAME") != std::string::npos)
    {
        int roomId = std::stoi(pkg.substr(strlen("STARTGAME") + 1));
        printf("%d\n", roomId);
        Game *new_game = RoomMgrSin::instance().StartGame(roomId);
        if(new_game == NULL)
        {
            printf("ERROR: start game error!\n");
        }

        return "";
        //start_game(sock, roomId);
    }
    //else if(pkg.find("UPDATEGAME") != std::string::npos)
    //{
    //    //send message to other clients
    //    int roomId = std::stoi(pkg.substr(strlen("UPDATEGAME") + 1));
    //    printf("%d\n", roomId);
    //    //update_game(sock, roomId, pkg);
    //}

    return "";
}

//void Session::Recv()
//{
//    try
//    {
//        printf("before for\n");
//        for(;;)
//        {
//            printf("pkg size: %d\n", kMaxPkgSize);
//
//            boost::system::error_code error;
//            if(m_sock_ptr == NULL)
//            {
//            }
//            size_t length = m_sock_ptr->read_some(boost::asio::buffer(m_pkg), error);
//            printf("%s\n", m_pkg);
//
//            if(error == boost::asio::error::eof)
//            {
//                printf("eof\n");
//                break; //connection closed cleanly by peer
//            }
//            else if(error)
//            {
//                printf("error\n");
//                std::cout<<"value: " <<error.value()<< std::endl;
//                std::cout<<"message: " <<error.message()<< std::endl;
//                throw boost::system::system_error(error); //some other error
//            }
//
//            std::cout<<"recv from client length: " << length <<std::endl;
//            //handle_message(sock, std::string(data));
//            //boost::asio::write(sock, boost::asio::buffer(data, length));
//        }
//    }
//    catch(std::exception &e)
//    {
//        std::cerr <<"Exception in thread: "<<e.what() <<std::endl;
//    }
//}

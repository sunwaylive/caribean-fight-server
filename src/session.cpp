#include "session.h"
#include <boost/asio.hpp>


/**************************************************************/
void Session::Start()
{
    m_socket.async_read_some(boost::asio::buffer(m_pkg, MAX_PKG_SIZE),
            boost::bind(&Session::HandleRead, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}

void Session::HandleRead(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        m_rsp = this->HandlePkg(m_pkg);

        boost::asio::async_write(m_socket,
                boost::asio::buffer(m_rsp, m_rsp.length()),
                boost::bind(&Session::HandleWrite, this,
                    boost::asio::placeholders::error));
    }
    else
    {
        cout<<"delete this" <<endl;
        cout<< "before delete " <<this << endl;
        delete this;
    }
}

void Session::HandleWrite(const boost::system::error_code& error)
{
    if (!error)
    {
        m_socket.async_read_some(boost::asio::buffer(m_pkg, MAX_PKG_SIZE),
                boost::bind(&Session::HandleRead, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        delete this;
    }
}

/**************************************************************/
void Session::FspStart()
{
    m_fsp_socket.async_read_some(boost::asio::buffer(m_fsp_pkg, MAX_PKG_SIZE),
            boost::bind(&Session::FspHandleRead, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}

void Session::FspHandleRead(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        m_fsp_rsp = this->HandlePkg(m_fsp_pkg);

        boost::asio::async_write(m_fsp_socket,
                boost::asio::buffer(m_fsp_rsp, m_fsp_rsp.length()),
                boost::bind(&Session::FspHandleWrite, this,
                    boost::asio::placeholders::error));
    }
    else
    {
        delete this;
    }
}

void Session::FspHandleWrite(const boost::system::error_code& error)
{
    if (!error)
    {
        m_fsp_socket.async_read_some(boost::asio::buffer(m_fsp_pkg, MAX_PKG_SIZE),
                boost::bind(&Session::FspHandleRead, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        delete this;
    }
}

/**************************************************************/
string Session::HandlePkg(std::string pkg)
{
    std::cout<<"I have received pkg: " <<pkg <<std::endl;

    if(pkg.find("CREATEROOM") != std::string::npos)
    {
        if(m_rid != 0)
        {
            printf("m_rid: %d, ERROR: already in a room!\n", m_rid);
            return "";
        }

        int playerNum = std::stoi(pkg.substr(strlen("CREATEROOM") + 1));
        Room *new_room = RoomMgrSin::instance().CreateRoom(this, playerNum);
        m_rid = new_room->GetId();
        printf("after create room, m_rid: %d\n", m_rid);

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

        //BUG:  set rsp string
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

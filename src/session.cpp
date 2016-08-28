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
void Session::GameStart()
{
    m_game_socket.async_read_some(boost::asio::buffer(m_game_pkg, MAX_PKG_SIZE),
            boost::bind(&Session::GameHandleRead, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}

void Session::GameHandleRead(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        m_game_rsp = this->HandlePkg(m_game_pkg);

        boost::asio::async_write(m_game_socket,
                boost::asio::buffer(m_game_rsp, m_game_rsp.length()),
                boost::bind(&Session::GameHandleWrite, this,
                    boost::asio::placeholders::error));
    }
    else
    {
        delete this;
    }
}

void Session::GameHandleWrite(const boost::system::error_code& error)
{
    if (!error)
    {
        m_game_socket.async_read_some(boost::asio::buffer(m_game_pkg, MAX_PKG_SIZE),
                boost::bind(&Session::GameHandleRead, this,
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
        std::cout<<"I have received pkg: " <<pkg <<std::endl;
        int roomId = std::stoi(pkg.substr(strlen("STARTGAME") + 1));
        printf("start game %d\n", roomId);
        Game *new_game = RoomMgrSin::instance().StartGame(roomId);
        if(new_game == NULL)
        {
            printf("ERROR: start game error!\n");
        }

        //BUG:  set rsp string
        return "";
    }
    else if(pkg.find("UPDATEGAME") != std::string::npos)
    {
        //use fsp
        //this->HandleActionPkg(pkg);

        //use ssp
        this->SspHandleStatePkg(pkg);
    }

    return "";
}

//flushcache()
void Session::SendFrameCacheToClient(FrameMgr *frame_mgr)
{
    if(frame_mgr == NULL)
    {
        return;
    }

    //BUG:
}

/*********************Ssp related*********************/
void Session::SspHandleStatePkg(std::string pkg)
{
    //BUG: now a pkg is one state, may be changed
    m_state_cache.push(pkg);
}

int Session::PopState(std::string& state)
{
    if(m_state_cache.empty())
    {
        cout<<"TRACE: empty state cache in PopState()" <<endl;
        return -1;
    }
    
    //use the first one in the ring queue to avoid write conflict
    state = m_state_cache.front(); 
    m_state_cache.pop();
    return 0;
}

void Session::SendStateCacheToClient(FrameMgr *frame_mgr)
{
    if(frame_mgr == NULL)
    {
        return;
    }

    std::string s = frame_mgr->GetState(); 
    //BUG:
    boost::asio::async_write(m_game_socket, boost::asio::buffer(s, s.length()),
                                           boost::bind(&Session::SendStateHandleWrite, this,
                                                        boost::asio::placeholders::error));
}

void Session::SendStateHandleWrite(const boost::system::error_code& error)
{
    if(error)
    {
        cout<<"ERROR: SendStateCacheToClient error! "  << error.message() <<endl;
        return;
    }
    else
    {
        //do nothing
    }
}

/*********************Fsp related*********************/
void Session::FspHandleActionPkg(std::string pkg)
{
    //BUG, there may be more than one action in one frame, so a for loop is needed
    Action new_action;
    //BUG
    int role_idx = 0;
    if(!m_action_cache.empty())
    {
        const Action& last = m_action_cache.back(); 
        if(last.vkey == new_action.vkey && last.arg == new_action.arg)
        {
            cout<<"TRACE: drop action in frame_id: "  <<new_action.client_frame_id <<endl;
        }
    }

    int ret = m_action_cache.push(Action{new_action.vkey, 
                                         new_action.arg,
                                         role_idx,
                                         new_action.client_frame_id});
    if(ret != 0)
    {
        cout<<"WARN: push action failed!" <<endl; 
    }

    return;
}



int Session::PopAction(Action& action)
{
    if(m_action_cache.empty())
    {
        return -1; 
    }
    else
    {
        if(m_action_cache.size() > m_max_cache_size)
        {
            m_max_cache_size = m_action_cache.size(); 
        }

        const Action& a = m_action_cache.front();
        action = a;
        m_action_cache.pop();
        return 0;
    }
}



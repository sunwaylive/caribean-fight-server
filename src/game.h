#ifndef _GAME_H_
#define _GAME_H_

#include "common_def.h"
#include "frame.h"
#include "frame_mgr.h"
#include "session.h"

class Session;
class Frame;

class Game
{
public:
    Game()
    {}

    Game(unsigned int rid, vector<Session*> *player_list)
        : m_rid(rid), m_player_list(player_list)
    {}

    ~Game() {}

    /************Synchronize to client*************/
    int FrameTick();

    //Ssp
    int SspCalculateFrame();
    void SendStateNtfToAll();

    //Fsp
    int FspCalculateFrame();
    int AddActionToCurFrame(Action action);
    void SendFrameNtfToAll();

    void SendStartGameNtfToAll();
    void HandleWrite(const boost::system::error_code& error);
    void HandleRead(const boost::system::error_code& error, size_t bytes_transferred);

    vector<Session*>* GetPlayerList() const { return m_player_list; }

    unsigned int GetRoomId() const { return m_rid; }
    void SetRoomId(unsigned int id) { m_rid = id; }

    unsigned int GetGameId() const { return m_gid; }
    void SetGameId(unsigned int id) { m_gid = id; }

private:
    //game comes from room
    unsigned int m_rid;
    unsigned int m_gid;

    //point to the player list of the room
    vector<Session*> *m_player_list;

    //Fsp related
    Frame m_cur_frame;

    //Ssp related
    std::string m_cur_state;

    //Frame mgr 在这里起了 统一管理客户端 状态/操作 的作用, 是session和game的中间层
    FrameMgr m_frame_mgr;
};

#endif

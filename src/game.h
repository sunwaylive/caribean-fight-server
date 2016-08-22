#ifndef _GAME_H_
#define _GAME_H_

#include "frame.h"
#include "session.h"

class Session;
class Frame;

class Game
{
public:
    Game() { }

    Game(unsigned int rid, vector<Session*> *player_list)
        : m_rid(rid), m_player_list(player_list)
    {}

    ~Game() {}

    int AddActionToCurFrame();
    void SendStartGameNtfToAll();

    void HandleWrite(const boost::system::error_code& error);

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

    Frame m_cur_frame;
};

#endif

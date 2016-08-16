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

    Game(unsigned rid, vector<Session*> *player_list)
        : m_rid(rid), m_player_list(player_list)
    {}

    ~Game() {}

    int AddActionToCurFrame();
    void SendStartGameNtfToAll();
    vector<Session*>* GetPlayerList() const { return m_player_list; }

private:
    //game comes from room
    unsigned m_rid;

    //point to the player list of the room
    vector<Session*> *m_player_list;

    Frame m_cur_frame;
};

#endif

#ifndef _ROOM_H_
#define _ROOM_H_

#include "session.h"
#include <cstdio>

class Session;

class Room
{
public:
    Room(Session *creator, int max_player_num)
        : m_max_player_num(max_player_num)
    {
        m_player_list.push_back(creator);
    }

    void SetId(unsigned id) { m_rid = id; }
    unsigned GetId() const { return m_rid; }

    unsigned GetMaxPlayerNum() const { return m_max_player_num; }
    unsigned GetCurPlayerNum() const { return m_player_list.size(); }

    int AddPlayer(Session *player);

private:
    unsigned m_rid;
    unsigned m_max_player_num;
    vector<Session*> m_player_list;
};

#endif

#ifndef _ROOM_H_
#define _ROOM_H_

#include "session.h"
#include <cstdio>

class Session;

class Room
{
public:
    Room(Session *creator, int max_player_num)
        : m_rid(0), 
          m_max_player_num(max_player_num),
          m_is_fighting(false)
    {
        m_player_list.push_back(creator);
    }

    void SetId(unsigned id) { m_rid = id; }
    unsigned GetId() const { return m_rid; }

    unsigned GetMaxPlayerNum() const { return m_max_player_num; }
    unsigned GetCurPlayerNum() const { return m_player_list.size(); }
    vector<Session*>* GetPlayerList() { return &m_player_list; }

    int AddPlayer(Session *player);

    bool IsFighting() const { return m_is_fighting; }
    void SetFightingState(bool is_fighting) { m_is_fighting = is_fighting; }

private:
    unsigned m_rid;
    unsigned m_max_player_num;
    vector<Session*> m_player_list;
    bool m_is_fighting;
};

#endif

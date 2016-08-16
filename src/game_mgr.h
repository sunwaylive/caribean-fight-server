#ifndef _GAME_MGR_H_
#define _GAME_MGR_H_

#include "room.h"
#include "game.h"
#include "util/singleton.h"

class Room;
class Game;

class GameMgr
{
public:
    Game* GetGame(unsigned gid) const;
    Game* CreateGame(Room *r); //game must be created in a room

    size_t Size() const { return m_game_map.size(); }

private:
    unordered_map<unsigned, Game*> m_game_map;
    static unsigned m_g_game_id;
};

typedef singleton_default<GameMgr> GameMgrSin;

#endif

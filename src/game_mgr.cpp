#include "game_mgr.h"

unsigned GameMgr::m_g_game_id = 1;

Game* GameMgr::GetGame(unsigned gid) const
{
    Game *ret = NULL;
    auto iter = m_game_map.find(gid);
    if(iter != m_game_map.end())
    {
        ret = iter->second;
    }

    return ret;
}

Game* GameMgr::CreateGame(Room *r)
{
    if(r == NULL || r->IsFighting())
    {
        return NULL;
    }

    r->SetFightingState(true);
    Game *new_game = new Game(r->GetId(), r->GetPlayerList());
    m_game_map.insert({m_g_game_id++, new_game});

    //game start
    new_game->SendStartGameNtfToAll();

    return new_game;
}

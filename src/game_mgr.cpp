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
    new_game->SetGameId(m_g_game_id++);
    printf("TRACE: New game %d started!\n", new_game->GetGameId());

    m_game_map.insert({new_game->GetGameId(), new_game});

    //game start
    new_game->SendStartGameNtfToAll();

    return new_game;
}

void GameMgr::Tick()
{
    for (auto iter = m_game_map.begin(); iter != m_game_map.end(); ++iter)
    {
        cout << "room: "<< iter->first << " frame tick" << endl;
        iter->second->FrameTick();
    }
}

#include "room.h"

//-1 failed, Not -1 success
int Room::AddPlayer(Session *player)
{
    auto iter = m_player_list.begin();
    for(; iter != m_player_list.end(); ++iter)
    {
        if(*iter == player)
        {
            break;
        }
    }

    if(iter != m_player_list.end())
    {
        printf("ERROR: player already in the room.\n");
        return -1;
    }

    if(m_player_list.size() >= m_max_player_num)
    {

        printf("ERROR: player already in the room.\n");
        return -2;
    }

    m_player_list.push_back(player);
    return m_player_list.size();
}

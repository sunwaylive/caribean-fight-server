#include "room_mgr.h"

uint32_t RoomMgr::g_room_id = 0;

Room* RoomMgr::CreateRoom(Session *creator, unsigned max_player_num)
{
    std::cout<<"**************create room*************************" <<std::endl;
    std::cout<<"create room for " << max_player_num<<" players!" <<std::endl;

    Room *new_room = new Room(creator, max_player_num);
    new_room->SetId(g_room_id++);

    m_room_map.insert({new_room->GetId(), new_room});

    std::cout<<"***************************************" <<std::endl;
    return new_room;
}

Room* RoomMgr::JoinRoom(Session *player, unsigned rid)
{
    std::cout<<"************join room***************************" <<std::endl;
    Room *r = NULL;
    auto iter = m_room_map.find(rid);
    if(iter != m_room_map.end())
    {
        r = iter->second;
        if(r != NULL)
        {
            r->AddPlayer(player);
        }
    }
    else
    {
        printf("ERROR: can not find room id: %d\n", rid);
    }

    std::cout<<"***************************************" <<std::endl;
    return r;
}


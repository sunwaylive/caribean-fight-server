#ifndef _ROOM_MGR_H_
#define _ROOM_MGR_H_

#include <unordered_map>
#include "util/singleton.h"
#include "room.h"
#include "session.h"

class Room;
class Session;

class RoomMgr
{
public:
    Room* CreateRoom(Session *creator, unsigned player_num);
    Room* JoinRoom(Session *player, unsigned rid);

   const unordered_map<unsigned, Room*>* GetAllRoomMap() const { return &m_room_map; }

private:
    unordered_map<unsigned, Room*> m_room_map;
    static uint32_t g_room_id;
};

//declare singleton
typedef singleton_default<RoomMgr> RoomMgrSin;

#endif

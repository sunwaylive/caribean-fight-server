#ifndef _SESSION_H_
#define _SESSION_H_

#include <stddef.h>
#include <stdint.h>
#include <string>
#include "action.h"
#include "ring_queue.h"
#include "udp_pkg_def.h"
#include "common_def.h"
#include "room.h"
#include "room_mgr.h"

class Frame;
class FrameMgr;

typedef struct SeqInfo
{
    SeqInfo() : seq_id(0), max_frame_id(0), total_frame_store(0), used(0)
    {}

    SeqInfo(int seq_id, int max_frame_id, int total_frame_store, int used)
            : seq_id(seq_id), max_frame_id(max_frame_id), total_frame_store(total_frame_store), used(used)
    {}

    int seq_id;
    int max_frame_id;
    int total_frame_store;
    size_t used;
    int id() const { return seq_id; }
}SeqInfo;

/*Every TCP conn is a session, which stand for a player*/
class Session
{
public:
    Session(SocketPtr sock_ptr) : m_sock_ptr(sock_ptr){}

    SocketPtr GetSocketPtr() const { return m_sock_ptr; }
    string HandlePkg(std::string pkg);

    /******************/
    ~Session()
    {
        Reset();
    }

    void SetId(std::string id) { m_sid = id; }
    std::string GetId() const { return m_sid; }

    //清楚session的状态，用于new round
    void Clear();

    //释放时及时重置
    void Reset();

public:
    //发送缓存的数据
    void FlushCache(FrameMgr *frame_mgr);

    //操作缓存的action, 成功返回0, 非0表示弹出失败，可能action队列空了
    int PopAction(Action& action);
    size_t GetRemainActionCnt() { return m_action_cache.size(); }

private:
    std::string m_sid; //session id
    unsigned m_rid;    //room id
    unsigned m_gid;    //game id

    SocketPtr m_sock_ptr;
    char m_pkg[kMaxPkgSize];

    RingQueue<Action, 100> m_action_cache; //100 must be enough for a normal player
    /******************/
    int m_client_seq;
    int m_client_ack;
    int m_server_seq; //每次调用Send()借口发送时，都会是server_seq加1
    int m_sock_fd;
    char m_key[kMaxKeySize + 1]; //用于加密的key

    SeqInfo m_ack_info; //已经被确认的最新(大)的发送序列信息

    int m_use_check_sum; //是否开启校验报的checksum, 如果开启，最后两个字节为校验和
};

#endif

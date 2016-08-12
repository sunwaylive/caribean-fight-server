#ifndef _FRAME_H_
#define _FRAME_H_

#include "action.h"
#include "common_def.h"
#include <arpa/inet.h>

class Frame
{
public:
    Frame(int f) : m_frame_id(f), m_action_count(0), m_is_need_send(false) {}
    Frame() : m_frame_id(0), m_action_count(0), m_is_need_send(false) {}

    Frame& operator++()
    {
        ++m_frame_id;
        m_action_count = 0; //增加帧时，情况上一帧的action
        return *this;
    }

    Frame& operator++(int)
    {
        ++m_frame_id;
        m_action_count = 0;
        return *this;
    }

public:
    void SetFrameId(int fid) { m_frame_id = fid; }
    int GetFrameId() const { return m_frame_id; }

    //action为玩家在此帧内的所有操作
    int AddAction(unsigned vkey, unsigned arg, unsigned role_idx);
    const Action& GetAction(size_t idx) const { return m_action_list[idx]; }

    //帧内action的数目
    inline size_t GetActionCnt() const { return m_action_count; }

    //是否是空帧
    inline bool IsEmpty() const { return !m_is_need_send && m_action_count == 0; }

    //是否满了
    inline bool IsFull() const {return m_action_count >= kMaxActionNumInOneFrame; }

    //设置无论是否空帧都下发
    void SetNeedSend() { m_is_need_send = true; }

    //返回Frame序列化后的字节数
    size_t ByteSize() const;

    //将此帧序列化到字节流中去
    bool SerializeToArray(char *array, size_t& len) const ;

    //清楚该帧， reset
    inline void Clear() { m_frame_id = 0; m_action_count = 0; m_is_need_send = false; }

private:
    int m_frame_id;
    int m_action_count; //需要计数，因为action本身不判断是否为空
    bool m_is_need_send; //有些情况下需要强制下发，即使是空帧
    Action m_action_list[kMaxActionNumInOneFrame]; //认为一帧内玩家的操作数量是可预期的
};

#endif

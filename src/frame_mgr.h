#ifndef _FRAME_MGR_H_
#define _FRAME_MGR_H_
#include "common_def.h"
#include <stddef.h>
#include <string.h>

class Frame;

class FrameMgr
{
public:
    FrameMgr() : m_used(0),
                 m_max_frame_id(0),
                 m_total_frame_store(0),
                 m_is_prev_frame_empty(false),
                 m_last_frame_pos(0),
                 m_frame_segment_cnt(0)
    {}

    void SetGameId(int game_id) { m_game_id = game_id; }
    
    /**************Ssp********************/
    int AddState(std::string state);
    std::string GetState();
    void ClearState();

    /**************Fsp********************/
    //将新收集完毕的一帧加进来
    int AddFrame(const Frame *frame);

    //从used之后的帧是否为空
    bool IsNextFrameEmpty(int max_frame_id, size_t used) const;

    //获取总共存储的帧数
    int GetTotalFrameCnt() const { return m_total_frame_store; }

    int GetMaxFrameId() const { return m_max_frame_id; }

    size_t GetUsed() const { return m_used; }

    const char* GetFrameBufBegin(int max_frame_id, size_t used) const;
    const char* GetFrameBufEnd(const char *buf) const;

    //获取分段后，某一位置它的 frame count, 用于计算下发的帧的实际长度
    size_t GetFrameCntOfSegment(const char *buf) const;

    size_t GetUsedOffset(const char *buf) const;

    const char* GetFrameBuf() const { return m_frame_buf; }
    size_t GetFrameBufLen() const { return m_last_frame_pos; }
    size_t GetFrameBufUsed() const { return m_used; }

    //清楚所有
    void Clear();

    //输出当前的使用状态
    void Dump(int round_num, int game_id) const;

private:
    //获得剩余空间
    inline size_t GetFreeSize() const { return kMaxFrameBufInGame - m_used; }

private:
    int m_game_id; //记录这个用于定位一些问题

    /**************Ssp********************/
    std::string m_state_buf; //buffer for saving all client states

    /**************Fsp********************/
    char m_frame_buf[kMaxFrameBufInGame]; //存储了所有帧的信息，对空帧有压缩存储
    size_t m_used; //指向已经使用的下一个
    int m_max_frame_id;//当前的最大帧号, bug： 啥意思？
    int m_total_frame_store; //缓存着累积帧数量，N压缩为1只记作1

    bool m_is_prev_frame_empty; //上一帧是否为空
    size_t m_last_frame_pos; //最后一帧的偏移位置

    //分段信息，记录了frame_buf每一段的偏移值
    int m_frame_segment_arr[kMaxSegmentSize + 1];
    int m_frame_cnt_of_frame_segment_arr[kMaxSegmentSize + 1]; //用于记录某一个段中,实际存储的帧的数量(注意不是帧号)
    int m_frame_segment_cnt;
};

#endif

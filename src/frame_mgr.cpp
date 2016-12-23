#include "frame_mgr.h"
#include "frame.h"


/**************Ssp********************/
int FrameMgr::AddState(std::string state)
{
    if (state.length() > 0)
    {
        m_state_buf += state + "$";
        return 0;
    }

    return -1;
}

std::string FrameMgr::GetState()
{
    return m_state_buf;
}

void FrameMgr::ClearState()
{
    m_state_buf.clear();
} 

/**************Fsp********************/
int FrameMgr::AddFrame(const Frame *frame)
{
    size_t free_len = GetFreeSize();
    size_t frame_size = frame->ByteSize();

    if(m_used + frame_size >= kMaxFrameBufInGame)
    {
        return -1;
    }

    //如果上一帧和本帧都是空的，则进行合并
    if(m_is_prev_frame_empty && frame->IsEmpty())
    {
        char *last_frame = m_frame_buf + m_last_frame_pos;
        //uint16_t last_frame_id = ntohs(*(uint16_t*)(last_frame));

        uint16_t *new_frame_id = (uint16_t*) last_frame;
        *new_frame_id = htons(frame->GetFrameId());
    }
    else
    {
        //如果一帧内容过多，直接丢弃
        if(frame_size >= kUdpSegmentSize)
        {
            return -1;
        }

        //更新分段信息
        int last_frame_segment_use = m_frame_segment_cnt > 0 ? m_frame_segment_arr[m_frame_segment_cnt - 1] : 0;

        if((m_used + frame_size - last_frame_segment_use) > kUdpSegmentSize)
        {
            m_frame_segment_arr[m_frame_segment_cnt] = m_used;
            m_frame_cnt_of_frame_segment_arr[m_frame_segment_cnt] = m_total_frame_store; //用于记录某一段中它的实际帧数
            ++m_frame_segment_cnt;
        }
        else
        {
            return -1;
        }
    }

    //free_len在SerializeToArray函数中被修改，作返回值用
    if(frame->SerializeToArray(m_frame_buf + m_used, free_len))
    {
        ++m_total_frame_store;
        m_last_frame_pos = m_used;
        m_used += free_len;
    }
    else
    {
        return -1;
    }

    //记录下此帧的状态
    m_is_prev_frame_empty = frame->IsEmpty();

    //处理成功后，更新最大帧号
    m_max_frame_id = frame->GetFrameId();

    return 0;
}

bool FrameMgr::IsNextFrameEmpty(int max_frame_id, size_t used) const
{
    return used == m_used;
}

const char* FrameMgr::GetFrameBufBegin(int max_frame_id, size_t used) const
{
    if(used < m_used)
    {
        return m_frame_buf + used;
    }
    else if(used == m_used) //说明是一个空帧
    {
        if(max_frame_id == m_max_frame_id)
        {
            return NULL;
        }
        else
        {
            return m_frame_buf + m_last_frame_pos;
        }
    }
    else
    {
        return NULL;
    }
}

const char* FrameMgr::GetFrameBufEnd(const char* buf) const
{
    size_t total_remain = (m_frame_buf + m_used) - buf;
    if(total_remain <= kUdpSegmentSize)
    {
        return m_frame_buf + m_used;
    }
    else
    {
        size_t cur_use = GetUsedOffset(buf);
        for(int i = 0; i < m_frame_segment_cnt && i < (int)kMaxSegmentSize; ++i)
        {
            if(m_frame_segment_arr[i] > (int)cur_use)
            {
                return m_frame_buf + m_frame_segment_arr[i];
            }
        }
    }

    return buf;
}

size_t FrameMgr::GetFrameCntOfSegment(const char* buffer) const
{
    size_t total_remain = (m_frame_buf + m_used) - buffer;
    if(total_remain <= kUdpSegmentSize)
    {
        return m_total_frame_store;
    }
    else
    {
        size_t cur_use = GetUsedOffset(buffer);
        for(int i = 0; i < m_frame_segment_cnt && i < (int)kMaxSegmentSize; ++i)
        {
            if(m_frame_segment_arr[i] > (int)cur_use)
            {
                return m_frame_cnt_of_frame_segment_arr[i];
            }
        }
    }

    return 0;
}

//指针相减获得两个地址间元素的元素数目?, BUG
size_t FrameMgr::GetUsedOffset(const char *buffer) const
{
    return buffer - m_frame_buf;
}

void FrameMgr::Clear()
{
    memset(m_frame_buf, 0, sizeof(m_frame_buf));
    m_used = 0;
    m_max_frame_id = 0;
    m_total_frame_store = 0;
    m_is_prev_frame_empty = false;
    m_last_frame_pos = 0;

    memset(m_frame_segment_arr, 0, sizeof(m_frame_segment_arr));
    m_frame_segment_cnt = 0;
}

void FrameMgr::Dump(int round_num, int game_id) const
{
}

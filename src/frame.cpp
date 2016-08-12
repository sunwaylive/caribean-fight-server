#include "frame.h"
#include "udp_pkg_def.h"

int Frame::AddAction(unsigned vkey, unsigned arg, unsigned role_idx)
{
    if(m_action_count < kMaxActionNumInOneFrame)
    {
        m_action_list[m_action_count] = Action{vkey, arg, role_idx};
        ++m_action_count;
        return 0;
    }

    return -1;
}

//获取Frame序列化后的字节数
size_t Frame::ByteSize() const
{
    size_t frame_head = offsetof(SendFrame, actions);
    size_t frame_body = sizeof(SendAction) * m_action_count;
    return frame_head + frame_body;
}

bool Frame::SerializeToArray(char *bytes, size_t& len) const
{
    size_t need_len = this->ByteSize();
    if(len < need_len)
    {
        printf("ERROR: frame_id: %d, len: %d, need_len: %d", m_frame_id, len, need_len);
        return false;
    }

    SendFrame *sf = (SendFrame*)bytes;
    sf->frame_id = htons(m_frame_id);
    sf->action_len = m_action_count;

    for(int i = 0; i < m_action_count && i < kMaxActionNumInOneFrame; ++i)
    {
        const Action& a = m_action_list[i];

        SendAction *sa = &(sf->actions[i]);
        sa->vkey = htons(a.vkey);
        sa->arg = htons(a.arg);
        sa->role_idx = a.role_idx;
    }

    //修改len长度，表示此帧使用的字节长度
    len = need_len;

    return true;
}



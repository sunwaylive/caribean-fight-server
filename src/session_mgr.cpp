#include "session_mgr.h"

Session* SessionMgr::GetSession(std::string sid) const
{
    auto iter = m_session_map.find(sid);
    if(iter != m_session_map.end())
    {
        return iter->second;
    }
    else
    {
        return NULL;
    }
}

Session* SessionMgr::AddSession(std::string sid, SocketPtr sock_ptr)
{
    auto iter = m_session_map.find(sid);
    if(iter != m_session_map.end())
    {
        std::cout<<"ERROR: session already exist!" <<std::endl;
        return iter->second;
    }
    else
    {
        //create a new session, and add it to map
        std::cout<<"TRACE: a new session added." <<std::endl;
        Session *s = new Session(sock_ptr);
        s->SetId(sid);

        m_session_map.insert({sid, s});
        return s;
    }
}

void SessionMgr::DelSession(std::string sid)
{
    m_session_map.erase(sid);
}

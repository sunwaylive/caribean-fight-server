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

Session* SessionMgr::AddSession(Session *sess)
{
    if(sess == NULL)
    {
        return NULL;
    }

    std::string sid = sess->GetId();
    cout<<"TRACE: SessionMgr::AddSession sid: " <<sid <<endl;

    auto iter = m_session_map.find(sid);
    if(iter != m_session_map.end())
    {
        std::cout<<"ERROR: session already exist!" <<std::endl;
        return iter->second;
    }

    m_session_map.insert({sid, sess});
    std::cout<<"TRACE: a new session added!" <<std::endl;
    return sess;
}

void SessionMgr::DelSession(std::string sid)
{
    m_session_map.erase(sid);
}

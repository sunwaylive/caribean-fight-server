#ifndef _SESSION_MGR_H_
#define _SESSION_MGR_H_

#include <string>
#include "common_def.h"
#include "session.h"
#include "util/singleton.h"

//use boost shared ptr to wrap socket
using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> SocketPtr;

class SessionMgr
{
public:
    Session* GetSession(std::string sid) const;
    Session* AddSession(Session *sess);
    void DelSession(std::string sid);

    size_t Size() const { return m_session_map.size(); }

private:
    std::unordered_map<std::string, Session*> m_session_map;
};

//declare singleton
typedef singleton_default<SessionMgr> SessionMgrSin;

#endif

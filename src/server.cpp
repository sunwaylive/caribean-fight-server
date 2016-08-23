#include "server.h"

void Server::HandleAccept(Session* new_session, const boost::system::error_code& error)
{
    if (!error)
    {
        if(new_session != NULL)
        {
            new_session->SetId(new_session->Socket().remote_endpoint().address().to_string());
            SessionMgrSin::instance().AddSession(new_session);

            new_session->Start();
        }

        Session *new_session_2 = new Session(m_io_service);
        m_acceptor.async_accept(new_session_2->Socket(),
                boost::bind(&Server::HandleAccept, this, new_session_2,
                    boost::asio::placeholders::error));
    }
    else
    {
        delete new_session;
    }
}

void Server::FspHandleAccept(Session *new_session, const boost::system::error_code &error)
{
    if(!error)
    {
    }
    else
    {
        printf("ERROR: Fsp Socket establish failed!\n");
        return;
    }
}

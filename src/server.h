#ifndef _SERVER_H_
#define _SERVER_H_

#include <utility>
#include <iostream>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "session_mgr.h"
#include "session.h"

using boost::asio::ip::tcp;
//use boost shared ptr to wrap socket
typedef boost::shared_ptr<tcp::socket> SocketPtr;

class Server
{
public:
    Server(boost::asio::io_service& io_service, short port, short fsp_port)
        : m_io_service(io_service),
          m_acceptor(io_service, tcp::endpoint(tcp::v4(), port))
          //m_fsp_acceptor(io_service, tcp::endpoint(tcp::v4(), fsp_port))
    {
        Session* new_session = new Session(m_io_service);

        m_acceptor.async_accept(new_session->Socket(),
                                boost::bind(&Server::HandleAccept, this, new_session,
                                            boost::asio::placeholders::error));
        cout<<fsp_port <<endl;
        //m_fsp_acceptor.async_accept(new_session->FspSocket(),
        //                            boost::bind(&Server::FspHandleAccept, this, new_session,
        //                                        boost::asio::placeholders::error));
    }

    void HandleAccept(Session* new_session, const boost::system::error_code& error);
    void FspHandleAccept(Session* new_session, const boost::system::error_code& error);

private:
    boost::asio::io_service& m_io_service;
    tcp::acceptor m_acceptor;
    //tcp::acceptor m_fsp_acceptor;
};

#endif

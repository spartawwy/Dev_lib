#include "tsystem_tcp_dock.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <iostream>

#include "tcp_session.h"

#include "TLib/core/tsystem_connection.h"
#include "TLib/core/tsystem_communication_dock.h"

#ifdef WIN32
#pragma comment(lib,"Ws2_32.lib")
#endif

typedef boost::asio::ip::tcp  TCP;

using namespace TSystem;
using namespace TSystem::communication;

TcpDock::TcpDock(CommunicationDock& dock,const TaskPool& pool)
    : communication_dock_(dock)
    , task_pool_(pool)
    , strand_(pool)
    //, strand_write_(pool)
    , socket_(new tcp::socket(pool.Pool().service())) 
    , p_io_service_(&pool.Pool().service())
    , acceptor_(nullptr) 
    , temp_tcp_connections_()
    , temp_connections_lock_()
    //, work_(new WorkPtr(&pool.Pool().service())
{
     
}

TcpDock::~TcpDock()
{

}

void TcpDock::StartPornt(unsigned short port, ConnectingHandler&& handler)
{
    if( ! acceptor_ )
    {
        connect_hander_ = std::move(handler);
        acceptor_ = new TCP::acceptor( *p_io_service_, TCP::endpoint(TCP::v4(), port) );
    #if 1
	    auto tcp_session = std::make_shared<TcpSession>(*this);
	    std::shared_ptr<Connection> new_session = tcp_session;
        (static_cast<TCP::acceptor*>(acceptor_))->async_accept(*tcp_session->socket(),
                boost::bind(&TcpDock::handle_accept, this, new_session, boost::asio::placeholders::error));
    #endif
    }
}

void TcpDock::AddTempConnection(std::shared_ptr<TcpSession>& conn)
{
    std::lock_guard<std::mutex>  locker(temp_connections_lock_);
    temp_tcp_connections_.push_back(std::move(conn));
}

void TcpDock::RemoveTempConnection(TcpSession* conn)
{
    std::lock_guard<std::mutex>  locker(temp_connections_lock_);
    auto iter = std::find_if( std::begin(temp_tcp_connections_), std::end(temp_tcp_connections_), [conn](std::shared_ptr<Connection> entry)
    {
        return entry.get() == conn;
    });
    if( iter != temp_tcp_connections_.end() )
        temp_tcp_connections_.erase(iter);
    /*std::remove_if(std::begin(temp_tcp_connections_), std::end(temp_tcp_connections_), [&conn](std::shared_ptr<Connection>& entry)
    {
        return entry.get() == conn.get();
    });*/
}

void TcpDock::Register(TcpSession* conn)
{
    std::lock_guard<std::mutex>  locker(temp_connections_lock_);
    auto iter = std::find_if( std::begin(temp_tcp_connections_), std::end(temp_tcp_connections_), [conn](std::shared_ptr<Connection> entry)
    {
        return entry.get() == conn;
    });
    assert(iter != temp_tcp_connections_.end() );

    communication_dock_.Register((*iter)->shared_this());
}

std::shared_ptr<Connection> TcpDock::UnRegister(TcpSession* conn)
{
    assert(conn);
    return communication_dock_.Unregister(conn->connid());
}

void TcpDock::handle_accept(std::shared_ptr<Connection>&  session, const boost::system::error_code& error)
{

    if (!error)
    {
        // std::cout <<"handle_accept:" << session->socket()->remote_endpoint().address().to_string() << std::endl;
        communication_dock_.Register(session);
		
        TSystem::TError er = MakeTError(CoreErrorCategory::ErrorCode::OK); 
        //session->status_ = Connection::Status::connected;// ndchk :
        connect_hander_(session, er);
 
        //session->TriggerRead();
 
        //session->Start();
    }else
    {
		session.reset();
        return;
    }

#if 1
    auto tcp_session = std::make_shared<TcpSession>(*this);
	std::shared_ptr<Connection> new_session = tcp_session;
    (static_cast<TCP::acceptor*>(acceptor_))->async_accept(*tcp_session->socket(),
        boost::bind(&TcpDock::handle_accept, this, new_session, boost::asio::placeholders::error));
#endif
}
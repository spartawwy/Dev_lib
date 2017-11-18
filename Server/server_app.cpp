//http://blog.chinaunix.net/uid-28163274-id-4984766.html

#include "server_app.h"

//#include <winsock.h>
//#include <windows.h>

static int count111 = 0;
static time_t oldtime = 0, nowtime = 0;


#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include "session.h"

#pragma comment(lib,"Ws2_32.lib")

typedef boost::asio::ip::tcp  TCP;

//static boost::asio::io_service  s_g_io_service;
 
ServerApp::ServerApp(const std::string& type, const std::string& name, const std::string& version) : AppBase(type, name, version)
    , io_svr_(new boost::asio::io_service)
    , work_(nullptr)
    //, session_(nullptr)
    , acceptor_(nullptr)
{
    port(6003);
    task_pool().PostTask([this]()
    {
        while(1)
            this->io_svr_->run();
    });

    work_ = std::unique_ptr<WorkPtr>( new WorkPtr(io_svr_.get()) );

    acceptor_ = new TCP::acceptor( *io_svr_, TCP::endpoint(TCP::v4(), port()) );
    Session* new_session = new Session(*io_svr_, *this);

    (static_cast<TCP::acceptor*>(acceptor_))->async_accept(new_session->socket(),
            boost::bind(&ServerApp::handle_accept, this, new_session, boost::asio::placeholders::error));
}

ServerApp::~ServerApp()
{
    if( acceptor_ )
        delete static_cast<TCP::acceptor*>(acceptor_);
}

void ServerApp::handle_accept(Session* session, const boost::system::error_code& error)
{
    if (!error)
    {
        std::cout <<"handle_accept:" << session->socket().remote_endpoint().address().to_string() << std::endl;
        session->Start();
    }
    else
    {
        delete session;
        return;
    }

    Session* new_session = new Session(*io_svr_, *this);
    (static_cast<TCP::acceptor*>(acceptor_))->async_accept(new_session->socket(),
        boost::bind(&ServerApp::handle_accept, this, new_session, boost::asio::placeholders::error));
}
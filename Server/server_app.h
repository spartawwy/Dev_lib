#ifndef SERVER_APP_H_
#define SERVER_APP_H_

#include <TLib/core/tsystem_appbase.h>

#include "work_ptr.h"

//using boost::asio::ip::tcp;
namespace boost
{
    namespace system
    {
        class error_code;
    }
}

class Session;  
//boost::asio::io_service  s_g_io_service;

class ServerApp : public TSystem::AppBase
{
public:

    ServerApp(const std::string& type="test", const std::string& name="Server1", const std::string& version="0.1");
    virtual ~ServerApp();

    void handle_accept(Session* new_session, const boost::system::error_code& error);

private:

    std::unique_ptr<boost::asio::io_service>  io_svr_;
    std::unique_ptr<WorkPtr> work_;

   // std::unique_ptr<Session>  session_;
    void* acceptor_;
};

#endif // SERVER_APP_H_
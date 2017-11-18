#ifndef  APP_H_
#define  APP_H_

#include <TLib/core/tsystem_message_system.h>
#include <TLib/core/tsystem_appbase.h>

#include <TLib/tool/tsystem_layinfo.h>

#include <TLib/core/tsystem_task_service.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

typedef boost::asio::ip::tcp  TCP;

class App : public TSystem::AppBase
{
public:
	App();
    ~App();

	void Start();

	int ProcessCmd(int argc, char* argv[]);

    void connect_accept(const boost::system::error_code& error, TCP::resolver::iterator endp_iter);

private:
	TSystem::layinfo::LayInfo lay_;

    TSystem::TaskStrand  strand_;

    std::unique_ptr<boost::asio::io_service::work> work_;
    TCP::socket  socket_;

    TSystem::MessageSystem  msg_system_;
};


#endif // APP_H_
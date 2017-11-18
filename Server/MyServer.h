#ifndef MYSERVER_H_
#define MYSERVER_H_
#include "TLib/tool/tsystem_server_appbase.h"

class MyServer : public TSystem::ServerAppBase
{
public:

    MyServer(const std::string& name, const std::string& version);
    virtual ~MyServer() override;

    void Initiate();

    virtual void Shutdown() override;

private:

    virtual void UpdateState();

	virtual void HandleNodeHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override;
	virtual void HandleNodeDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn, const TSystem::TError& te) override {}
	virtual void HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) override;
	virtual void HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn
		, const TSystem::TError& te) override;

    void TimerFunc(const TSystem::TError& te);

private:

    TSystem::TaskTimer  timer_;
};

#endif // MYSERVER_H_
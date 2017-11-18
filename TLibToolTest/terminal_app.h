#ifndef TERMINAL_APP_H_
#define TERMINAL_APP_H_

#include <TLib/tool/tsystem_server_client_appbase.h>

#include "TLib/core/tsystem_connection.h"

using namespace TSystem;

class TerminalApp : public TSystem::ServerClientAppBase
{
public:

    TerminalApp();
    virtual ~TerminalApp(){}

    bool Initiate();
    void Shutdown();

private:

    virtual void HandleNodeHandShake(communication::Connection* p, const Message& msg) override; 
	virtual void HandleNodeDisconnect(std::shared_ptr<communication::Connection>& pconn
			, const TError& te) override;

    TSystem::ClientSocket  client_socket_;
    TSystem::ClientSocket  msg_relay_socket_;
    // sync 
	//TSystem::MessageHandlerGroup                           msg_handlers_;

};

#endif // TERMINAL_APP_H_
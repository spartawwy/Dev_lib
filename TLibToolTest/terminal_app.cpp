#include "terminal_app.h"

#include "TLib/core/tsystem_serialization.h"
#include "TLib/core/tsystem_utility_functions.h"

#include "TLib/tool/tsystem_connection_handshake.pb.h"
#include "TLib/tool/tsystem_topic_client_socket.h"

#include "shared/wze_message_system.h"
#include "WZELib/wze_query_msg.h"

enum class SlashTopic : unsigned char
{
    HAPPY_TOPIC = 100,
};

using namespace TSystem;

TerminalApp::TerminalApp() : ServerClientAppBase("exch_front_svr", "front_server2", "0.1")
    , client_socket_(*this)
    , msg_relay_socket_(*this)
{

}
 
bool TerminalApp::Initiate()
{
     // load wze class id
	option_load_class_id(false);
	WZESystem::LoadClassID(msg_system_);
    //===================
    ServerClientAppBase::Initiate();
 
    //------------------
	// register handler  
	//------------------
    client_socket_.msg_handlers().RegisterHandler( "HandShake", [this](communication::Connection* pconn, const Message& msg)
	{
#if 1
		HandShake hs;
		if( Decode(msg, hs, this->msg_system_) )
		{
			local_logger().LogLocal( utility::FormatStr("receive handshake from connection: %d pid:%d "
                , pconn->connid(), hs.pid()) );
            ///pconn->hand_shaked(true);

			//if( monitor_flag_ )
			//	SubscribeMarketData(nullptr); // subscribe all products' market data information 
		}
#endif
	});

    msg_relay_socket_.on_handshake_handler( [this](communication::Connection* p, const Message& msg)
	{
		HandShake hs;
		if( Decode(msg, hs, msg_system_) )
		{
			// hand shake
			p->hand_shaked(true);
			const layinfo::AppResource& res = resource_manager_.FindUnique(hs.pid());
			if( !res )
				return;
			local_logger().LogLocal("message relay connected: " + res.name);
			//this->msg_relay_topic_.AsyncAddDestination(p->shared_this(), communication::TopicData::DataStartPos::LATEST);

			// subscribe FrontServerTopic
			TopicClientSocket socket(*this, p->shared_this());
            socket.Subscribe(static_cast<unsigned short>(SlashTopic::HAPPY_TOPIC), 0, communication::TopicData::DataStartPos::FIRST);
		}
	});

	msg_relay_socket_.on_disconnect_handler( [this](std::shared_ptr<communication::Connection>& pconn, const TError& te, const layinfo::AppResource& res)
	{
		local_logger().LogLocal("message relay disconnected: " + res.name);
		//this->msg_relay_topic_.AsyncRemoveDestination(pconn->connid());
	});

    msg_relay_socket_.msg_handlers().RegisterHandler("QueryRequest", [this](communication::Connection* pconn, const Message& msg)
    {
        QueryRequest query_request;
        if( Decode(msg, query_request, this->msg_system()) )
        {
            printf("request_id : %d \n", query_request.request_id() );
            //local_logger().LogLocal(  );
        }
    });

    msg_relay_socket_.AsyncConnect("msg_relay1", ClientSocket::ConnectingSetting(0, true));

    ConnectNode();
    return true;
}

void TerminalApp::Shutdown()
{
    ServerClientAppBase::Shutdown();
}

void TerminalApp::HandleNodeHandShake(communication::Connection* p, const Message& msg)
{
    HandShake hs;
	if( Decode(msg, hs, msg_system_) )
	{
		p->hand_shaked(true); 
        auto sd_p = p->shared_this();
		SetupNodeRelayConnection(sd_p);
 
#ifdef DERECT_CONNECT_NODERELAY
        //client_socket_.msg_handlers().LinkConnection(sd_p);

        TopicClientSocket  socket(*this, p->shared_this());
        //socket.Subscribe( static_cast<unsigned short>(SlashTopic::HAPPY_TOPIC), "1", 0, communication::TopicData::DataStartPos::LATEST, false );
        socket.Subscribe( static_cast<unsigned short>(SlashTopic::HAPPY_TOPIC), "1", 0, communication::TopicData::DataStartPos::FIRST, false );
#endif
	}
}

void TerminalApp::HandleNodeDisconnect(std::shared_ptr<communication::Connection>& pconn
			, const TError& te)
{

}
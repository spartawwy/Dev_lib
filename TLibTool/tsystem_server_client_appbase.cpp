#include "TLib/tool/tsystem_server_client_appbase.h"

#include "TLib/core/tsystem_core_common.h"
#include "TLib/core/tsystem_serialization.h"
#include "TLib/core/tsystem_time.h"
#include "TLib/tool/tsystem_connection_handshake.pb.h"
#include "TLib/tool/tsystem_request_help.h"
#include "TLib/tool/tsystem_request_ping.h"
#include "TLib/tool/tsystem_request_reload_configs.h"
#include "TLib/tool/tsystem_request_shutdown.h"
#include "TLib/tool/tsystem_layinfo.h"

#include "TLib/tool/tsystem_heartbeat_msg.pb.h"

using namespace TSystem;

ServerClientAppBase::ServerClientAppBase(const std::string& type, const std::string& name, const std::string& version)
	: AppBase(type, name, version)
	, resource_manager_()
	, msg_system_()
	, generic_request_kernel_(msg_system_)
	, comm_dock_(task_pool_)
	, node_relay_socket_(*this), node_relay_topic_(task_pool_)
	, init_option_(std::make_tuple(true, true)), config_mutex_()
{}

void ServerClientAppBase::Shutdown()
{
	comm_dock_.Shutdown();
	AppBase::Shutdown();
}

void ServerClientAppBase::Initiate()
{
	layinfo::LayInfo info_db(utility::DevTag());

	if( option_validate_app() )
	{
		resource_manager_ = info_db.LoadAppResourceManager();
		layinfo::RegisterApp(*this, resource_manager_);
	}

	AppBase::Initiate();
     
	if( option_load_class_id() )
    {
        auto load_class_id = [](TSystem::MessageSystem& system)
        {
            const char* ClassIDArray[] = 
	        {
		        "GenericRequest", 
		        "HandShake",
		        "Heartbeat",
		        "LogMessage",
                "TopicRequest",
	        };

	        int class_id = 0;
	        std::for_each( ClassIDArray, ClassIDArray+sizeof(ClassIDArray)/sizeof(const char*)
		        , [&system, &class_id](const char* name){system.RegisterClassID(name, ++class_id);});
        };
        load_class_id(msg_system_);
        //msg_system_ = info_db.LoadClassID();
    }
       
	//--------------------------------
	// basic generic request handlers
	//--------------------------------
	if( msg_system_.FindClassID("GenericRequest", std::nothrow) )
	{
		node_relay_socket_.msg_handlers().RegisterHandler("GenericRequest", [this](communication::Connection* p, const Message& msg)
		{
            try
            {
                this->generic_request_kernel_.HandleMessage(p, msg);
            }catch(const TException& te)
            {
                LogError( LogMessage::TRIVIAL, CoreErrorCategory::ErrorCode::BAD_CONTENT
                    , "ServerClientAppBase::Initiate"
                    , ErrorString(te.error())
                    , this->local_logger());
            }
        });

		generic_request_kernel_.RegisterHandler( std::unique_ptr<RequestHandler>(new RequestHelpHandler(*this, generic_request_kernel_)) );
		generic_request_kernel_.RegisterHandler( std::unique_ptr<RequestHandler>(new RequestPingHandler(*this)) );
		generic_request_kernel_.RegisterHandler( std::unique_ptr<RequestHandler>(new RequestShutdownHandler(*this)) );
		generic_request_kernel_.RegisterHandler( std::unique_ptr<RequestHandler>(new RequestReloadConfigsHandler(*this)) );
	}

	// add node_relay_topic as log_topic
	local_logger().SetRemote(&node_relay_topic_, pid(), msg_system_);
}

void ServerClientAppBase::ReloadConfigs()
{
	try
	{
		std::lock_guard<std::mutex> lock(config_mutex_);

		static auto min_reload_gap = std::chrono::seconds(5);

		if( GetClock().Now() - configs_.last_update() > min_reload_gap )
		{
			// reload
			LoadAppConfig(*this, configs_);

			// call any on fly part
			this->OnLoadConfigs();
		}else
		{
			LogError( LogMessage::VITAL
				, CoreErrorCategory::ErrorCode::BAD_ARGUMENT
				, "ServerClientAppBase::ReloadConfigs"
				, "reload configs too fast"
				, local_logger_);
		}

	}catch(const TException& te)
	{
		LogError( LogMessage::VITAL
			, te.error()
			, local_logger_);
	}
}

void ServerClientAppBase::ConnectNode()
{
	const layinfo::AppResource& res_node = resource_manager_.FindNodeRelay(node());
	if( !res_node )
		ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
		 , "ServerClientAppBase::Initiate"
		 , utility::FormatStr("no node relay found for pid: %d", pid()));

	// set node relay socket
	node_relay_socket_.on_handshake_handler([this](communication::Connection* p, const Message& msg)
	{ 
		this->local_logger().LogLocal( utility::FormatStr("receive handshake from node relay connection: %d", p->connid()) );
		this->HandleNodeHandShake(p, msg); 
	});

	node_relay_socket_.on_disconnect_handler(
		[this](std::shared_ptr<communication::Connection>& pconn, const TError& te
			, const layinfo::AppResource& res)
	{
		local_logger().LogLocal("node relay disconnected");
		this->HandleNodeDisconnect(pconn, te);
	});
    // temp -----------nddel-----------
    //node_relay_socket_.msg_handlers().RegisterHandler("Heartbeat", [this](communication::Connection* p, const Message& msg)
    //{
    //    //this->local_logger().LogLocal( utility::FormatStr("receive Heartbeat from node relay connection: %d", p->connid()) );
    //    printf( "receive Heartbeat from node relay connection:%d ", p->connid() );
    //    Heartbeat  h_b;
    //    if( Decode(msg, h_b, msg_system_) )
    //    {
    //        printf("pid: %u \n", h_b.pid() );
    //    }
    //});

    // end -----------------------

	// connect
	local_logger().LogLocal("connecting node relay: " + res_node.name);
	node_relay_socket_.AsyncConnect( res_node.name, ClientSocket::ConnectingSetting() );
}

void ServerClientAppBase::SetupNodeRelayConnection(std::shared_ptr<communication::Connection>& pconn)
{
	local_logger().LogLocal( utility::FormatStr("node relay connection connected: %d", pconn->connid()) );
	local_logger().LogLocal("remote logging to node relay");
	node_relay_topic_.AsyncAddDestination(pconn, communication::TopicData::DataStartPos::LATEST);
}

std::shared_ptr<communication::Connection> ServerClientAppBase::ConnectResource(const layinfo::AppResource& res, TSystem::TError& err)
{
	if( !res.port )
		ThrowTException(CoreErrorCategory::ErrorCode::BAD_ARGUMENT, "Connect", 
			"Resource is unconnectible: " + res.name);

	local_logger().LogLocal("sync-connecting resource: " + res.name);
	return comm_dock_.Connect(res.host + "." + res.domain, res.port, err, communication::Connection::Type::tcp);
}

void ServerClientAppBase::AsyncConnectResource(const layinfo::AppResource& res, communication::ConnectingHandler&& handler)
{
	if( !res.port )
		ThrowTException(CoreErrorCategory::ErrorCode::BAD_ARGUMENT, "AsyncConnectResource", 
			"Resource is unconnectible: " + res.name);

	local_logger().LogLocal("async-connecting resource: " + res.name);
	comm_dock_.AsyncConnect(res.host + "." + res.domain, res.port, std::move(handler), communication::Connection::Type::tcp);
}


void TSystem::SendHandShake(const ServerClientAppBase& app, std::shared_ptr<communication::Connection>& p)
{
	HandShake hand_shake;
	hand_shake.set_pid(app.pid());
	p->AsyncSend(Encode(hand_shake, app.msg_system(), Message::HeaderType(0, app.pid(), 0)));
}
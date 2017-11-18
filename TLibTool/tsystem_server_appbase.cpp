#include "TLib/tool/tsystem_server_appbase.h"

#include "TLib/core/tsystem_core_error.h"
#include "TLib/core/tsystem_return_code.h"
#include "TLib/core/tsystem_serialization.h"
#include "TLib/core/tsystem_utility_functions.h"

#include "TLib/tool/tsystem_connection_handshake.pb.h"
#include "TLib/tool/tsystem_topic_request.pb.h"


using namespace TSystem;
using namespace TSystem::communication;

ServerAppBase::ServerAppBase(const std::string& type, const std::string& name, const std::string& version)
	: ServerClientAppBase(type, name, version)
	, heart_beat_(task_pool_, msg_system_)
	, msg_handlers_(msg_system_)
	, topic_server_()
	, state_timer_(task_pool_)
	, init_option_(std::make_tuple(true))
{
    int i = 0;
    i = i;
}

void ServerAppBase::RollState()
{
	state_timer_.ExpirationDuration(std::chrono::milliseconds(5));
	state_timer_.PostTask([this](const TError& te)
	{ this->UpdateState(); });
}

void ServerAppBase::Shutdown()
{
	state_timer_.Cancel();
	heart_beat_.Shutdown();
	ServerClientAppBase::Shutdown();
}

void ServerAppBase::Initiate()
{
	ServerClientAppBase::Initiate();

#if 0 // tmp nouse it
	// start heart beat
	heart_beat_.UpdateStatus(Heartbeat::INITIATE, pid());
	heart_beat_.Start();
#endif
	// link topic server
	if( option_link_topic_server() )
	{
		msg_handlers_.RegisterHandler("TopicRequest", [this](communication::Connection* p, const Message& msg)
		{ this->HandleTopicRequest(p, msg); });
	}
  /*  msg_handlers_.RegisterHandler("HandShake", [this](communication::Connection* p, const Message& msg)
    { this->HandleInboundHandShake(p, msg); });*/
}

void ServerAppBase::SetupNodeRelayConnection(std::shared_ptr<communication::Connection>& pconn)
{
	ServerClientAppBase::SetupNodeRelayConnection(pconn);

	local_logger().LogLocal("start sending heart beat");
	heart_beat_.AddSubscriber(pconn);
}

void ServerAppBase::SetupInboundConnection(std::shared_ptr<communication::Connection>& pconn, const layinfo::AppResource& res)
{
	
	local_logger().LogLocal(utility::FormatStr("authorized connection %d, sending heat beat and handshake: %d(%s)"
		, pconn->connid(), res.pid, res.name.c_str()));

	SendHandShake(*this, pconn);
	heart_beat_.AddSubscriber(pconn);
}

void ServerAppBase::StartPort()
{
	assert( port() > 0 );
	
	// handle client handshake
	msg_handlers_.RegisterHandler("HandShake", [this](Connection* p, const Message& msg)
	{ 
		this->local_logger().LogLocal( utility::FormatStr("receive handshake from inbound connection: %d", p->connid()) );
		this->HandleInboundHandShake(p, msg); 
	});

	local_logger().LogLocal(utility::FormatStr("service port started: %d", port()));
	comm_dock_.StartPort(port()
		, [this](std::shared_ptr<communication::Connection>& pconn, const TError& te)
	{
		if( !te )
		{
			if( pconn )
			{
				local_logger().LogLocal(utility::FormatStr("accept inbound connection, waiting for client handshake: %d", pconn->connid()));

				// link connection to msg_group = setup on_msg_recv
				msg_handlers_.LinkConnection(pconn);

				// setup on_disconnect
				std::weak_ptr<communication::Connection> weak_pconn(pconn);
				pconn->event_handlers().on_disconnect = [weak_pconn, this](const TError& te)
				{
					std::shared_ptr<communication::Connection> p = weak_pconn.lock();
					if( p )
					{   
						local_logger().LogLocal(utility::FormatStr("disconnect inbound connection: %d", p->connid()));
						this->HandleInboundDisconnect(p, te);
					}else
						local_logger().LogLocal("disconnect inbound connection");
				};

				// start
				pconn->Start();
			}else
			{
				local_logger().LogLocal("service port closed");
			}
		}else
		{
			local_logger().LogLocal(ErrorString(te));
			local_logger().LogLocal("accept inbound connection fail");
		}
	});
}


void ServerAppBase::HandleTopicRequest(communication::Connection* p, const Message& msg)
{
	std::shared_ptr<Connection> pconn = p->shared_this();
	if( !pconn )
	{
		LogError( LogMessage::TRIVIAL, CoreErrorCategory::ErrorCode::BAD_IO, "TopicServer::HandleTopicRequest",
			"topic request from bad connection", local_logger());
		return ;
	}

	TopicRequest req;
	if( !Decode(msg, req, msg_system_) )	
	{
		// log as warning and send reqeust error
		LogError( LogMessage::TRIVIAL, CoreErrorCategory::ErrorCode::BAD_CONTENT, "TopicServer::HandleTopicRequest", 
					"corrupted topic request message", local_logger());

		RequestAck ack;
		FillRequestAck(static_cast<int>(CoreErrorCategory::ErrorCode::BAD_CONTENT), "corrupted topic request message", ack);
		pconn->AsyncSend(Encode(ack, msg_system_, Message::HeaderType(0, pid(), 0)));
		return ;
	}

	unsigned int   request_id = req.request_id();
	unsigned short topic_id = static_cast<unsigned short>(req.topic_id());
	Topic* p_topic = topic_server_.Find(topic_id);
	if( !p_topic )
	{
		// log as warning and send reqeust error
		LogError( LogMessage::TRIVIAL, CoreErrorCategory::ErrorCode::BAD_CONTENT, "TopicServer::HandleTopicRequest", 
					"Unknown topic", local_logger());

		RequestAck ack;
		FillRequestAck(request_id, static_cast<int>(CoreErrorCategory::ErrorCode::BAD_CONTENT), "unknown topic", ack);
		pconn->AsyncSend(Encode(ack, msg_system_, Message::HeaderType(0, pid(), 0)));
		return ;
	}

	TopicData::DataStartPos data_pos = TopicData::DataStartPos::LATEST;
	try
	{
		data_pos = communication::IntToDataStartPos(req.data_pos());
	}catch(const TException& e)
	{
		LogError( LogMessage::TRIVIAL, e.error(), local_logger());
		
		RequestAck ack;
		FillRequestAck(request_id, e.error().code(), e.error().other_info(), ack);
		pconn->AsyncSend(Encode(ack, msg_system_, Message::HeaderType(0, pid(), 0)));
		return ;
	}

	// lambda to send unknown key error
	auto send_unknown_key_error = [this](const std::shared_ptr<communication::Connection>& pconn
		, const std::string& key, unsigned short topic_id, unsigned int request_id)
	{
		std::string msg = utility::FormatStr("Unknown key %s for topic %d", key.c_str(), topic_id);
							
		LogError( LogMessage::TRIVIAL, CoreErrorCategory::ErrorCode::BAD_CONTENT, "TopicServer::HandleTopicRequest", 
			msg, local_logger());

		RequestAck ack;
		FillRequestAck(request_id, static_cast<int>(CoreErrorCategory::ErrorCode::BAD_CONTENT), msg, ack);
		pconn->AsyncSend(Encode(ack, msg_system_, Message::HeaderType(0, pid(), 0)));
	};

	if( req.all_available() )
	{
		switch( req.type() )
		{
		case TopicRequest::Subscribe:
			{
				local_logger().LogLocal(utility::FormatStr("add topic destination connid %d for: %d::All", pconn->connid(), topic_id));
				p_topic->AddDestinationForAll(pconn, data_pos); 
				break;
			}
		case TopicRequest::Request:
			{
				local_logger().LogLocal(utility::FormatStr("send topic once for: %d::All", topic_id));
				p_topic->SendOneForAll(pconn, data_pos); 

				// send finish msg
				RequestAck ack;
				FillRequestAck(request_id, ack);
				pconn->AsyncSend(Encode(ack, msg_system_, Message::HeaderType(0, pid(), 0)));
				break;
			}
		case TopicRequest::Unsubscribe:
			{
				local_logger().LogLocal(utility::FormatStr("add topic destination connid %d for: %d::All", pconn->connid(), topic_id));
				p_topic->RemoveDestinationFromAll(pconn->connid());
				break;
			}
		}
	}else
	{
		switch( req.type() )
		{
		case TopicRequest::Subscribe:
			{
				bool add_key = req.add_key();
				std::for_each( std::begin(req.keys()), std::end(req.keys())
					, [p_topic, &pconn, topic_id, request_id, add_key, data_pos, &send_unknown_key_error, this](const std::string& key)
					{
						TopicData* pdata = p_topic->Find(key);
						if( pdata )
						{
							this->local_logger().LogLocal(utility::FormatStr("add topic destination connid %d for: %d::%s", pconn->connid(), topic_id, key.c_str()));
							pdata->AsyncAddDestination(pconn, data_pos); 
						}
						else if( add_key && p_topic->allow_add_key() )
						{
							this->local_logger().LogLocal(utility::FormatStr("add topic and topic destination connid %d for: %d::%s", pconn->connid(), topic_id, key.c_str()));
							p_topic->AddTopicData(key, this->task_pool()).AsyncAddDestination(pconn, data_pos); 	
						}else// log as warning and send reqeust error
						{
							send_unknown_key_error(pconn, key, topic_id, request_id);
						}
					});
				break;
			}
		case TopicRequest::Request:
			{
				std::for_each( std::begin(req.keys()), std::end(req.keys())
					, [p_topic, &pconn, topic_id, request_id, data_pos, &send_unknown_key_error, this](const std::string& key)
					{
						TopicData* pdata = p_topic->Find(key);
						if( pdata )
						{
							local_logger().LogLocal(utility::FormatStr("send topic once for: %d::%s", topic_id, key.c_str()));
							pdata->AsyncSendOne(pconn, data_pos);

							// send finish msg
							RequestAck ack;
							FillRequestAck(request_id, ack);
							pconn->AsyncSend(Encode(ack, msg_system_, Message::HeaderType(0, pid(), 0)));
						}else
						{
							send_unknown_key_error(pconn, key, topic_id, request_id);
						}
					});
				break;
			}
		case TopicRequest::Unsubscribe:
			{
				std::for_each( std::begin(req.keys()), std::end(req.keys())
					, [p_topic, &pconn, topic_id, request_id, &send_unknown_key_error, this](const std::string& key)
					{
						TopicData* pdata = p_topic->Find(key);
						if( pdata )
						{
							this->local_logger().LogLocal(utility::FormatStr("remove topic destination connid %d from: %d::%s", pconn->connid(), topic_id, key.c_str()));
							pdata->AsyncRemoveDestination(pconn->connid());
						}
						else // log as warning and send reqeust error
						{
							send_unknown_key_error(pconn, key, topic_id, request_id);
						}
					});
				break;
			}
		}
	}
}



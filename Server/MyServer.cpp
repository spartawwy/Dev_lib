#include "MyServer.h"

#include <iostream>

#include "TLib/core/tsystem_serialization.h"

//#include "TLib/core/tsystem_connection.h"
#include "TLib/tool/tsystem_connection_handshake.pb.h"

#include "shared/wze_message_system.h"
#include "WZELib/wze_query_msg.h"

enum class SlashTopic : unsigned char
{
    HAPPY_TOPIC = 100,
};

using namespace TSystem;

MyServer::MyServer(const std::string& name, const std::string& version) 
    : ServerAppBase("exch_msg_relay", name, version)
    , timer_(task_pool())
{

}

MyServer::~MyServer()
{
    FireShutdown();
}

void MyServer::Initiate()
{
#if 1
    // load wze class id
	option_load_class_id(false);
	WZESystem::LoadClassID(msg_system_);

	// base class initiate
	option_load_config(true);
	ServerAppBase::Initiate();
#else
    option_load_class_id(true);
    ServerAppBase::Initiate();
#endif 
    

      
    //--------------------------
    topic_server_.AddTopicData(static_cast<unsigned short>(SlashTopic::HAPPY_TOPIC), "1", task_pool());
    
    timer_.ExpirationDuration( std::chrono::system_clock::duration(std::chrono::seconds(2)) );
    timer_.PostTask(std::bind(&MyServer::TimerFunc, this, std::placeholders::_1));

    // re-direct generic reqeust to generic_request_kernel_
	msg_handlers_.RegisterHandler("GenericRequest", [this](communication::Connection* pconn, const Message& msg)
	{  
		/*if( forward_module_.CheckForward(msg) )
			return;*/

		this->generic_request_kernel_.HandleMessage(pconn, msg);
	});

    ConnectNode();
    StartPort();
}

void MyServer::Shutdown()
{
    ServerAppBase::Shutdown();
}

void MyServer::UpdateState()
{
    /*heartbeat_module_.Start();
	heart_beat_.UpdateStatus(Heartbeat::ON_LINE, pid());*/
}
 
void MyServer::HandleNodeHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) 
{
    HandShake shake;

    if( Decode(msg, shake, msg_system()) )
    {
        p->hand_shaked(true);
        SetupNodeRelayConnection(p->shared_this());

    }
}

void MyServer::HandleInboundHandShake(TSystem::communication::Connection* p, const TSystem::Message& msg) 
{
    HandShake hs;

	if(Decode(msg, hs, msg_system_))
	{
		const layinfo::AppResource& res = resource_manager_.FindUnique(hs.pid()); 
		if( res )
		{
			auto pconn = p->shared_this();

			p->hand_shaked(true);
			SetupInboundConnection(pconn, res);

			// add connection for forward module
			// this->forward_module_.AddConnection(hs.pid(), pconn);

			// add connection for master selection module
			// this->master_selection_module_.AsyncAddResource(res.raid_id, res.pid, pconn);
		}
	}
}

void MyServer::HandleInboundDisconnect(std::shared_ptr<TSystem::communication::Connection>& pconn
	, const TSystem::TError& te)
{
    // todo:
}

void MyServer::TimerFunc(const TSystem::TError& te)
{
    static unsigned int  st_count = 0;

    // std::cout << "TimerFunc ----------------------------" << st_count ++ << std::endl;
     communication::TopicData* p_topic_data = topic_server_.Find(static_cast<unsigned short>(SlashTopic::HAPPY_TOPIC), "1");
    assert(p_topic_data);
#if 0
    
    Heartbeat h_b; 
    h_b.set_pid(st_count++);
    h_b.set_status(Heartbeat::ON_LINE);
    //p_topic_data->AsyncUpdateData( Encode(h_b, msg_system(), MessageHeader(0, pid(), 0)) );
    p_topic_data->AsyncAddData( Encode(h_b, msg_system(), MessageHeader(0, pid(), 0)) );
    local_logger().LogLocal("TimerFunc 104");
    local_logger().LogLocal("data", "TimerFunc 105");
#else

    for(int i=0; i<5; i++)
    {
        QueryRequest  query_req;
        query_req.set_request_id(i);
        query_req.set_user_id(1049);
        query_req.set_query_type(QueryType::POSITION); 

        /*
        p_topic_data->AsyncAddData(Encode(query_req, msg_system(), MessageHeader(0, pid(), 0)));
        */
        auto code_ptr = Encode(query_req, msg_system(), MessageHeader(0, pid(), 0));
        
        Message msg(code_ptr->data(), code_ptr->size());
        auto spid = msg.SenderPid();
        auto rpid = msg.ReceiverPid();
        auto tot = msg.tot_size();
        auto seq = msg.MsgSeq();
        auto ckval = *(communication::SizeHeaderType *)code_ptr->data();

        auto code_ptr_fd = ForwardEncode(msg);

        Message  msg1(code_ptr_fd->data(), code_ptr_fd->size());
        auto spid1 = msg1.SenderPid();
        auto rpid1 = msg1.ReceiverPid();
        auto tot1 = msg1.tot_size();
        auto seq1 = msg1.MsgSeq();

        auto ckval1 = *(communication::SizeHeaderType *)code_ptr_fd->data();

        auto check_forward = [this](Message & msg1) ->bool
        {
            if( *(communication::SizeHeaderType *)msg1.data() == FORWORD_FLAG_VAL )
                return true;
            else
                return false;
#if 0
            // not forward 
            if( *(communication::SizeHeaderType *)msg1.data() + sizeof(communication::SizeHeaderType) != 
                msg1.tot_size() )
             
            {
                return false;
            }else 
            {
                if( *(communication::SizeHeaderType *)msg1.data() != 0 )// also may be not forward  
                {
                    //  do as not forward
                    //  do as forward
                    return false;
                }else
                {
                    return true;
                    // do as forward
                }
            }
#endif
        };
        
        check_forward(msg);
        check_forward(msg1);
        p_topic_data->AsyncAddData(code_ptr_fd);
    }
#endif
   // timer_.ExpirationDuration( std::chrono::system_clock::duration(std::chrono::seconds(2)) );
   // timer_.PostTask(std::bind(&MyServer::TimerFunc, this, std::placeholders::_1));
}
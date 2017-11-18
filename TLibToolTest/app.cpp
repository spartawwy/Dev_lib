#include "app.h"

#include <iostream>
#include <array>

#include "TLib/core/tsystem_serialization.h"

#include "TLib/tool/tsystem_connection_handshake.pb.h"
#define USE_SYNC_CONN
boost::asio::io_service s_io_svr;

using namespace TSystem;

struct MessageHead
{
    unsigned int tot_size; // include head
    unsigned int msg_seq;
    unsigned short sender_id;
    unsigned short receiver_id;
    unsigned short class_id;
};

App::App()
		: AppBase("misc", "test", "0.1" )
		, lay_(TSystem::utility::DevTag())
        , strand_(task_pool())
        , socket_(s_io_svr)
{
    work_ = std::unique_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(s_io_svr));
    printf("App\n");
}

App::~App()
{
    printf("~App\n");
}

void App::Start()
{
    static auto make_datatime_string = []() ->std::string
    {
        time_t now = std::time(nullptr);
        return ctime(&now);
    };
    task_pool().PostTask([this]()
    {
        //this->task_pool()
        while(1)
            s_io_svr.run();
    });

	//------------------
	// app resource test
	//------------------
	layinfo::AppResourceManager res_mgr = lay_.LoadAppResourceManager();
	//layinfo::RegisterApp(*this, res_mgr);
    strand_.PostTask([]()
    {
        for(int i=0; i<10; ++i)
        {
            printf(__FUNCTION__);
            printf("\n");
        }
    });

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

#ifdef USE_SYNC_CONN
    // wwy 
    boost::system::error_code  error;
    TCP::endpoint  end_point(boost::asio::ip::address_v4::from_string("128.1.1.3"), 8999);
    socket_.connect(end_point, error);
    if (error)  
            throw boost::system::system_error(error);  
#else

    boost::system::error_code  error;
    TCP::endpoint  end_point(boost::asio::ip::address_v4::from_string("128.1.1.3"), 8999);
    
    TCP::resolver::iterator iter;
    socket_.async_connect(end_point, 
        boost::bind(&App::connect_accept, this, boost::asio::placeholders::error, iter));
         
    if (error)  
            throw boost::system::system_error(error);  
#endif

#if 0
    //while(true)
    for( int i=0; i<1; i++)
    {
        auto str = make_datatime_string();
        //str.begin();
        std::vector<char> buf;
        std::for_each(std::begin(str), std::end(str), [&buf](char entry)
        {
            buf.push_back(entry);
        }); 
        socket_.write_some(boost::asio::buffer(buf), error);
        if( error == boost::asio::error::eof )
            break;
        else if (error)  
            throw boost::system::system_error(error); // Some other error.  
        else
            std::cout << "Send " << str << std::endl;

        // recv 
        std::vector<char> buf_recv;
        socket_.read_some(boost::asio::buffer(buf_recv), error);
        if( error == boost::asio::error::eof )
            break;
        else if (error)  
            throw boost::system::system_error(error); // Some other error.  
        else
        {
            std::cout << "Recv " << std::endl;
            std::for_each(std::begin(buf_recv), std::end(buf_recv), [](char ch)
            {
                std::cout << ch;
            });
            std::cout << std::endl;
        }
    }
#else
     const char* ClassIDArray[] = 
	    {
		    "GenericRequest", //class id 1
		    "HandShake",
		    "Heartbeat",
		    "LogMessage",
            "TopicRequest",
	    };
    HandShake  hand_shake;
    hand_shake.set_pid(12);
    std::vector<char> str_vec(hand_shake.ByteSize());
    hand_shake.SerializeToArray(str_vec.data(), hand_shake.ByteSize());

    auto len = hand_shake.ByteSize();
    HandShake object;
    bool rett = object.ParseFromArray(str_vec.data(), len);
    // insert head
    //char temp_buf1[] = "123456";
    //str_vec.insert(str_vec.begin(), temp_buf1, temp_buf1+sizeof(temp_buf1)-1);

#if 0
    MessageHead head = {0};
    head.tot_size = sizeof(head) + str_vec.size();
    head.class_id = 2;  
    char *p = reinterpret_cast<char*>(&head);
    str_vec.insert(str_vec.begin(), p, p + sizeof(head));
     
    socket_.write_some(boost::asio::buffer(str_vec), error);

    //----------------------
#else
    auto data_encoded = Encode(hand_shake, msg_system_, Message::HeaderType(0, 9, 1));
    unsigned int msg_send_len = data_encoded->size();
    socket_.write_some(boost::asio::buffer( reinterpret_cast<char*>(&msg_send_len), sizeof(unsigned int) ), error);
    socket_.write_some(boost::asio::buffer(data_encoded->data(), data_encoded->size()), error);

#endif

#endif
}

int App::ProcessCmd(int argc, char* argv[])
{ 
	return 0;
}

void App::connect_accept(const boost::system::error_code& error, TCP::resolver::iterator endp_iter)
{
    if( !error )
    {
        boost::asio::socket_base::keep_alive option(true);
        socket_.set_option(option);
        socket_;
    }else
    {
        std::cout << error.message() << std::endl;
    }
}


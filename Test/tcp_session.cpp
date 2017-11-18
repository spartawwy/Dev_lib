#include "tcp_session.h"

#include <boost/timer.hpp>
#include <boost/bind.hpp>
#include <iostream>

#include "tsystem_tcp_dock.h"

typedef boost::asio::ip::tcp  TCP;

using namespace TSystem;
using namespace TSystem::communication;

TcpSession::TcpSession(TcpDock& tcp_dock, const std::string& target_address, int port, bool is_async_conn
                       , ConnectingHandler handler)
                       : Connection(Type::tcp)
                       , task_pool_(tcp_dock.task_pool())
                       , tcp_dock_(tcp_dock)
                       , strand_write_(tcp_dock.task_pool())
                       , strand_sync_read_(nullptr)
                       , socket_(new tcp::socket(*tcp_dock.io_service())) 
                       , target_address_(target_address)
                       , target_port_(port)
                       , is_async_conn_(is_async_conn)
                       , connect_handler_(handler)
                       , is_read_total_size_(true)
                       , rec_code_data_ptr_(nullptr)
                       , received_date_size_(0)
                       , start_read_flag_(false)
                       , write_process_flag_(false)
                       , has_write_body_(false)
{
    //event_handlers_.on_connect = handler;
    process_buf_.reserve(1024);
    process_buf_ext_.reserve(1024);
} 

//TcpSession::~TcpSession()
//{
//    std::cout << __FUNCTION__ << std::endl;
//}

// occure exception. why ?
std::shared_ptr<Connection> TcpSession::shared_this()
{
    return shared_from_this();
}

// connection initiation
void TcpSession::InitConnection()
{
    // todo:
    if( !target_address_.empty() && target_port_ > 0 )
    {
        //socket_->set_option(boost::asio::ip::tcp::acceptor::linger(true, 0));  
        //socket_->set_option(boost::asio::socket_base::keep_alive(true));  
        if( is_async_conn_ )
        {
#if 1
            TCP::endpoint  end_point(boost::asio::ip::address_v4::from_string(target_address_.c_str()), target_port_);
            auto error = MakeTError(CoreErrorCategory::ErrorCode::OK);
            connect_handler_(shared_this(), error);
            //auto iter = new tcp::resolver::iterator;
            socket_->async_connect(end_point, boost::bind(&TcpSession::handle_connect, this
                , boost::asio::placeholders::error/*, iter*/)); 
#endif
        }else
        {
            boost::system::error_code  error;
            TCP::endpoint  end_point(boost::asio::ip::address_v4::from_string(target_address_.c_str()), target_port_);

            socket_->connect(end_point, error);
            if (error)  
                throw boost::system::system_error(error);  

            boost::asio::socket_base::keep_alive option(true);
            socket_->set_option(option);
             
            //strand_sync_read_ = std::make_shared<TSystem::TaskStrand>(task_pool_);
            task_pool_.PostTask([this]()
            {
                boost::timer  tmr;
                while( !this->task_pool_.HasShutdown() )
                {
                    boost::system::error_code ec;
                    auto len = this->socket_->read_some(boost::asio::buffer(buf_), ec);
                    if( ec )
                    {
                         // todo: print erro
                        printf("TcpSession::InitConnection read_some occure error\n");
                        break;
                    }
                    if( len == 0)
                    {
                        Sleep(50);
                        continue;
                    }

                    unsigned int total_size = *reinterpret_cast<unsigned int*>(buf_); 
                    assert(total_size > 10);

                    rec_code_data_ptr_ = std::make_shared<communication::CodedData>(total_size);

                    auto len2 = socket_->read_some(boost::asio::buffer(rec_code_data_ptr_->data(), rec_code_data_ptr_->size())
                                        , ec);
                    if( ec )
                    {
                        printf("TcpSession::InitConnection read_some occure error\n");
                        break;
                    }

                    if( len2 == rec_code_data_ptr_->size() )
                    {
                        event_handlers_.on_msg_recv(rec_code_data_ptr_->data(), rec_code_data_ptr_->size());
                    } 
                                 
                } // while
            });
        }

    }else
    {
        auto error = MakeTError(CoreErrorCategory::ErrorCode::BAD_SOCKET);
        connect_handler_(shared_this(), error);
    }

}

// connection clean
void TcpSession::CloseConnection(const TError& ec)
{
    // todo:
}

// prepare to read 
void TcpSession::StartRead() 
{
    /*async_read跟客户端一样，还是不能进入handle_read函数,如果你能找到问题所在，请告诉我，谢谢*/
    // --已经解决，boost::asio::async_read(...)目标字节长度不能大于数据流的长度，否则就会进入
    // ioservice.run()线程等待，read后面的就不执行了。
    //boost::asio::async_read(m_socket,boost::asio::buffer(data_rec_,max_len),
    //        boost::bind(&clientSession::handle_read,shared_from_this(),
    //        boost::asio::placeholders::error));

    is_read_total_size_ = true;
    start_read_flag_ = true;
 
    if( connid() > 0 ) // means has registered
        boost::asio::async_read(*socket_, boost::asio::buffer(buf_, /*MAX_PACKET_LEN*/sizeof(unsigned int)),
                                boost::bind(&TcpSession::handle_read, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
 
}

// prepare to write
void TcpSession::StartWrite()  
{
    // lock
    {
        std::lock_guard<std::mutex> locker(buf_ext_mutex_);
        std::for_each( std::begin(data_queue_), std::end(data_queue_), [this](CodedDataPtr& data)
        {
            this->process_buf_ext_.push_back(std::move(data));
        });
    }
    data_queue_.clear();
    // unlock


    if( !write_process_flag_ )
    {
        write_process_flag_ = true;
        TriggerSwapWrite();
    }
}

// async queuing data
void TcpSession::AysncQueueData( std::vector<CodedDataPtr>&& batch_data )
{
    if( !batch_data.empty() )
    {
        std::for_each( std::begin(batch_data), std::end(batch_data), [this](CodedDataPtr& data)
        {
            this->data_queue_.push_back(std::move(data));
        });
        PrepareWrite();
    }
}

void TcpSession::AysncQueueData( CodedDataPtr&& data)
{
    data_queue_.push_back(std::move(data));
    PrepareWrite();
}

//void TcpSession::TriggerRead()
//{
//#if 1
//    socket_->async_read_some(boost::asio::buffer(buf_tmp_, /*MAX_PACKET_LEN*/1024),
//        boost::bind(&TcpSession::handle_read, this,
//        boost::asio::placeholders::error,
//        boost::asio::placeholders::bytes_transferred));
//#else
//    socket_->async_receive(boost::asio::buffer(buf, sizeof(unsigned int)),
//        boost::bind(&TcpSession::handle_read, shared_from_this(),
//        boost::asio::placeholders::error,
//        boost::asio::placeholders::bytes_transferred));
//#endif
//}

void TcpSession::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    if( !error )
    {
        std::cout << "rec " << socket_->remote_endpoint().address().to_string() 
            << ":" << socket_->remote_endpoint().port() << " " << bytes_transferred << " byte:" 
            /*<< data_rec_ */<< std::endl;
        if( is_read_total_size_ )
        {
            unsigned int total_size = *reinterpret_cast<unsigned int*>(buf_); 
            assert(total_size >= 10); //ndchk
            // todo: if size illegal throw exception
            is_read_total_size_ = false;
            received_date_size_ = 0;
            //rec_code_data_ptr_ = std::make_shared<communication::CodedData>(total_size - sizeof(total_size));
            rec_code_data_ptr_ = std::make_shared<communication::CodedData>(total_size);
            socket_->async_read_some(boost::asio::buffer(rec_code_data_ptr_->data() + received_date_size_, rec_code_data_ptr_->size() - received_date_size_)
                , boost::bind(&TcpSession::handle_read, this
                , boost::asio::placeholders::error
                , boost::asio::placeholders::bytes_transferred));

        }else
        {
            received_date_size_ += bytes_transferred;
            if( received_date_size_ == rec_code_data_ptr_->size() )
            {
                event_handlers_.on_msg_recv(rec_code_data_ptr_->data(), rec_code_data_ptr_->size());

                is_read_total_size_ = true;
                received_date_size_ = 0; 
                boost::asio::async_read(*socket_, boost::asio::buffer(buf_, /*MAX_PACKET_LEN*/sizeof(unsigned int))
                    , boost::bind(&TcpSession::handle_read, this
                    , boost::asio::placeholders::error
                    , boost::asio::placeholders::bytes_transferred)); 
            }else if( received_date_size_ < rec_code_data_ptr_->size() )
            {
                socket_->async_read_some(boost::asio::buffer(rec_code_data_ptr_->data() + received_date_size_, rec_code_data_ptr_->size() - received_date_size_)
                    , boost::bind(&TcpSession::handle_read, this
                    , boost::asio::placeholders::error
                    , boost::asio::placeholders::bytes_transferred));

            }else
            { 
                // throw exception 
                // delete this;
                // todo: CommunicationDock::Unregister
            }
        }
    }else
    {
        // throw exception
        std::cout << error.message() << "\n";
        //socket_->close();
        //delete this;
        // todo: CommunicationDock::Unregister
    }
}














void TcpSession::handle_write(const boost::system::error_code& error/*, size_t  byte_transfered*/)
{
    static auto do_on_buf_ext = [](TcpSession* tcp_session)
    {
        if( !tcp_session->process_buf_ext_.empty() )
            tcp_session->TriggerSwapWrite();
        else
            tcp_session->write_process_flag_ = false;
    };

    if (!error)
    {
        auto iter = process_buf_.begin();

        if( iter == process_buf_.end() )
        {
            do_on_buf_ext(this);
            return;
        }

        if( has_write_body_ )
        {
            // remove pre data
            if( iter != process_buf_.end() )
                process_buf_.erase(iter); 
            if( process_buf_.empty() ) // finish write
            {
                do_on_buf_ext(this);
                return;
            }


            has_write_body_ = false;

            auto len = process_buf_.front()->size();
            assert(len > 0);
            std::cout << connid() << " send len val:" << len << std::endl;
            socket_->async_send(boost::asio::buffer(&len, sizeof(decltype(len)))
                , boost::bind(&TcpSession::handle_write, this, boost::asio::placeholders::error));
        }else
        {
            if( process_buf_.empty() ) // finish write
            {
                write_process_flag_ = false;
                return;
            }
            std::cout << connid() << " send " << process_buf_.front()->size() << " BYTE" << std::endl;
            has_write_body_ = true;
            socket_->async_send(boost::asio::buffer(process_buf_.front()->data(), process_buf_.front()->size())
                , boost::bind(&TcpSession::handle_write, this, boost::asio::placeholders::error));

        }


    }else
    {
        std::cout << connid() << " handle_write error" << std::endl;
        // delete this;
        // todo: CommunicationDock::Unregister
        tcp_dock_.UnRegister(this);
        if( holder_remover_ )
            holder_remover_(connid());
    }
}

void TcpSession::handle_received_msg()
{
    // todo:
}

// for tcp client's async connect
void TcpSession::handle_connect(const boost::system::error_code& error/*, tcp::resolver::iterator *endpoint_iterator*/) 
{
    try{  
        if (!error)  
        {  
            std::cout << "TcpSession::handle_connect" << std::endl;
            //LOG4CXX_FATAL(firebird_log, "服务器:[" << pSession->get_business_type() <<"],连接远程地址:[" << pSession->get_remote_addr().c_str() << "]成功!");  
            //pSession->start();  

            ////向服务器注册服务类型  
            //message msg;   
            // pSession->async_write(msg);   
            auto error = MakeTError(CoreErrorCategory::ErrorCode::OK);
            tcp_dock_.Register(this); 
            //event_handlers().on_connect();
            if( start_read_flag_ )
            {
                socket_->async_receive(boost::asio::buffer(buf_, /*MAX_PACKET_LEN*/sizeof(unsigned int)),
                    boost::bind(&TcpSession::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            }

            //connect_handler_(shared_this(), error);
            event_handlers_.on_connect();
        }  
        //else if (*endpoint_iterator != tcp::resolver::iterator())  
        //{  
        //    //LOG4CXX_ERROR(firebird_log, "连接远程地址:[" << pSession->get_remote_addr().c_str() << "]失败，试图重连下一个地址。");  
        //    //pSession->socket().close();//此处用socket的close，不应用session的close触发连接，不然会导致一直重连  
        //    //tcp::endpoint endpoint = *endpoint_iterator;  
        //    //pSession->socket().async_connect(endpoint,  
        //    //    boost::bind(&client_socket_utils::handle_connect, this,  
        //    //    boost::asio::placeholders::error, ++endpoint_iterator, pSession));  
        //}  
        else  
        {  
            //LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << pSession->get_remote_addr().c_str() << "]失败！");  
            // pSession->socket().close();//此处用socket的close，不应用session的close触发连接，不然会导致一直重连  
            //socket_->close();
            std::cout << error.message() << "\n";

        }  
        tcp_dock_.RemoveTempConnection(this);
    }  
    catch(std::exception& e)  
    {  
        //LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << pSession->get_remote_addr().c_str() <<"],socket异常:[" << e.what() << "]");  

    }  
    catch(...)  
    {  
        //LOG4CXX_ERROR(firebird_log, KDS_CODE_INFO << "连接远程地址:[" << pSession->get_remote_addr().c_str() <<"],socket异常:[未知异常]");  
    }  
}

void TcpSession::send_data()
{

}

void TcpSession::TriggerSwapWrite()
{ 
    {
        std::lock_guard<std::mutex> locker(buf_ext_mutex_);
        process_buf_.swap(process_buf_ext_);
        process_buf_ext_.clear();
    }

    has_write_body_ = false;
    auto len = process_buf_.front()->size();
    ////////////////////////////////////////////
    // unsigned int msg_seq
    // unsigned short sender_pid
    // unsigned short receiver_pid
    // unsigned short classid
    ///////////////////////////////////////////
    assert(len >= 10); // some object size may be 0 // 
    std::cout << "connid: " << connid() << " TriggerSwapWrite len: " << len << std::endl;
    socket_->async_send(boost::asio::buffer(&len, sizeof(decltype(len)))
        , boost::bind(&TcpSession::handle_write, this, boost::asio::placeholders::error));
}
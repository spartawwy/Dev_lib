#include "session.h"

#include <boost/asio.hpp>
//#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>

#include <boost/circular_buffer.hpp>
#include <boost/lockfree/queue.hpp>

Session::Session(boost::asio::io_service& io_service, ServerApp& app)
    : socket_(new tcp::socket(io_service))
    , strand_(app.task_pool())
    , strand_write_(app.task_pool())
    , recv_times_(0)
    , packet_size_(0)
    , count111_(0) 
{
}

tcp::socket& Session::socket()
{
    return *socket_;
}

void Session::Start()
{
    /*async_read跟客户端一样，还是不能进入handle_read函数,如果你能找到问题所在，请告诉我，谢谢*/
    // --已经解决，boost::asio::async_read(...)目标字节长度不能大于数据流的长度，否则就会进入
    // ioservice.run()线程等待，read后面的就不执行了。
    //boost::asio::async_read(m_socket,boost::asio::buffer(data_rec_,max_len),

    //        boost::bind(&clientSession::handle_read,shared_from_this(),
    //        boost::asio::placeholders::error));
    //max_len可以换成较小的数字，就会发现async_read_some可以连续接收未收完的数据
    //memset(data_rec_, 0, sizeof(data_rec_));
     
    rec_bufitem_que_.emplace_back();
    boost::asio::async_read(*socket_, boost::asio::buffer(&rec_bufitem_que_.back().head, /*MAX_PACKET_LEN*/sizeof(WYMessageHead)),
                            boost::bind(&Session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));

    // test async write ---------

    //memcpy(data_send_, data_rec_, sizeof(data_rec_));
   

}

void Session::AsyncSend(std::shared_ptr<google::protobuf::MessageLite>& buf, size_t len)
{
    // todo: post strand to send
    strand_write_.PostTask([&buf, len, this]()
    {
        write_buffer_que_.push_back(std::make_pair(buf, len));
        boost::asio::async_write(*socket_, boost::asio::buffer(write_buffer_que_.begin()->first.get(), write_buffer_que_.begin()->second),   
            boost::bind(&Session::handle_write, this, boost::asio::placeholders::error));
    });
    
}

void Session::handle_read(const boost::system::error_code& error, size_t transferred)
{
    if (!error)
    {
        ++ recv_times_;

        count111_ ++;

        struct tm *today = nullptr;
        time_t ltime;
        time( &nowtime_ );

        if( nowtime_ != oldtime_ )
        {
            printf("%d\n", count111_);
            oldtime_ = nowtime_;
            count111_ = 0;
        }

        std::cout << "rec " << socket().remote_endpoint().address().to_string() 
            << ":" << socket().remote_endpoint().port() << " " << transferred << " byte:" 
            /*<< data_rec_ */<< std::endl;

        //std::vector<char> buf_vec;
        //buf_vec.data();
        //strand_.PostTask(
        // next async read ---------
        
        //memset(data_rec_, 0, sizeof(data_rec_));

        MessageBufItem& latest_item = rec_bufitem_que_.back(); 
        assert( latest_item.head.tot_size >= latest_item.received_size );
        // todo: check length if no ok throw exception
        latest_item.received_size += transferred;
        assert( latest_item.head.tot_size >= latest_item.received_size );
        // todo: check length if no ok throw exception

        unsigned int target_len = 0;
        if( latest_item.IsFinish() )
        {
            strand_.PostTask(std::bind(&Session::handle_received_msg, this));

            target_len = sizeof(WYMessageHead);
            rec_bufitem_que_.emplace_back();

            /*socket_->async_read_some(boost::asio::buffer(&rec_bufitem_que_.back().head, target_len),
                            boost::bind(&Session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));*/
#if 0
            socket_->async_receive(boost::asio::buffer(&rec_bufitem_que_.back().head, target_len),
                            boost::bind(&Session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
#endif
             boost::asio::async_read(*socket_, boost::asio::buffer(&rec_bufitem_que_.back().head, /*MAX_PACKET_LEN*/sizeof(WYMessageHead)),
                            boost::bind(&Session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));

        }else if( latest_item.received_size < sizeof(WYMessageHead) )
        {
            target_len = sizeof(WYMessageHead) - latest_item.received_size;
            char *p = reinterpret_cast<char*>(&rec_bufitem_que_.back().head);
            socket_->async_read_some(boost::asio::buffer(p + latest_item.received_size, target_len),
                            boost::bind(&Session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
        }else
        {
            target_len = latest_item.head.tot_size - latest_item.received_size;
            if( !latest_item.p_proto_buf )
                latest_item.p_proto_buf = new char[latest_item.head.tot_size - sizeof(WYMessageHead) < 1024 ? 1024 : latest_item.head.tot_size - sizeof(WYMessageHead)];
            
            char *p = reinterpret_cast<char*>(&rec_bufitem_que_.back().head);
            socket_->async_read_some(boost::asio::buffer(rec_bufitem_que_.back().p_proto_buf + latest_item.received_size, target_len),
                            boost::bind(&Session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
        }
        
        //std::cout << data_rec_ << std::endl;

        // test ---------------
#if 0
        //boost::circular_buffer<char>  circle_buf;
         
        //boost::lockfree::queue<char> que;
        std::deque<char>  contain_deque;
        contain_deque.begin();
        socket_->async_read_some(boost::asio::buffer(recv_vector_.data(), MAX_PACKET_LEN),
                            boost::bind(&Session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
#endif
    }
    else
    {
        delete this;
    }
}

void Session::handle_write(const boost::system::error_code& error/*, size_t  byte_transfered*/)
{
    if (!error)
    {
        std::cout << "send data" << std::endl;
        write_buffer_que_.pop_front();
        if( !write_buffer_que_.empty() ) 
        {
             boost::asio::async_write(*socket_, boost::asio::buffer(write_buffer_que_.begin()->first.get(), write_buffer_que_.begin()->second),   
             boost::bind(&Session::handle_write, this, boost::asio::placeholders::error));
        }
    }
    else
    {
        delete this;
    }
}

void Session::handle_received_msg()
{
    if( rec_bufitem_que_.front().IsFinish() )
    {
        // todo: invoke registed function
        std::cout << "Session::handle_received_msg" << std::endl;

        delete [] rec_bufitem_que_.front().p_proto_buf;
        rec_bufitem_que_.front().p_proto_buf = nullptr;
        rec_bufitem_que_.pop_front();
    }
}
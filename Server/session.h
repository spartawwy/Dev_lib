#ifndef SESSION_H_
#define SESSION_H_

#include <deque>
#include <boost/asio.hpp>

#include "server_app.h"
//boost::mutex cout_mtx;

enum {MAX_PACKET_LEN = 4096};

using boost::asio::ip::tcp;

//namespace tcp
//{
//    class socket;
//}
//class ServerApp; 

struct WYMessageHead
{
    unsigned int tot_size; // include head
    unsigned int msg_seq;
    unsigned short sender_id;
    unsigned short receiver_id;
    unsigned short class_id;
    WYMessageHead() : tot_size(0), msg_seq(0), sender_id(0), receiver_id(0), class_id(0) {}
};

const unsigned int cst_min_proto_buf_size = 1024;
class MessageBufItem
{
public:
    WYMessageHead  head;
    unsigned int received_size;
    char *p_proto_buf;
    MessageBufItem() : received_size(0), p_proto_buf(nullptr) {}
    ~MessageBufItem() { if( p_proto_buf ) delete [] p_proto_buf; }
    bool IsFinish() { return head.tot_size != 0 && head.tot_size == received_size; }
};

typedef std::pair<std::shared_ptr<google::protobuf::MessageLite>, std::size_t> TBufferType; //buffer_type;
typedef std::deque<TBufferType> TBufferDeque;

typedef std::deque<MessageBufItem> TMsgBufItemDeque;
class Session
{
public:
    Session(boost::asio::io_service& io_service, ServerApp& app);

    virtual ~Session()
    {
       // boost::mutex::scoped_lock lock(cout_mtx);
    }

    tcp::socket& socket();

    void Start();
    void AsyncSend(std::shared_ptr<google::protobuf::MessageLite>& buf, size_t len);

private:

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
     
    void handle_write(const boost::system::error_code& error/*, size_t  byte_transfered*/);

    void handle_received_msg();

private:

    //ServerApp& app_;
    TSystem::TaskStrand strand_;
    TSystem::TaskStrand strand_write_;
    tcp::socket* socket_;

    std::vector<char> recv_vector_;
    char data_rec_[MAX_PACKET_LEN];
    char data_send_[MAX_PACKET_LEN];
    int recv_times_;

    int packet_size_;
    int count111_;
    time_t oldtime_, nowtime_;

    TBufferDeque write_buffer_que_;
    TMsgBufItemDeque  rec_bufitem_que_;
};

#endif // SEESION_H_
#ifndef TCP_SESSION_H_
#define TCP_SESSION_H_

#include <boost/asio.hpp>

#include "TLib/core/tsystem_connection.h"

#include "persist_service.h"

using boost::asio::ip::tcp;

 
namespace TSystem
{
    namespace communication
    {
		class TcpDock;
		class Connection;
        class CommunicationDock;
        class TcpSession : public Connection, public std::enable_shared_from_this<TcpSession>
        {
        public:

            TcpSession(TcpDock& tcp_dock, const std::string& target_address="", int port=0, bool async_conn=false, ConnectingHandler handler=nullptr);
            //~TcpSession();
            tcp::socket* socket() { return socket_.get(); }

			virtual std::shared_ptr<Connection> shared_this()        override;
            // connection initiation
			virtual void InitConnection()                    override;

			// connection clean
			virtual void CloseConnection(const TError& ec)   override;
        
        protected:

			// prepare to read 
			virtual void StartRead()                         override;

			// prepare to write
			virtual void StartWrite()                        override;
		
			// async queuing data
			virtual void AysncQueueData( std::vector<CodedDataPtr>&& batch_data )     override;
			virtual void AysncQueueData( CodedDataPtr&& data)                         override;

            // tmpcode
            //virtual void TriggerRead() override;

        private:

            void Connect();
             
            void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
     
            void handle_write(const boost::system::error_code& error/*, size_t  byte_transfered*/);

            void handle_received_msg();

            void handle_connect(const boost::system::error_code& error/*, tcp::resolver::iterator *endpoint_iterator*/) ;

            void send_data();

            void TriggerSwapWrite();

        private:

            const TSystem::TaskPool  &task_pool_;
            TcpDock& tcp_dock_;
            TSystem::TaskStrand strand_write_;
            std::shared_ptr<TSystem::TaskStrand> strand_sync_read_;
            std::unique_ptr<tcp::socket> socket_;
            std::string target_address_;
            int target_port_;
            bool is_async_conn_;
            ConnectingHandler connect_handler_;
			//std::shared_ptr<Connection> connection_;
            //boost::asio::io_service *p_io_service_;
			char buf_[sizeof(unsigned int)];
			char buf_tmp_[1024];//
			bool is_read_total_size_;
			communication::CodedDataPtr rec_code_data_ptr_;
			size_t received_date_size_; 

            bool start_read_flag_;

            std::vector<communication::CodedDataPtr> process_buf_; // used by async hand_write
            std::vector<communication::CodedDataPtr> process_buf_ext_;
            std::atomic<bool> write_process_flag_;
            std::mutex buf_ext_mutex_;
            bool has_write_body_;

			friend class TcpDock;
        };
    }
}
#endif //TCP_SESSION_H_
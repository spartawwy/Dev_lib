#ifndef  TSYSTEM_TCP_DOCK_H_
#define TSYSTEM_TCP_DOCK_H_

#include <boost/asio.hpp>
#include <mutex>

#include <TLib/core/tsystem_task_service.h>
#include <TLib/core/tsystem_communication_common.h>

#include "persist_service.h"
//#include "work_ptr.h"

using boost::asio::ip::tcp;

enum {MAX_PACKET_LEN = 4096};

namespace TSystem
{

// forward declaration
class TaskPool;

// forward declaration;
namespace communication
{
    class TcpSession;
    class CommunicationDock;
    class TcpDock
    {
    public:

        TcpDock(CommunicationDock& dock, const TaskPool& pool);
        ~TcpDock();

        void StartPornt(unsigned short port, ConnectingHandler&& handler);

        const TaskPool& task_pool() { return task_pool_; }
        std::unique_ptr<tcp::socket>& socket() { return socket_; }
        boost::asio::io_service* io_service() { return p_io_service_; }

        void AddTempConnection(std::shared_ptr<TcpSession>& conn);
        void RemoveTempConnection(TcpSession* conn);
        void Register(TcpSession* conn);
        std::shared_ptr<Connection> UnRegister(TcpSession* conn);

    private:

		void handle_accept(std::shared_ptr<Connection>& session, const boost::system::error_code& error);

    private:

        //ServerApp& app_;
        CommunicationDock& communication_dock_;
        const TaskPool& task_pool_;

        TSystem::TaskStrand strand_;
        std::unique_ptr<tcp::socket> socket_;

        boost::asio::io_service *p_io_service_;
        void *acceptor_;

        ConnectingHandler  connect_hander_;
		
        std::list<std::shared_ptr<TcpSession> > temp_tcp_connections_;
        std::mutex  temp_connections_lock_;
		 
    };
}
}
#endif // TSYSTEM_TCP_DOCK_H_

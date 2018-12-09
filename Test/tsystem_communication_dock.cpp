#include "TLib/core/tsystem_communication_dock.h"

#include <stdlib.h>
#include <stdio.h>
#include <regex>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/exception/all.hpp>

#include "tsystem_tcp_dock.h"
#include "tcp_session.h"

typedef boost::asio::ip::tcp  TCP;
typedef boost::error_info<struct tag_err_str, std::string> err_str; 

using namespace TSystem;
using namespace TSystem::communication;

static std::vector<std::string> Domain2ip(const std::string& domain, int port);

CommunicationDock::CommunicationDock(const TaskPool& pool)
    : pool_(pool)
    , connections_lock_()
    , connections_(64)
    , tcp_dock_(new communication::TcpDock(*this, pool))
{
    shutdown_ = false;
    next_connid_ = 0;
};

CommunicationDock::~CommunicationDock()
{

}
			
// disconnect all connections 
void CommunicationDock::Shutdown()
{
    // ndchk
    std::vector<int> pids;
    std::for_each( std::begin(connections_), std::end(connections_)
        , [&pids](std::unordered_map<int, std::shared_ptr<Connection> > ::reference entry)
    {
        pids.push_back(entry.first); 
    });
    std::for_each( std::begin(pids), std::end(pids), [this](int id){ this->Unregister(id); } );
}

// get a connection, return a weak_ptr in case of removed connid
std::weak_ptr<Connection> CommunicationDock::GetConnection(int connid) 
{
    auto iter = connections_.find(connid);
    if( iter != connections_.end() )
        return iter->second;
    else
        return std::weak_ptr<Connection>();
}


//-----------------------
// connection operations
//-----------------------

// start a tcp port, throw TException when socket fails
void CommunicationDock::StartPort(unsigned short port, ConnectingHandler&& handler )
{
    // todo: accept connection
    tcp_dock_->StartPornt(port, std::move(handler));

}

// blocking connect to an (address, port) using tcp or udp, return connection
std::shared_ptr<Connection> CommunicationDock::Connect(const std::string& address, int port, TSystem::TError& err
	, Connection::Type type)
{
    static auto is_ipstr_regex = [](const std::string &ip_sample)->bool
    { 
        const static std::regex ip_regex("^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9])(\\.)(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)(\\.)(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)(\\.)(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$");

        std::smatch match_res;
        if( std::regex_match( ip_sample.cbegin(), ip_sample.cend(), match_res, ip_regex ) )
		    return true;
	    else
            return false;
    };
    std::string  target_ip;
	auto str_vec = Domain2ip(address, port);
    if( str_vec.size() > 1 )
    {
        if( str_vec.size() < 2 )
            ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
				    , "CommunicationDock::AsyncConnect"
				    , "address:" + address); 
        int i = 0;
        for(; i < str_vec.size(); ++i)
        {
            if( is_ipstr_regex(str_vec.at(i)) ) break;
        }
        if( i >= str_vec.size() )
            ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
				    , "CommunicationDock::AsyncConnect"
				    , "address:" + address); 
        target_ip = str_vec.at(i);
    }else
    {
        if( !is_ipstr_regex(str_vec.at(0)) )
            ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
				    , "CommunicationDock::AsyncConnect"
				    , "address:" + address); 
        target_ip = str_vec.at(0);
    }
    auto tcp_sesstion = std::make_shared<TcpSession>(*tcp_dock_, target_ip.c_str(), port, false);

    try
    {
      tcp_sesstion->InitConnection();
    }catch(boost::exception& e)
    {
        //err = MakeTError(CoreErrorCategory::ErrorCode::BAD_SOCKET);
        ThrowTException(CoreErrorCategory::ErrorCode::BAD_SOCKET, "CommunicationDock::Connect", "");
    }

    return tcp_sesstion;
}

// async connect to an (address, port) using tcp or udp
void CommunicationDock::AsyncConnect(const std::string& address, int port, ConnectingHandler&& handler 
	, Connection::Type type)
{
    std::shared_ptr<TcpSession> tcp_sesstion = nullptr;
    auto str_vec = Domain2ip(address, port);
    if( str_vec.size() > 1 )
    {
        if( str_vec.size() < 2 )
            ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
				    , "CommunicationDock::AsyncConnect"
				    , "address:" + address); 
    
        tcp_sesstion = std::make_shared<TcpSession>(*tcp_dock_, str_vec.at(2), port, true, handler);
    }else
    {
        tcp_sesstion = std::make_shared<TcpSession>(*tcp_dock_, str_vec.at(0), port, true, handler);
    }
    tcp_sesstion->InitConnection();
    tcp_dock_->AddTempConnection(tcp_sesstion);
    
}

int  CommunicationDock::NextConnID()
{
    return static_cast<int>(++next_connid_);
}

void CommunicationDock::Register(const std::shared_ptr<Connection>& pconn)
{
    std::lock_guard<std::recursive_mutex > locker(connections_lock_);
    if( pconn->conn_id_ <= 0 )
        pconn->conn_id_ = NextConnID();
    if( connections_.find(pconn->conn_id_) == connections_.end() )
    {
        connections_.insert(std::make_pair(pconn->conn_id_, pconn));
        pconn->status_ = Connection::Status::connected;
    }
}

std::shared_ptr<Connection> CommunicationDock::Unregister(int connid )
{
    std::lock_guard<std::recursive_mutex > locker(connections_lock_);
	std::shared_ptr<Connection> conn = nullptr;
    auto iter = connections_.find(connid);
    if( iter != connections_.end() )
	{
		conn = iter->second;
        connections_.erase(iter);
        conn->status_ = Connection::Status::disconnected;
	}
	return conn;
}

std::vector<std::string> Domain2ip(const std::string& domain, int port)
{
    boost::asio::io_service ios;
	//创建resolver对象
	boost::asio::ip::tcp::resolver slv(ios);
	//创建query对象
    
    std::string port_str = std::to_string(port);
    boost::asio::ip::tcp::resolver::query qry(domain.c_str(), std::to_string(port));//将int型端口转换为字符串
    //使用resolve迭代端点
	boost::asio::ip::tcp::resolver::iterator it=slv.resolve(qry);
   
	boost::asio::ip::tcp::resolver::iterator end;
	std::vector<std::string> ip;
	for(; it != end; it++)
	{
		ip.push_back((*it).endpoint().address().to_string());
	}
	return ip;
}
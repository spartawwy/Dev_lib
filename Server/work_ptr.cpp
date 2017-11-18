#include "work_ptr.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio.hpp>

WorkPtr::WorkPtr(boost::asio::io_service* io_svr)
    : holder_(new boost::asio::io_service::work(*io_svr))
{
}

WorkPtr::~WorkPtr()
{
    if( holder_ )
        delete holder_;
}
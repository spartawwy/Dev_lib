#include "TLib/core/tsystem_connection.h"

#include "TLib/core/tsystem_error.h"

using namespace TSystem;
using namespace TSystem::communication;

Connection::~Connection()
{
    // todo:
}
 
// start connection message pulling, may throw TException on socket error
void Connection::Start()
{
    StartRead();
}

// normal disconnect and close connection
void Connection::Disconnect()
{
    // todo: 
}

Connection::Connection(Type t) 
    : status_(Status::disconnected)
    , message_processed_(0)
    , type_(Type::tcp)
    , conn_id_(0)
    , is_writing_(false)
    , holder_remover_(nullptr)
{
    hand_shaked_ = false;
}

void Connection::InitConnection()
{
    // override by sub class
}

 
 	
// async queuing data
void Connection::AysncQueueData( std::vector<CodedDataPtr>&& batch_data )
{
    // override by sub class
}

void Connection::AysncQueueData( CodedDataPtr&& data)
{
    // override by sub class
}

void Connection::PrepareWrite()
{
    // todo:
    is_writing_ = true;
    StartWrite();
    is_writing_ = false;
}

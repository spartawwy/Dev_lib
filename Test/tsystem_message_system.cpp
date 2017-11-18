#include <TLib/core/tsystem_message_system.h>

#include <algorithm>

#include <TLib/core/tsystem_error.h>
#include <TLib/core/tsystem_core_error.h>
#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_connection.h>
#include <TLib/core/tsystem_serialization.h>

using namespace TSystem;

MessageSystem::MessageSystem()
    : class_id_map_(64)
{
}

MessageSystem::MessageSystem(MessageSystem&& in)
	: class_id_map_(std::move(in.class_id_map_))
{
}

MessageSystem& MessageSystem::operator=(MessageSystem&& in)
{
	this->class_id_map_ = std::move(in.class_id_map_);
	return *this;
}

// register class name with id, throw TException when name / id duplication found
void MessageSystem::RegisterClassID(const std::string& name, unsigned short id)
{
	class_id_map_.insert(std::make_pair(name, id));
}

// find class id from registered class, throw TException when name not found
unsigned short MessageSystem::FindClassID( const std::string& name) const
{
	auto iter = class_id_map_.find(name);
	if( iter == class_id_map_.end() )
		ThrowTException(CoreErrorCategory::ErrorCode::BAD_CONTENT 
						, "FindClassID"
						, utility::FormatStr("can't find %s", name.c_str()));
	return iter->second;
}

// find class id with no throw, return 0 if not found
unsigned short MessageSystem::FindClassID( const std::string& name, const std::nothrow_t& tag) const
{
	auto iter = class_id_map_.find(name);
	if( iter == class_id_map_.end() )
		return 0;
	return iter->second;
}

//////////////////////////////////////////////////////////////////////////

MessageHandlerGroup::MessageHandlerGroup(const MessageSystem& msg_system)
	: msg_system_(msg_system)
{
    //static auto func = [](communication::Connection*, const Message&){ };
    
    for( auto i = 0; i < 64; ++i )
    {
        handlers_.emplace_back(nullptr);
    }
}

// MessageHandler(unsigned short connid, const Message& msg)
//typedef std::function<void(communication::Connection*, const Message&)> MessageHandler;

// throw exception if name is not registered with a class id
// multiple register will overwrite the old handler
void MessageHandlerGroup::RegisterHandler(const std::string& name, MessageHandler&& handler)
{
	auto class_id = msg_system_.FindClassID(name); // class id is begin with 1

	if( handlers_.size() <= class_id - 1 )
        ThrowTException(CoreErrorCategory::ErrorCode::BAD_CONTENT 
						, "MessageHandlerGroup::RegisterHandler"
						, utility::FormatStr("handlers_.size():%d <= %d", handlers_.size(), class_id));
	handlers_.at(class_id-1) = std::move(handler);
}

// check if has handler
bool MessageHandlerGroup::HasHandler(const std::string& name) const
{
    auto class_id = msg_system_.FindClassID(name); // class id is begin with 1
     
    return handlers_.size() >= class_id - 1 && handlers_[class_id - 1] != nullptr;
}

// invoke handler
void MessageHandlerGroup::InvokeHandler(communication::Connection* p, const Message& msg) const
{
    std::for_each( std::begin(handlers_), std::end(handlers_), [&](std::vector<MessageHandler>::const_reference entry)
    {
       if( entry != nullptr) 
           entry(p, msg);
    });
}

// link connection to msg_group = setup on_msg_recv
void MessageHandlerGroup::LinkConnection(std::shared_ptr<communication::Connection>& pconn) const
{
    static std::vector<std::shared_ptr<communication::Connection> > conn_ptr_holder;
    conn_ptr_holder.push_back(pconn);

    auto p_conn = pconn.get();
    pconn->event_handlers().on_msg_recv = [p_conn, this](const char* buf, std::size_t len)
    { 
        char * buf_m = const_cast<char*>(buf);
        std::size_t len_m = len;
        if( *(communication::SizeHeaderType*)buf == FORWORD_FLAG_VAL )
        {
            buf_m = const_cast<char*>(buf + sizeof(communication::SizeHeaderType));
            len_m = len - sizeof(communication::SizeHeaderType);
        }
         
        TSystem::Message msg(buf_m, len_m);
        printf("msg.serial_data().ClassID()-1 : %d handlers_.size:%d addr:%I64u\n", msg.serial_data().ClassID()-1, handlers_.size(), &handlers_);
        if ( msg.serial_data().ClassID() <= handlers_.size() )
        {
            auto addr = &handlers_;
             
            if( handlers_[msg.serial_data().ClassID()-1] != nullptr )
                handlers_[msg.serial_data().ClassID()-1](p_conn, msg);
        }
       // if( msg.serial_data().tot_size() + sizeof(MessageHeader) != len ) // forward
       // {
       //     TSystem::Message msg1(buf + sizeof(communication::SizeHeaderType), len - sizeof(communication::SizeHeaderType));

       //     printf("msg1.serial_data().ClassID()-1 : %d handlers_.size:%d addr:%I64u\n", msg1.serial_data().ClassID()-1, handlers_.size(), &handlers_);
       //     if ( msg1.serial_data().ClassID() <= handlers_.size() )
       //     {
       //         auto addr = &handlers_;
       //      
       //         if( handlers_[msg1.serial_data().ClassID()-1] != nullptr )
       //             handlers_[msg1.serial_data().ClassID()-1](p_conn, msg1);
       //     }

       // }else
       // {
       //     printf("msg.serial_data().ClassID()-1 : %d handlers_.size:%d addr:%I64u\n", msg.serial_data().ClassID()-1, handlers_.size(), &handlers_);
       //     if ( msg.serial_data().ClassID() <= handlers_.size() )
       //     {
       //         auto addr = &handlers_;
       //      
       //         if( handlers_[msg.serial_data().ClassID()-1] != nullptr )
       //             handlers_[msg.serial_data().ClassID()-1](p_conn, msg);
       //     }
       //}
    };
}

std::string MessageHandlerGroup::PrintHandler() const
{
    std::string  str = utility::FormatStr("handlers size:%d ", handlers_.size());
    for( auto i = 0; i < handlers_.size(); ++i )
    {
        if( handlers_[i] != nullptr)
        {
            str += utility::FormatStr("%d ", i);
        }
    }
    return str;
}
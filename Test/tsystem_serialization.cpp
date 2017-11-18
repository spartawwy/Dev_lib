#include "TLib/core/tsystem_serialization.h"

#include <iostream>

#include <google/protobuf/message.h>

#include "TLib/core/tsystem_communication_common.h"
#include "TLib/core/tsystem_message_system.h"

using namespace TSystem;

template<>
bool TSystem::Decode(const SerialData& data, google::protobuf::MessageLite& object
		, const MessageSystem& msg_system)
{
    // todo:
    std::cout << "class id:" << data.ClassID() << " type Name:" << object.GetTypeName() << std::endl;
    std::cout << "data.tot_size:" << data.tot_size() << " data_size:" << data.data_size() << std::endl;

    if( data.ClassID() == msg_system.FindClassID(object.GetTypeName()) )
    {
        return object.ParseFromArray(data.coded_data(), data.data_size());
    }else
    {
        return false;
    }
}

std::pair<communication::CodedDataPtr, char*> 
		TSystem::EncodeHelper(const google::protobuf::MessageLite& object, unsigned int ext_header_length, const MessageSystem& msg_system)
{
    // ndchk:
    auto cls_id = msg_system.FindClassID(object.GetTypeName());

    communication::CodedDataPtr p_targ = std::make_shared<communication::CodedData>( ext_header_length + sizeof(cls_id) + object.ByteSize() ); 
    memcpy(p_targ->data() + ext_header_length, &cls_id, sizeof(cls_id));
    auto ck_sz = object.ByteSize();
    object.SerializeToArray(p_targ->data() + ext_header_length + sizeof(cls_id), object.ByteSize());
    return std::make_pair(p_targ, p_targ->data());
}

std::pair<communication::CodedDataPtr, char*> 
	TSystem::EncodeHelper(const SerialData& data, unsigned int ext_header_length)
{
    // ndchk:
    communication::CodedDataPtr p_targ 
        = std::make_shared<communication::CodedData>(  ext_header_length + data.tot_size() ); 
    memcpy(p_targ->data() + ext_header_length, data.data(), data.tot_size());
    return std::make_pair(p_targ, p_targ->data());
}
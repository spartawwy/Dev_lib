#include "TLib/core/tsystem_return_code.h"

void TSystem::FillReturnCode(int code, const std::string& str, ReturnCode& msg)
{
    // todo:
}

void TSystem::FillReturnCode(int code, const char* str, ReturnCode& msg)
{
    // todo:
}

// request ok(code 0)
void TSystem::FillRequestAck(int request_id, RequestAck& msg)
{

}

// request code with message
void TSystem::FillRequestAck(int request_id, int code, const std::string& str, RequestAck& msg)
{
    // todo:
}

void TSystem::FillRequestAck(int request_id, int code, const char* str, RequestAck& msg)
{
    // todo:
}

//------------------------------
// fill request without request id
// a default constructed request ack means request ok without request id
//------------------------------

// request code with message
void TSystem::FillRequestAck(int code, const std::string& str, RequestAck& msg)
{
    // todo:
}

void TSystem::FillRequestAck(int code, const char* str, RequestAck& msg)
{
    // todo:
}

// to string
std::string TSystem::ToString( const ReturnCode& code)
{
    // todo:
    return "";
}
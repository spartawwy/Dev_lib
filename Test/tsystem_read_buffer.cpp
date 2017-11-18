#include "TLib/core/tsystem_read_buffer.h"

using namespace TSystem;
using namespace TSystem::utility;

ReadBuffer::ReadBuffer( std::size_t buf_size, std::size_t header_len)
    : header_length_(header_len)
    , valid_ofset_(0)
    , valid_size_(buf_size)
    , cur_pos_(0)
    , last_header_parsed_(-1)
{
    read_buf_.resize(buf_size); 
    memset(&read_buf_[0], 0, buf_size);//ndchk
}

ReadBuffer::~ReadBuffer()
{
    // todo:
}

std::tuple<const char*, std::size_t> ReadBuffer::ParseOneMessage(std::size_t trans_len)
{
    //auto pre_valid_ofset = valid_ofset_;
    valid_ofset_ += trans_len;
    valid_size_ -= trans_len;
     
    last_header_parsed_ = *(unsigned int*)&read_buf_[cur_pos_];

    if( valid_ofset_ - cur_pos_  >= last_header_parsed_ + sizeof(last_header_parsed_) )
        return std::make_tuple( &read_buf_[cur_pos_+sizeof(last_header_parsed_)], last_header_parsed_ );
    else
        return std::make_tuple( nullptr, 0 );
}

void ReadBuffer::RearrageBuffer(std::size_t ofset, std::size_t length)
{
    // todo:
}
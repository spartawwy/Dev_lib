// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: tsystem_return_code.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include <TLib/core/tsystem_return_code.pb.h>

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
// @@protoc_insertion_point(includes)

void protobuf_ShutdownFile_TLib_2fcore_2ftsystem_5freturn_5fcode_2eproto() {
  delete ReturnCode::default_instance_;
  delete ReturnPrint::default_instance_;
  delete RequestAck::default_instance_;
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
void protobuf_AddDesc_tsystem_5freturn_5fcode_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#else
void protobuf_AddDesc_TLib_2fcore_2ftsystem_5freturn_5fcode_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#endif
  ReturnCode::default_instance_ = new ReturnCode();
  ReturnPrint::default_instance_ = new ReturnPrint();
  RequestAck::default_instance_ = new RequestAck();
  ReturnCode::default_instance_->InitAsDefaultInstance();
  ReturnPrint::default_instance_->InitAsDefaultInstance();
  RequestAck::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_TLib_2fcore_2ftsystem_5freturn_5fcode_2eproto);
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_tsystem_5freturn_5fcode_2eproto_once_);
void protobuf_AddDesc_tsystem_5freturn_5fcode_2eproto() {
  ::google::protobuf::::google::protobuf::GoogleOnceInit(&protobuf_AddDesc_tsystem_5freturn_5fcode_2eproto_once_,
                 &protobuf_AddDesc_tsystem_5freturn_5fcode_2eproto_impl);
}
#else
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_tsystem_5freturn_5fcode_2eproto {
  StaticDescriptorInitializer_tsystem_5freturn_5fcode_2eproto() {
    protobuf_AddDesc_TLib_2fcore_2ftsystem_5freturn_5fcode_2eproto();
  }
} static_descriptor_initializer_tsystem_5freturn_5fcode_2eproto_;
#endif

// ===================================================================

#ifndef _MSC_VER
const int ReturnCode::kCodeFieldNumber;
const int ReturnCode::kErrorMsgFieldNumber;
#endif  // !_MSC_VER

ReturnCode::ReturnCode()
  : ::google::protobuf::MessageLite() {
  SharedCtor();
}

void ReturnCode::InitAsDefaultInstance() {
}

ReturnCode::ReturnCode(const ReturnCode& from)
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  MergeFrom(from);
}

void ReturnCode::SharedCtor() {
  _cached_size_ = 0;
  code_ = 0;
  error_msg_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ReturnCode::~ReturnCode() {
  SharedDtor();
}

void ReturnCode::SharedDtor() {
  if (error_msg_ != &::google::protobuf::internal::kEmptyString) {
    delete error_msg_;
  }
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
  }
}

void ReturnCode::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ReturnCode& ReturnCode::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_tsystem_5freturn_5fcode_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_TLib_2fcore_2ftsystem_5freturn_5fcode_2eproto();
#endif
  return *default_instance_;
}

ReturnCode* ReturnCode::default_instance_ = NULL;

ReturnCode* ReturnCode::New() const {
  return new ReturnCode;
}

void ReturnCode::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    code_ = 0;
    if (has_error_msg()) {
      if (error_msg_ != &::google::protobuf::internal::kEmptyString) {
        error_msg_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

bool ReturnCode::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 code = 1 [default = 0];
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &code_)));
          set_has_code();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_error_msg;
        break;
      }

      // optional string error_msg = 2 [default = ""];
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_error_msg:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_error_msg()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ReturnCode::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 code = 1 [default = 0];
  if (has_code()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->code(), output);
  }

  // optional string error_msg = 2 [default = ""];
  if (has_error_msg()) {
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->error_msg(), output);
  }

}

int ReturnCode::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 code = 1 [default = 0];
    if (has_code()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->code());
    }

    // optional string error_msg = 2 [default = ""];
    if (has_error_msg()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->error_msg());
    }

  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ReturnCode::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const ReturnCode*>(&from));
}

void ReturnCode::MergeFrom(const ReturnCode& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_code()) {
      set_code(from.code());
    }
    if (from.has_error_msg()) {
      set_error_msg(from.error_msg());
    }
  }
}

void ReturnCode::CopyFrom(const ReturnCode& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ReturnCode::IsInitialized() const {

  return true;
}

void ReturnCode::Swap(ReturnCode* other) {
  if (other != this) {
    std::swap(code_, other->code_);
    std::swap(error_msg_, other->error_msg_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string ReturnCode::GetTypeName() const {
  return "ReturnCode";
}


// ===================================================================

#ifndef _MSC_VER
const int ReturnPrint::kStdOutFieldNumber;
const int ReturnPrint::kStdErrFieldNumber;
#endif  // !_MSC_VER

ReturnPrint::ReturnPrint()
  : ::google::protobuf::MessageLite() {
  SharedCtor();
}

void ReturnPrint::InitAsDefaultInstance() {
}

ReturnPrint::ReturnPrint(const ReturnPrint& from)
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  MergeFrom(from);
}

void ReturnPrint::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ReturnPrint::~ReturnPrint() {
  SharedDtor();
}

void ReturnPrint::SharedDtor() {
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
  }
}

void ReturnPrint::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ReturnPrint& ReturnPrint::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_tsystem_5freturn_5fcode_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_TLib_2fcore_2ftsystem_5freturn_5fcode_2eproto();
#endif
  return *default_instance_;
}

ReturnPrint* ReturnPrint::default_instance_ = NULL;

ReturnPrint* ReturnPrint::New() const {
  return new ReturnPrint;
}

void ReturnPrint::Clear() {
  std_out_.Clear();
  std_err_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

bool ReturnPrint::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated string std_out = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_std_out:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_std_out()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_std_out;
        if (input->ExpectTag(18)) goto parse_std_err;
        break;
      }

      // repeated string std_err = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_std_err:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_std_err()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_std_err;
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ReturnPrint::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated string std_out = 1;
  for (int i = 0; i < this->std_out_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->std_out(i), output);
  }

  // repeated string std_err = 2;
  for (int i = 0; i < this->std_err_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->std_err(i), output);
  }

}

int ReturnPrint::ByteSize() const {
  int total_size = 0;

  // repeated string std_out = 1;
  total_size += 1 * this->std_out_size();
  for (int i = 0; i < this->std_out_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->std_out(i));
  }

  // repeated string std_err = 2;
  total_size += 1 * this->std_err_size();
  for (int i = 0; i < this->std_err_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->std_err(i));
  }

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ReturnPrint::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const ReturnPrint*>(&from));
}

void ReturnPrint::MergeFrom(const ReturnPrint& from) {
  GOOGLE_CHECK_NE(&from, this);
  std_out_.MergeFrom(from.std_out_);
  std_err_.MergeFrom(from.std_err_);
}

void ReturnPrint::CopyFrom(const ReturnPrint& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ReturnPrint::IsInitialized() const {

  return true;
}

void ReturnPrint::Swap(ReturnPrint* other) {
  if (other != this) {
    std_out_.Swap(&other->std_out_);
    std_err_.Swap(&other->std_err_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string ReturnPrint::GetTypeName() const {
  return "ReturnPrint";
}


// ===================================================================

#ifndef _MSC_VER
const int RequestAck::kRequestIdFieldNumber;
const int RequestAck::kResultFieldNumber;
const int RequestAck::kPrintFieldNumber;
#endif  // !_MSC_VER

RequestAck::RequestAck()
  : ::google::protobuf::MessageLite() {
  SharedCtor();
}

void RequestAck::InitAsDefaultInstance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  result_ = const_cast< ::ReturnCode*>(
      ::ReturnCode::internal_default_instance());
#else
  result_ = const_cast< ::ReturnCode*>(&::ReturnCode::default_instance());
#endif
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  print_ = const_cast< ::ReturnPrint*>(
      ::ReturnPrint::internal_default_instance());
#else
  print_ = const_cast< ::ReturnPrint*>(&::ReturnPrint::default_instance());
#endif
}

RequestAck::RequestAck(const RequestAck& from)
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  MergeFrom(from);
}

void RequestAck::SharedCtor() {
  _cached_size_ = 0;
  request_id_ = 0u;
  result_ = NULL;
  print_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

RequestAck::~RequestAck() {
  SharedDtor();
}

void RequestAck::SharedDtor() {
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
    delete result_;
    delete print_;
  }
}

void RequestAck::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const RequestAck& RequestAck::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_tsystem_5freturn_5fcode_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_TLib_2fcore_2ftsystem_5freturn_5fcode_2eproto();
#endif
  return *default_instance_;
}

RequestAck* RequestAck::default_instance_ = NULL;

RequestAck* RequestAck::New() const {
  return new RequestAck;
}

void RequestAck::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    request_id_ = 0u;
    if (has_result()) {
      if (result_ != NULL) result_->::ReturnCode::Clear();
    }
    if (has_print()) {
      if (print_ != NULL) print_->::ReturnPrint::Clear();
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

bool RequestAck::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional uint32 request_id = 1 [default = 0];
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &request_id_)));
          set_has_request_id();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_result;
        break;
      }

      // optional .ReturnCode result = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_result:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_result()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_print;
        break;
      }

      // optional .ReturnPrint print = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_print:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_print()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void RequestAck::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional uint32 request_id = 1 [default = 0];
  if (has_request_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->request_id(), output);
  }

  // optional .ReturnCode result = 2;
  if (has_result()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessage(
      2, this->result(), output);
  }

  // optional .ReturnPrint print = 3;
  if (has_print()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessage(
      3, this->print(), output);
  }

}

int RequestAck::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional uint32 request_id = 1 [default = 0];
    if (has_request_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->request_id());
    }

    // optional .ReturnCode result = 2;
    if (has_result()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->result());
    }

    // optional .ReturnPrint print = 3;
    if (has_print()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->print());
    }

  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void RequestAck::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const RequestAck*>(&from));
}

void RequestAck::MergeFrom(const RequestAck& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_request_id()) {
      set_request_id(from.request_id());
    }
    if (from.has_result()) {
      mutable_result()->::ReturnCode::MergeFrom(from.result());
    }
    if (from.has_print()) {
      mutable_print()->::ReturnPrint::MergeFrom(from.print());
    }
  }
}

void RequestAck::CopyFrom(const RequestAck& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RequestAck::IsInitialized() const {

  return true;
}

void RequestAck::Swap(RequestAck* other) {
  if (other != this) {
    std::swap(request_id_, other->request_id_);
    std::swap(result_, other->result_);
    std::swap(print_, other->print_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string RequestAck::GetTypeName() const {
  return "RequestAck";
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)

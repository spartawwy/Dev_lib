// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: tsystem_time.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION

#include <TLib/core/tsystem_time.pb.h>

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
// @@protoc_insertion_point(includes)

void protobuf_ShutdownFile_TLib_2fcore_2ftsystem_5ftime_2eproto() {
  delete Time::default_instance_;
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
void protobuf_AddDesc_tsystem_5ftime_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#else
void protobuf_AddDesc_TLib_2fcore_2ftsystem_5ftime_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#endif
  Time::default_instance_ = new Time();
  Time::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_TLib_2fcore_2ftsystem_5ftime_2eproto);
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_tsystem_5ftime_2eproto_once_);
void protobuf_AddDesc_tsystem_5ftime_2eproto() {
  ::google::protobuf::::google::protobuf::GoogleOnceInit(&protobuf_AddDesc_tsystem_5ftime_2eproto_once_,
                 &protobuf_AddDesc_tsystem_5ftime_2eproto_impl);
}
#else
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_tsystem_5ftime_2eproto {
  StaticDescriptorInitializer_tsystem_5ftime_2eproto() {
    protobuf_AddDesc_TLib_2fcore_2ftsystem_5ftime_2eproto();
  }
} static_descriptor_initializer_tsystem_5ftime_2eproto_;
#endif

// ===================================================================

#ifndef _MSC_VER
const int Time::kTimeValueFieldNumber;
const int Time::kFracSecFieldNumber;
#endif  // !_MSC_VER

Time::Time()
  : ::google::protobuf::MessageLite() {
  SharedCtor();
}

void Time::InitAsDefaultInstance() {
}

Time::Time(const Time& from)
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  MergeFrom(from);
}

void Time::SharedCtor() {
  _cached_size_ = 0;
  time_value_ = GOOGLE_LONGLONG(0);
  frac_sec_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Time::~Time() {
  SharedDtor();
}

void Time::SharedDtor() {
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
  }
}

void Time::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const Time& Time::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_tsystem_5ftime_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_TLib_2fcore_2ftsystem_5ftime_2eproto();
#endif
  return *default_instance_;
}

Time* Time::default_instance_ = NULL;

Time* Time::New() const {
  return new Time;
}

void Time::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    time_value_ = GOOGLE_LONGLONG(0);
    frac_sec_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

bool Time::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int64 time_value = 1 [default = 0];
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, &time_value_)));
          set_has_time_value();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(17)) goto parse_frac_sec;
        break;
      }

      // optional double frac_sec = 2 [default = 0];
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_frac_sec:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   double, ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE>(
                 input, &frac_sec_)));
          set_has_frac_sec();
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

void Time::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int64 time_value = 1 [default = 0];
  if (has_time_value()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64(1, this->time_value(), output);
  }

  // optional double frac_sec = 2 [default = 0];
  if (has_frac_sec()) {
    ::google::protobuf::internal::WireFormatLite::WriteDouble(2, this->frac_sec(), output);
  }

}

int Time::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int64 time_value = 1 [default = 0];
    if (has_time_value()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int64Size(
          this->time_value());
    }

    // optional double frac_sec = 2 [default = 0];
    if (has_frac_sec()) {
      total_size += 1 + 8;
    }

  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Time::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const Time*>(&from));
}

void Time::MergeFrom(const Time& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_time_value()) {
      set_time_value(from.time_value());
    }
    if (from.has_frac_sec()) {
      set_frac_sec(from.frac_sec());
    }
  }
}

void Time::CopyFrom(const Time& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Time::IsInitialized() const {

  return true;
}

void Time::Swap(Time* other) {
  if (other != this) {
    std::swap(time_value_, other->time_value_);
    std::swap(frac_sec_, other->frac_sec_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string Time::GetTypeName() const {
  return "Time";
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)

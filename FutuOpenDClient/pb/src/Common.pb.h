// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Common.proto

#ifndef PROTOBUF_Common_2eproto__INCLUDED
#define PROTOBUF_Common_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3005001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace protobuf_Common_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
void InitDefaultsPacketIDImpl();
void InitDefaultsPacketID();
inline void InitDefaults() {
  InitDefaultsPacketID();
}
}  // namespace protobuf_Common_2eproto
namespace Common {
class PacketID;
class PacketIDDefaultTypeInternal;
extern PacketIDDefaultTypeInternal _PacketID_default_instance_;
}  // namespace Common
namespace Common {

enum RetType {
  RetType_Succeed = 0,
  RetType_Failed = -1,
  RetType_TimeOut = -100,
  RetType_Unknown = -400
};
bool RetType_IsValid(int value);
const RetType RetType_MIN = RetType_Unknown;
const RetType RetType_MAX = RetType_Succeed;
const int RetType_ARRAYSIZE = RetType_MAX + 1;

const ::google::protobuf::EnumDescriptor* RetType_descriptor();
inline const ::std::string& RetType_Name(RetType value) {
  return ::google::protobuf::internal::NameOfEnum(
    RetType_descriptor(), value);
}
inline bool RetType_Parse(
    const ::std::string& name, RetType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<RetType>(
    RetType_descriptor(), name, value);
}
enum PacketEncAlgo {
  PacketEncAlgo_FTAES_ECB = 0,
  PacketEncAlgo_None = -1,
  PacketEncAlgo_AES_ECB = 1
};
bool PacketEncAlgo_IsValid(int value);
const PacketEncAlgo PacketEncAlgo_MIN = PacketEncAlgo_None;
const PacketEncAlgo PacketEncAlgo_MAX = PacketEncAlgo_AES_ECB;
const int PacketEncAlgo_ARRAYSIZE = PacketEncAlgo_MAX + 1;

const ::google::protobuf::EnumDescriptor* PacketEncAlgo_descriptor();
inline const ::std::string& PacketEncAlgo_Name(PacketEncAlgo value) {
  return ::google::protobuf::internal::NameOfEnum(
    PacketEncAlgo_descriptor(), value);
}
inline bool PacketEncAlgo_Parse(
    const ::std::string& name, PacketEncAlgo* value) {
  return ::google::protobuf::internal::ParseNamedEnum<PacketEncAlgo>(
    PacketEncAlgo_descriptor(), name, value);
}
// ===================================================================

class PacketID : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:Common.PacketID) */ {
 public:
  PacketID();
  virtual ~PacketID();

  PacketID(const PacketID& from);

  inline PacketID& operator=(const PacketID& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  PacketID(PacketID&& from) noexcept
    : PacketID() {
    *this = ::std::move(from);
  }

  inline PacketID& operator=(PacketID&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const PacketID& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const PacketID* internal_default_instance() {
    return reinterpret_cast<const PacketID*>(
               &_PacketID_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(PacketID* other);
  friend void swap(PacketID& a, PacketID& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline PacketID* New() const PROTOBUF_FINAL { return New(NULL); }

  PacketID* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const PacketID& from);
  void MergeFrom(const PacketID& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(PacketID* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required uint64 connID = 1;
  bool has_connid() const;
  void clear_connid();
  static const int kConnIDFieldNumber = 1;
  ::google::protobuf::uint64 connid() const;
  void set_connid(::google::protobuf::uint64 value);

  // required uint32 serialNo = 2;
  bool has_serialno() const;
  void clear_serialno();
  static const int kSerialNoFieldNumber = 2;
  ::google::protobuf::uint32 serialno() const;
  void set_serialno(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:Common.PacketID)
 private:
  void set_has_connid();
  void clear_has_connid();
  void set_has_serialno();
  void clear_has_serialno();

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::uint64 connid_;
  ::google::protobuf::uint32 serialno_;
  friend struct ::protobuf_Common_2eproto::TableStruct;
  friend void ::protobuf_Common_2eproto::InitDefaultsPacketIDImpl();
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// PacketID

// required uint64 connID = 1;
inline bool PacketID::has_connid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PacketID::set_has_connid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PacketID::clear_has_connid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PacketID::clear_connid() {
  connid_ = GOOGLE_ULONGLONG(0);
  clear_has_connid();
}
inline ::google::protobuf::uint64 PacketID::connid() const {
  // @@protoc_insertion_point(field_get:Common.PacketID.connID)
  return connid_;
}
inline void PacketID::set_connid(::google::protobuf::uint64 value) {
  set_has_connid();
  connid_ = value;
  // @@protoc_insertion_point(field_set:Common.PacketID.connID)
}

// required uint32 serialNo = 2;
inline bool PacketID::has_serialno() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void PacketID::set_has_serialno() {
  _has_bits_[0] |= 0x00000002u;
}
inline void PacketID::clear_has_serialno() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void PacketID::clear_serialno() {
  serialno_ = 0u;
  clear_has_serialno();
}
inline ::google::protobuf::uint32 PacketID::serialno() const {
  // @@protoc_insertion_point(field_get:Common.PacketID.serialNo)
  return serialno_;
}
inline void PacketID::set_serialno(::google::protobuf::uint32 value) {
  set_has_serialno();
  serialno_ = value;
  // @@protoc_insertion_point(field_set:Common.PacketID.serialNo)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace Common

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::Common::RetType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Common::RetType>() {
  return ::Common::RetType_descriptor();
}
template <> struct is_proto_enum< ::Common::PacketEncAlgo> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Common::PacketEncAlgo>() {
  return ::Common::PacketEncAlgo_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Common_2eproto__INCLUDED

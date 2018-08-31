#include "APIProtoHeader.h"

#include <memory.h>

using namespace ftq;
using namespace std;

namespace ftq
{

int APIProtoHeader::next_packet_no_ = 0;

void APIProtoHeader::init(int proto_id)
{
    memset(this, 0, sizeof(*this));
    header_flag_[0] = 'F';
    header_flag_[1] = 'T';

    proto_id_ = proto_id;

    proto_fmt_type_ = 0; //pb
    proto_ver_ = 0;
    serial_no_ = ++next_packet_no_;
}

APIProtoHeader::APIProtoHeader()
{

}

APIProtoHeader::APIProtoHeader(int proto_id)
{
    init(proto_id);
}

const u8_t* APIProtoHeader::get_sha() const
{
    return body_sha1_;
}

void APIProtoHeader::set_sha(u8_t * sha)
{
    memcpy(body_sha1_, sha, sizeof(body_sha1_));
}

const u32_t APIProtoHeader::get_body_len() const
{
    return body_len_;
}

void APIProtoHeader::set_body_len(u32_t body_len)
{
    body_len_ = body_len;
}

const u32_t APIProtoHeader::get_proto_id() const
{
    return proto_id_;
}

const u8_t APIProtoHeader::get_fmt_type() const
{
    return proto_fmt_type_;
}

const u32_t APIProtoHeader::get_serial_no() const
{
    return serial_no_;
}


}


#pragma once

#include "SHA1.h"

namespace ftq
{

#pragma pack(push, APIProtoHeader, 1)
class APIProtoHeader
{
private:
    u8_t header_flag_[2];     //包头起始标志，固定为“FT”
    u32_t proto_id_;           //协议ID
    u8_t proto_fmt_type_;       //协议格式类型，0为Protobuf格式，1为Json格式
    u8_t proto_ver_;           //协议版本，用于迭代兼容
    u32_t serial_no_;          //包序列号
    u32_t body_len_;           //包体长度
    u8_t body_sha1_[SHA_LEN];     //包体原数据(解密后)的SHA1哈希值
    u8_t reserved_[8];      //保留8字节扩展

public:
    void init(int proto_id);
    APIProtoHeader();
    APIProtoHeader(int proto_id);

    const u8_t* get_sha() const;
    void set_sha(u8_t* sha);

    const u32_t get_body_len() const;
    void set_body_len(u32_t body_len);

    const u32_t get_proto_id() const;
    const u8_t get_fmt_type() const;

    const u32_t get_serial_no() const;

private:
    static int next_packet_no_;
};
#pragma pack(pop, APIProtoHeader)

}

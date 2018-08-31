#pragma once

#include "Common.h"

namespace ftq
{

#pragma pack(push, APIProtoHeader, 1)
class APIProtoHeader
{
public:
    u8_t header_flag_[2];     //包头起始标志，固定为“FT”
    u32_t proto_id_;           //协议ID
    u8_t proto_fmt_type_;       //协议格式类型，0为Protobuf格式，1为Json格式
    u8_t proto_ver_;           //协议版本，用于迭代兼容
    u32_t serial_no_;          //包序列号
    u32_t body_len_;           //包体长度
    u8_t body_sha1_[20];     //包体原数据(解密后)的SHA1哈希值
    u8_t reserved_[8];      //保留8字节扩展

public:
    void init();
    APIProtoHeader();
    APIProtoHeader(int proto_id);
};
#pragma pack(pop, APIProtoHeader)

}

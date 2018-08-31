#pragma once

#include "APIProtoHeader.h"

namespace ftq
{

#pragma pack(push, APIProtoPkg, 1)
class APIProtoPkg
{
public:
    APIProtoPkg(int id);
    int set_body(std::string b);
    int set_body_with_rsa(std::string b, std::string pub_file);
    int set_body_with_aes(std::string b, std::string aes_key);
    APIProtoHeader head_;
    uint8_t body_[40960];
    int size_;
};
#pragma pack(pop, APIProtoPkg)

}

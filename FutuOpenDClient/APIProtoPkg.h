#pragma once

#include "APIProtoHeader.h"

namespace ftq
{

enum API_PKG_MODE
{
    API_MODE_NO_ENCRYPE = 0,
    API_MODE_USE_RSA,
    API_MODE_USE_AES,
};


enum API_PKG_ERR
{
    API_ERR_NO_ERROR = 0,
    API_ERR_HEAD_PROTO_ERR,
    API_ERR_BODY_LEN_IS_NOT_ENOUGH,
    API_ERR_SHA_ERR,
};


#pragma pack(push, APIProtoPkg, 1)
class APIProtoPkg
{
private:
    APIProtoHeader head_;
    uint8_t body_[40960];

    int set_body(std::string b);
    int set_body_with_rsa(std::string b, std::string pub_file);
    int set_body_with_aes(std::string b, std::string aes_key);

    APIProtoPkg(int id);

    int size_;

    API_PKG_ERR try_copy_and_check_len(char * data, int data_len);
    API_PKG_ERR check_sha();

public:
    APIProtoPkg() {};

    API_PKG_ERR recv_with_rsa(char * data, int data_len, std::string rsa_key_path);
    API_PKG_ERR recv_with_aes(char * data, int data_len, std::string aes_key);

    const char* get_head_pointer() const;
    const char* get_body_pointer() const;
    const int get_body_len() const;

};
#pragma pack(pop, APIProtoPkg)

}

#include "APIProtoPkg.h"
#include "SHA1.h"
#include "myrsa.h"
#include "myaes.h"

namespace ftq
{

APIProtoPkg::APIProtoPkg(int id): head(id), size(0)
{
    memset(body, 0, sizeof(body));
}

int APIProtoPkg::set_body(std::string b)
{
    head.body_len_ = b.size();
    memcpy(body, b.data(), head.body_len_);
    SHA1((char*)head.body_sha1_, (char*)body, head.body_len_);
    //SHA1((unsigned char*)head.arrBodySHA1, head.body_len_, (unsigned char*)body);
    size = sizeof(head) + head.body_len_ + 1;

    return size;
}

int APIProtoPkg::set_body(std::string b, std::string pub_file)
{
    set_body(b);
    int a = my_encrypt_pub((char*)body, strlen((char*)body), pub_file.c_str(), (char*)body);
    head.body_len_ = a;
    size = sizeof(head) + head.body_len_ + 1;

    return size;
}

int APIProtoPkg::set_body_with_aes(std::string b, std::string aes_key)
{
    set_body(b);
    head.body_len_ = my_encrypt_aes(body, head.body_len_, aes_key, body);
    size = sizeof(head) + head.body_len_ + 1;
    LOGD("size %d body %d all %d", size, head.body_len_, sizeof(head) + head.body_len_ + 1);

    return size;
}

}

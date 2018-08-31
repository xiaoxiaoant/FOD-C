#include "APIProtoPkg.h"
#include "SHA1.h"
#include "myrsa.h"
#include "myaes.h"

namespace ftq
{

APIProtoPkg::APIProtoPkg(int id): head_(id), size_(0)
{
    memset(body_, 0, sizeof(body_));
}

int APIProtoPkg::set_body(std::string b)
{
    u32_t len = static_cast<u32_t>(b.size());
    head_.set_body_len(len);
    memcpy(body_, b.data(), len);
    SHA1((char*)head_.get_sha(), (char*)body_, len);
    //SHA1((unsigned char*)head.arrBodySHA1, head.body_len_, (unsigned char*)body);
    size_ = sizeof(head_) + len + 1;

    return size_;
}

int APIProtoPkg::set_body_with_rsa(std::string b, std::string pub_file)
{
    set_body(b);
    int a = my_encrypt_pub((char*)body_, static_cast<int>(strlen((char*)body_)), pub_file.c_str(), (char*)body_);
    head_.set_body_len(a);
    size_ = sizeof(head_) + head_.get_body_len() + 1;

    return size_;
}

int APIProtoPkg::set_body_with_aes(std::string b, std::string aes_key)
{
    set_body(b);
    u32_t len = my_encrypt_aes(body_, head_.get_body_len(), aes_key, body_);
    head_.set_body_len(len);
    size_ = sizeof(head_) + len + 1;
    LOGD("size %d body %d all %d", size_, len, sizeof(head_) + len + 1);

    return size_;
}

}

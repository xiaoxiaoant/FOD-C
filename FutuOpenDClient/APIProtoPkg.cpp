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
    char sha[SHA_LEN];
    SHA1(sha, (char*)body_, len);
    head_.set_sha(reinterpret_cast<u8_t*>(sha));
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



API_PKG_ERR APIProtoPkg::recv_with_rsa(char * data, int data_len, std::string rsa_key_path)
{
    auto ret = try_copy_and_check_len(data, data_len);

    if (API_ERR_NO_ERROR != ret)
    {
        return ret;
    }

    auto original_body_len_ = head_.get_body_len();
    size_ = original_body_len_ + sizeof(APIProtoHeader);

    {
        LOGT("aes_key is NULL, use rsa_key.txt to decrypt");
        int de_ret1 = my_decrypt_pri((char*)body_, original_body_len_, rsa_key_path.c_str(), (char*)body_);
        LOGT("de_ret: %d", de_ret1);
        head_.set_body_len(de_ret1);
    }

    ret = check_sha();
    if (API_ERR_NO_ERROR != ret)
    {
        return ret;
    }


    return API_ERR_NO_ERROR;
}

API_PKG_ERR APIProtoPkg::recv_with_aes(char * data, int data_len, std::string aes_key)
{
    auto ret = try_copy_and_check_len(data, data_len);

    if (API_ERR_NO_ERROR != ret)
    {
        return ret;
    }

    auto original_body_len_ = head_.get_body_len();
    size_ = original_body_len_ + sizeof(APIProtoHeader);

    {
        unsigned char out[40960] = {0};

        int mod_len = body_[original_body_len_ - 1];

        int body_real_len = original_body_len_ - 16;
        int body_part = body_real_len / 16;

        LOGT("size %d b_len %d m_len %d body_rlen %d b_part %d", nLen, header.body_len_, mod_len, body_real_len, body_part);

        AES_KEY dkey;
        AES_set_decrypt_key((unsigned char*)aes_key.c_str(), 128, &dkey);
        LOGT("use aes_key %s to decrypt", aes_key.c_str());

        for (int i = 0; i < body_part; i++)
            AES_ecb_encrypt((const unsigned char*)body_ + 16 * i, out + 16 * i, &dkey, AES_DECRYPT);
        memcpy((char*)body_, out, original_body_len_);

        if (mod_len == 0)
            head_.set_body_len(body_real_len);
        else
            head_.set_body_len(body_real_len - 16 + mod_len);
    }

    ret = check_sha();
    if (API_ERR_NO_ERROR != ret)
    {
        return ret;
    }


    return API_ERR_NO_ERROR;
}

const int APIProtoPkg::get_body_len() const
{
    return head_.get_body_len();
}

const int APIProtoPkg::get_whole_pkg_len() const
{
    return size_;
}

const char * APIProtoPkg::get_head_pointer() const
{
    return (char*)&head_;
}

const char * APIProtoPkg::get_body_pointer() const
{
    return (char*)body_;
}


API_PKG_ERR APIProtoPkg::try_copy_and_check_len(char * data, int data_len)
{
    if (data_len < (i32_t)sizeof(APIProtoHeader))
    {
        LOGE("len error, len is %d", data_len);
        return API_ERR_HEAD_PROTO_ERR;
    }

    LOGT("buffer starts with %c%c", pData[0], pData[1]);

    memcpy((char*)&head_, data, sizeof(APIProtoHeader));

    auto body_len = head_.get_body_len();
    LOGT("proto_id_ is %d, body_len_ is %d", header.get_proto_id(), body_len);
    //LOGT("aes_key is %s", conn_aes_key.c_str());

    if (data_len < (i32_t)sizeof(APIProtoHeader) + (i32_t)body_len)
    {
        LOGE("len error, len is %d, body_len_ is %d", data_len, body_len);
        return API_ERR_BODY_LEN_IS_NOT_ENOUGH;
    }

    memcpy(body_, data + sizeof(APIProtoHeader), body_len);

    return API_ERR_NO_ERROR;
}

API_PKG_ERR APIProtoPkg::check_sha()
{
    u8_t sha1[SHA_LEN] = {0};

    SHA1((char*)sha1, (const char *)body_, (int)head_.get_body_len());

    if (memcmp(sha1, head_.get_sha(), SHA_LEN) != 0)
    {
        LOGE("sha check fail");
        return API_ERR_SHA_ERR;
    }

    return API_ERR_NO_ERROR;
}


}

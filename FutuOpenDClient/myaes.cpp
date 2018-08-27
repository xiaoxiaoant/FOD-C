#include"myaes.h"
#include"DebugLog.h"

int my_encrypt_aes(const unsigned char *str, const int len, const std::string aes_key, unsigned char *en)
{
    if (!str || len <= 0)
    {
        DEBUGLOG("error! len is %d", len);
        return 0;
    }

    unsigned char tmp[2048] = {0};

    memcpy(tmp, str, len);

    memset(en, 0, len + 40);

    AES_KEY ekey;
    AES_set_encrypt_key((unsigned char*)aes_key.c_str(), 128, &ekey);

    int aes_part = (len - 1) / 16 + 1;
    en[(aes_part + 1) * 16 - 1] = len % 16;

    for (int i = 0; i < aes_part; i++)
    {
        AES_ecb_encrypt(tmp + i * 16, en + i * 16, &ekey, AES_ENCRYPT);
    }

    return (aes_part + 1) * 16;
}

int my_decrypt_aes(const unsigned char *str, const int len, const std::string aes_key, unsigned char *de)
{
    if (!str || len <= 0 || len % 16)
    {
        DEBUGLOG("error! len is %d", len);
        return 0;
    }

    AES_KEY ekey;
    AES_set_decrypt_key((unsigned char*)aes_key.c_str(), 128, &ekey);

    int aes_part = len / 16 - 1;

    for (int i = 0; i < aes_part; i++)
    {
        AES_ecb_encrypt(str + i * 16, de + i * 16, &ekey, AES_DECRYPT);
    }

    int mod_len = str[len - 1];
    if (mod_len == 0)
    {
        de[aes_part * 16] = 0;
        return aes_part * 16;
    }
    else
    {
        de[(aes_part - 1) * 16 + mod_len] = 0;
        return (aes_part - 1) * 16 + mod_len;
    }


}

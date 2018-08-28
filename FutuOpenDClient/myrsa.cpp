#include"myrsa.h"
#include"DebugLog.h"

int my_encrypt_pri(const char *str, const int flen, const char *path_key, char *en)
{
    RSA *p_rsa;
    FILE *file;
    int rsa_len;
    if ((file = fopen(path_key, "r")) == NULL) {
        perror("open key file error");
        return 0;
    }
    //if((p_rsa=PEM_read_RSA_PUBKEY(0,file,NULL,NULL,NULL))==NULL)
    //if((p_rsa=PEM_read_RSAPublicKey(0,file,NULL,NULL,NULL))==NULL)
    if ((p_rsa = PEM_read_RSAPrivateKey(file, NULL, NULL, NULL)) == NULL)
    //if((p_rsa=PEM_read_RSA_PRIKEY(file,NULL,NULL,NULL))==NULL)
    {
        //if((p_rsa=PEM_read_RSAPublicKey(file,NULL,NULL,NULL))==NULL){
        ERR_print_errors_fp(stdout);
        return 0;
    }

    //RSA_print_fp(stdout, p_rsa, 0);


    //flen=strlen(str);
    rsa_len = RSA_size(p_rsa);
    char *p_en = (char*)malloc(rsa_len + 1);
    memset(p_en, 0, rsa_len + 1);
    //int ret = RSA_public_encrypt(flen,(unsigned char *)str,(unsigned char*)p_en,p_rsa,RSA_PKCS1_PADDING);
    int ret = RSA_private_encrypt(flen, (unsigned char *)str, (unsigned char*)p_en, p_rsa, RSA_PKCS1_PADDING);
    LOGD("RSA_private_encrypt ret = %d flen %d rsa_len %d str %p p_en %p", ret, flen, rsa_len, str, p_en);
    if (ret < 0)
    {
        free(p_en);
        return 0;
    }
    memcpy(en, p_en, ret);
    en[ret] = 0;
    free(p_en);
    RSA_free(p_rsa);
    fclose(file);
    return ret;
    //return rsa_len;
}

int my_encrypt_pub(const char *str, const int flen, const char *path_key, char *en)
{
    RSA *p_rsa;
    FILE *file;
    int rsa_len;
    if ((file = fopen(path_key, "r")) == NULL) {
        perror("open key file error");
        return 0;
    }
    if ((p_rsa = PEM_read_RSA_PUBKEY(file, NULL, NULL, NULL)) == NULL)
    //if((p_rsa=PEM_read_RSAPublicKey(file,NULL,NULL,NULL))==NULL)
    //if((p_rsa=PEM_read_RSAPrivateKey(file,NULL,NULL,NULL))==NULL)
    {
        //if((p_rsa=PEM_read_RSAPublicKey(file,NULL,NULL,NULL))==NULL){
        ERR_print_errors_fp(stdout);
        return 0;
    }

    //RSA_print_fp(stdout, p_rsa, 0);


    //flen=strlen(str);
    rsa_len = RSA_size(p_rsa);
    char *p_en = (char*)malloc(rsa_len + 1);
    memset(p_en, 0, rsa_len + 1);
    int ret = RSA_public_encrypt(flen, (unsigned char *)str, (unsigned char*)p_en, p_rsa, RSA_PKCS1_PADDING);
    //int ret = RSA_private_encrypt(flen, (unsigned char *)str,(unsigned char*)p_en,p_rsa,RSA_PKCS1_PADDING);
    LOGD("RSA_public_encrypt ret = %d flen %d rsa_len %d str %p p_en %p", ret, flen, rsa_len, str, p_en);
    if (ret < 0)
    {
        free(p_en);
        return 0;
    }
    memcpy(en, p_en, ret);
    en[ret] = 0;
    free(p_en);
    RSA_free(p_rsa);
    fclose(file);
    return ret;
    //return rsa_len;
}

int my_decrypt_pri(const char *str, const int len, const char *path_key, char *de)
{
    RSA *p_rsa;
    FILE *file;
    int rsa_len;
    if ((file = fopen(path_key, "r")) == NULL) {
        perror("open key file error");
        return 0;
    }
    if ((p_rsa = PEM_read_RSAPrivateKey(file, NULL, NULL, NULL)) == NULL) {
        ERR_print_errors_fp(stdout);
        return 0;
    }

    //RSA_print_fp(stdout, p_rsa, 0);

    rsa_len = RSA_size(p_rsa);
    char *p_de = (char *)malloc(rsa_len + 1);
    memset(p_de, 0, rsa_len + 1);

    int n = int((len - 1) / 128.0) + 1;
    char *de_p = de;
    for (int i = 0; i < n; i++)
    {
        int ret_len = RSA_private_decrypt(rsa_len, (unsigned char *)str + 128 * i, (unsigned char*)p_de, p_rsa, RSA_PKCS1_PADDING);
        LOGD("RSA_private_decrypt ret = %d", ret_len);
        if (ret_len < 0)
        {
            free(p_de);
            return 0;
        }
        memcpy(de_p, p_de, ret_len);
        de_p += ret_len;
    }
    de_p[0] = 0;
    int ret = de_p - de;
    free(p_de);
    RSA_free(p_rsa);
    fclose(file);
    return ret;
    //return rsa_len;
}

int my_decrypt_pub(const char *str, const char *path_key, char *de)
{
    RSA *p_rsa;
    FILE *file;
    int rsa_len;
    if ((file = fopen(path_key, "r")) == NULL) {
        perror("open key file error");
        return 0;
    }
    //if((p_rsa=PEM_read_RSAPrivateKey(file,NULL,NULL,NULL))==NULL)
    if ((p_rsa = PEM_read_RSA_PUBKEY(file, NULL, NULL, NULL)) == NULL)
    {
        ERR_print_errors_fp(stdout);
        return 0;
    }

    //RSA_print_fp(stdout, p_rsa, 0);

    rsa_len = RSA_size(p_rsa);
    char *p_de = (char *)malloc(rsa_len + 1);
    memset(p_de, 0, rsa_len + 1);
    int ret = RSA_public_decrypt(rsa_len, (unsigned char *)str, (unsigned char*)p_de, p_rsa, RSA_PKCS1_PADDING);
    LOGD("RSA_private_decrypt ret = %d", ret);
    if (ret < 0)
    {
        free(p_de);
        return 0;
    }
    memcpy(de, p_de, ret);
    de[ret] = 0;
    free(p_de);
    RSA_free(p_rsa);
    fclose(file);
    return ret;
    //return rsa_len;
}


#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<openssl/rsa.h>
#include<openssl/pem.h>
#include<openssl/err.h>


int my_encrypt_pub(const char *str, const int flen, const char *path_key, char *p_en);
int my_encrypt_pri(const char *str, const int flen, const char *path_key, char *p_en);

//int my_decrypt_pri(const char *str, const char *path_key, char *p_de);
int my_decrypt_pri(const char *str, const int len, const char *path_key, char *de);
int my_decrypt_pub(const char *str, const char *path_key, char *de);


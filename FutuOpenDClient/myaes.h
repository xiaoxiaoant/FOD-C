#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<openssl/aes.h>
#include<openssl/pem.h>
#include<openssl/err.h>


int my_encrypt_aes(const unsigned char *str, const int len, const std::string aes_key, unsigned char *en);
int my_decrypt_aes(const unsigned char *str, const int len, const std::string aes_key, unsigned char *de);





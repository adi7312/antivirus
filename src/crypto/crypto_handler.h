#pragma once

#include <openssl/md5.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdio.h>
#define AES_KEY_SIZE 32


char* compute_md5(const char* filename);
int get_key(const char* filename,  const unsigned char* key);
void encrypt_file(const char* input_filename, const unsigned char* ukey);
char* rename_enc(const char* filename);
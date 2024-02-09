#pragma once

#include <openssl/md5.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <sqlite3.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#define AES_KEY_SIZE 32


char* compute_md5(const char* filename);
int find_signature_in_db(const char* hashstring, sqlite3 **db);
int connect_db(sqlite3 **db);
int scan(const char* filename, sqlite3 **db, const unsigned char* key);
void isolate(const char* filename, const char* hash,  const unsigned char* key);
void encrypt_file(const char* input_filename, const unsigned char* ukey);
void relocate(const char* filename, const char* hash);
void drop_privileges(const char* filename);
int get_key(const char* filename,  const unsigned char* key);
char* rename_enc(const char* filename);
#pragma once

#include <openssl/md5.h>

char* compute_md5(const char* filename);
int find_signature_in_db(const char* hashstring, sqlite3 **db);
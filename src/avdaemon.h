#pragma once

#include <sqlite3.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>


int scan(const char* filename, sqlite3 **db, const unsigned char* key);
void isolate(const char* filename, const char* hash,  const unsigned char* key);
void relocate(const char* filename, const char* hash);
void drop_privileges(const char* filename);


#pragma once

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#define AV_RESOURCES "/var/lib/av/resources/av.db"

int find_signature_in_db(const char* hashstring, sqlite3 **db);
int connect_db(sqlite3 **db);
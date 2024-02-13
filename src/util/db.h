#pragma once

#include <sqlite3.h>
#include <stdio.h>

int find_signature_in_db(const char* hashstring, sqlite3 **db);
int connect_db(sqlite3 **db);
#include "db.h"

int connect_db(sqlite3 **db)
{
    int rc = sqlite3_open("av.db", db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return 1;
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
}




int find_signature_in_db(const char* hashstring, sqlite3 **db){
    const char* sql;
    sqlite3_stmt *res;
    sql = "SELECT * FROM SIGNATURES WHERE MD5_SIGNATURE=?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res,0);
    if (rc == SQLITE_OK){
        sqlite3_bind_text(res, 1, hashstring, -1, SQLITE_STATIC);
    } else{
        fprintf(stderr, "Failed to execute SQL statement: %s\n",sqlite3_errmsg(db));
    }
    int step = sqlite3_step(res);
    sqlite3_finalize(res);
    free(sql);
    if (step == SQLITE_ROW){
        return 0;
    }
    return 1;
}
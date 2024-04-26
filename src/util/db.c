#include "db.h"
#include "../log/log.h"

int connect_db(sqlite3 **db)
{
    int rc = sqlite3_open(AV_RESOURCES, db);
    if (rc)
    {
        log(AV_LOG, ERROR, "Failed to open database");
        return 1;
    } 
    log(AV_LOG, SUCCESS, "Opened database successfully.");
    return 0;
    
}

int find_signature_in_db(const char* hashstring, sqlite3 **db){
    const char* sql;
    sql = "SELECT * FROM SIGNATURES WHERE MD5_SIGNATURE=?";
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(*db, sql, -1, &res,0);
    if (rc == SQLITE_OK){
        sqlite3_bind_text(res, 1, hashstring, -1, SQLITE_STATIC);
    } else{
        log(AV_LOG,ERROR, "Failed to execute SQL statement: %s\n",sqlite3_errmsg(*db));
    }
    int step = sqlite3_step(res);
    sqlite3_finalize(res);
    if (step == SQLITE_ROW){
        return 0;
    }
    return 1;
}
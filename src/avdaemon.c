#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h>
#include "avdaemon.h"


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


int main(int argc, char const *argv[])
{
    sqlite3 *db;
    connect_db(&db);
    return 0;
}

char* compute_md5(const char *filename)
{
    unsigned char signature[MD5_DIGEST_LENGTH];
    FILE *file = fopen(filename, "rb");
    MD5_CTX context;
    unsigned int bytes;
    unsigned char data[1024];
    char* hash_string;

    if (file == NULL){
        fprintf(stderr, "Cannot open file.");
    }

    MD5_Init(&context);
    

    while ((bytes = fread(data,1,1024, file)) != 0){
        MD5_Update(&context, data, bytes);
    }

    MD5_Final(signature, &context);
    fclose(file);

    hash_string = (char* )malloc(2 * MD5_DIGEST_LENGTH + 1);

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(&hash_string[2*i], "%02x",(unsigned int)signature[i]);
    }
    hash_string[2* MD5_DIGEST_LENGTH] = '\0';
    return hash_string;
}
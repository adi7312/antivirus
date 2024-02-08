#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "avdaemon.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_substring(const char* string){
    char* substring = (char*)malloc(7); // Allocate memory for substring (including null terminator)
    strncpy(substring, string, 6);
    substring[6] = '\0'; // Add null terminator
    return substring;
}

int main(int argc, char const *argv[])
{
    //sqlite3 *db;
    //connect_db(&db);
    isolate("test","d41d8cd98f00b204e9800998ecf8427e");
    return 0;
}

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

void encrypt_file(const char* filename, sqlite3 **db){
    return 0;
}

void drop_privileges(const char* filename){
    int drop_priv = chmod(filename, 0000);
    if (drop_priv != 0){
         fprintf(stderr, "Cannot drop privileges");
     } 
}

void relocate(const char* filename, const char* hash){
    const char* new_filename = get_substring(hash);
    if (new_filename == NULL){
        fprintf(stderr, "Failed to create new filename");
        return;
    }
    const char* extension = ".0";
    const char* quarantine_dir = "/var/lib/av/quarantine/";
    size_t new_filepath_size = strlen(quarantine_dir) + strlen(new_filename) + strlen(extension) + 1;
    char* new_filepath = (char*)malloc(new_filepath_size);
    if (new_filepath == NULL){
        fprintf(stderr, "Memory Allocation failed for new_filepath");
        free(new_filepath);
        return;
    }
    snprintf(new_filepath, new_filepath_size, "%s%s%s",quarantine_dir,new_filename,extension);
    if (rename(filename, new_filepath)!=0){
        fprintf(stderr, "File Relocation error");
    }
    free(new_filename);
    free(new_filepath);
}

void isolate(const char* filename, const char* hash){
    
     drop_privileges(filename);
     relocate(filename,hash);

    // const char* new_filename = get_substring(hash);
    
    // const char* extension = ".0";
    // const char* full_filename = strcat(new_filename, extension);
    // const char* quarantine_dir = "/var/lib/av/quarantine/";
    // char* new_filepath = malloc(strlen(quarantine_dir) + strlen(full_filename) + 1);
    // strcpy(new_filepath, quarantine_dir);
    // strcat(new_filepath, full_filename);
    // int relocate = rename(filename, new_filepath);
    // free(new_filename);
    
}

int scan(const char* filename, sqlite3 **db){
    char* hashstring = compute_md5(filename);
    int found = find_signature_in_db(hashstring, &db);
    if (found == 0){
        isolate(filename, hashstring);
    }
}
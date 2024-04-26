#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "util/misc.h"
#include "util/db.h"
#include "crypto/crypto_handler.h"
#include "avdaemon.h"
#include "log/log.h"



int main(int argc, char const *argv[])
{   
    sqlite3 *db;
    connect_db(&db);
    unsigned char* key = (unsigned char*)malloc(AES_KEY_SIZE * sizeof(unsigned char));
    if (key == NULL) {
        log(AV_LOG,ERROR, "Memory allocation failed when allocating memory for AES key.\n");
        return -1;
    }
    if (get_key("/home/av/security/enc.key", key) != 0) {
        log(AV_LOG,ERROR,"Failed to save a key.");
        return -1;
    }
    scan("test",&db,key);
    free(key);
    sqlite3_close(db);
    
    return 0;
}

int drop_privileges(const char* filename){
    int drop_priv = chmod(filename, 0000);
    if (drop_priv != 0){
        log(AV_LOG,WARN,"Cannot drop privileges for: %s.",filename);
        return 1;
    }
    log(AV_LOG,INFO, "Dropped privileges for: %s.\n",filename);
    return 0;
}


int relocate(const char* filename, const char* hash){
    char* new_filename = get_substring(hash);
    if (new_filename == NULL){
        log(AV_LOG,ERROR,"Failed to create new filename.\n");
        return 1;
    }
    const char* extension = ".0";
    const char* quarantine_dir = "/var/lib/av/quarantine/";
    size_t new_filepath_size = strlen(quarantine_dir) + strlen(new_filename) + strlen(extension) + 1;
    char* new_filepath = (char*)malloc(new_filepath_size);
    if (new_filepath == NULL){
        log(AV_LOG,ERROR,"Failed to allocate new_filepath.\n");
        free(new_filepath);
        return 2;
    }
    snprintf(new_filepath, new_filepath_size, "%s%s%s",quarantine_dir,new_filename,extension);
    if (rename(filename, new_filepath)!=0){
        log(AV_LOG,ERROR,"Failed to relocate file.\n");
        return 3;
    }
    log(AV_LOG, INFO, "File: %s has been relocated to: %s\n",filename, new_filepath);
    free(new_filename);
    free(new_filepath);
    return 0;
}

void isolate(const char* filename, const char* hash, const unsigned char* key){
    int is_renamed = 0;
    log(AV_LOG,INFO,"Executing quarantine mechanism.\n");
    if(encrypt_file(filename, key)!=0){
        log(AV_LOG,WARN,"File encryption procedure failed for file %s.\n",filename);
    }
    const char* output_filename = rename_enc(filename);
    if (output_filename == NULL){
        log(AV_LOG,ERROR,"Renaming file failed.\n");
        return;
    }
    if(drop_privileges(output_filename)!= 0){
        log(AV_LOG,WARN, "Failed to drop privileges for %s.\n",output_filename);
    }
    if(relocate(output_filename,hash) != 0){
        log(AV_LOG,WARN,"Failed to relocate file: %s\n.",output_filename);
    }
    log(AV_LOG,SUCCESS,"Finished quarantine mechanism for %s.\n",filename);
    free(output_filename);
}

int scan(const char* filename, sqlite3 **db, const unsigned char* key){
    log(AV_LOG,INFO,"Scanning: %s.\n",filename);
    const char* hashstring = compute_md5(filename);
    int found = find_signature_in_db(hashstring, db);
    if (found == 0){
        log(AV_LOG,CRITICAL,"Detected possible malware: %s \n",filename);
        log(AV_LOG,INFO,"Reason: signature of file matches signature in database.\n");
        isolate(filename, hashstring, key);
    }
    free(hashstring);
}









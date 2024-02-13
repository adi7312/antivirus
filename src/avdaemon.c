#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "util/misc.h"
#include "util/db.h"
#include "crypto/crypto_handler.h"
#include "avdaemon.h"



int main(int argc, char const *argv[])
{   
    const unsigned char* key = (unsigned char*)malloc(AES_KEY_SIZE * sizeof(unsigned char));
    if (key == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }
    if (get_key("/home/av/security/enc.key", key) != 0) {
        fprintf(stderr,"Failed to save a key.");
        return -1;
    }
    isolate("test", "d45a886a6cdd86f4ea8e10032c8f1e97", key); // simple test vec
    
    return 0;
}

void drop_privileges(const char* filename){
    int drop_priv = chmod(filename, 0000);
    if (drop_priv != 0){
         fprintf(stderr, "Cannot drop privileges.\n");
    }
}

void change_owner(const char* filename){
    const char* newowner = "dummyuser";
    const char* newgroup = "dummygroup";
    struct passwd *pw = getpwnam(newowner);
    if (pw == NULL){
        fprintf(stderr, "Failed to find user: %s\n",newowner);
    }
    uid_t uid = pw->pw_uid;
    struct group *gr = getgrnam(newgroup);
    if (gr == NULL){
        fprintf(stderr, "Failed to find group: %s\n",newgroup);
    }
    gid_t gid = gr->gr_gid;
    if (chown(filename, uid, gid) != 0){
        fprintf(stderr, "Failed to change owner\n");
    }
}

void relocate(const char* filename, const char* hash){
    char* new_filename = get_substring(hash);
    if (new_filename == NULL){
        fprintf(stderr, "Failed to create new filename\n");
        return;
    }
    const char* extension = ".0";
    const char* quarantine_dir = "/var/lib/av/quarantine/";
    size_t new_filepath_size = strlen(quarantine_dir) + strlen(new_filename) + strlen(extension) + 1;
    char* new_filepath = (char*)malloc(new_filepath_size);
    if (new_filepath == NULL){
        fprintf(stderr, "Memory Allocation failed for new_filepath\n");
        free(new_filepath);
        return;
    }
    snprintf(new_filepath, new_filepath_size, "%s%s%s",quarantine_dir,new_filename,extension);
    if (rename(filename, new_filepath)!=0){
        fprintf(stderr, "File Relocation error\n");
    }
    free(new_filename);
    free(new_filepath);
}

void isolate(const char* filename, const char* hash, const unsigned char* key){
    encrypt_file(filename, key);
    const char* output_filename = rename_enc(filename);
    drop_privileges(output_filename);
    change_owner(output_filename);
    relocate(output_filename,hash);
    
}

int scan(const char* filename, sqlite3 **db, const unsigned char* key){
    const char* hashstring = compute_md5(filename);
    int found = find_signature_in_db(hashstring, db);
    if (found == 0){
        isolate(filename, hashstring, key);
    }
}








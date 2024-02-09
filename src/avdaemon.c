#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "avdaemon.h"



char* get_substring(const char* string){
    char* substring = (char*)malloc(7); 
    strncpy(substring, string, 6);
    substring[6] = '\0';
    return substring;
}

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

void encrypt_file(const char* input_filename ,const unsigned char* ukey){
    FILE *ifp, *ofp;
    const char* output_filename = rename_enc(input_filename);
    ifp = fopen(input_filename, "rb");
    ofp = fopen(output_filename, "wb");
    unsigned char init_vec[AES_BLOCK_SIZE];
    unsigned char read_buff[1024];
    unsigned char cipher_buf[sizeof(read_buff) + AES_BLOCK_SIZE];
    int num_read;

    RAND_bytes(init_vec, AES_BLOCK_SIZE);
    fwrite(init_vec,1,AES_BLOCK_SIZE,ofp);
    AES_KEY aes_key;
    AES_set_encrypt_key(ukey, 256, &aes_key);
    while ((num_read = fread(read_buff, 1, sizeof(read_buff),ifp)) > 0){
        AES_cbc_encrypt(read_buff, cipher_buf, num_read, &aes_key, init_vec, AES_ENCRYPT);
        fwrite(cipher_buf,1,num_read, ofp);
    }
    fclose(ifp);
    fclose(ofp);

}

void drop_privileges(const char* filename){
    int drop_priv = chmod(filename, 0000);
    if (drop_priv != 0){
         fprintf(stderr, "Cannot drop privileges");
    }
}

void change_owner(const char* filename){
    const char* newowner = "dummyuser";
    const char* newgroup = "dummygroup";
    struct passwd *pw = getpwnam(newowner);
    if (pw == NULL){
        fprintf(stderr, "Failed to find user: %s",newowner);
    }
    uid_t uid = pw->pw_uid;
    struct group *gr = getgrnam(newgroup);
    if (gr == NULL){
        fprintf(stderr, "Failed to find group: %s",newgroup);
    }
    gid_t gid = gr->gr_gid;
    if (chown(filename, uid, gid) != 0){
        fprintf(stderr, "Failed to change owner");
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

void isolate(const char* filename, const char* hash, const unsigned char* key){
    encrypt_file(filename, key);
    const char* output_filename = rename_enc(filename);
    drop_privileges(output_filename);
    change_owner(output_filename);
    relocate(output_filename,hash);
    
}

int scan(const char* filename, sqlite3 **db, const unsigned char* key){
    const char* hashstring = compute_md5(filename);
    int found = find_signature_in_db(hashstring, &db);
    if (found == 0){
        isolate(filename, hashstring, key);
    }
}

int get_key(const char* filename, const unsigned char* key){
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL){
        fprintf(stderr, "Failed to load key file");
        return -1;
    }
    if (fread(key, 1, AES_KEY_SIZE, fp) < AES_KEY_SIZE){
        fprintf(stderr, "Failed to load AES key file.");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

char* rename_enc(const char* filename){
    const char* extension = ".enc";
    size_t new_filename_size = strlen(filename) + strlen(extension) + 1;
    
    const char* output_filename = (char*)malloc(new_filename_size);
    if (output_filename == NULL){
            fprintf(stderr, "Memory Allocation failed for new_filepath");
            free(output_filename);
            return;
    }
    snprintf(output_filename, new_filename_size, "%s%s",filename,extension);
    return output_filename;
}
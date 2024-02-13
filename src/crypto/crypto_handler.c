#include "crypto_handler.h"

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

void encrypt_file(const char* input_filename ,const unsigned char* ukey){
    FILE *ifp, *ofp;
    const char* output_filename = rename_enc(input_filename);
    ifp = fopen(input_filename, "rb");
    ofp = fopen(output_filename, "wb");
    if (ifp == NULL || ofp == NULL){
        fprintf(stderr, "Failed to open descriptor to input/output file.");
        return;
    }
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
    free(output_filename);
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
 
void md5_hash(const char *input, char *output) {
    MD5_CTX ctx;
    unsigned char digest[MD5_DIGEST_LENGTH];
 
    MD5_Init(&ctx);
    MD5_Update(&ctx, input, strlen(input));
    MD5_Final(digest, &ctx);
 
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&output[i * 2], "%02x", (unsigned int)digest[i]);
    }
}
 
int main_md5_hash() {
    const char *input = "Hello, MD5!";
    char md5_result[MD5_DIGEST_LENGTH * 2 + 1]; // +1 for null-terminator
 
    md5_hash(input, md5_result);
 
    printf("Input: %s\n", input);
    printf("MD5 Hash: %s\n", md5_result);
 
    return 0;
}
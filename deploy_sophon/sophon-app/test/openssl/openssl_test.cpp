#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>
#include <iostream>
#include <openssl/evp.h>


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

std::string md5(const std::string &input)
{
    // init EVP
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    mdctx = EVP_MD_CTX_new();
    md = EVP_md5();

    // init MD5 context
    EVP_DigestInit_ex(mdctx, md, NULL);

    // update context
    EVP_DigestUpdate(mdctx, input.c_str(), input.length());

    // get md5 result
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);

    // destory context
    EVP_MD_CTX_free(mdctx);

    std::string result;
    for(unsigned int i = 0; i<md_len; i++)
    {
        char hex[3];
        sprintf(hex, "%02x", md_value[i]);
        result += hex;
    }

    return result;
}




 
int main() {
    const char *input = "Hello, MD5!";
    char md5_result[MD5_DIGEST_LENGTH * 2 + 1]; // +1 for null-terminator

    md5_hash(input, md5_result);

    printf("Input: %s\n", input);
    printf("MD5 Hash: %s\n", md5_result);

    std::string input2 = "Hello, MD5!";
    std::string hash = md5(input2);

    std::cout << "Input: " << input2 << std::endl;
    std::cout << "MD5 Hash: " << hash << std::endl;

 
    return 0;
}
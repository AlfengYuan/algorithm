//
// Created by yyf on 23-12-19.
//


#include "miniocpp/client.h"
#include <iostream>
#define IMG_JPG "data:image/jpeg;base64,"    //jpg图片信息，其他类似

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
std::string base64_encode(const char * bytes_to_encode, unsigned int in_len);

int main(int argc, char *argv[]) {
    // Create S3 base URL.
    minio::s3::BaseUrl base_url("play.min.io");

    // Create credential provider
    minio::creds::StaticProvider provider(
            "test", "test"
    );

    // Create S3 client
    minio::s3::Client client(base_url, &provider);

    // Create get object arguments
    minio::s3::GetObjectArgs args;
    args.bucket = "my-bucket";
    args.object = "data/se50_data/resnet50/20231115112043liuyu.jpg";
    args.datafunc = [](minio::http::DataFunctionArgs args) -> bool {
        std::cout << args.datachunk << std::endl;
//        std::string result = base64_encode(args.datachunk.c_str(), args.datachunk.length());
//        std::ofstream ofs("hello.jpg");
//        if(!ofs.bad())
//        {
//            ofs << args.datachunk << std::endl;
//            ofs.close();
//        }
        return true;
    };

    // Call get object
    minio::s3::GetObjectResponse resp = client.GetObject(args);

    // Handle response.
    if(resp){
        std::cout << std::endl
                << "data of my-object is received successfully " << std::endl;
    } else {
        std::cout << "unable to get object; " << resp.Error().String() << std::endl;
    }

    return 0;
}


//加密
std::string base64_encode(const char * bytes_to_encode, unsigned int in_len)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if(i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for(i = 0; (i <4) ; i++)
            {
                ret += base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }
    if(i)
    {
        for(j = i; j < 3; j++)
        {
            char_array_3[j] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for(j = 0; (j < i + 1); j++)
        {
            ret += base64_chars[char_array_4[j]];
        }

        while((i++ < 3))
        {
            ret += '=';
        }

    }
    return ret;
}
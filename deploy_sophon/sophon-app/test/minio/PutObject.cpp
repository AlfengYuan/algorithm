//
// Created by yyf on 23-12-19.
//

#include "miniocpp/client.h"

int main(int argc, char *argv[]) {
    // Create S3 base URL
    minio::s3::BaseUrl base_url("play.min.io");

    // Create credential provider
    minio::creds::StaticProvider provider(
            "test", "test"
    );

    // Create S3 Client
    minio::s3::Client client(base_url, &provider);

    // Create put object arguments
    std::string filepath = "/data/se50_data/resnet50/20231115112043liuyu.jpg";
    std::ifstream file(filepath.c_str());
    long object_size = static_cast<long>(std::filesystem::file_size(filepath));

    minio::s3::PutObjectArgs args(file, object_size, 0);
    args.bucket = "my-bucket";
    args.object = filepath;

    // Call put object
    minio::s3::PutObjectResponse resp = client.PutObject(args);

    // Handle response.
    if(resp){
        std::cout << "my-object is successfully created" << std::endl;
    } else {
        std::cout << "unable to do put object; " << resp.Error().String() << std::endl;
    }

    return 0;
}

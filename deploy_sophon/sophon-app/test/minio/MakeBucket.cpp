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

    // Create S3 client
    minio::s3::Client client(base_url, &provider);

    // Create make bucket arguments
    minio::s3::MakeBucketArgs args;
    args.bucket = "my-bucket";

    // Call make bucket.
    minio::s3::MakeBucketResponse resp = client.MakeBucket(args);

    // Handle response
    if(resp){
        std::cout << "my-bucket is created successfully" << std::endl;
    } else {
        std::cout << "unable to create bucket; " << resp.Error().String() << std::endl;
    }

    return 0;
}

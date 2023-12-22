//
// Created by yyf on 23-12-19.
//

#include "miniocpp/client.h"

int main(int argc, char *argv[]) {
    // Create S3 base URL.
    minio::s3::BaseUrl base_url("play.min.io", false);

    // Create credential provider
    minio::creds::StaticProvider provider(
            "test", "test"
    );

    // Create S3 client
    minio::s3::Client client(base_url, &provider);

    // Create remove bucket argument
    minio::s3::RemoveBucketArgs args;
    args.bucket = "my-bucket";

    // Call remove bucket
    minio::s3::RemoveBucketResponse resp = client.RemoveBucket(args);

    // Handle response
    if(resp){
        std::cout << "my-bucket is removed successfully" << std::endl;
    } else {
        std::cout << "unable to remove bucket; " << resp.Error().String() << std::endl;
    }

    return 0;
}

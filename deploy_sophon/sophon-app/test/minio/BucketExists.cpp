//
// Created by yyf on 23-12-19.
//

#include "miniocpp/client.h"

int main(int argc, char *argv[]) {
    // Create S3 base URL.
    minio::s3::BaseUrl base_url("play.min.io");

    // Create credential provider.
    minio::creds::StaticProvider provider(
            "test", "test"
    );

    // Create S3 Client
    minio::s3::Client client(base_url, &provider);

    // Create bucket exits arguments
    minio::s3::BucketExistsArgs args;
    args.bucket = "my-bucket";

    // Call bucket exists
    minio::s3::BucketExistsResponse resp = client.BucketExists(args);

    // Handle response
    if(resp){
        if(resp.exist){
            std::cout << "my-bucket exists" << std::endl;
        } else {
            std::cout << "my-bucket does not exist " << std::endl;
        }
    } else {
        std::cout << "unable to do bucket existence check; "
                << resp.Error().String() << std::endl;
    }

    return 0;
}

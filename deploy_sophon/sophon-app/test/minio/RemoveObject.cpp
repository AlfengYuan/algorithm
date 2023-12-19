//
// Created by yyf on 23-12-19.
//

#include "miniocpp/client.h"

int main(int argc, char *argv[]) {
    // Create S3 base URL.
    minio::s3::BaseUrl base_url("play.min.io");

    // Create credential provider
    minio::creds::StaticProvider provider(
            "test", "test"
    );

    // Create S3 client
    minio::s3::Client client(base_url, &provider);

    // Create remove object arguments
    minio::s3::RemoveObjectArgs args;
    args.bucket = "my-bucket";
    args.object = "/aa/bb/cc/20231115112043liuyu.jpg";

    // Call remove object.
    minio::s3::RemoveObjectResponse resp = client.RemoveObject(args);

    // Handle response
    if(resp){
        std::cout << "my-object is removed successfully " << std::endl;
    } else {
        std::cout << "unable to remove object; " << resp.Error().String() << std::endl;
    }

    return EXIT_SUCCESS;
}

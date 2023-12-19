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

    // Create download object arguments
    minio::s3::DownloadObjectArgs args;
    args.bucket = "my-bucket";
    args.object = "data/se50_data/resnet50/20231115112043liuyu.jpg";
    args.filename = "mytest.jpg";

    // Call download object.
    minio::s3::DownloadObjectResponse resp = client.DownloadObject(args);

    // Handle response
    if(resp){
        std::cout << "my-object is successfully downloaded to mytest.jpg" << std::endl;
    } else {
        std::cout << "unable to download object; " << resp.Error().String() << std::endl;
    }

    return 0;
}

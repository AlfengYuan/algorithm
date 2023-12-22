//
// Created by yyf on 23-12-19.
//

#include "miniocpp/client.h"
#include <list>
int main(int argc, char *argv[]) {
    // Create S3 base URL.
    minio::s3::BaseUrl base_url("play.min.io");

    // Create credential provider
    minio::creds::StaticProvider provider(
            "test", "test"
    );

    // Create S3 Client
    minio::s3::Client client(base_url, &provider);

    // Create compose object arguments.
    minio::s3::ComposeObjectArgs args;
    args.bucket = "my-bucket-new";
    args.object = "my-object-new";

    std::list<minio::s3::ComposeSource> sources;

    minio::s3::ComposeSource source1;
    source1.bucket = "asiatrip";
    source1.object = "bbb.out";
    sources.push_back(source1);

    minio::s3::ComposeSource source2;
    source2.bucket = "my-bucket";
    source2.object = "data/se50_data/resnet50/20231115112043liuyu.jpg";
    sources.push_back(source2);

    args.sources = sources;

    // Call compose object.
    minio::s3::ComposeObjectResponse resp = client.ComposeObject(args);

    // Handle response
    if(resp){
        std::cout << "my-object is successfully created" << std::endl;
    } else {
        std::cout << "unable to compose object; " << resp.Error().String() << std::endl;
    }

    return 0;
}

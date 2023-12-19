//
// Created by yyf on 23-12-19.
//

#include "miniocpp/client.h"

int main(int argc, char *argv[])
{
    // Create S3 base URL.
    minio::s3::BaseUrl base_url("play.min.io");


    // Create credential provider
    minio::creds::StaticProvider provider(
            "test", "test"
            );
    // Create S3 client
    minio::s3::Client client(base_url, &provider);

    // Create get presigned object url arguments
    minio::s3::GetPresignedObjectUrlArgs args;
    args.bucket = "my-bucket";
    args.object = "/aa/bb/cc/20231115112043liuyu.jpg";
    args.method = minio::http::Method::kGet;
    args.expiry_seconds = 60 * 60 * 24; // 1 day.

    // Call get presigned object url.
    minio::s3::GetPresignedObjectUrlResponse resp = client.GetPresignedObjectUrl(args);

    // Handle response
    if(resp)
    {
        std::cout << "presigned URL to get object: " << resp.url << std::endl;
    }
    else
    {
        std::cout << "unable to get presigned object url; " << resp.Error().String() << std::endl;
    }

    return 0;
}
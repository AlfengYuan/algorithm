//
// Created by yyf on 23-12-18.
//


#include "miniocpp/client.h"


const std::string url{"play.min.io"};

int main(int argc, char *argv[]) {
    std::string address = argc > 1 ? std::string(argv[1]) : url;
    // Create S3 base URL.
    minio::s3::BaseUrl base_url(address, false);
    // Create credential provider
    minio::creds::StaticProvider provider(
            "yyf", "yz123456"
    );
    // Create S3 client
    minio::s3::Client client(base_url, &provider);
    client.Debug(true);

    minio::s3::UploadObjectArgs uploadArgs;
    uploadArgs.bucket = "my-bucket";
    uploadArgs.object = "/aa/bb/cc/20231115112043liuyu.jpg";
    uploadArgs.filename = "/data/se50_data/resnet50/20231115112043liuyu.jpg";

    // Check bucket exist or not.
    bool exist;
    {
        minio::s3::BucketExistsArgs  bktExtArgs;
        bktExtArgs.bucket = uploadArgs.bucket;

        minio::s3::BucketExistsResponse resp = client.BucketExists(bktExtArgs);
        if(!resp)
        {
            std::cout << "unable to do bucket existence check; " << resp.Error() << std::endl;
            return EXIT_FAILURE;
        }

        exist = resp.exist;
    }

    // Make bucket if not exist
    if(!exist)
    {
        minio::s3::MakeBucketArgs makeBktArgs;
        makeBktArgs.bucket = uploadArgs.bucket;

        minio::s3::MakeBucketResponse resp = client.MakeBucket(makeBktArgs);
        if(!resp)
        {
            std::cout << "unable to create bucket; " << resp.Error() << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Call upload object
    minio::s3::UploadObjectResponse resp = client.UploadObject(uploadArgs);
    if(!resp)
    {
        std::cout << "unable to upload object; " << resp.Error() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << uploadArgs.filename << " is successfully uploaded to "
              << uploadArgs.bucket << " as " << uploadArgs.object
              << std::endl;

    return EXIT_SUCCESS;
}

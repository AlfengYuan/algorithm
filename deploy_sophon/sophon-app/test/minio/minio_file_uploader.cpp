//
// Created by yyf on 23-12-17.
//

#include "miniocpp/client.h"

int main(int argc, char *argv[]) {
    // Create S3 base URL.
    minio::s3::BaseUrl base_url("play.min.io");

    // Create credential provider
    minio::creds::StaticProvider provider(
            "Q3AM3UQ867SPQQA43P2F", "zuf+tfteSlswRu7BJ86wekitnifILbZam1KYY3TG"
    );

    // Create S3 client
    minio::s3::Client client(base_url, &provider);
    client.Debug(true);
    std::string bucket_name = "asiatrip";

    // Check 'asiatrip' bucket exist or not.
    bool exist;
    {
        minio::s3::BucketExistsArgs  args;
        args.bucket = bucket_name;

        minio::s3::BucketExistsResponse resp = client.BucketExists(args);
        if(!resp)
        {
            std::cout << "unable to do bucket existence check; " << resp.Error() << std::endl;
            return EXIT_FAILURE;
        }

        exist = resp.exist;
    }

    // Make 'asiatrip' bucket if not exist
    if(!exist)
    {
        minio::s3::MakeBucketArgs args;
        args.bucket = bucket_name;

        minio::s3::MakeBucketResponse resp = client.MakeBucket(args);
        if(!resp)
        {
            std::cout << "unable to create bucket; " << resp.Error() << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Upload '/home/user/Photos/asiaphotos.zip' as object name
    // 'asiaphotos-2015.zip' to bucket 'asiatrip'
    minio::s3::UploadObjectArgs args;
    args.bucket = bucket_name;
    args.object = "asiaphotos-2015.zip";
    args.filename = "/home/user/Photos/asiaphotos.zip";

    minio::s3::UploadObjectResponse resp = client.UploadObject(args);
    if(!resp)
    {
        std::cout << "unable to upload object; " << resp.Error() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "'/home/user/Photos/asiaphotos.zip' is successfully uploaded as "
             << "object 'asiaphotos-2015.zip' to bucket 'asiatrip'"
             << std::endl;

    return EXIT_SUCCESS;

}
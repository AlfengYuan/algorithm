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

    // Create stat object.
    minio::s3::StatObjectArgs  args;
    args.bucket = "my-bucket";
    args.object = "/aa/bb/cc/20231115112043liuyu.jpg";

    // Call stat object
    minio::s3::StatObjectResponse resp = client.StatObject(args);

    // Handle response
    if(resp){
        std::cout << "Version ID: " << resp.version_id << std::endl;
        std::cout << "ETag: " << resp.etag << std::endl;
        std::cout << "Size: " << resp.size << std::endl;
        std::cout << "Last Modified: " << resp.last_modified << std::endl;
        std::cout << "Retention Mode: ";
        if(minio::s3::IsRetentionModeValid(resp.retention_mode)){
            std::cout << minio::s3::RetentionModeToString(resp.retention_mode) << std::endl;
        }
        else{
            std::cout << "-" << std::endl;
        }
        std::cout << "Retention Retain Until Date: ";
        if(resp.retention_retain_until_date){
            std::cout << resp.retention_retain_until_date.ToHttpHeaderValue() << std::endl;
        }else{
            std::cout << "-" << std::endl;
        }
        std::cout << "Legal Hold: ";
        if(minio::s3::IsLegalHoldValid(resp.legal_hold)){
            std::cout << minio::s3::LegalHoldToString(resp.legal_hold) << std::endl;
        }else{
            std::cout << "-" << std::endl;
        }
        std::cout << "Delete Marker: "
                  << minio::utils::BoolToString(resp.delete_marker) << std::endl;
        std::cout << "User Metadata: " << std::endl;
        std::list<std::string> keys = resp.user_metadata.Keys();
        for(auto &key: keys)
        {
            std::cout << " " << key << ": " << resp.user_metadata.GetFront(key) << std::endl;
        }
    }  else {
        std::cout << "unable to get stat object; " << resp.Error().String() << std::endl;
    }
    return 0;
}

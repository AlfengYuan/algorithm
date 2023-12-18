//
// Created by yyf on 23-12-18.
//

#include "miniocpp/client.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>	// For sleep
#include <atomic>
#include <chrono>
#include <cstring>
#include "mqtt/async_client.h"

const std::string DFLT_SERVER_ADDRESS { "mqtt://localhost:1883" };

const std::string TOPIC { "test" };
const int QOS = 1;

const char* PAYLOADS[] = {
        "Hello World!",
        "Hi there!",
        "Is anyone listening?",
        "Someone is always listening.",
        nullptr
};

const auto TIMEOUT = std::chrono::seconds(10);


const std::string minio_url{"play.min.io"};

int main(int argc, char *argv[]) {
    std::string address_minio = argc > 1 ? std::string(argv[1]) : minio_url;
    std::string address_mqtt = argc > 2 ? std::string(argv[2]) : DFLT_SERVER_ADDRESS;
    // Create S3 base URL.
    minio::s3::BaseUrl base_url(address_minio, false);
    mqtt::async_client mqttcli(address_mqtt, "MINIO_MQTT_TEST");
    // Create credential provider
    minio::creds::StaticProvider provider(
            "Q3AM3UQ867SPQQA43P2F", "zuf+tfteSlswRu7BJ86wekitnifILbZam1KYY3TG"
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

    try {
        std::cout << "\nMQTT Connecting..." << std::endl;
        mqttcli.connect()->wait();
        std::cout << "  ...OK" << std::endl;

        std::cout << "\nMQTT Publishing messages..." << std::endl;

        mqtt::topic top(mqttcli, TOPIC, QOS);
        mqtt::token_ptr tok;

        size_t i = 0;
        while (PAYLOADS[i]) {
            tok = top.publish(PAYLOADS[i++]);
        }
        tok->wait();	// Just wait for the last one to complete.
        std::cout << "OK" << std::endl;

        // Disconnect
        std::cout << "\nMQTT Disconnecting..." << std::endl;
        mqttcli.disconnect()->wait();
        std::cout << "  ...OK" << std::endl;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << exc << std::endl;
        return 1;
    }

    return EXIT_SUCCESS;
}

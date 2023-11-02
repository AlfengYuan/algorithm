//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Sophgo Technologies Inc.  All rights reserved.
//
// SOPHON-DEMO is licensed under the 2-Clause BSD License except for the
// third-party components.
//
//===----------------------------------------------------------------------===//
#include <sys/types.h>
#include <sstream>
#include <fstream>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sail/decoder_multi.h>
#include "json.hpp"
#include "opencv2/opencv.hpp"
#include "ff_decode.hpp"
#include "yolov5.hpp"
#include "resnet.hpp"
#include <cassert>
#include "MQTTClient.h"
#include "configuration.h"

using json = nlohmann::json;
using namespace std;
#define USE_OPENCV_DECODE 0

#define ADDRESS     "tcp://broker.emqx.io:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         2
#define TIMEOUT     10000L

int yolov5_main(int argc, char* argv[]);
int resnet50_main(int argc, char *argv[]);
string getSuffix(string filename);


// const char *APP_ARG_STRING = "{config | ./cameras_multi_myself.json | path to cameras_multi.json}";

int main_old(int argc, char *argv[])
{
    // bmlib_log_set_level(BMLIB_LOG_VERBOSE); 
    int ret = 0;

    // ret = yolov5_main(argc, argv);
    ret = resnet50_main(argc, argv);
    return ret;
}

string getSuffix(string filename)
{
    string res;
    res=filename.substr(filename.find_last_of('.') + 1);//获取文件后缀
    return  res;
}

int resnet50_main(int argc, char *argv[]) {
    cout.setf(ios::fixed);
    // get params
    const char *keys="{input | ../../datasets/imagenet_val_1k/img | input path, image file path}"
        "{bmodel | ../../models/BM1684X/resnet50_fp32_1b.bmodel | bmodel file path}"
        "{dev_id | 0 | TPU device id}"
        "{help | 0 | print help information.}"
        "{config | ./cameras_multi_myself.json | path to cameras_multi.json}";
    cv::CommandLineParser parser(argc, argv, keys);
    if (parser.get<bool>("help")) {
        parser.printMessage();  
        return 0;
    }
    string bmodel_file = parser.get<string>("bmodel");
    string input = parser.get<string>("input");
    string config = parser.get<string>("config");
    int dev_id = parser.get<int>("dev_id");

    // check params
    struct stat info;
    if (stat(bmodel_file.c_str(), &info) != 0) {
        cout << "Cannot find valid model file." << endl;
        exit(1);
    }

    if(stat(config.c_str(), &info) != 0)
    {
        cout << "Cannot find config path. \n" << endl;
    }

    // creat MQTTClient
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    char buffer[20];

    int rc = 0;
    if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    
    // creat handle
    BMNNHandlePtr handle = make_shared<BMNNHandle>(dev_id);
    cout << "set device id: "  << dev_id << endl;
    bm_handle_t h = handle->handle();

    // load bmodel
    shared_ptr<BMNNContext> bm_ctx = make_shared<BMNNContext>(handle, bmodel_file.c_str());

    // initialize net
    RESNET resnet(bm_ctx);
    CV_Assert(0 == resnet.Init());

    // profiling
    TimeStamp resnet_ts;
    TimeStamp *ts = &resnet_ts;
    resnet.enableProfile(ts);

    // get batch_size
    int batch_size = resnet.batch_size();

    // test images 
    vector<bm_image> batch_imgs;
    vector<string> batch_names;
    vector<pair<int, float>> results;
    vector<json> results_json;
    if(stat(config.c_str(), &info) == 0 && getSuffix(config).compare("json") == 0)
    {
        cout << "run config path" << endl;
        Config cfg(config.c_str());
        if(!cfg.valid_check())
        {
            cout << "ERROR:" << config << " error, please check!" << endl;
        }

        std::vector<std::string> urls = cfg.cardUrls(dev_id);
        std::vector<int> channels;
        sail::MultiDecoder mtdecoder(1000, dev_id, 0);
        mtdecoder.set_read_timeout(3);
        for(string url:urls)
        {
            int channel = mtdecoder.add_channel(url, 15);
            channels.push_back(channel);
        }

        while(1)
        {
            bool flag = true;
            for(int i = 0; i<urls.size(); i++)
            {
                flag = true;
                batch_imgs.resize(batch_size);
                for(int j = 0; j < batch_size; j++){
                    int ret = mtdecoder.read_(channels[i], batch_imgs[j], 1);
                    if(ret != 0)
                    {
                        sprintf(buffer, "Hello, World, %d!", 4);
                        pubmsg.payload = buffer;
                        pubmsg.payloadlen = (int)strlen(buffer);
                        pubmsg.qos = QOS;
                        pubmsg.retained = 0;
                        if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
                        {
                            printf("Failed to publish message, return code %d\n", rc);
                            exit(EXIT_FAILURE);
                        }
                        flag = false;
                        break;
                    }
                }

                if(!flag) continue;

                bm_image packed_img;
                // int packed_stride = 1018;
                bm_image_create(h, 1080, 1920, FORMAT_BGR_PACKED, DATA_TYPE_EXT_1N_BYTE, &packed_img, NULL);
                if(bmcv_image_vpp_convert(h, 1, batch_imgs[0], &packed_img, NULL) != BM_SUCCESS)
                {
                    cout << "+++++++++++++++++++++++++++++" << endl;
                    cout << "bmcv_image_vpp_convert failed!!!!!!!!!" << endl;
                    cout << "++++++++++++++++++++++++++++++" << endl;
                }


                cout << "===================" << endl;
                cout << "bmimg::weidth::" << batch_imgs[0].width << endl;
                cout << "bmimg::height::" << batch_imgs[0].height << endl;
                cout << "bmimg::image_format::" << batch_imgs[0].image_format << endl;
                cout << "bmimg::data_type::" << batch_imgs[0].data_type << endl;
                cout << "===================" << endl;

                CV_Assert(0 == resnet.Classify(batch_imgs, results));

                for(int j = 0; j<batch_size; j++)
                {
                    cout << urls[i] << " pred: " << results[j].first << ", score:" << results[j].second << endl;
                }

            }

            // for(int i = 0; i<urls.size(); i++)
            // {
            //     sail::Decoder decoder(urls[i], true, dev_id);


                

            // }
            // for(int i = 0; i<decoders.size(); i++)
            // {
            //     bm_image *img = decoders[i].grab();
            //     if(!img)
            //     {
            //         batch_imgs.clear();
            //         continue;
            //     }
            //     batch_imgs.push_back(*img);
            //     if((int)batch_imgs.size() == batch_size)
            //     {
            //         // predict
            //         CV_Assert(0 == resnet.Classify(batch_imgs, results));
            //         // print and save the results
            //         for (int i = 0; i < batch_size; i++) 
            //         {
            //             cout << urls[i] << " pred: " << results[i].first << ", score:" << results[i].second << endl;
            //             if(batch_imgs[i].image_format != 0)
            //             {
            //                 bm_image frame;
            //                 bm_image_create(h, batch_imgs[i].height, batch_imgs[i].width, FORMAT_YUV420P, batch_imgs[i].data_type, &frame);
            //                 bmcv_image_storage_convert(h, 1, &batch_imgs[i], &frame);
            //                 bm_image_destroy(batch_imgs[i]);
            //                 batch_imgs[i] = frame;
            //             }
            //             bm_image_destroy(batch_imgs[i]);
            //         }
            //     }
            //     batch_imgs.clear();         
            // }
        }

    }
    else
    {
        if (stat(input.c_str(), &info) != 0)
        {
            cout << "Cannot find input path." << endl;
            exit(1);
        }
        
        if (info.st_mode & S_IFREG) {
            if (batch_size != 1) {
                cout << "ERROR: batch_size of model is " << batch_size << endl;
                exit(-1);
            }
            ts->save("resnet overall");
            ts->save("read image");
            // decode jpg
            bm_image bmimg;
            picDec(h, input.c_str(), bmimg);

            // int stride = 0;
            // bm_image_get_stride(bmimg, &stride);
            // cout << "++++++++++++++++++++++++++" << endl;
            // cout << "stride: " << stride << endl;
            // cout << "++++++++++++++++++++++++++" << endl;
            ts->save("read image");
            batch_imgs.push_back(bmimg);

            cout << "===================" << endl;
            cout << "bmimg::image_format::" << bmimg.image_format << endl;
            cout << "bmimg::data_type::" << bmimg.data_type << endl;
            cout << "===================" << endl;
            // do infer
            CV_Assert(0 == resnet.Classify(batch_imgs, results));
            ts->save("resnet overall");
            bm_image_destroy(batch_imgs[0]);
            // print the results
            cout << input << " pred: " << results[0].first << ", score:" << results[0].second << endl;
        }
        else if (info.st_mode & S_IFDIR) {
            // get files
            vector<string> files_vector;
            DIR *pDir;
            struct dirent* ptr;
            pDir = opendir(input.c_str());
            while((ptr = readdir(pDir))!=0) {
                if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
                    files_vector.push_back(input + "/" + ptr->d_name);
                }
            }
            closedir(pDir);

            vector<string>::iterator iter;
            ts->save("resnet overall");
            for (iter = files_vector.begin(); iter != files_vector.end(); iter++) {
                string img_file = *iter;
                ts->save("read image");
                // cv::Mat img = cv::imread(img_file, cv::IMREAD_COLOR, dev_id);
                bm_image bmimg;
                picDec(h, img_file.c_str(), bmimg);
                ts->save("read image");
                size_t index = img_file.rfind("/");
                string img_name = img_file.substr(index + 1);
                batch_imgs.push_back(bmimg);
                batch_names.push_back(img_name);
                if ((int)batch_imgs.size() == batch_size) {
                    // predict
                    CV_Assert(0 == resnet.Classify(batch_imgs, results));
                    // print and save the results
                    for (int i = 0; i < batch_size; i++) {
                        img_name = batch_names[i];
                        cout << img_name << " pred: " << results[i].first << ", score:" << results[i].second << endl;
                        json res_json;
                        res_json["filename"] = img_name;
                        res_json["prediction"] = results[i].first;
                        res_json["score"] = results[i].second;
                        results_json.push_back(res_json);
                        bm_image_destroy(batch_imgs[i]);
                    }
                    batch_imgs.clear();
                    batch_names.clear();
                    results.clear();
                }
            }
            if (!batch_imgs.empty()) {
                CV_Assert(0 == resnet.Classify(batch_imgs, results));
                // print and save the results
                for (int i = 0; i < batch_imgs.size(); i++) 
                {
                    string img_name = batch_names[i];
                    cout << img_name << " pred: " << results[i].first << ", score:" << results[i].second << endl;
                    json res_json;
                    res_json["filename"] = img_name;
                    res_json["prediction"] = results[i].first;
                    res_json["score"] = results[i].second;
                    results_json.push_back(res_json);
                    bm_image_destroy(batch_imgs[i]);
                }
                batch_imgs.clear();
                batch_names.clear();
                results.clear();     
            }
            ts->save("resnet overall");

            // save the results to the txt file
            if (access("results", 0) != F_OK) 
            {
                mkdir("results", S_IRWXU);
            }
            size_t index = input.rfind("/");
            if(index == input.length() - 1)
            {
                input = input.substr(0, input.length() - 1);
                index = input.rfind("/");
            }
            string dataset_name = input.substr(index + 1);
            index = bmodel_file.rfind("/");
            string model_name = bmodel_file.substr(index + 1);
            string json_file = "results/" + model_name + "_" + dataset_name + "_bmcv_cpp" + "_result.json";
            cout << "================" << endl;
            cout << "result saved in " << json_file << endl;
            ofstream(json_file) << std::setw(4) << results_json;
        }
        else
        {
            cout << "Is not a valid path: " << input << endl;
            exit(1);
        }
    }
    // print speed
    time_stamp_t base_time = time_point_cast<microseconds>(steady_clock::now());
    resnet_ts.calbr_basetime(base_time);
    resnet_ts.build_timeline("resnet infer");
    resnet_ts.show_summary("resnet infer");
    resnet_ts.clear();

    cout << endl;
    return 0;
}

int yolov5_main(int argc, char* argv[]) {
    cout.setf(ios::fixed);
    // get params
    const char* keys =
        "{bmodel | ./yolov5s_v6.1_3output_int8_1b.bmodel | ./resnet50_int8_1b.bmodel | bmodel file path}"
        "{dev_id | 0 | TPU device id}"
        "{conf_thresh | 0.001 | confidence threshold for filter boxes}"
        "{nms_thresh | 0.6 | iou threshold for nms}"
        "{help | 0 | print help information.}"
        "{input | rtsp://admin:yz123456@192.168.150.14:554/h264/ch1/main/av_stream | input path, images direction or video file path}"
        "{classnames | ./voc.names | class names file path}";
    cv::CommandLineParser parser(argc, argv, keys);
    if (parser.get<bool>("help")) {
        parser.printMessage();
        return 0;
    }
    string bmodel_file = parser.get<string>("bmodel");
    string input = parser.get<string>("input");

    int dev_id = parser.get<int>("dev_id");

    // check params
    struct stat info;
    if (stat(bmodel_file.c_str(), &info) != 0) {
        cout << "Cannot find valid model file." << endl;
        exit(1);
    }
    string coco_names = parser.get<string>("classnames");
    if (stat(coco_names.c_str(), &info) != 0) {
        cout << "Cannot find classnames file." << endl;
        exit(1);
    }

    // if (stat(input.c_str(), &info) != 0) {
    //     cout << "Cannot find input path." << endl;
    //     exit(1);
    // }

    // creat MQTTClient
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    char buffer[20];

    int rc = 0;
    if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    // creat handle
    auto handle = sail::Handle(dev_id);
    sail::Bmcv bmcv(handle); // for imwrite
    cout << "set device id: " << dev_id << endl;

    // initialize net
    YoloV5 yolov5(dev_id, bmodel_file);
    CV_Assert(0 == yolov5.Init(parser.get<float>("conf_thresh"), parser.get<float>("nms_thresh"), coco_names));

    // profiling
    TimeStamp yolov5_ts;
    TimeStamp* ts = &yolov5_ts;
    yolov5.enableProfile(&yolov5_ts);

    // get batch_size
    int batch_size = yolov5.batch_size();

    // creat save path
    if (access("results", 0) != F_OK)
        mkdir("results", S_IRWXU);
    if (access("results/images", 0) != F_OK)
        mkdir("results/images", S_IRWXU);

    // test images
    if (info.st_mode & S_IFDIR) {
        // get files
        vector<string> files_vector;
        DIR* pDir;
        struct dirent* ptr;
        pDir = opendir(input.c_str());
        while ((ptr = readdir(pDir)) != 0) {
            if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
                files_vector.push_back(input + "/" + ptr->d_name);
            }
        }
        closedir(pDir);
        vector<string> batch_names;
        vector<YoloV5BoxVec> boxes;
        vector<json> results_json;
        int cn = files_vector.size();
        int id = 0;
        for (vector<string>::iterator iter = files_vector.begin(); iter != files_vector.end(); iter++) {
            vector<cv::Mat> cvmats;
            cvmats.resize(batch_size); //bmimage attach to cvmats, so we must keep cvmat present.
            vector<sail::BMImage> batch_imgs; // re declare for every batch, to free sail::BMImage inside
            batch_imgs.resize(batch_size); //push_back forbidden, sail use BMImageArray to manage BMImage
            for (int i = 0; i < batch_size; i++) {               
                if (iter == files_vector.end()){
                    iter--;
                    cout << "using last img file to complete img batch" << endl;
                }
                string img_file = *iter;
                string img_name = img_file.substr(img_file.rfind("/")+1); 
                batch_names.push_back(img_name); //batch_names has real batch_size
                id++;
                cout << id << "/" << cn << ", img_file: " << img_file << endl;
                ts->save("read image");
            #if USE_OPENCV_DECODE
                cvmats[i] = cv::imread(img_file, cv::IMREAD_COLOR, dev_id);
                bmcv.mat_to_bm_image(cvmats[i], batch_imgs[i]);
            #else
                sail::Decoder decoder((const string)img_file, true, dev_id);
                int ret = decoder.read(handle, batch_imgs[i]);
                if (ret != 0) {
                    cout << "read failed" << "\n";
                }
            #endif

            #if DEBUG
                cout<<"batch img:"<<batch_imgs[i].format()<<" "<<batch_imgs[i].dtype()<<endl;
            #endif
                ts->save("read image");

                iter++;
            }
            iter--;
            CV_Assert(0 == yolov5.Detect(batch_imgs, boxes));
            for (int i = 0; i < batch_size; i++) { //use real batch size
                if (i > 0 && batch_names[i] == batch_names[i-1]){
                    break; // last batch may have same conponents.
                }
                vector<json> bboxes_json;
                for (auto bbox : boxes[i]) {
#if DEBUG
                        cout << "  class id=" << bbox.class_id << ", score = " << bbox.score << " (x=" << bbox.x
                             << ",y=" << bbox.y << ",w=" << bbox.width << ",h=" << bbox.height << ")" << endl;
#endif
                    // draw image
                    yolov5.draw_bmcv(bbox.class_id, bbox.score, bbox.x, bbox.y, bbox.width, bbox.height,
                                     batch_imgs[i], false);
                    // save result
                    json bbox_json;
                    bbox_json["category_id"] = bbox.class_id;
                    bbox_json["score"] = bbox.score;
                    bbox_json["bbox"] = {bbox.x, bbox.y, bbox.width, bbox.height};
                    bboxes_json.push_back(bbox_json);
                }
                json res_json;
                res_json["image_name"] = batch_names[i];
                res_json["bboxes"] = bboxes_json;
                results_json.push_back(res_json);
                bmcv.imwrite("./results/images/" + batch_names[i], batch_imgs[i]);
            }
            // batch_imgs.clear(); //severe bug here, do not free batch_imgs!
            batch_names.clear();
            boxes.clear();
        }
        // save results
        size_t index = input.rfind("/");
        if (index == input.length() - 1) {
            input = input.substr(0, input.length() - 1);
            index = input.rfind("/");
        }
        string dataset_name = input.substr(index + 1);
        index = bmodel_file.rfind("/");
        string model_name = bmodel_file.substr(index + 1);
        string json_file = "results/" + model_name + "_" + dataset_name + "_sail_cpp" + "_result.json";
        cout << "================" << endl;
        cout << "result saved in " << json_file << endl;
        ofstream(json_file) << setw(4) << results_json;
    }
    // test video
    else {
        vector<YoloV5BoxVec> boxes;
        vector<json> results_json;
        while(1){
            try
            {
                sail::Decoder decoder(input, true, dev_id);
                int id = 0;
                bool flag = true;
                while (1) 
                {
                    vector<sail::BMImage> batch_imgs;
                    batch_imgs.resize(batch_size);
                    for(int i = 0; i < batch_size; i++){
                        int ret = decoder.read(handle, batch_imgs[i]);
                        if(ret != 0){
                            flag = false;
                            break; // discard last batch.
                        }
                    }
                    if(flag == false){
                        break;
                    }

                    CV_Assert(0 == yolov5.Detect(batch_imgs, boxes));
                    for (int i = 0; i < batch_size; i++) { //use real batch size
                        cout << ++id << ", det_nums: " << boxes[i].size() << endl;
                        vector<json> bboxes_json;
                        for (auto bbox : boxes[i]) {
#if DEBUG
                            cout << "  class id=" << bbox.class_id << ", score = " << bbox.score << " (x=" << bbox.x
                                << ",y=" << bbox.y << ",w=" << bbox.width << ",h=" << bbox.height << ")" << endl;
#endif
                            // draw image
                            yolov5.draw_bmcv(bbox.class_id, bbox.score, bbox.x, bbox.y, bbox.width, bbox.height,
                                            batch_imgs[i], false);

                            // save result
                            json bbox_json;
                            bbox_json["category_id"] = bbox.class_id;
                            bbox_json["score"] = bbox.score;
                            bbox_json["bbox"] = {bbox.x, bbox.y, bbox.width, bbox.height};
                            bboxes_json.push_back(bbox_json);
                        }
                        json res_json;
                        res_json["image_name"] = to_string(id) + ".jpg";
                        res_json["bboxes"] = bboxes_json;
                        results_json.push_back(res_json);
                        bmcv.imwrite("./results/images/" + to_string(id) + ".jpg" , batch_imgs[i]);

                        sprintf(buffer, "Hello, World, %d!", id);
                        pubmsg.payload = buffer;
                        pubmsg.payloadlen = (int)strlen(buffer);
                        pubmsg.qos = QOS;
                        pubmsg.retained = 0;
                        if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
                        {
                            printf("Failed to publish message, return code %d\n", rc);
                            exit(EXIT_FAILURE);
                        }
                    }
                    boxes.clear();
                }
            }
            catch(runtime_error &e)
            {
                // cout << "Failed to open input file: " << input << endl;
                break;
            }
        }
        string dataset_name = "voc";
        // index = bmodel_file.rfind("/");
        // string model_name = bmodel_file.substr(index + 1);
        string model_name = "yolov5s_v6.1_3output_int8_1b";
        string json_file = "results/" + model_name + "_" + dataset_name + "_sail_cpp" + "_result.json";
        cout << "================" << endl;
        cout << "result saved in " << json_file << endl;
        ofstream(json_file) << setw(4) << results_json;
    }
    
    // print speed
    time_stamp_t base_time = time_point_cast<microseconds>(steady_clock::now());
    yolov5_ts.calbr_basetime(base_time);
    yolov5_ts.build_timeline("yolov5 test");
    yolov5_ts.show_summary("yolov5 test");
    yolov5_ts.clear();

    printf("Waiting for up to %d seconds for publication of %s\n"
                "on topic %s for client with ClientID: %s\n",
                (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);

    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
        printf("Failed to disconnect, return code %d\n", rc);

    MQTTClient_destroy(&client);

    return 0;
}

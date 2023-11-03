/* Copyright 2016-2022 by Sophgo Technologies Inc. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

==============================================================================*/

#include "processor.h"
#include "json/json.h"


#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__MSC_VER)
#pragma warning(disable: 4996)
#endif

using namespace std;

/**
 * @brief Load a bmodel and do inference.
 *
 * @param bmodel_path  Path to bmodel
 * @param input_path   Path to input file
 * @param tpu_id       ID of TPU to use
 * @return Program state
 *     @retval true  Success
 *     @retval false Failure
 */
bool inference(
    const std::string& bmodel_path,
    const std::string& input_path,
    int                tpu_id,
    int*               out_value){
    // init Engine
    sail::Engine engine(tpu_id);    

    // load bmodel without builtin input and output tensors
    engine.load(bmodel_path);

    // get model info
    auto graph_name = engine.get_graph_names().front();
    auto input_name = engine.get_input_names(graph_name).front();
    auto output_name = engine.get_output_names(graph_name).front();
    std::vector<int> input_shape = {1, 3, 224, 224};
    std::map<std::string, std::vector<int>> input_shapes;
    input_shapes[input_name] = input_shape;
    auto output_shape = engine.get_output_shape(graph_name, output_name);
    auto input_dtype = engine.get_input_dtype (graph_name, input_name);
    auto output_dtype = engine.get_output_dtype(graph_name, output_name);

    // get handle to create input and output tensors
    sail::Handle handle = engine.get_handle();

    // allocate input and output tensors with both system and device memory
    sail::Tensor in(handle, input_shape, input_dtype, false, false);
    sail::Tensor out(handle, output_shape, output_dtype, true, true);
    std::map<std::string, sail::Tensor*> input_tensors = {{input_name, &in}};
    std::map<std::string, sail::Tensor*> output_tensors = {{output_name, &out}};
    
    // prepare input and output data in system memory with data type of float32
    float* input = nullptr;
    float* output = nullptr;
    int in_size = std::accumulate(input_shape.begin(), input_shape.end(),
                                    1, std::multiplies<int>());
    int out_size = std::accumulate(output_shape.begin(), output_shape.end(),
                                    1, std::multiplies<int>());
    if (input_dtype == BM_FLOAT32) {
        input = reinterpret_cast<float*>(in.sys_data());
    } else {
        input = new float[in_size];
    }
    if (output_dtype == BM_FLOAT32) {
        output = reinterpret_cast<float*>(out.sys_data());
    } else {
        output = new float[out_size];
    }

    // set io_mode SYSO:Both input and output tensors are in system memory.
    engine.set_io_mode(graph_name, sail::SYSO);

    // init preprocessor and postprocessor
    sail::Bmcv bmcv(handle);
    auto img_dtype = bmcv.get_bm_image_data_format(input_dtype);
    float scale = engine.get_input_scale(graph_name, input_name);
    BmcvPreProcessor preprocessor(bmcv, scale);
    
    PostProcessor postprocessor(output_shape[0], output_shape[1], 3);

    // init decoder.
    sail::Decoder decoder(input_path, true, tpu_id);
    bool status = true;
    
    // read an image from a image file or a video file
    sail::BMImage img0;
    int ret = decoder.read(handle, img0);
    if (ret != 0) {
        cout<<"Finished to read the video!"<<endl;
        return false;
    }
        
    // preprocess
    sail::BMImage img1(handle, input_shape[2], input_shape[3],
                    FORMAT_BGR_PLANAR, img_dtype);
    preprocessor.process(img0, img1);
    bmcv.bm_image_to_tensor(img1, in);  
        
    //inference
    engine.process(graph_name, input_tensors, input_shapes, output_tensors);
    auto real_output_shape = engine.get_output_shape(graph_name, output_name);

    // scale output data if input data type is int8 or uint8
    if (output_dtype != BM_FLOAT32) {
        float scale = engine.get_output_scale(graph_name, output_name);
        if (output_dtype == BM_INT8) {
            engine.scale_int8_to_fp32(reinterpret_cast<int8_t*>(out.sys_data()),
                                    output, scale, out_size);
        } else if (output_dtype == BM_UINT8) {
            engine.scale_uint8_to_fp32(reinterpret_cast<uint8_t*>(out.sys_data()),
                                        output, scale, out_size);
        }
    }

    //postprocess
    auto result = postprocessor.process(output);

    // debug yyf
    // for(int i = 0; i<3; i++)
    // {
    //     std::cout << output[i] << ", ";
    // }
    // std::cout << std::endl;
    // debug yyf
    // print result
    for (auto item : result) {
        spdlog::info("Top 3 of in thread {} on tpu {}: [{}]", input_path, engine.get_device_id(), fmt::join(item, ", "));
    }

    *out_value =  result[0][0];

    // free data
    if (input_dtype != BM_FLOAT32) {
        delete [] input;
    }
    if (output_dtype != BM_FLOAT32) {
        delete [] output;
    }

    return status;
}

typedef struct Cameras
{
    string rtsp;
    int state;
    string addres;
}Cameras;



int main(int argc, char *argv[]){
    //read params 
    if (argc < 3) {
        cout << "USAGE:" << endl;
        cout << "  " << argv[0] << " <image path> <bmodel path> <device id>\n" << endl;
        exit(1);
    }
    std::string input_path(argv[1]);
    std::string bmodel_path(argv[2]);
    cout << "bmodel file: " << bmodel_path << endl;
    
    int tpu_id = atoi(argv[3]);
    assert(tpu_id == 0);

    vector<struct Cameras> mycameras; 

    //parser json config<image path>
    Json::Reader reader;
    Json::Value root;
    Json::Value::Members members;
    ifstream ifs;
    ifs.open(input_path);
    if(!ifs.is_open())
    {
        cout << "open " << input_path << "error." << endl;
        return -1;
    }

    if(!reader.parse(ifs, root))
    {
        cout << "parser " << input_path << "error." << endl;
        return -1;
    }

    if(root.isMember("cameras"))
    {
        members = root["cameras"].getMemberNames();
    }

    for(Json::Value::Members::iterator it = members.begin(); it != members.end(); it++)
    {
        struct Cameras camera;
        string key = *it;
        camera.addres = key;
        camera.rtsp = root["cameras"][key]["rtsp"].asString();
        camera.state = root["cameras"][key]["state"].asInt();
        mycameras.push_back(camera);
    }

    // cout << "=====================" << endl;
    // for(size_t i = 0; i<mycameras.size(); i++)
    // {
    //     cout << "camera: " << i << endl;
    //     cout << "rtsp:" << mycameras[i].rtsp << endl;
    //     cout << "addres:" << mycameras[i].addres << endl;
    //     cout << "state:" << mycameras[i].state << endl;
    //     cout << endl;
    // }
    // cout << "======================" << endl;



    bool status = true;
    //load bmodel and do inference
    while(1)
    {
        for(size_t i = 0; i<mycameras.size(); i++)
        {
            string input_path = mycameras[i].rtsp;
            int out_value = -1;
            status = inference(bmodel_path, input_path, tpu_id, &out_value);
            cout << "output: " << out_value << endl;
        }
    }
    // bool status = inference(bmodel_path, input_path, tpu_id);
    if (status){
        return 0;
    }else{
        return 1;
    }

    return 0;
}
/* Copyright 2016-2022 by Bitmain Technologies Inc. All rights reserved.

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

/** @file     algokit.h
 *  @brief    Header file of algo kit
 *  @author   sophgo
 *  @version  3.3.0
 *  @date     2019-12-27
 */

#pragma once
#ifndef TPUKERNRL_OFF

#ifdef PYTHON
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#endif
#include <tensor.h>
#include <string>
#include <vector>
#include <iostream>
#include <algokit.h>
#include <map>
#include "bmlib_runtime.h"
using namespace std;

namespace sail {
typedef enum {
  SAIL_TPU_KERNEL_SUCCESS = 0,
  SAIL_TPU_KERNEL_ERROR_PARAM_MISMATCH = 60001,
  SAIL_TPU_KERNEL_ERROR_SHAPE = 60002,
  SAIL_TPU_KERNEL_ERROR_BATCH_SIZE = 60003,
} sail_tpu_kernel_status;

class DECL_EXPORT tpu_kernel_api_yolov5_detect_out{
    public:
    /**
     * @brief Constructor
     * 
     * @param device_id         Device id.
     * @param shape             Input Data shape 
     * @param network_w         Network input width 
     * @param network_h         Network input height 
     * @param module_file       TPU Kernel module file.
     */
    explicit tpu_kernel_api_yolov5_detect_out(int device_id, 
                                            const std::vector<std::vector<int>>& shapes, 
                                            int network_w=640, 
                                            int network_h=640,
                                            std::string module_file = "/opt/sophon/libsophon-current/lib/tpu_module/libbm1684x_kernel_module.so");

    /**
     * @brief Destructor 
     */  
    ~tpu_kernel_api_yolov5_detect_out();

    /**
     * @brief Porcess
     * 
     * @param input             Input Data 
     * @param out_doxs          Detect results 
     * @param dete_threshold    Detection threshold
     * @param nms_threshold     NMS threshold
     * @return 0 for success and other for failure
     */
    int process(std::vector<TensorPTRWithName>& input, 
                std::vector<std::vector<DeteObjRect>> &out_doxs,
                float dete_threshold,
                float nms_threshold);

    /**
     * @brief Porcess
     * 
     * @param input             Input Data 
     * @param dete_threshold    Detection threshold
     * @param nms_threshold     NMS threshold
     * @return Detect results[left,top,right,bottom, class_id, score]
     */
    std::vector<std::vector<std::tuple<int, int, int, int ,int, float>>> process(std::vector<TensorPTRWithName>& input, float dete_threshold, float nms_threshold);
    
    std::vector<std::vector<std::tuple<int, int, int, int ,int, float>>> process(std::map<std::string, Tensor&>& input, float dete_threshold, float nms_threshold);
    
    /**
     * @brief Reset Anchor
     * @param anchors  new anchors
     * @return 0 for success and other for failure
     */
    int reset_anchors(std::vector<std::vector<std::vector<int>>> anchors_new);

    private:
    class tpu_kernel_api_yolov5_detect_out_cc;
    class tpu_kernel_api_yolov5_detect_out_cc* const _impl;
};

class DECL_EXPORT tpu_kernel_api_yolov5_out_without_decode{
    public:
    /**
     * @brief Constructor
     * 
     * @param device_id         Device id.
     * @param shape             Input Data shape 
     * @param network_w         Network input width 
     * @param network_h         Network input height 
     * @param module_file       TPU Kernel module file.
     */
    explicit tpu_kernel_api_yolov5_out_without_decode(int device_id, 
                                            const std::vector<int>& shapes, 
                                            int network_w=640, 
                                            int network_h=640,
                                            std::string module_file = "/opt/sophon/libsophon-current/lib/tpu_module/libbm1684x_kernel_module.so");

    /**
     * @brief Destructor 
     */  
    ~tpu_kernel_api_yolov5_out_without_decode();

    /**
     * @brief Porcess
     * 
     * @param input             Input Data 
     * @param out_doxs          Detect results 
     * @param dete_threshold    Detection threshold
     * @param nms_threshold     NMS threshold
     * @return 0 for success and other for failure
     */
    int process(TensorPTRWithName& input, 
                std::vector<std::vector<DeteObjRect>> &out_doxs,
                float dete_threshold,
                float nms_threshold);

    /**
     * @brief Porcess
     * 
     * @param input             Input Data 
     * @param dete_threshold    Detection threshold
     * @param nms_threshold     NMS threshold
     * @return Detect results[left,top,right,bottom, class_id, score]
     */
    std::vector<std::vector<std::tuple<int, int, int, int ,int, float>>> process(TensorPTRWithName& input, float dete_threshold, float nms_threshold);
    
    std::vector<std::vector<std::tuple<int, int, int, int ,int, float>>> process(std::map<std::string, Tensor&>& input, float dete_threshold, float nms_threshold);

    private:
    class tpu_kernel_api_yolov5_out_without_decode_cc;
    class tpu_kernel_api_yolov5_out_without_decode_cc* const _impl;
};

}
#endif
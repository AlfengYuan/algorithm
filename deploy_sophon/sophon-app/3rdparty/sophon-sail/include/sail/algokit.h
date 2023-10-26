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

#ifdef PYTHON
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#endif
#include <tensor.h>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

typedef enum {
  SAIL_ALGO_SUCCESS = 0,
  SAIL_ALGO_BUFFER_FULL = 50001,    
  SAIL_ALGO_BUFFER_EMPRT = 50002,
  SAIL_ALGO_ERROR_D2S = 50003,
  SAIL_ALGO_ERROR_BATCHSIZE = 50004,
  SAIL_ALGO_ERROR_SHAPES = 50005,
} sail_algo_status;

namespace sail {

struct DeteObjRect {
    unsigned int class_id;
    float score;
    float left;
    float top;
    float right;
    float bottom;
    float width;
    float height;
};

struct TrackObjRect : public DeteObjRect {
    struct DeteObjRect;
    unsigned int track_id;
};

#ifdef USE_OPENCV
class DECL_EXPORT algo_yolov5_post_1output {
    public:
    /**
     * @brief Constructor
     * 
     * @param shape             Input Data shape 
     * @param network_w         Network input width 
     * @param network_h         Network input height 
     * @param max_queue_size    Data queue max size 
     */
    explicit algo_yolov5_post_1output(const std::vector<int>& shape, int network_w=640, int network_h=640, int max_queue_size=20);

    /**
     * @brief Destructor 
     */  
    ~algo_yolov5_post_1output();


    /**
     * @brief Push Data
     * 
     * @param channel_idx       Channel index number of the image.
     * @param image_idx         Image index number of the image.
     * @param data              Input Data ptr
     * @param dete_threshold    Detection threshold
     * @param nms_threshold     NMS threshold
     * @param ost_w             Original image width
     * @param ost_h             Original image height
     * @param padding_left      Padding left
     * @param padding_top       Padding top
     * @param padding_width     Padding width
     * @param padding_height    Padding height
     * @return 0 for success and other for failure 
     */
#ifdef PYTHON
    int push_npy(
        int channel_idx, 
        int image_idx, 
        pybind11::array_t<float> data, 
        float dete_threshold, 
        float nms_threshold,
        int ost_w, 
        int ost_h, 
        int padding_left, 
        int padding_top, 
        int padding_width, 
        int padding_height);
#endif

    /**
     * @brief Push Data
     * 
     * @param channel_idx       Channel index number of the image.
     * @param image_idx         Image index number of the image.
     * @param input_data        Input Data
     * @param dete_threshold    Detection threshold
     * @param nms_threshold     NMS threshold
     * @param ost_w             Original image width
     * @param ost_h             Original image height
     * @param padding_attr      Padding Attribute(start_x, start_y, width, height)
     * @return 0 for success and other for failure 
     */
    int push_data(
        std::vector<int> channel_idx, 
        std::vector<int> image_idx, 
        TensorPTRWithName input_data, 
        std::vector<float> dete_threshold,
        std::vector<float> nms_threshold,
        std::vector<int> ost_w,
        std::vector<int> ost_h,
        std::vector<std::vector<int>> padding_attr);
        
    /**
     * @brief Get Result
     * 
     * @return Detect results, channle index, image index
     */
    std::tuple<std::vector<DeteObjRect>,int,int> get_result();

    /**
     * @brief Get Result with numpy array
     * 
     * @return Detect results[left,top,right,bottom, class_id, score], channle index, image index
     */
    std::tuple<std::vector<std::tuple<int, int, int, int ,int, float>>,int,int> get_result_npy();

    private:
    class algo_yolov5_post_1output_cc;
    class algo_yolov5_post_1output_cc* const _impl;

    /**
     * @brief Forbidden copy constructor.
     * @brief Copy constructor.
     *
     * @param other An other algo_yolov5_post_1output instance.
     */
    algo_yolov5_post_1output(const algo_yolov5_post_1output& other) = delete;

    /**
     * @brief Forbidden assignment function.
     * @brief Assignment function.
     *
     * @param other An other algo_yolov5_post_1output instance.
     * @return Reference of a algo_yolov5_post_1output instance.
     */
    algo_yolov5_post_1output& operator=(const algo_yolov5_post_1output& other) = delete;
};

// algo_yolov5_post_3output
class DECL_EXPORT algo_yolov5_post_3output {
    public:
    /**
     * @brief Constructor
     * 
     * @param shape             Input Data shape 
     * @param network_w         Network input width 
     * @param network_h         Network input height 
     * @param max_queue_size    Data queue max size 
     */
    explicit algo_yolov5_post_3output(const std::vector<std::vector<int>>& shape, int network_w=640, int network_h=640, int max_queue_size=20);

    /**
     * @brief Destructor 
     */  
    ~algo_yolov5_post_3output();

    /**
     * @brief Push Data
     * 
     * @param channel_idx       Channel index number of the image.
     * @param image_idx         Image index number of the image.
     * @param input_data        Input Data
     * @param dete_threshold    Detection threshold
     * @param nms_threshold     NMS threshold
     * @param ost_w             Original image width
     * @param ost_h             Original image height
     * @param padding_attr      Padding Attribute(start_x, start_y, width, height)
     * @return 0 for success and other for failure 
     */
    int push_data(
        std::vector<int> channel_idx, 
        std::vector<int> image_idx, 
        std::vector<TensorPTRWithName> input_data, 
        std::vector<float> dete_threshold,
        std::vector<float> nms_threshold,
        std::vector<int> ost_w,
        std::vector<int> ost_h,
        std::vector<std::vector<int>> padding_attr);
        
    /**
     * @brief Get Result
     * 
     * @return Detect results, channle index, image index
     */
    std::tuple<std::vector<DeteObjRect>,int,int> get_result();

    /**
     * @brief Get Result with numpy array
     * 
     * @return Detect results[left,top,right,bottom, class_id, score], channle index, image index
     */
    std::tuple<std::vector<std::tuple<int, int, int, int ,int, float>>,int,int> get_result_npy();

    /**
     * @brief Reset Anchor
     * @param anchors  new anchors
     * @return 0 for success and other for failure
     */
    int reset_anchors(std::vector<std::vector<std::vector<int>>> anchors_new);

    private:
    class algo_yolov5_post_3output_cc;
    class algo_yolov5_post_3output_cc* const _impl;

    /**
     * @brief Forbidden copy constructor.
     * @brief Copy constructor.
     *
     * @param other An other algo_yolov5_post_3output instance.
     */
    algo_yolov5_post_3output(const algo_yolov5_post_3output& other) = delete;

    /**
     * @brief Forbidden assignment function.
     * @brief Assignment function.
     *
     * @param other An other algo_yolov5_post_3output instance.
     * @return Reference of a algo_yolov5_post_3output instance.
     */
    algo_yolov5_post_3output& operator=(const algo_yolov5_post_3output& other) = delete;
};

/*下面的内容都是跟目标跟踪有关*/
class DECL_EXPORT deepsort_tracker_controller {
    public:
    explicit deepsort_tracker_controller(float max_cosine_distance, int nn_budget, int k_feature_dim, float max_iou_distance = 0.7, int max_age = 30, int n_init = 3);
    ~deepsort_tracker_controller();
    int process(const vector<DeteObjRect>& detected_objects, vector<Tensor>& feature, vector<TrackObjRect>& tracked_objects);
    
    private:
    class deepsort_tracker_controller_cc;
    class deepsort_tracker_controller_cc* const _impl;
};

class DECL_EXPORT bytetrack_tracker_controller {
    public:
    explicit bytetrack_tracker_controller(int frame_rate = 30, int track_buffer = 30);
    ~bytetrack_tracker_controller();
    int process(const vector<DeteObjRect>& detected_objects, vector<TrackObjRect>& tracked_objects);
    
    private:
    class bytetrack_tracker_controller_cc;
    class bytetrack_tracker_controller_cc* const _impl;
};
#endif

}// namespace sail


cmake_minimum_required(VERSION 3.24)
project(sophon-app)
#include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(CMAKE_BUILD_TYPE "Debug")
#add_compile_options(-Wall -Wextra -fpermissive)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O3")
# Enable C++17
set(CMAKE_CXX_STANDARD 17)
#add_definitions(-w)

set(TARGET_ARCH soc)
set(SDK ${PROJECT_SOURCE_DIR}/3rdparty/soc-sdk)
set(SAIL_PATH ${PROJECT_SOURCE_DIR}/3rdparty/sophon-sail)
set(MQTT_PATH ${PROJECT_SOURCE_DIR}/3rdparty/paho-mqtt)
set(MINIO_PATH ${PROJECT_SOURCE_DIR}/3rdparty/minio)
set(NLOHMANN_JSON_PATH ${PROJECT_SOURCE_DIR}/3rdparty/nlohmann-json)
set(CURLPP_PATH ${PROJECT_SOURCE_DIR}/3rdparty/curlpp)
set(CURL_PATH ${PROJECT_SOURCE_DIR}/3rdparty/curl)
set(OPENSSL3_PATH ${PROJECT_SOURCE_DIR}/3rdparty/openssl3)
set(PUGIXML_PATH ${PROJECT_SOURCE_DIR}/3rdparty/pugixml)
set(INIH_PATH ${PROJECT_SOURCE_DIR}/3rdparty/inih)
set(ZLIB_PATH ${PROJECT_SOURCE_DIR}/3rdparty/zlib)
set(JSONCPP_PATH ${PROJECT_SOURCE_DIR}/3rdparty/jsoncpp)
set(BOOST_PATH ${PROJECT_SOURCE_DIR}/3rdparty/boost)
#check_and_replace_path(${SDK})
#check_and_replace_path(${SAIL_PATH})
#check_and_replace_path(${MQTT_PATH})
#check_and_replace_path(${JSONCPP_PATH})
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_ASM_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(BM_LIBS bmlib bmrt bmcv yuv)
set(JPU_LIBS bmjpuapi bmjpulite)
set(OPENCV_LIBS opencv_imgproc opencv_core opencv_highgui opencv_imgcodecs opencv_videoio)

message("SOC-SDK: ${SDK}")
include_directories("${SDK}/include/")
include_directories("${SDK}/include/opencv4/")
link_directories("${SDK}/lib/")

message("SAIL_PATH: ${SAIL_PATH}")
include_directories("${SAIL_PATH}/include")
include_directories("${SAIL_PATH}/include/sail")
set(SAIL_LIBS "${SAIL_PATH}/lib/libsail.so")

message("PAHO-MQTT: ${MQTT_PATH}")
include_directories("${MQTT_PATH}/include")
link_directories("${MQTT_PATH}/lib")
set(MQTTLIBS paho-mqttpp3 paho-mqtt3as paho-mqtt3c)

message("MINIO: ${MINIO_PATH}")
include_directories("${MINIO_PATH}/include")
link_directories("${MINIO_PATH}/lib")
set(MINIOLIBS miniocpp)

message("NLOHMANN_JSON: ${NLOHMANN_JSON_PATH}")
include_directories("${NLOHMANN_JSON_PATH}/include")

message("CURLPP: ${CURLPP_PATH}")
include_directories("${CURLPP_PATH}/include")
link_directories("${CURLPP_PATH}/lib")
set(CURLPPLIBS curlpp)

message("CURL: ${CURL_PATH}")
include_directories("${CURL_PATH}/include")
link_directories("${CURL_PATH}/lib")
set(CURLLIBS curl)

message("OPENSSL3: ${OPENSSL3_PATH}")
include_directories("${OPENSSL3_PATH}/include")
link_directories("${OPENSSL3_PATH}/lib")
set(OPENSSLLIBS ssl crypto)

message("PUGIXML: ${PUGIXML_PATH}")
include_directories("${PUGIXML_PATH}/include")
link_directories("${PUGIXML_PATH}/lib")
set(PUGIXMLLIBS pugixml)

message("INIH: ${INIH_PATH}")
include_directories("${INIH_PATH}/include")
link_directories("${INIH_PATH}/lib")
set(INIHLIBS inih)

message("ZLIB: ${ZLIB_PATH}")
include_directories("${ZLIB_PATH}/include")
link_directories("${ZLIB_PATH}/lib")
set(ZLIBLIBS z)

message("JSONCPP: ${JSONCPP_PATH}")
include_directories("${JSONCPP_PATH}/include")
link_directories("${JSONCPP_PATH}/lib")
set(JSONCPPLIBS jsoncpp)

message("BOOST: ${BOOST_PATH}")
include_directories("${BOOST_PATH}/include")
link_directories("${BOOST_PATH}/lib")



add_compile_definitions(USE_BMCV=1)
add_compile_definitions(USE_FFMPEG=1)
add_compile_definitions(USE_OPENCV=1)
include_directories("${PROJECT_SOURCE_DIR}/include")
aux_source_directory(./src SRC_FILES)

add_executable(${PROJECT_NAME}.soc ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}.soc ${BM_LIBS} ${OPENCV_LIBS} ${JPU_LIBS} ${SAIL_LIBS} ${MQTTLIBS} ${MINIOLIBS}
        ${CURLPPLIBS} ${CURLLIBS} ${OPENSSLLIBS} ${PUGIXMLLIBS} ${INIHLIBS} ${ZLIBLIBS} ${JSONCPPLIBS}
        -lavcodec -lavformat -lavutil -lbmcv -lpthread -ldl)

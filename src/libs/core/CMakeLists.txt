cmake_minimum_required(VERSION 3.20)

project(qrcode_detect_core CXX)

message(STATUS "Configuration: ${PROJECT_NAME}")


set(YOLOV3_MODEL_DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/download)
set(YOLOV3_MODEL_UNZIP_DIR ${CMAKE_BINARY_DIR}/download/yolov3-qrcode)

download_and_extract(
    URL https://github.com/0x1306a94/qrcode_detect/releases/download/yolov3_model_v1/yolov3-qrcode.zip
    FILENAME ${YOLOV3_MODEL_DOWNLOAD_DIR}/yolov3-qrcode.zip
    HASH_TYPE SHA256
    HASH 30bacfe6552fe20f9d8d060f7b16c31a65b1f05c99ca544aa15594d903bb48f6
    EXTRACT_DIR ${YOLOV3_MODEL_UNZIP_DIR}
)

if(APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

find_library(SPDLOG_LIB spdlog HINTS "${SPDLOG_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "SPDLOG_LIB: ${SPDLOG_LIB}")

find_library(ZBAR_LIBS libzbar.a HINTS "${ZBAR_LIB_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "ZBAR_LIBS: ${ZBAR_LIBS}")

find_package(Eigen3 REQUIRED)
message(STATUS "Eigen3_LIBS: ${Eigen3_LIBS}")

find_package(Iconv REQUIRED)
message(STATUS "Iconv_LIBS: ${Iconv_LIBS}")
set(OpenCV_DIR ${OPENCV_DIR})
set(OpenCV_STATIC ON)
set(OpenCV_SHARED OFF)
find_package(OpenCV REQUIRED 
    COMPONENTS 
    opencv_core
    opencv_imgcodecs 
    opencv_imgproc 
    opencv_flann 
    opencv_dnn 
    opencv_features2d 
    opencv_calib3d 
    opencv_wechat_qrcode
    PATHS ${OPENCV_DIR} NO_DEFAULT_PATH)
message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")

if(APPLE)
  set(FILE_GLOB_OPTION RELATIVE)
endif()

file(GLOB SRC_SRCS ./*.h ./*.hpp ./*.cpp)

source_group(src FILES ${SRC_SRCS})

add_library(${PROJECT_NAME} STATIC ${SRC_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE 
./
${SPDLOG_DIR}/include
${OPENCV_DIR}/include/opencv4
${ZBAR_LIB_DIR}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC 
${SPDLOG_LIB}
${OpenCV_LIBS}
${ZBAR_LIBS}
)

if(APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE FALSE
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
    XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
    XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
  )
elseif(UNIX)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})
endif()

if(NOT BUILD_DEB_PACKAGE)
  install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin
  )
endif()

install(DIRECTORY
  ${OPENCV_SOURCE_DIR}/cmake_build/downloads/wechat_qrcode
  ${YOLOV3_MODEL_UNZIP_DIR}
  DESTINATION models
)
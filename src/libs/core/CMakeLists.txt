cmake_minimum_required(VERSION 3.20)

project(qrcode_detect_core CXX)

message(STATUS "Configuration: ${PROJECT_NAME}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

configure_file(
  version.hpp.in
  ${CONFIGURE_AUTO_HEADER_ROOT_DIR}/${INSTALL_PUBLIC_INCLUDE_ROOT}/core/version.hpp
  @ONLY)

find_package(spdlog REQUIRED PATHS ${SPDLOG_DIR} NO_DEFAULT_PATH)

find_library(ZBAR_LIBS zbar HINTS "${LIBZBAR_DIR}/lib" "${LIBZBAR_DIR}/lib64" NO_DEFAULT_PATH REQUIRED)
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

file(GLOB SRC_SRCS ./*.h ./*.hpp ./*.cpp)
source_group(src FILES ${SRC_SRCS})

file(
  GLOB LIB_PUBLIC_HEADERS ${FILE_GLOB_OPTION}
  ${WORKSPACE_PUBLIC_INCLUDE_ROOT}/${INSTALL_PUBLIC_INCLUDE_ROOT}/core/*.h
  ${WORKSPACE_PUBLIC_INCLUDE_ROOT}/${INSTALL_PUBLIC_INCLUDE_ROOT}/core/*.hpp)
list(
  APPEND
  LIB_PUBLIC_HEADERS
  ${CONFIGURE_AUTO_HEADER_ROOT_DIR}/${INSTALL_PUBLIC_INCLUDE_ROOT}/core/version.hpp
)

source_group("Public Header Files" FILES ${LIB_PUBLIC_HEADERS})
message(STATUS "LIB_PUBLIC_HEADERS: ${LIB_PUBLIC_HEADERS}")

add_library(${PROJECT_NAME} SHARED ${SRC_SRCS} ${LIB_PUBLIC_HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE 
./
${SPDLOG_DIR}/include
${OPENCV_DIR}/include/opencv4
${LIBZBAR_DIR}/include)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${LIB_PUBLIC_HEADERS}")

target_link_libraries(${PROJECT_NAME} PRIVATE 
  spdlog::spdlog
  ${OpenCV_LIBS}
  ${ZBAR_LIBS}
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES PUBLIC_HEADER "${LIB_PUBLIC_HEADERS}"
             VERSION "${QRCODE_DETECT_VERSION}" # 库的版本号
             SOVERSION "${QRCODE_DETECT_API_VERSION}" # 库的API版本号
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


install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  BUNDLE DESTINATION bin
  PUBLIC_HEADER DESTINATION include/${INSTALL_PUBLIC_INCLUDE_ROOT}/core
)

install(DIRECTORY
  ${OPENCV_SOURCE_DIR}/cmake_build/downloads/wechat_qrcode
  ${YOLOV3_MODEL_UNZIP_DIR}
  DESTINATION models
)

if(APPLE)
    message(STATUS "TARGET_FINAL_FILE: ${TARGET_FINAL_FILE}")
    install(CODE "
        message(STATUS \"Checking dependencies for ${PROJECT_NAME}\")
        execute_process(
            COMMAND /usr/bin/otool -L \${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
            OUTPUT_VARIABLE OTOOL_OUTPUT
            RESULT_VARIABLE OTOOL_RESULT
        )
        if (NOT \${OTOOL_RESULT} EQUAL 0)
            message(FATAL_ERROR \"otool failed with exit code \${OTOOL_RESULT}\")
        endif()
        string(FIND \"\${OTOOL_OUTPUT}\" \"${LIBZBAR_DIR}/lib/libzbar.0.dylib\" OTOOL_INDEX)
        if (NOT \${OTOOL_INDEX} EQUAL -1)
          execute_process(
              COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change ${LIBZBAR_DIR}/lib/libzbar.0.dylib @rpath/libzbar.dylib \${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
              OUTPUT_VARIABLE OUTPUT
              ERROR_VARIABLE ERROR
              RESULT_VARIABLE RESULT
          )
          message(STATUS \"Modifying install_name for libzbar\")
        else()
          message(STATUS \"libzbar.0.dylib not found in dependencies, skipping install_name_tool\")
        endif()
    ")
endif()

if(EXISTS "${SPDLOG_DIR}/lib64")
  file(GLOB SPDLOG_LIB_FILES "${SPDLOG_DIR}/lib64/lib*.dylib" "${SPDLOG_DIR}/lib64/lib*.so" "${SPDLOG_DIR}/lib64/lib*.so.*")
else()
  file(GLOB SPDLOG_LIB_FILES "${SPDLOG_DIR}/lib/lib*.dylib" "${SPDLOG_DIR}/lib/lib*.so" "${SPDLOG_DIR}/lib/lib*.so.*")
endif()
message(STATUS "SPDLOG_LIB_FILES: ${SPDLOG_LIB_FILES}")
install(FILES ${SPDLOG_LIB_FILES} DESTINATION lib)

if(EXISTS "${OPENCV_DIR}/lib64")
  file(GLOB OPENCV_LIB_FILES "${OPENCV_DIR}/lib64/lib*.dylib" "${OPENCV_DIR}/lib64/lib*.so" "${OPENCV_DIR}/lib64/lib*.so.*")
else()
  file(GLOB OPENCV_LIB_FILES "${OPENCV_DIR}/lib/lib*.dylib" "${OPENCV_DIR}/lib/lib*.so" "${OPENCV_DIR}/lib/lib*.so.*")
endif()
message(STATUS "OPENCV_LIB_FILES: ${OPENCV_LIB_FILES}")
install(FILES ${OPENCV_LIB_FILES} DESTINATION lib)

if(EXISTS "${LIBZBAR_DIR}/lib64")
  file(GLOB ZBAR_LIB_FILES "${LIBZBAR_DIR}/lib64/lib*.dylib" "${LIBZBAR_DIR}/lib/lib*.so" "${LIBZBAR_DIR}/lib/lib*.so.*")
else()
  file(GLOB ZBAR_LIB_FILES "${LIBZBAR_DIR}/lib/lib*.dylib" "${LIBZBAR_DIR}/lib/lib*.so" "${LIBZBAR_DIR}/lib/lib*.so.*")
endif()
message(STATUS "ZBAR_LIB_FILES: ${ZBAR_LIB_FILES}")
install(FILES ${ZBAR_LIB_FILES} DESTINATION lib)
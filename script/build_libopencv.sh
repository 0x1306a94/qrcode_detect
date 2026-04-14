#!/usr/bin/env bash

set -ex

source "$(realpath "$(dirname "$0")")/check_env.sh"

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
LIB_NAME=opencv
THIRD_PARTY_DIR=$GIT_ROOT_DIR/third_party
THIRD_PARTY_BUILD_DIR=$GIT_ROOT_DIR/third_party_build
BUILD_SOURCE_DIR=$THIRD_PARTY_DIR/$LIB_NAME
WECHAT_QR_CODE_SOURCE_DIR=$THIRD_PARTY_BUILD_DIR/opencv_contrib_wechat_qrcode
BUILD_INSTALL_DIR=$THIRD_PARTY_BUILD_DIR/$LIB_NAME
BUILD_TEMP_DIR=$THIRD_PARTY_BUILD_DIR/cmake_build/$LIB_NAME

if [[ -d "$BUILD_TEMP_DIR" ]]; then
    rm -rf $BUILD_TEMP_DIR
fi

if [[ -d "$BUILD_INSTALL_DIR" ]]; then
    rm -rf $BUILD_INSTALL_DIR
fi

rm -rf $WECHAT_QR_CODE_SOURCE_DIR
mkdir -p $WECHAT_QR_CODE_SOURCE_DIR/modules
cp -rf $THIRD_PARTY_DIR/opencv_contrib/modules/wechat_qrcode $WECHAT_QR_CODE_SOURCE_DIR/modules

BUILD_SHARED_LIBS=ON

function append_common_cmake_flags() {
    CMAKE_ARGS+=(
        -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE=Release
        -DOPENCV_GENERATE_PKGCONFIG=ON
        -DOPENCV_FORCE_3RDPARTY_BUILD=ON
        -DBUILD_LIST=core,imgproc,imgcodecs,objdetect,dnn,wechat_qrcode,highgui
        -DBUILD_JPEG=ON
        -DBUILD_PNG=ON
        -DBUILD_WEBP=ON
        -DBUILD_ZLIB=OFF
        -DBUILD_TBB=OFF
        -DBUILD_IPP_IW=OFF
        -DBUILD_ITT=OFF
        -DWITH_JPEG=ON
        -DWITH_PNG=ON
        -DWITH_WEBP=ON
        -DWITH_OPENCL=OFF
        -DOPENCV_DNN_OPENCL=OFF
        -DJPEG_INCLUDE_DIR="$BUILD_SOURCE_DIR/3rdparty/libjpeg"
        -DPNG_INCLUDE_DIR="$BUILD_SOURCE_DIR/3rdparty/libpng"
        -DWEBP_INCLUDE_DIR="$BUILD_SOURCE_DIR/3rdparty/libwebp"
        -DOPENCV_EXTRA_MODULES_PATH="$WECHAT_QR_CODE_SOURCE_DIR/modules"
        -DBUILD_opencv_wechat_qrcode=ON
        -DWITH_AVFOUNDATION=OFF
        -DWITH_FFMPEG=OFF
        -DWITH_PROTOBUF=ON
        -DBUILD_opencv_highgui=ON
        -DBUILD_TESTS=OFF
        -DBUILD_opencv_java=OFF
        -DBUILD_opencv_python=OFF
        -DBUILD_opencv_python2=OFF
        -DBUILD_opencv_python3=OFF
        -DBUILD_opencv_js=OFF
        -DBUILD_opencv_ts=OFF
        -DCV_TRACE=OFF
        -DCMAKE_INSTALL_PREFIX="$BUILD_INSTALL_DIR"
    )
}

function append_macos_cmake_flags() {
    local host_arch=$(uname -m)
    echo "[*] configure macOS host: ${host_arch}"
}

function append_linux_cmake_flags() {
    local host_arch=$(uname -m)
    echo "[*] configure Linux host: ${host_arch}"

    if [[ "$host_arch" == "x86_64" ]]; then
        # Keep the baseline conservative for broad compatibility.
        # Dispatch entries are optional runtime-selected optimized paths.
        CMAKE_ARGS+=(
            -DCPU_BASELINE=SSE3
            -DCPU_DISPATCH=SSE4_1,SSE4_2,AVX,FP16,AVX2
            -DENABLE_LTO=ON
        )
    fi
}

function configure_cmake_args() {
    append_common_cmake_flags

    local host_arm64_mac=1
    local host_x86_64_mac=1
    local host_linux=1

    if check_is_arm64_macos; then
        host_arm64_mac=0
    fi

    if check_is_x86_64_macos; then
        host_x86_64_mac=0
    fi

    if check_is_linux; then
        host_linux=0
    fi

    if [[ $host_arm64_mac -eq 0 || $host_x86_64_mac -eq 0 ]]; then
        append_macos_cmake_flags
    elif [[ $host_linux -eq 0 ]]; then
        append_linux_cmake_flags
    else
        echo "[*] unsupported host: $(uname -s) $(uname -m)"
        exit 1
    fi
}

CMAKE_ARGS=()
configure_cmake_args

cmake -S $BUILD_SOURCE_DIR -B $BUILD_TEMP_DIR "${CMAKE_ARGS[@]}"

# -DBUILD_opencv_stitching=OFF \
# -DBUILD_opencv_world=OFF \
# -DBUILD_opencv_highgui=OFF \
# -DBUILD_opencv_gapi=OFF \
# -DBUILD_opencv_photo=OFF \
# -DBUILD_opencv_ml=OFF \
# -DBUILD_opencv_video=OFF \
# -DBUILD_opencv_videoio=OFF \

cmake --build $BUILD_TEMP_DIR -j8
# cmake --build $OPENCV_BUILD_DIR --target opencv_core --target opencv_imgcodecs --target opencv_imgproc --target wechat_qrcode -j8
cmake --install $BUILD_TEMP_DIR 

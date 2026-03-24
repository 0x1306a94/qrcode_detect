#!/usr/bin/env bash

set -ex


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
cmake -S $BUILD_SOURCE_DIR -B $BUILD_TEMP_DIR \
-DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS} \
-DCMAKE_BUILD_TYPE=Release \
-DPENCV_GENERATE_PKGCONFIG=ON \
-DOPENCV_FORCE_3RDPARTY_BUILD=ON \
-DBUILD_JPEG=ON \
-DBUILD_PNG=ON \
-DBUILD_WEBP=ON \
-DBUILD_ZLIB=OFF \
-DBUILD_TBB=OFF \
-DBUILD_IPP_IW=OFF \
-DBUILD_ITT=OFF \
-DWITH_JPEG=ON \
-DWITH_PNG=ON \
-DWITH_WEBP=ON \
-DJPEG_INCLUDE_DIR="$BUILD_SOURCE_DIR/3rdparty/libjpeg" \
-DPNG_INCLUDE_DIR="$BUILD_SOURCE_DIR/3rdparty/libpng" \
-DWEBP_INCLUDE_DIR="$BUILD_SOURCE_DIR/3rdparty/libwebp" \
-DOPENCV_EXTRA_MODULES_PATH="$WECHAT_QR_CODE_SOURCE_DIR/modules" \
-DBUILD_opencv_wechat_qrcode=ON \
-DWITH_AVFOUNDATION=OFF \
-DWITH_FFMPEG=OFF \
-DWITH_PROTOBUF=ON \
-DBUILD_opencv_highgui=ON \
-DBUILD_TESTS=OFF \
-DBUILD_opencv_java=OFF \
-DBUILD_opencv_python=OFF \
-DBUILD_opencv_python2=OFF \
-DBUILD_opencv_python3=OFF \
-DBUILD_opencv_js=OFF \
-DBUILD_opencv_ts=OFF \
-DCMAKE_INSTALL_PREFIX="$BUILD_INSTALL_DIR"

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

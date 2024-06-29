#!/usr/bin/env bash

set -ex

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
OPENCV_VERSION="4.9.0"
OPENCV_DIR=$DEPS_DIR/opencv_$OPENCV_VERSION
OPENCV_SOURCE_CONTRIB_DIR=$DEPS_DIR/opencv_contrib_$OPENCV_VERSION
OPENCV_INSTALL_DIR=$GIT_ROOT_DIR/build_3rdparty/opencv_$OPENCV_VERSION
OPENCV_BUILD_DIR=$OPENCV_DIR/cmake_build

if [[ -d "$OPENCV_BUILD_DIR" ]]; then
    rm -rf $OPENCV_BUILD_DIR
fi

if [[ -d "$OPENCV_INSTALL_DIR" ]]; then
    rm -rf $OPENCV_INSTALL_DIR
fi

WECHAT_QR_CODE_SOURCE_DIR=$DEPS_DIR/opencv_contrib_wechat_qrcode

rm -rf $WECHAT_QR_CODE_SOURCE_DIR
mkdir -p $WECHAT_QR_CODE_SOURCE_DIR/modules
cp -rf $OPENCV_SOURCE_CONTRIB_DIR/modules/wechat_qrcode $WECHAT_QR_CODE_SOURCE_DIR/modules

BUILD_SHARED_LIBS=ON
cmake -S $OPENCV_DIR -B $OPENCV_BUILD_DIR \
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
-DJPEG_INCLUDE_DIR="$OPENCV_DIR/3rdparty/libjpeg" \
-DPNG_INCLUDE_DIR="$OPENCV_DIR/3rdparty/libpng" \
-DWEBP_INCLUDE_DIR="$OPENCV_DIR/3rdparty/libwebp" \
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
-DCMAKE_INSTALL_PREFIX="$OPENCV_INSTALL_DIR"

# -DBUILD_opencv_stitching=OFF \
# -DBUILD_opencv_world=OFF \
# -DBUILD_opencv_highgui=OFF \
# -DBUILD_opencv_gapi=OFF \
# -DBUILD_opencv_photo=OFF \
# -DBUILD_opencv_ml=OFF \
# -DBUILD_opencv_video=OFF \
# -DBUILD_opencv_videoio=OFF \

cmake --build $OPENCV_BUILD_DIR -j8
# cmake --build $OPENCV_BUILD_DIR --target opencv_core --target opencv_imgcodecs --target opencv_imgproc --target wechat_qrcode -j8
cmake --install $OPENCV_BUILD_DIR 

cd $CUR_DIR
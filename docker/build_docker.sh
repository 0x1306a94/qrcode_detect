#!/bin/sh -e

set -e

VERSION="v1.0.0"
DOCKER_IMAGE_NAME="qrcode_detect:${VERSION}"
INSTALL_DIR_NAME="qrcode_detect"
GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
CURRENT_SCRIPT_DIR=$(realpath $(dirname $0))
CMAKE_BUILD_DIR=$CURRENT_SCRIPT_DIR/cmake_build
CMAKE_INSTALL_DIR=$CURRENT_SCRIPT_DIR/cmake_install/qrcode_detect

if [ -d "${CMAKE_BUILD_DIR}" ]; then
    rm -rf $CMAKE_BUILD_DIR
fi

if [ -d "${CMAKE_INSTALL_DIR}" ]; then
    rm -rf $CMAKE_INSTALL_DIR
fi

cmake -S ${GIT_ROOT_DIR} \
-B $CMAKE_BUILD_DIR \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX="$CMAKE_INSTALL_DIR"

cmake --build $CMAKE_BUILD_DIR -j8
cmake --install $CMAKE_BUILD_DIR

mkdir -p $CMAKE_INSTALL_DIR/config
echo "/opt/qrcode_detect/lib" > $CMAKE_INSTALL_DIR/config/qrcode_detect.conf

docker build -t "${DOCKER_IMAGE_NAME}" $CURRENT_SCRIPT_DIR
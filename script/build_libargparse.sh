#!/usr/bin/env bash

set -ex

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
LIB_NAME=argparse
THIRD_PARTY_DIR=$GIT_ROOT_DIR/third_party
THIRD_PARTY_BUILD_DIR=$GIT_ROOT_DIR/third_party_build
BUILD_SOURCE_DIR=$THIRD_PARTY_DIR/$LIB_NAME
BUILD_INSTALL_DIR=$THIRD_PARTY_BUILD_DIR/$LIB_NAME
BUILD_TEMP_DIR=$THIRD_PARTY_BUILD_DIR/cmake_build/$LIB_NAME

if [[ -d "$BUILD_TEMP_DIR" ]]; then
    rm -rf $BUILD_TEMP_DIR
fi

if [[ -d "$BUILD_INSTALL_DIR" ]]; then
    rm -rf $BUILD_INSTALL_DIR
fi


cmake -S $BUILD_SOURCE_DIR -B $BUILD_TEMP_DIR \
-DCMAKE_INSTALL_PREFIX="$BUILD_INSTALL_DIR"

cmake --build $BUILD_TEMP_DIR -j8
cmake --install $BUILD_TEMP_DIR

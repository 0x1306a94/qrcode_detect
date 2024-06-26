#!/usr/bin/env bash

set -ex

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
SPDLOG_VERSION="v1.11.0"
SPDLOG_DIR=$DEPS_DIR/spdlog_$SPDLOG_VERSION
SPDLOG_INSTALL_DIR=$GIT_ROOT_DIR/build_3rdparty/spdlog_$SPDLOG_VERSION
SPDLOG_BUILD_DIR=$SPDLOG_DIR/cmake_build

if [[ -d "$SPDLOG_BUILD_DIR" ]]; then
    rm -rf $SPDLOG_BUILD_DIR
fi

if [[ -d "$SPDLOG_INSTALL_DIR" ]]; then
    rm -rf $SPDLOG_INSTALL_DIR
fi

cd $SPDLOG_DIR

cmake -S . -B $SPDLOG_BUILD_DIR \
-DCMAKE_BUILD_TYPE=Release \
-DSPDLOG_BUILD_PIC=ON \
-DSPDLOG_BUILD_SHARED=ON \
-DCMAKE_INSTALL_PREFIX="$SPDLOG_INSTALL_DIR"

cmake --build $SPDLOG_BUILD_DIR -j8
cmake --install $SPDLOG_BUILD_DIR

cd $CUR_DIR
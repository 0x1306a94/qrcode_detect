#!/usr/bin/env bash

set -ex

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
ARGPARSE_VERSION="v3.0"
ARGPARSE_DIR=$DEPS_DIR/argparse_$ARGPARSE_VERSION
ARGPARSE_INSTALL_DIR=$GIT_ROOT_DIR/build_3rdparty/argparse_$ARGPARSE_VERSION
ARGPARSE_BUILD_DIR=$ARGPARSE_DIR/cmake_build

if [[ -d "$ARGPARSE_BUILD_DIR" ]]; then
    rm -rf $ARGPARSE_BUILD_DIR
fi

if [[ -d "$ARGPARSE_INSTALL_DIR" ]]; then
    rm -rf $ARGPARSE_INSTALL_DIR
fi

cd $ARGPARSE_DIR

cmake -S . -B $ARGPARSE_BUILD_DIR \
-DCMAKE_INSTALL_PREFIX="$ARGPARSE_INSTALL_DIR"

cmake --build $ARGPARSE_BUILD_DIR -j8
cmake --install $ARGPARSE_BUILD_DIR

cd $CUR_DIR
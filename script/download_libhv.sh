#!/usr/bin/env bash

set -e

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
LIBHV_VERSION="v1.3.2"
LIBHV_DIR=$DEPS_DIR/libhv_$LIBHV_VERSION

if [[ ! -d "${DEPS_DIR}" ]]; then
    mkdir $DEPS_DIR
fi

rm -rf $LIBHV_DIR

echo "GIT_ROOT_DIR: ${GIT_ROOT_DIR}"

git clone --single-branch --depth 1 -b $LIBHV_VERSION https://github.com/ithewei/libhv $LIBHV_DIR

cd $CUR_DIR
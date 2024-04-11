#!/usr/bin/env bash

set -e

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
OPENCV_VERSION="4.9.0"
OPENCV_DIR=$DEPS_DIR/opencv_$OPENCV_VERSION
OPENCV_SOURCE_CONTRIB_DIR=$DEPS_DIR/opencv_contrib_$OPENCV_VERSION

if [[ ! -d "${DEPS_DIR}" ]]; then
    mkdir $DEPS_DIR
fi

rm -rf $OPENCV_DIR
rm -rf $OPENCV_SOURCE_CONTRIB_DIR

echo "GIT_ROOT_DIR: ${GIT_ROOT_DIR}"

git clone --single-branch --depth 1 -b $OPENCV_VERSION https://github.com/opencv/opencv $OPENCV_DIR
git clone --single-branch --depth 1 -b $OPENCV_VERSION https://github.com/opencv/opencv_contrib $OPENCV_SOURCE_CONTRIB_DIR

cd $CUR_DIR
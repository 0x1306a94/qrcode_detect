#!/usr/bin/env bash

set -e

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
LIBZBAR_DIR=$DEPS_DIR/libzbar

if [[ ! -d "${DEPS_DIR}" ]]; then
    mkdir $DEPS_DIR
fi

rm -rf $LIBZBAR_DIR

echo "GIT_ROOT_DIR: ${GIT_ROOT_DIR}"

git clone -b 0.23.93 https://github.com/mchehab/zbar.git $LIBZBAR_DIR

cd $CUR_DIR
#!/usr/bin/env bash

set -e

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
ARGPARSE_VERSION="v3.0"
ARGPARSE_DIR=$DEPS_DIR/argparse_$ARGPARSE_VERSION

if [[ ! -d "${DEPS_DIR}" ]]; then
    mkdir $DEPS_DIR
fi

rm -rf $ARGPARSE_DIR

echo "GIT_ROOT_DIR: ${GIT_ROOT_DIR}"

git clone --single-branch --depth 1 -b $ARGPARSE_VERSION https://github.com/p-ranav/argparse $ARGPARSE_DIR

cd $CUR_DIR
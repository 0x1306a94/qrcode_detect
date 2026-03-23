#!/usr/bin/env bash

# set -e

source $(realpath $(dirname $0))/check_env.sh

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
LIB_NAME=libhv
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


function configure_macos_arm64() {
    echo "[*] configure arm64 macos"
    cmake -S $BUILD_SOURCE_DIR -B $BUILD_TEMP_DIR \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE="$BUILD_SOURCE_DIR/cmake/ios.toolchain.cmake" \
        -DPLATFORM=MAC_ARM64 \
        -DWITH_OPENSSL=OFF \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_UNITTEST=OFF \
        -DCMAKE_INSTALL_PREFIX="$BUILD_INSTALL_DIR"
}

function configure_macos_x86_64() {
    echo "[*] configure x86_64 macos"
    cmake -S $BUILD_SOURCE_DIR -B $BUILD_TEMP_DIR \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE="$BUILD_SOURCE_DIR/cmake/ios.toolchain.cmake" \
        -DPLATFORM=MAC \
        -DWITH_OPENSSL=OFF \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_UNITTEST=OFF \
        -DCMAKE_INSTALL_PREFIX="$BUILD_INSTALL_DIR"
}

function configure_linux() {
    echo "[*] configure linux"
    cmake -S $BUILD_SOURCE_DIR -B $BUILD_TEMP_DIR \
        -DCMAKE_BUILD_TYPE=Release \
        -DWITH_OPENSSL=ON \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_UNITTEST=OFF \
        -DCMAKE_INSTALL_PREFIX="$BUILD_INSTALL_DIR"
}

check_is_arm64_macos
HOST_ARM64_MAC=$?

check_is_x86_64_macos
HOST_X86_64_MAC=$?

check_is_linux
HOST_LINUX=$?

echo "Host is arm64 macOS: $HOST_ARM64_MAC"
echo "Host is x86_64 macOS: $HOST_X86_64_MAC"
echo "Host is Linux: $HOST_LINUX"

if [[ $HOST_ARM64_MAC -eq 0 ]]; then
    configure_macos_arm64
elif [[ $HOST_X86_64_MAC -eq 0 ]]; then
    configure_macos_x86_64
elif [[ $HOST_LINUX -eq 0 ]]; then
    configure_linux
else 
    echo "[*] non-supported platform"
    exit 0
fi

cmake --build $BUILD_TEMP_DIR -j8
cmake --install $BUILD_TEMP_DIR
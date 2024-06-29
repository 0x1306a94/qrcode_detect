#!/usr/bin/env bash

# set -e

source $(realpath $(dirname $0))/check_env.sh

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
LIBHV_VERSION="v1.3.2"
LIBHV_DIR=$DEPS_DIR/libhv_$LIBHV_VERSION
LIBHV_INSTALL_DIR=$GIT_ROOT_DIR/build_3rdparty/libhv_$LIBHV_VERSION
LIBHV_BUILD_DIR=$LIBHV_DIR/cmake_build

if [[ -d "$LIBHV_BUILD_DIR" ]]; then
    rm -rf $LIBHV_BUILD_DIR
fi

if [[ -d "$LIBHV_INSTALL_DIR" ]]; then
    rm -rf $LIBHV_INSTALL_DIR
fi

function configure_macos_arm64() {
    echo "[*] configure arm64 macos"
    cmake -S $LIBHV_DIR -B $LIBHV_BUILD_DIR \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE="$LIBHV_DIR/cmake/ios.toolchain.cmake" \
        -DPLATFORM=MAC_ARM64 \
        -DWITH_OPENSSL=OFF \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_UNITTEST=OFF \
        -DCMAKE_INSTALL_PREFIX="$LIBHV_INSTALL_DIR"
}

function configure_macos_x86_64() {
    echo "[*] configure x86_64 macos"
    cmake -S $LIBHV_DIR -B $LIBHV_BUILD_DIR \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE="$LIBHV_DIR/cmake/ios.toolchain.cmake" \
        -DPLATFORM=MAC \
        -DWITH_OPENSSL=OFF \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_UNITTEST=OFF \
        -DCMAKE_INSTALL_PREFIX="$LIBHV_INSTALL_DIR"
}

function configure_linux() {
    echo "[*] configure linux"
    cmake -S $LIBHV_DIR -B $LIBHV_BUILD_DIR \
        -DCMAKE_BUILD_TYPE=Release \
        -DWITH_OPENSSL=ON \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_UNITTEST=OFF \
        -DCMAKE_INSTALL_PREFIX="$LIBHV_INSTALL_DIR"
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
    echo "[*] nonsupport"
    exit 0
fi

cmake --build $LIBHV_BUILD_DIR -j8
cmake --install $LIBHV_BUILD_DIR


cd $CUR_DIR
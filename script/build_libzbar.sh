#!/usr/bin/env bash

set -e


GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
LIB_NAME=zbar
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

pushd $BUILD_SOURCE_DIR

# export CPPFLAGS="-I/usr/local/jpeg-turbo/include"
# export LDFLAGS="-L/usr/local/jpeg-turbo/lib"

# macOS: qrdectxt.c uses iconv; link against system libiconv (not merged into default link line).
if [[ "$(uname -s)" == "Darwin" ]]; then
    export LIBS="${LIBS:+$LIBS }-liconv"
fi

autoreconf -vfi
./configure --prefix="${BUILD_INSTALL_DIR}" \
--enable-static \
--disable-silent-rules \
--disable-video \
--without-dbus \
--without-x \
--without-xv \
--without-xshm \
--without-imagemagick \
--without-python \
--without-java \
--without-qt \
--without-gtk

make install

popd
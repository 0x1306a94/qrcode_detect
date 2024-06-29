#!/usr/bin/env bash

set -e

CUR_DIR=$PWD

GIT_ROOT_DIR=$(realpath $(dirname $(dirname $0)))
DEPS_DIR=$GIT_ROOT_DIR/download_deps
LIBZBAR_VERSION="0.23.93"
ZBAR_DIR=$DEPS_DIR/libzbar_$LIBZBAR_VERSION
ZBAR_INSTALL_DIR=$GIT_ROOT_DIR/build_3rdparty/libzbar_$LIBZBAR_VERSION

if [[ -d "$ZBAR_INSTALL_DIR" ]]; then
    rm -rf $ZBAR_INSTALL_DIR
fi

cd $ZBAR_DIR

# export CPPFLAGS="-I/usr/local/jpeg-turbo/include"
# export LDFLAGS="-L/usr/local/jpeg-turbo/lib"

autoreconf -vfi
./configure --prefix="${ZBAR_INSTALL_DIR}" \
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

cd $CUR_DIR
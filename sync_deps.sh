#!/bin/bash
cd $(dirname $0)

PROJECT_DIR=$PWD

./install_tools.sh

if [[ $(uname -s) == Linux ]]; then
    export PATH=$PWD/install_tools/depctl:$PATH
fi

depctl || exit 1

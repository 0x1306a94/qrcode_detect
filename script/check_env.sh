#!/usr/bin/env bash

# 检查是否是 arm64 macOS
function check_is_arm64_macos() {
    local os_type=$(uname -s)
    if [[ "$os_type" == "Darwin" ]]; then
        local platform=$(uname -p)
        if [[ "$platform" == "arm" || "$platform" == "arm64" ]]; then
            return 0
        else
            return 1
        fi
    else
        return 1
    fi
}

# 检查是否是 x86 macOS
function check_is_x86_64_macos() {
    os_type=$(uname -s)
    if [[ "$os_type" == "Darwin" ]]; then
        platform=$(uname -p)
        if [[ "$platform" == "i386" || "$platform" == "x86_64" ]]; then
            return 0
        else
            return 1
        fi
    else
        return 1
    fi
}

# 检查是否是 Linux
function check_is_linux() {
    os_type=$(uname -s)
    if [[ "$os_type" == "Linux" ]]; then
        return 0
    else
        return 1
    fi
}
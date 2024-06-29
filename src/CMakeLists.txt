cmake_minimum_required(VERSION 3.20)

set(CONFIGURE_AUTO_HEADER_ROOT_DIR ${CMAKE_BINARY_DIR}/auto_header)
include_directories(${CONFIGURE_AUTO_HEADER_ROOT_DIR})

add_subdirectory(libs)
add_subdirectory(server)
add_subdirectory(cli)
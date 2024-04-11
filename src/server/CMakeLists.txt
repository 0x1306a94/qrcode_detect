cmake_minimum_required(VERSION 3.20)

project(qrcode_detect_server CXX)

message(STATUS "Configuration: ${PROJECT_NAME}")

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.0
)
FetchContent_MakeAvailable(argparse)

if(APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/qrcode_detect_server.service.in ${CMAKE_INSTALL_PREFIX}/service/qrcode_detect_server.service @ONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/version.hpp.in autogen/include/${PROJECT_NAME}/version.hpp @ONLY)

find_library(HV_LIB hv_static HINTS "${LIBHV_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "HV_LIB: ${HV_LIB}")

find_library(SPDLOG_LIB spdlog HINTS "${SPDLOG_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "SPDLOG_LIB: ${SPDLOG_LIB}")

if(APPLE)
  set(FILE_GLOB_OPTION RELATIVE)
endif()

file(GLOB SRC_SRCS ./*.h ./*.hpp ./*.cpp)
file(GLOB HANDLER_SRCS ./handler/*.hpp ./handler/*.cpp)

source_group(src FILES ${SRC_SRCS})
source_group(handler FILES ${HANDLER_SRCS})

add_executable(${PROJECT_NAME} ${SRC_SRCS} ${HANDLER_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE 
./
../libs
${LIBHV_DIR}/include
${SPDLOG_DIR}/include
${CMAKE_CURRENT_BINARY_DIR}/autogen/include)

target_link_libraries(${PROJECT_NAME} PRIVATE 
${HV_LIB}
${SPDLOG_LIB}
qrcode_detect_core
argparse
)

# libhv ssl
if(APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE "-framework CoreFoundation -framework Security")
elseif(UNIX)
  set(OPENSSL_USE_STATIC_LIBS TRUE)
  find_package(OpenSSL)
  if(OpenSSL_FOUND)
      target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
  else()
      target_link_libraries(${PROJECT_NAME} PRIVATE ssl crypto)
  endif()
endif()

if(APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE FALSE
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
    XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
    XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
  )
elseif(UNIX)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
endif()

set(YOLOV3_MODEL_UNZIP_DIR ${CMAKE_BINARY_DIR}/download/yolov3-qrcode)
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${OPENCV_SOURCE_DIR}/cmake_build/downloads/wechat_qrcode $<TARGET_FILE_DIR:${PROJECT_NAME}>/models/wechat_qrcode
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${YOLOV3_MODEL_UNZIP_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/models/yolov3-qrcode
    COMMENT "Copying wechat_qrcode models to binary directory")

    
install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  BUNDLE DESTINATION bin
)

install(DIRECTORY
  ${OPENCV_SOURCE_DIR}/cmake_build/downloads/wechat_qrcode
  DESTINATION models
)
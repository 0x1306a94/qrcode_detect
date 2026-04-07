cmake_minimum_required(VERSION 3.20)

project(qrcode_detect_server CXX)

message(STATUS "Configuration: ${PROJECT_NAME}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

if(APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/qrcode_detect_server.service.in ${CMAKE_INSTALL_PREFIX}/service/qrcode_detect_server.service @ONLY)

find_library(HV_LIB hv_static HINTS "${LIBHV_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "HV_LIB: ${HV_LIB}")

find_package(argparse REQUIRED PATHS ${ARGPARSE_DIR} NO_DEFAULT_PATH)

find_library(SPDLOG_LIB spdlog HINTS "${SPDLOG_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
message(STATUS "SPDLOG_LIB: ${SPDLOG_LIB}")

file(GLOB SRC_SRCS ./*.h ./*.hpp ./*.cpp)
file(GLOB HANDLER_SRCS ./handler/*.hpp ./handler/*.cpp)

source_group(src FILES ${SRC_SRCS})
source_group(handler FILES ${HANDLER_SRCS})

add_executable(${PROJECT_NAME} ${SRC_SRCS} ${HANDLER_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE 
./
../libs
${LIBHV_DIR}/include
${SPDLOG_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE 
${HV_LIB}
${SPDLOG_LIB}
qrcode_detect_core
argparse::argparse
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

add_executable(${PROJECT_NAME}_ordered_worker_pool_test
  tests/ordered_worker_pool_test.cpp
)
target_include_directories(${PROJECT_NAME}_ordered_worker_pool_test PRIVATE
./
)
if(UNIX)
  find_package(Threads REQUIRED)
  target_link_libraries(${PROJECT_NAME}_ordered_worker_pool_test PRIVATE Threads::Threads)
endif()
add_test(NAME ${PROJECT_NAME}_ordered_worker_pool_test COMMAND ${PROJECT_NAME}_ordered_worker_pool_test)

# 便于测试
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${WECHAT_QRCODE_MODELS_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/models/wechat_qrcode
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${YOLOV3_MODELS_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/models/yolov3-qrcode
    COMMENT "Copying wechat_qrcode models to binary directory"
)

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  BUNDLE DESTINATION bin
)

if(EXISTS "${OPENCV_DIR}/lib64")
  file(GLOB OPENCV_LIB_FILES "${OPENCV_DIR}/lib64/lib*")
else()
  file(GLOB OPENCV_LIB_FILES "${OPENCV_DIR}/lib/lib*")
endif()
install(FILES ${OPENCV_LIB_FILES} DESTINATION lib)

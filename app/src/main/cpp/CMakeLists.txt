
cmake_minimum_required(VERSION 3.4.1)

# 引入头文件目录
include_directories(inc/libjpeg)
include_directories(/)

add_library(
        native-lib

        SHARED

        native_jpeg_compressor.cpp
        native_jpeg_decompressor.cpp
        JpegCompressor.cpp
        JpegDecompressor.cpp
        )

# DEBUG模式下开启日志输出(jni_common.h)
if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_definitions("-DLOG_OPEN")
endif ()

find_library(
        log-lib
        log)

find_library(
        bitmap-lib
        jnigraphics)


target_link_libraries(
        native-lib
        ${PROJECT_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libturbojpeg.so
        ${log-lib}
        ${bitmap-lib}
        )
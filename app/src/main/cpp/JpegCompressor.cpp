//
// Created by Administrator on 2020/1/11.
//

#include <android/log.h>
#include "JpegCompressor.h"

#include "JpegCommon.h"
#include "jni_common.h"

/**
 * init compressor
 * @return  {@enum SUCCESS} when success,  {@enum INIT_FAILED}  when failed
 */
int JpegCompressor::init() {
    handle = tjInitCompress();
    return handle == nullptr ? JC_INIT_FAILED : JC_SUCCESS;
}

/**
 * destroy compressor
 * @return  {@enum SUCCESS} when success,  {@enum NOT_INIT}  when the compressor is null initialized
 */
int JpegCompressor::destroy() {
    if (handle == nullptr) {
        return JC_NOT_INIT;
    }
    int code = 0;
    if ((code = tjDestroy(handle)) == 0) {
        handle = nullptr;
        return JC_SUCCESS;
    } else {
        return code;
    }
}

int JpegCompressor::compressRgba8888ToJpeg(const unsigned char *srcBuf, int width, int height,
                                           unsigned char **jpegBuf,
                                           unsigned long *jpegSize, int jpegSubsamp, int jpegQual,
                                           int flags) {
    return tjCompress2(handle, srcBuf, width, width << 2, height, TJPF_RGBA, jpegBuf, jpegSize,
                       jpegSubsamp, jpegQual, flags);
}

int JpegCompressor::compressI420ToJpeg(const unsigned char *srcBuf, int width, int height,
                                       unsigned char **jpegBuf,
                                       unsigned long *jpegSize, int jpegQual,
                                       int flags) {
    const unsigned char *srcPlanes[3];
    int strides[3];
    srcPlanes[0] = srcBuf;
    srcPlanes[1] = srcBuf + width * height;
    srcPlanes[2] = srcBuf + width * height + width * height / 4;
    strides[0] = width;
    strides[1] = width / 2;
    strides[2] = width / 2 ;

    return tjCompressFromYUVPlanes(handle, srcPlanes, width, strides, height, TJSAMP_420, jpegBuf,
                                   jpegSize,
                                   jpegQual, flags);
//    int need_size = tjBufSizeYUV2(width, 1, height, TJSAMP_420);
//    if (need_size != width * height * 3 / 2) {
//        LOGI("we detect yuv size: %d, but you give: %d, check again.\n", need_size, width * height * 3 / 2);
//        return 0;
//    }
//    return tjCompressFromYUV(handle, srcBuf, width, 4, height, TJSAMP_420, jpegBuf,
//                                   jpegSize,
//                                   jpegQual, flags);
}


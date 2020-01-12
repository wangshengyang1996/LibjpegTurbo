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


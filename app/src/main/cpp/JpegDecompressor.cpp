//
// Created by Administrator on 2020/1/11.
//

#include <android/log.h>
#include "JpegDecompressor.h"
#include "JpegCommon.h"
#include "jni_common.h"

/**
 * init decompressor
 * @return  {@enum SUCCESS} when success,  {@enum INIT_FAILED}  when failed
 */
int JpegDecompressor::init() {
    handle = tjInitDecompress();
    return handle == nullptr ? JC_INIT_FAILED : JC_SUCCESS;
}

/**
 * destroy decompressor
 * @return  {@enum SUCCESS} when success,  {@enum NOT_INIT}  when the compressor is null initialized
 */
int JpegDecompressor::destroy() {
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

int JpegDecompressor::jpegToRgba8888(const unsigned char *jpegBuf, long jpegSize,
                                     unsigned char *rgbaBuf, int width,
                                     int height, int flags) {
    return tjDecompress2(handle, jpegBuf, jpegSize, rgbaBuf, width, width << 2, height, TJPF_RGBA,
                         flags);
}

int
JpegDecompressor::getHeaderInfo(unsigned char *jpegBuf, unsigned long jpegSize, int *width,
                                int *height, int *jpegSubSample, int *jpegColorSpace) {
    return tjDecompressHeader3(handle, jpegBuf, jpegSize, width, height, jpegSubSample,
                               jpegColorSpace);
}

int JpegDecompressor::jpegToYuv420p(unsigned char *jpegBuf, long jpegSize,
                                    unsigned char **yuv420pBuf, int width, int *strides,
                                    int height) {
    return tjDecompressToYUVPlanes(handle, jpegBuf, jpegSize, yuv420pBuf, width, nullptr, height,
                                   0);
}

int JpegDecompressor::getPlaneWidth(int componentID, int width, int subsamp) {
    return tjPlaneWidth(componentID, width, subsamp);
}

//
// Created by Administrator on 2020/1/11.
//

#ifndef LIBJPEGTURBO_JPEGDECOMPRESSOR_H
#define LIBJPEGTURBO_JPEGDECOMPRESSOR_H


#include "libjpeg_turbo/turbojpeg.h"

class JpegDecompressor {
private:
    tjhandle handle;
public:
    int init();

    int
    jpegToRgba8888(const unsigned char *jpegBuf, long jpegSize, unsigned char *rgbaBuf, int width,
                   int height, int flags);

    int jpegToYuv420p(unsigned char *jpegBuf, long jpegSize,
                      unsigned char **yuv420pBuf, int width, int *strides,
                      int height);

    int getPlaneWidth(int componentID, int width, int subsamp);
    int getHeaderInfo(unsigned char *jpegBuf, unsigned long jpegSize, int *width, int *height,
                      int *jpegSubSample, int *jpegColorSpace
    );

    int destroy();
};


#endif //LIBJPEGTURBO_JPEGDECOMPRESSOR_H

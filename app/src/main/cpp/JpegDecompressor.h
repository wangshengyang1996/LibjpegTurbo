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

    int jpegToRgba8888(const unsigned char *jpegBuf, long jpegSize,
                       unsigned char *rgbaBuf, int width,
                       int height, int flags);

    int getHeaderInfo(unsigned char *jpegBuf,
                      unsigned long jpegSize,
                      int *width,
                      int *height
    );

    int destroy();
};


#endif //LIBJPEGTURBO_JPEGDECOMPRESSOR_H

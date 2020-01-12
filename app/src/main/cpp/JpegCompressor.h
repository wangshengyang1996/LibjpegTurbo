//
// Created by Administrator on 2020/1/11.
//

#ifndef LIBJPEGTURBO_JPEGCOMPRESSOR_H
#define LIBJPEGTURBO_JPEGCOMPRESSOR_H

#include "turbojpeg.h"


class JpegCompressor {
private:
    tjhandle handle;
public:
    int init();

    int compressRgba8888ToJpeg(const unsigned char *srcBuf, int width, int height,
                   unsigned char **jpegBuf, unsigned long *jpegSize, int jpegSubsamp, int jpegQual,
                   int flags);

    int destroy();

};


#endif //LIBJPEGTURBO_JPEGCOMPRESSOR_H

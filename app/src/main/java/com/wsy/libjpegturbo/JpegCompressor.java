package com.wsy.libjpegturbo;

import android.graphics.Bitmap;

public class JpegCompressor {

    /**
     * 4:4:4 chrominance subsampling (no chrominance subsampling).  The JPEG or
     * YUV image will contain one chrominance component for every pixel in the
     * source image.
     */
    public static final int TJSAMP_444 = 0;
    /**
     * 4:2:2 chrominance subsampling.  The JPEG or YUV image will contain one
     * chrominance component for every 2x1 block of pixels in the source image.
     */
    public static final int TJSAMP_422 = 1;
    /**
     * 4:2:0 chrominance subsampling.  The JPEG or YUV image will contain one
     * chrominance component for every 2x2 block of pixels in the source image.
     */
    public static final int TJSAMP_420 = 2;
    /**
     * Grayscale.  The JPEG or YUV image will contain no chrominance components.
     */
    public static final int TJSAMP_GRAY = 3;
    /**
     * 4:4:0 chrominance subsampling.  The JPEG or YUV image will contain one
     * chrominance component for every 1x2 block of pixels in the source image.
     *
     * @note 4:4:0 subsampling is not fully accelerated in libjpeg-turbo.
     */
    public static final int TJSAMP_440 = 4;
    /**
     * 4:1:1 chrominance subsampling.  The JPEG or YUV image will contain one
     * chrominance component for every 4x1 block of pixels in the source image.
     * JPEG images compressed with 4:1:1 subsampling will be almost exactly the
     * same size as those compressed with 4:2:0 subsampling, and in the
     * aggregate, both subsampling methods produce approximately the same
     * perceptual quality.  However, 4:1:1 is better able to reproduce sharp
     * horizontal features.
     *
     * @note 4:1:1 subsampling is not fully accelerated in libjpeg-turbo.
     */
    public static final int TJSAMP_411 = 5;

    static {
        System.loadLibrary("native-lib");
    }

    /**
     * native handle, for native call
     */
    private long nativeHandle = 0;

    private native boolean nativeCreate();

    private native boolean nativeDestroy();

    public native byte[] bitmapToJpeg(Bitmap bitmap, int sample, int quality);

    public native byte[] i420ToJpeg(byte[] nv21, int width, int height, int quality);


    public boolean createCompressor() {
        return nativeCreate();
    }

    public boolean destroyCompressor() {
        return nativeDestroy();
    }
}

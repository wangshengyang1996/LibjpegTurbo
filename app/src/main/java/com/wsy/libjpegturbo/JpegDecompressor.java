package com.wsy.libjpegturbo;

import android.graphics.Bitmap;

public class JpegDecompressor {
    static {
        System.loadLibrary("native-lib");
    }
    private long nativeHandle = 0;
    private native boolean nativeCreate();
    private native boolean nativeDestroy();
    public native Bitmap jpegToBitmap(byte[]jpeg);
    public native byte[] jpegToI420(byte[]jpeg);


    public boolean createDecompressor(){
        return nativeCreate();
    }

    public boolean destroyDecompressor(){
        return nativeDestroy();
    }
}

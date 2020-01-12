#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <android/log.h>
#include "JpegCompressor.h"
#include "JpegCommon.h"
#include "jni_common.h"


//---------------------------------------------
// compressor
extern "C" JNIEXPORT jboolean JNICALL
Java_com_wsy_libjpegturbo_JpegCompressor_nativeCreate(
        JNIEnv *env,
        jobject jCompressor) {
    jclass compressorClazz = env->GetObjectClass(jCompressor);
    jfieldID handleId = env->GetFieldID(compressorClazz, "nativeHandle", "J");
    jlong handle = env->GetLongField(jCompressor, handleId);
    if (handle != 0) {
        return JNI_TRUE;
    }
    JpegCompressor *compressor = new JpegCompressor();
    if (compressor->init() == JC_SUCCESS) {
        env->SetLongField(jCompressor, handleId, (jlong) compressor);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}
extern "C" JNIEXPORT jint JNICALL
Java_com_wsy_libjpegturbo_JpegCompressor_nativeDestroy(
        JNIEnv *env,
        jobject jCompressor) {
    jclass compressorClazz = env->GetObjectClass(jCompressor);
    jfieldID handleId = env->GetFieldID(compressorClazz, "nativeHandle", "J");
    jlong handle = env->GetLongField(jCompressor, handleId);
    if (handle == 0) {
        return JC_NOT_INIT;
    }
    JpegCompressor *compressor = reinterpret_cast<JpegCompressor *>(handle);

    int destroyResult = compressor->destroy();
    if (destroyResult == 0) {
        env->SetLongField(jCompressor, handleId, 0);
        delete compressor;
    }
    return destroyResult;
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_wsy_libjpegturbo_JpegCompressor_bitmapToJpeg(
        JNIEnv *env,
        jobject jCompressor,
        jobject bitmap,
        jint sample,
        jint quality) {
    jclass compressorClazz = env->GetObjectClass(jCompressor);
    jfieldID handleId = env->GetFieldID(compressorClazz, "nativeHandle", "J");
    jlong handle = env->GetLongField(jCompressor, handleId);
    if (handle == 0) {
        LOGE("not initialized!");
        return nullptr;
    }
    JpegCompressor *jpegCompressor = reinterpret_cast<JpegCompressor *>(handle);

    AndroidBitmapInfo bitmapInfo;
    int errorCode = 0;
    if ((errorCode = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo)) !=
        ANDROID_BITMAP_RESULT_SUCCESS) {
        LOGE("get bitmapInfo failed, code is %d", errorCode);
        return nullptr;
    }
    if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("bitmap format is not supported");
        return nullptr;
    }
    unsigned char *pixelData;
    AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixelData));

    unsigned char *jpegBuff;
    unsigned long jpegSize = 0;
    LOGI("compressRgba8888ToJpeg start");
    int compressResult = jpegCompressor->compressRgba8888ToJpeg(pixelData, bitmapInfo.width,
                                                                bitmapInfo.height, &jpegBuff,
                                                                &jpegSize, sample, quality,
                                                                0);
    LOGI("compressRgba8888ToJpeg finished");
    AndroidBitmap_unlockPixels(env, bitmap);

    if (compressResult != 0) {
        LOGE("compress failed , errorCode is %d", compressResult);
        return nullptr;
    } else {
        jbyteArray jpegByteArray = env->NewByteArray(jpegSize);
        env->SetByteArrayRegion(jpegByteArray, 0, jpegSize,
                                reinterpret_cast<const jbyte *>(jpegBuff));
        free(jpegBuff);
        return jpegByteArray;
    }


}
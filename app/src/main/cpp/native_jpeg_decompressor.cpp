//
// Created by Administrator on 2020/1/11.
//

#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <android/log.h>
#include "JpegDecompressor.h"
#include "JpegCommon.h"
#include "jni_common.h"

extern "C" JNIEXPORT jboolean JNICALL
Java_com_wsy_libjpegturbo_JpegDecompressor_nativeCreate(
        JNIEnv *env,
        jobject jDecompressor) {
    jclass decompressorClazz = env->GetObjectClass(jDecompressor);
    jfieldID handleId = env->GetFieldID(decompressorClazz, "nativeHandle", "J");
    jlong handle = env->GetLongField(jDecompressor, handleId);
    if (handle != 0) {
        return JNI_TRUE;
    }
    JpegDecompressor *decompressor = new JpegDecompressor();
    if (decompressor->init() == JC_SUCCESS) {
        env->SetLongField(jDecompressor, handleId, (jlong) decompressor);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}
extern "C" JNIEXPORT jint JNICALL
Java_com_wsy_libjpegturbo_JpegDecompressor_nativeDestroy(
        JNIEnv *env,
        jobject jDecompressor) {
    jclass decompressorClazz = env->GetObjectClass(jDecompressor);
    jfieldID handleId = env->GetFieldID(decompressorClazz, "nativeHandle", "J");
    jlong handle = env->GetLongField(jDecompressor, handleId);
    if (handle == 0) {
        return JC_NOT_INIT;
    }
    JpegDecompressor *decompressor = reinterpret_cast<JpegDecompressor *>(handle);

    int destroyResult = decompressor->destroy();
    if (destroyResult == 0) {
        env->SetLongField(jDecompressor, handleId, 0);
        delete decompressor;
    }
    return destroyResult;
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_wsy_libjpegturbo_JpegDecompressor_jpegToBitmap(JNIEnv *env,
                                                        jobject jDecompressor,
                                                        jbyteArray jpegData) {
    jclass decompressorClazz = env->GetObjectClass(jDecompressor);
    jfieldID handleId = env->GetFieldID(decompressorClazz, "nativeHandle", "J");
    jlong handle = env->GetLongField(jDecompressor, handleId);
    if (handle == 0 || jpegData == nullptr) {
        return nullptr;
    }
    JpegDecompressor *decompressor = reinterpret_cast<JpegDecompressor *>(handle);
    jbyte *jpeg = env->GetByteArrayElements(jpegData, JNI_FALSE);
    int jpegLength = env->GetArrayLength(jpegData);
    int width, height;
    LOGI("getHeaderInfo start");
    int decodeHeaderResult = decompressor->getHeaderInfo((unsigned char *) jpeg, jpegLength, &width,
                                                         &height);
    LOGI("getHeaderInfo finished, width = %d, height = %d", width, height);
    if (decodeHeaderResult != 0) {
        LOGE("decode jpeg header failed, code is: %d", decodeHeaderResult);
        env->ReleaseByteArrayElements(jpegData, jpeg, JNI_FALSE);
        return nullptr;
    }

    // create a bitmap
    jclass bmpClazz = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMethod = env->GetStaticMethodID(bmpClazz, "createBitmap",
                                                          "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jclass bmpConfigClazz = env->FindClass("android/graphics/Bitmap$Config");
    jfieldID argbConfigId = env->GetStaticFieldID(bmpConfigClazz, "ARGB_8888",
                                                  "Landroid/graphics/Bitmap$Config;");
    jobject configObj = env->GetStaticObjectField(bmpConfigClazz, argbConfigId);
    jobject bitmap = env->CallStaticObjectMethod(bmpClazz, createBitmapMethod, width, height,
                                                 configObj);

    // start decompress
    unsigned char *pixel;
    if ((AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixel))) !=
        ANDROID_BITMAP_RESULT_SUCCESS) {
        LOGE("failed to lock pixel of bitmap");
    } else {
        LOGI("jpegToRgba8888 start");
        int code = decompressor->jpegToRgba8888(reinterpret_cast<const unsigned char *>(jpeg),
                                                jpegLength, pixel, width, height, 0);
        LOGI("jpegToRgba8888 finished");
        if (code != 0) {
            LOGE("decompress failed, code is %d", code);
        }
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    env->ReleaseByteArrayElements(jpegData, jpeg, JNI_FALSE);
    return bitmap;
}
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
    int width, height, jpegSubSample, jpegColorSpace;
    LOGI("getHeaderInfo start");
    int decodeHeaderResult = decompressor->getHeaderInfo((unsigned char *) jpeg, jpegLength, &width,
                                                         &height, &jpegSubSample, &jpegColorSpace);
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


extern "C"
JNIEXPORT jobject JNICALL
Java_com_wsy_libjpegturbo_JpegDecompressor_jpegToI420(JNIEnv *env,
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
    int width, height, jpegSubSample, jpegColorSpace;
    LOGI("getHeaderInfo start");
    int decodeHeaderResult = decompressor->getHeaderInfo((unsigned char *) jpeg, jpegLength, &width,
                                                         &height, &jpegSubSample, &jpegColorSpace);
    if (decodeHeaderResult != 0) {
        return nullptr;
    }
    LOGI("getHeaderInfo finished, width = %d, height = %d,jpegSubSample = %d, jpegColorSpace = %d",
         width, height, jpegSubSample, jpegColorSpace);
    int yuvSize = width * height * 3 / 2;
    jbyteArray yuvByteArray = env->NewByteArray(yuvSize);
    char *yuv420pData[3] = {0};
    int strides[3] = {
            // TODO: ANALYZE
            // 解析结果中，jpegColorSpace的值是1，也就是TJSAMP_422 步长分别为500 250 250，
            // 但是实际上，YUV420的步长比例应该为4:1:1，即500 125 125
//            decompressor->getPlaneWidth(0,width,jpegSubSample),
//            decompressor->getPlaneWidth(1,width,jpegSubSample),
//            decompressor->getPlaneWidth(2,width,jpegSubSample)
            width * height, width * height >> 2, width * height >> 2
    };
    yuv420pData[0] = static_cast<char *>(malloc(strides[0]));
    yuv420pData[1] = static_cast<char *>(malloc(strides[1]));
    yuv420pData[2] = static_cast<char *>(malloc(strides[2]));
    int decompressCode = decompressor->jpegToYuv420p(reinterpret_cast<unsigned char *>(jpeg),
                                                     jpegLength,
                                                     reinterpret_cast<unsigned char **>(yuv420pData),
                                                     width, strides, height);
    if (decompressCode == 0) {
        env->SetByteArrayRegion(yuvByteArray, 0, strides[0],
                                reinterpret_cast<const jbyte *>(yuv420pData[0]));
        env->SetByteArrayRegion(yuvByteArray, strides[0], strides[1],
                                reinterpret_cast<const jbyte *>(yuv420pData[1]));
        env->SetByteArrayRegion(yuvByteArray, strides[0] + strides[1], strides[2],
                                reinterpret_cast<const jbyte *>(yuv420pData[2]));
    } else {
        yuvByteArray = nullptr;
    }
    free(yuv420pData[0]);
    free(yuv420pData[1]);
    free(yuv420pData[2]);

    return yuvByteArray;
}
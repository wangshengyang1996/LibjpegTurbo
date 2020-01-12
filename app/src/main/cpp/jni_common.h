//
// Created by Administrator on 2020/1/11.
//

#ifndef LIBJPEGTURBO_JNI_COMMON_H
#define LIBJPEGTURBO_JNI_COMMON_H

#endif //LIBJPEGTURBO_JNI_COMMON_H

#ifdef LOG_OPEN
#define LOG_TAG "wsy"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define LOGI(...)
#define LOGE(...)
#endif
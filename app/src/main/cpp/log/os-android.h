//
// Created by DRAGON-JIANG on 2016/2/17.
//
#include <jni.h>
#include <android/log.h>

#ifndef LOG_TAG
#define LOG_TAG "V8"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG ,__VA_ARGS__) // 定义LOGF类型

#define PRI_DEBUG ANDROID_LOG_DEBUG
#define PRI_INFO  ANDROID_LOG_INFO
#define PRI_ERROR ANDROID_LOG_ERROR
#define PRI_WARN  ANDROID_LOG_WARN

#define __PRINT_LOG(PRI, TAG, FMT, ...)		\
        __android_log_print(PRI, TAG, "[%s:%d] " FMT, __FILE_NAME__, __LINE__, ## __VA_ARGS__)
#endif
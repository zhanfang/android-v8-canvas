//
// Created by zhanfang on 2021/4/28.
//

#ifndef ANDROID_V8_CANVAS_V8ENGINEWRAPPER_H
#define ANDROID_V8_CANVAS_V8ENGINEWRAPPER_H

#include <jni.h>

class V8EngineWrapper {
public:
    V8EngineWrapper(JNIEnv* env, jobject obj);
    virtual ~V8EngineWrapper();

private:
    jlong mThreadId = 0;
};


#endif //ANDROID_V8_CANVAS_V8ENGINEWRAPPER_H

//
// Created by zhanfang on 2021/4/28.
//

#ifndef ANDROID_V8_CANVAS_V8ENGINEWRAPPER_H
#define ANDROID_V8_CANVAS_V8ENGINEWRAPPER_H

#include <jni.h>
#include <string>
#include <v8.h>

class V8EngineWrapper {
public:
    V8EngineWrapper(JNIEnv* env, jobject obj);
    virtual ~V8EngineWrapper();

    v8::Isolate* getIsolate() {return mIsolate;}

    void initialize(jstring globalAlias, jlong threadId);
    void require(std::string src, std::string filename);
    jstring runScript(jstring sourceScript);
    jlong registerJavaMethod(jlong objectHandle, jstring functionName, jboolean voidMethod);

private:
    jlong mThreadId = 0;
    v8::Isolate* mIsolate;
    v8::Persistent<v8::Context> context_;
    v8::Persistent<v8::Object>* globalObject_;

};


#endif //ANDROID_V8_CANVAS_V8ENGINEWRAPPER_H

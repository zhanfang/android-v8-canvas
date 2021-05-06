//
// Created by zhanfang on 2021/4/28.
//

#ifndef ANDROID_V8_CANVAS_V8Runtime_H
#define ANDROID_V8_CANVAS_V8Runtime_H

#include <jni.h>
#include <string>
#include <v8.h>

class V8Runtime {
public:
    static void OnLoad();

    V8Runtime(JNIEnv* env, jobject obj);
    virtual ~V8Runtime();

    v8::Isolate* getIsolate() {return mIsolate;}

    v8::Isolate* mIsolate;
    v8::Persistent<v8::Context> context_;
    jobject v8;

    void acquireLocker(v8::Locker* locker) {locker_ = locker;}
    void releaseLocker() {
        delete(locker_);
        locker_ = nullptr;
    }

    void createIsolate(jstring globalAlias);
    void require(std::string src, std::string filename);
    jstring runScript(jstring sourceScript);
    jlong initNewV8Object();
    jlong getGlobalObject() {return reinterpret_cast<jlong>(globalObject_);}
    jlong registerJavaMethod(jlong objectHandle, jstring functionName, jboolean voidMethod);

    // inspector
    void createInspector();

private:
    JNIEnv* env_;
    v8::Persistent<v8::Object>* globalObject_;
    v8::Locker* locker_;
};


#endif //ANDROID_V8_CANVAS_V8Runtime_H

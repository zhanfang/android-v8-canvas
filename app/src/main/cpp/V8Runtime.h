//
// Created by zhanfang on 2021/4/28.
//

#ifndef ANDROID_V8_CANVAS_V8Runtime_H
#define ANDROID_V8_CANVAS_V8Runtime_H

#include <jni.h>
#include <string>
#include <v8.h>

#define V8_NULL 0L
#define V8_UNKNOWN 0L
#define V8_INTEGER 1L
#define V8_INT_32_ARRAY 1L
#define V8_DOUBLE 2L
#define V8_BOOLEAN 3L
#define V8_STRING 4L
#define V8_ARRAY 5L
#define V8_OBJECT 6L
#define V8_FUNCTION 7L
#define V8_TYPED_ARRAY 8L
#define V8_BYTE 9L
#define V8_INT_8_ARRAY 10L
#define V8_UNSIGNED_INT_8_ARRAY 11L
#define V8_UNSIGNED_INT_8_CLAMPED_ARRAY 12L
#define V8_INT_16_ARRAY 13L
#define V8_UNSIGNED_INT_16_ARRAY 14L
#define V8_UNSIGNED_INT_32_ARRAY 15L
#define V8_FLOAT_32_ARRAY 16L
#define V8_UNDEFINED 99L

class V8Runtime {
public:
    static void OnLoad();

    V8Runtime(jobject obj);
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
    jstring toString(jlong objectHandle);
    jobject arrayGet(jint expectedType, jlong arrayHandle, jint index);
    jlong getGlobalObject() {return reinterpret_cast<jlong>(globalObject_);}
    jlong registerJavaMethod(jlong objectHandle, jstring functionName, jboolean voidMethod);

    // inspector
    void createInspector();

private:
    v8::Persistent<v8::Object>* globalObject_;
    v8::Locker* locker_;
};


#endif //ANDROID_V8_CANVAS_V8Runtime_H

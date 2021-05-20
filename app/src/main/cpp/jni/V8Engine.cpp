#include <jni.h>
#include <v8.h>
#include "JEnv.h"
#include "libplatform/libplatform.h"

#include "File.h"
#include "ArgConverter.h"
#include "V8Runtime.h"

#include "canvas/Canvas.h"
#include "canvas/CanvasContext2d.h"

#include <android/native_window.h>
#include <android/native_window_jni.h>

using namespace tns;
using namespace std;
using namespace Engine;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JEnv::Init(vm);

    v8::V8::InitializeICU();
    v8::Platform* platform_ = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform_);
    v8::V8::Initialize();

    V8Runtime::OnLoad();

    return JNI_VERSION_1_6;
}


extern "C" JNIEXPORT jlong JNICALL Java_com_example_v8engine_V8__1createIsolate(
        JNIEnv *env, jobject v8 /* this */, jstring globalAlias) {
    V8Runtime* ptr = new V8Runtime(env, v8);
    ptr->createIsolate(globalAlias);

    jlong nativePtr = reinterpret_cast<jlong>(ptr);
    return nativePtr;
}

extern "C" jint JNIEXPORT Java_com_example_v8engine_V8_getType(
        JNIEnv *env, jobject obj, jlong nativeV8Runtime, jlong objectHandle) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);

    return 0;
}

extern "C" void JNIEXPORT Java_com_example_v8engine_V8__1require(
        JNIEnv *env, jobject obj, jlong nativeV8Runtime, jstring filePath) {
    string filename = ArgConverter::jstringToString(filePath);
    string src = File::ReadText(filename);
    V8Runtime* ptr = reinterpret_cast<V8Runtime*>(nativeV8Runtime);
    ptr->require(src, filename);
//    Canvas::globalCanvas->flush();
}

extern "C" jstring JNIEXPORT Java_com_example_v8engine_V8__1runScript(
        JNIEnv *env, jobject obj, jlong nativeV8Runtime, jstring sourceScript) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);

    return runtime->runScript(sourceScript);
}

extern "C" JNIEXPORT jlong JNICALL Java_com_example_v8engine_V8__1registerJavaMethod(
        JNIEnv *env, jobject, jlong nativeV8Runtime, jlong objectHandle, jstring functionName, jboolean voidMethod) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);
    return runtime->registerJavaMethod(objectHandle, functionName, false);
}

extern "C" JNIEXPORT jlong JNICALL Java_com_example_v8engine_V8__1initNewV8Object(
        JNIEnv *env, jobject, jlong nativeV8Runtime) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);
    return runtime->initNewV8Object();
}

extern "C" JNIEXPORT jstring JNICALL Java_com_example_v8engine_V8__1toString(
        JNIEnv *env, jobject, jlong nativeV8Runtime, jlong objectHandle) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);
    return runtime->toString(objectHandle);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_example_v8engine_V8__1arrayGet(
        JNIEnv *env, jobject, jlong nativeV8Runtime, jint expectedType, jlong arrayHandle, jint index) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);
    return runtime->arrayGet(expectedType, arrayHandle, index);
}

extern "C" JNIEXPORT jlong JNICALL Java_com_example_v8engine_V8__1getGlobalObject(
        JNIEnv *env, jobject, jlong nativeV8Runtime) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);
    return runtime->getGlobalObject();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_v8engine_V8_acquireLock(
        JNIEnv *env, jobject, jlong nativeV8Runtime) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);
    if (runtime->getIsolate()->InContext()) {
        // throw exception "Cannot acquire lock while in a V8 Context"
        return;
    }

    runtime->acquireLocker( new Locker(runtime->getIsolate()));
}

extern "C" JNIEXPORT void JNICALL Java_com_example_v8engine_V8_releaseLock(
        JNIEnv *env, jobject, jlong nativeV8Runtime) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);
    if (runtime->getIsolate()->InContext()) {
        // throw exception "Cannot acquire lock while in a V8 Context"
        return;
    }

    runtime->releaseLocker();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_v8engine_V8_release(
        JNIEnv *env, jobject, jlong nativeV8Runtime, jlong objectHandle) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);
    Isolate* isolate = runtime->getIsolate();
    Locker locker(isolate);
    HandleScope handleScope(isolate);
    reinterpret_cast<Persistent<Object>*>(objectHandle)->Reset();
    delete(reinterpret_cast<Persistent<Object>*>(objectHandle));
}

// canvas
extern "C"
JNIEXPORT void JNICALL
Java_com_example_v8engine_V8_onSurfaceCreate(JNIEnv *env, jclass clazz, jobject jSurface,
                                                  jint width, jint height) {
    // TODO: implement onSurfaceCreate()
    // 获取与 Surface 对应的 ANativeWindow 对象
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, jSurface);
    Canvas* canvas = new Canvas(width, height, nativeWindow);

    Canvas::globalCanvas = canvas;
}

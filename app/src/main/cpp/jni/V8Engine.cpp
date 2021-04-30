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

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT long JNICALL Java_com_example_v8engine_V8_NewV8Engine(
        JNIEnv *env, jobject obj) {
    V8Runtime* ptr = new V8Runtime(env, obj);
    jlong nativePtr = reinterpret_cast<jlong>(ptr);

    return nativePtr;
}

extern "C" JNIEXPORT void JNICALL Java_com_example_v8engine_V8_initV8(
        JNIEnv *env, jobject /* this */, jlong nativeV8Engine, jstring globalAlias, jlong threadId) {
    V8Runtime* ptr = reinterpret_cast<V8Runtime*>(nativeV8Engine);
    ptr->initialize(globalAlias, threadId);
}

extern "C" void JNIEXPORT Java_com_example_v8engine_V8_require(
        JNIEnv *env, jobject obj, jlong nativeV8Engine, jstring filePath) {
    string filename = ArgConverter::jstringToString(filePath);
    string src = File::ReadText(filename);
    V8Runtime* ptr = reinterpret_cast<V8Runtime*>(nativeV8Engine);
    ptr->require(src, filename);
    Canvas::globalCanvas->flush();
}

extern "C" jstring JNIEXPORT Java_com_example_v8engine_V8_runScript(
        JNIEnv *env, jobject obj, jlong nativeV8Engine, jstring sourceScript) {
    V8Runtime* engine = reinterpret_cast<V8Runtime*>(nativeV8Engine);

    return engine->runScript(sourceScript);
}

extern "C" JNIEXPORT jlong JNICALL Java_com_example_v8engine_V8_registerJavaMethod(
        JNIEnv *env, jobject, jlong nativeV8Engine, jlong objectHandle, jstring functionName, jboolean voidMethod) {
    V8Runtime* engine = reinterpret_cast<V8Runtime*>(nativeV8Engine);
    v8::Isolate* isolate = engine->getIsolate();

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

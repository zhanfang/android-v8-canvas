#include <jni.h>
#include <v8.h>
#include <nan.h>
#include "JEnv.h"
#include "inspector/InspectorClient.h"

#include "File.h"
#include "ArgConverter.h"
#include "log/os-android.h"
#include "V8EngineWrapper.h"

#include "canvas/Canvas.h"
#include "canvas/CanvasContext2d.h"

#include <android/native_window.h>
#include <android/native_window_jni.h>

using namespace tns;
using namespace std;
using namespace Engine;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JEnv::Init(vm);
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT long JNICALL Java_com_example_v8engine_V8_NewV8Engine(
        JNIEnv *env, jobject obj) {
    V8EngineWrapper* ptr = new V8EngineWrapper(env, obj);
    jlong nativePtr = reinterpret_cast<jlong>(ptr);
    return nativePtr;
}

extern "C" JNIEXPORT void JNICALL Java_com_example_v8engine_V8_initV8(
        JNIEnv *env, jobject /* this */, jlong nativeV8Engine, jlong threadId) {
    V8EngineWrapper* ptr = reinterpret_cast<V8EngineWrapper*>(nativeV8Engine);
    ptr->initialize(threadId);
}

extern "C" void JNIEXPORT Java_com_example_v8engine_V8_require(
        JNIEnv *env, jobject obj, jlong nativeV8Engine, jstring filePath) {
    string filename = ArgConverter::jstringToString(filePath);
    string src = File::ReadText(filename);
    V8EngineWrapper* ptr = reinterpret_cast<V8EngineWrapper*>(nativeV8Engine);
    ptr->require(src, filename);
    Canvas::globalCanvas->flush();
}

extern "C" jstring JNIEXPORT Java_com_example_v8engine_V8_runScript(
        JNIEnv *env, jobject obj, jlong nativeV8Engine, jstring sourceScript) {
    V8EngineWrapper* engine = reinterpret_cast<V8EngineWrapper*>(nativeV8Engine);

    return engine->runScript(sourceScript);
}

// inspector
extern "C" JNIEXPORT void Java_com_example_v8engine_V8_connect(JNIEnv *env, jobject instance, jobject connection) {
    InspectorClient::GetInstance()->connect(connection);
}

extern "C" JNIEXPORT void Java_com_example_v8engine_V8_waitForFrontend(JNIEnv *env, jobject instance, jobject connection) {
    InspectorClient::GetInstance()->waitForFrontend();
}

extern "C" JNIEXPORT void Java_com_example_v8engine_V8_scheduleBreak(JNIEnv *env, jobject instance) {
    InspectorClient::GetInstance()->scheduleBreak();
}

extern "C" JNIEXPORT void Java_com_example_v8engine_V8_disconnect(JNIEnv *env, jobject instance) {
    InspectorClient::GetInstance()->disconnect();
}

extern "C" JNIEXPORT void Java_com_example_v8engine_V8_dispatchMessage(JNIEnv *env, jobject instance, jstring jMessage) {
    std::string message = ArgConverter::jstringToString(jMessage);

    LOGD("%s", message.c_str());

    InspectorClient::GetInstance()->dispatchMessage(message);
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

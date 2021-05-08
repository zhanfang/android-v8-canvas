#include <jni.h>
#include <ArgConverter.h>
#include "V8Runtime.h"
#include "inspector/InspectorClient.h"

extern "C" JNIEXPORT void Java_com_example_v8engine_V8Inspector_createInspector(
    JNIEnv *env, jobject, jlong nativeV8Runtime) {
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(nativeV8Runtime);
    runtime->createInspector();
}

extern "C" JNIEXPORT void Java_com_example_v8engine_V8Inspector_connect(JNIEnv *env, jobject instance, jobject connection) {
    InspectorClient::GetInstance()->connect(connection);
}

extern "C" JNIEXPORT void Java_com_example_v8engine_V8Inspector_waitForFrontend(JNIEnv *env, jobject instance, jobject connection) {
    InspectorClient::GetInstance()->waitForFrontend();
}

extern "C" JNIEXPORT void Java_com_example_v8engine_V8Inspector_scheduleBreak(JNIEnv *env, jobject instance) {
    InspectorClient::GetInstance()->scheduleBreak();
}

extern "C" JNIEXPORT void Java_com_example_v8engine_V8Inspector_disconnect(JNIEnv *env, jobject instance) {
    InspectorClient::GetInstance()->disconnect();
}

extern "C" JNIEXPORT void Java_com_example_v8engine_V8Inspector_dispatchMessage(JNIEnv *env, jobject instance, jstring jMessage) {
    std::string message = tns::ArgConverter::jstringToString(jMessage);

    InspectorClient::GetInstance()->dispatchMessage(message);
}


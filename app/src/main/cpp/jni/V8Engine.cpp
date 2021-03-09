#include <jni.h>
#include <v8-inspector.h>
#include <nan.h>

#include "File.h"
#include "V8Engine.h"
#include "ArgConverter.h"
#include "inspector/InspectorClient.h"
#include "log/os-android.h"
#include "console/Console.h"
#include "canvas/CanvasContext2d.h"

#include <android/bitmap.h>
#include <android/native_window_jni.h>
#include <include/core/SkBitmap.h>

using namespace tns;
using namespace std;

v8::Platform* platform_;
v8::Isolate *mIsolate;
v8::Persistent<v8::Context> mPersistentContext;

SkCanvas *skCanvas;

ANativeWindow *nativeWindow;

extern "C" JNIEXPORT void JNICALL Java_com_example_zhanfang_test_V8_initV8(
        JNIEnv *env,
        jobject /* this */) {
    // Initialize V8.
    LOGD("init v8");
    v8::V8::InitializeICU();
    platform_ = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform_);
    v8::V8::Initialize();

    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    mIsolate = v8::Isolate::New(create_params);

    v8::Isolate::Scope isolate_scope(mIsolate);
    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(mIsolate);

    const auto readOnlyFlags = static_cast<v8::PropertyAttribute>(v8::PropertyAttribute::DontDelete | v8::PropertyAttribute::ReadOnly);

    auto globalFunctionTemplate = v8::FunctionTemplate::New(mIsolate);
    globalFunctionTemplate->SetClassName(Nan::New("WindowObject").ToLocalChecked());
    auto globalTemplate = v8::ObjectTemplate::New(mIsolate, globalFunctionTemplate);


    v8::Local <v8::Context> context = v8::Context::New(mIsolate, nullptr, globalTemplate);
    context->Enter();
    auto global = context->Global();
    global->DefineOwnProperty(context, Nan::New("global").ToLocalChecked(), global, readOnlyFlags);
    global->DefineOwnProperty(context, Nan::New("__global").ToLocalChecked(), global, readOnlyFlags);

    // console
    v8::Local<v8::Object> console = Console::createConsole(context, nullptr, 1024);
    global->DefineOwnProperty(context, Nan::New("console").ToLocalChecked(), console, readOnlyFlags);

    // canvas
    v8::Local<v8::Object> bindings = v8::Object::New(mIsolate);
    Context2d::Initialize(bindings);
    global->DefineOwnProperty(context, Nan::New("bindings").ToLocalChecked(), bindings);

    // attach the context to the persistent context, to avoid V8 GC-ing it
    mPersistentContext.Reset(mIsolate, context);

    InspectorClient::GetInstance()->init();
}

void require(string src, string filename) {
    auto isolate = mIsolate;
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, mPersistentContext);

    auto source = Nan::New(src).ToLocalChecked();
    auto originName = "file:/" + filename;

    LOGD("require %s", originName.c_str());

    v8::TryCatch tc(isolate);

    v8::Local<v8::Script> script;
    v8::ScriptOrigin origin(Nan::New(originName).ToLocalChecked());

    InspectorClient::GetInstance()->scheduleBreak();

    v8::Script::Compile(context, source, &origin).ToLocal(&script);

    if (!script.IsEmpty()) {
        v8::Local<v8::Value> result;
        script->Run(context).ToLocal(&result);
    }
}

extern "C" void JNIEXPORT Java_com_example_zhanfang_test_V8_require(
        JNIEnv *env, jobject obj, jstring filePath) {
    string filename = ArgConverter::jstringToString(filePath);
    string src = File::ReadText(filename);

    ANativeWindow_Buffer *buffer = new ANativeWindow_Buffer();
    ANativeWindow_lock(nativeWindow, buffer, 0);
    int bpr = buffer->stride * 4;
    SkBitmap bitmap;
    SkImageInfo imageInfo = SkImageInfo::MakeN32(buffer->width, buffer->height, SkAlphaType::kPremul_SkAlphaType);

    bitmap.setInfo(imageInfo, bpr);
    bitmap.setPixels(buffer->bits);

    skCanvas = new SkCanvas(bitmap);
    require(src, filename);
    ANativeWindow_unlockAndPost(nativeWindow);
}

// inspector
extern "C" JNIEXPORT void Java_com_example_zhanfang_test_V8_connect(JNIEnv *env, jobject instance, jobject connection) {
    InspectorClient::GetInstance()->connect(connection);
}

extern "C" JNIEXPORT void Java_com_example_zhanfang_test_V8_waitForFrontend(JNIEnv *env, jobject instance, jobject connection) {
    InspectorClient::GetInstance()->waitForFrontend();
}

extern "C" JNIEXPORT void Java_com_example_zhanfang_test_V8_scheduleBreak(JNIEnv *env, jobject instance) {
    InspectorClient::GetInstance()->scheduleBreak();
}

extern "C" JNIEXPORT void Java_com_example_zhanfang_test_V8_disconnect(JNIEnv *env, jobject instance) {
    InspectorClient::GetInstance()->disconnect();
}

extern "C" JNIEXPORT void Java_com_example_zhanfang_test_V8_dispatchMessage(JNIEnv *env, jobject instance, jstring jMessage) {
    std::string message = ArgConverter::jstringToString(jMessage);

    LOGD("%s", message.c_str());

    InspectorClient::GetInstance()->dispatchMessage(message);
}

// canvas
extern "C"
JNIEXPORT void JNICALL
Java_com_example_zhanfang_test_V8_onSurfaceCreate(JNIEnv *env, jclass clazz, jobject jSurface,
                                                  jint width, jint height) {
    // TODO: implement onSurfaceCreate()
    // 获取与 Surface 对应的 ANativeWindow 对象
    nativeWindow = ANativeWindow_fromSurface(env, jSurface);
    // 设置 buffer 的尺寸和格式
    ANativeWindow_setBuffersGeometry(nativeWindow, width, height, WINDOW_FORMAT_RGBA_8888);
}
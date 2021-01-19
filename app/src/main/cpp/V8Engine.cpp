#include <jni.h>
#include <v8-inspector.h>

#include "File.h"
#include "V8Engine.h"
#include "ArgConverter.h"
#include "InspectorClient.h"
#include "log/os-android.h"

using namespace tns;
using namespace std;

v8::Platform* platform_;
v8::Isolate *mIsolate;
v8::Persistent<v8::Context> mPersistentContext;

void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length() < 1) {
        return;
    }

    v8::HandleScope scope(args.GetIsolate());

    std::stringstream str;

    int l = args.Length();
    for (int i = 0; i < l; i++) {
        v8::String::Utf8Value utf8(args[i]);
        str << " " << *utf8;
    }

    LOGD("%s", str.str().c_str());
}

void createGlobalObject(v8::Local<v8::ObjectTemplate>& globalObjTpl) {
    // Add methods to console function
    v8::Local<v8::ObjectTemplate> con = v8::ObjectTemplate::New(mIsolate);
    con->Set(v8::String::NewFromUtf8(mIsolate, "log"), v8::FunctionTemplate::New(mIsolate, LogCallback));
    globalObjTpl->Set(v8::String::NewFromUtf8(mIsolate, "console"), con);

}

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
    globalFunctionTemplate->SetClassName(ArgConverter::ConvertToV8String(mIsolate, "WindowObject"));
    auto globalTemplate = v8::ObjectTemplate::New(mIsolate, globalFunctionTemplate);
    globalTemplate->Set(ArgConverter::ConvertToV8String(mIsolate, "testReadOnly"),
                        ArgConverter::ConvertToV8String(mIsolate, "0"), readOnlyFlags);

    createGlobalObject(globalTemplate);

    v8::Local <v8::Context> context = v8::Context::New(mIsolate, nullptr, globalTemplate);
    context->Enter();
    auto global = context->Global();
    global->DefineOwnProperty(context, ArgConverter::ConvertToV8String(mIsolate, "global"), global, readOnlyFlags);
    global->DefineOwnProperty(context, ArgConverter::ConvertToV8String(mIsolate, "__global"), global, readOnlyFlags);

    // attach the context to the persistent context, to avoid V8 GC-ing it
    mPersistentContext.Reset(mIsolate, context);

    InspectorClient::GetInstance()->init();
}

extern "C" void JNIEXPORT Java_com_example_zhanfang_test_V8_require(
        JNIEnv *env, jobject obj, jstring filePath) {
    auto isolate = mIsolate;
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, mPersistentContext);
    auto filename = ArgConverter::jstringToString(filePath);
    auto src = File::ReadText(filename);

    auto source = ArgConverter::ConvertToV8String(mIsolate, src);


    auto originName = "file:/" + filename;

    LOGD("v8 require %s", originName.c_str());

    v8::TryCatch tc(isolate);

    v8::Local<v8::Script> script;
    v8::ScriptOrigin origin(ArgConverter::ConvertToV8String(isolate, originName));

    InspectorClient::GetInstance()->scheduleBreak();

    auto maybeScript = v8::Script::Compile(context, source, &origin).ToLocal(&script);

    if (!script.IsEmpty()) {
        v8::Local<v8::Value> result;
        auto maybeResult = script->Run(context).ToLocal(&result);

        v8::String::Utf8Value utf8(result);
        LOGD("require done %s", *utf8);

    }

    return;
}

extern "C" void JNIEXPORT Java_com_example_zhanfang_test_V8_init(JNIEnv *env, jobject object) {
    InspectorClient::GetInstance()->init();
}

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
    InspectorClient::GetInstance()->dispatchMessage(message);
}

extern "C" JNIEXPORT jstring JNICALL Java_com_example_zhanfang_test_V8_stringFromJNI(JNIEnv *env, jobject /* this */) {
    std::string hello = "Hello v8 from C++!\n";

    v8::Isolate::Scope isolate_scope(mIsolate);
    v8::HandleScope handle_scope(mIsolate);

    // Enter the context for compiling and running the hello world script.
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(mIsolate, mPersistentContext);
    v8::Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    v8::Local<v8::String> source = v8::String::NewFromUtf8(
            mIsolate, "'Hello' + ', from Javascript!'", v8::NewStringType::kNormal).ToLocalChecked();

    // Compile the source code.
    v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();

    // Run the script to get the result.
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

    // Convert the result to an UTF8 string and print it.
    v8::String::Utf8Value utf8(result);
    printf("%s\n", *utf8);
    hello += *utf8;

    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL Java_com_example_zhanfang_test_V8_stringFromJNI2(JNIEnv *env, jobject /* this */) {
    std::string hello = "\n\n";

    v8::Isolate::Scope isolate_scope(mIsolate);
    v8::HandleScope handle_scope(mIsolate);

    // Enter the context for compiling and running the hello world script.
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(mIsolate, mPersistentContext);
    v8::Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    v8::Local<v8::String> source = v8::String::NewFromUtf8(
            mIsolate, "'Hello2' + ', from Javascript2!'", v8::NewStringType::kNormal).ToLocalChecked();

    // Compile the source code.
    v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();

    // Run the script to get the result.
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

    // Convert the result to an UTF8 string and print it.
    v8::String::Utf8Value utf8(result);
    printf("%s\n", *utf8);
    hello += *utf8;

    return env->NewStringUTF(hello.c_str());
}

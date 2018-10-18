#include <jni.h>
#include <os-android.h>
#include <libplatform/libplatform.h>
#include "V8Engine.h"
#include <v8-inspector.h>
#include <macros.h>

using namespace v8;

using namespace v8_inspector;

v8::Local<v8::Context> CreateShellContext(v8::Isolate *isolate);
// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

V8Engine::V8Engine() {
//    v8::V8::InitializeICUDefaultLocation(nullptr);
//    v8::V8::InitializeExternalStartupData(nullptr);
//    v8::Platform* platform_ = v8::platform::CreateDefaultPlatform();
//    v8::V8::InitializePlatform(platform_);
//    v8::V8::Initialize();
//
//    this->platform_ = platform_;
//    LOGI("v8engine inited");
//
//    // Create a new Isolate and make it the current one.
//    v8::Isolate::CreateParams create_params;
//    create_params.array_buffer_allocator =
//            v8::ArrayBuffer::Allocator::NewDefaultAllocator();
//    v8::Isolate* isolate = v8::Isolate::New(create_params);
//    this->isolate_ = isolate;
}

V8Engine::~V8Engine() {

}

void V8Engine::destory() {
//    LOGI("V8Engine Cleaning up");
//    this->isolate_->Dispose();
//    v8::V8::Dispose();
//    v8::V8::ShutdownPlatform();
//
//    delete this->platform_;
//    if (g_array_buffer_allocator)
//        delete g_array_buffer_allocator;
//    this->isolate_ = nullptr;
//    LOGI("v8engine end");
}

void V8Engine::hello() {
//    v8::Isolate::Scope isolate_scope(isolate_);
//    // Create a stack-allocated handle scope.
//    v8::HandleScope handle_scope(isolate_);
//    // Create a new context.
////    v8::Local<v8::Context> context = v8::Context::New(isolate_);
//    v8::Local<v8::Context> context = CreateShellContext(isolate_);
//    // Enter the context for compiling and running the hello world script.
//    v8::Context::Scope context_scope(context);
//    // Create a string containing the JavaScript source code.
//    v8::Local<v8::String> source =
//            v8::String::NewFromUtf8(isolate_, "print('zhanfang'); const a = 1; const b = 2;a + b",
//                                    v8::NewStringType::kNormal)
//                    .ToLocalChecked();
//    // Compile the source code.
//    v8::Local<v8::Script> script =
//            v8::Script::Compile(context, source).ToLocalChecked();
//    // Run the script to get the result.
//    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
//    // Convert the result to an UTF8 string and print it.
//    v8::String::Utf8Value utf8(isolate_, result);
//    LOGI("v8engine %s", *utf8);
}

extern "C" JNIEXPORT jstring
JNICALL
Java_com_example_zhanfang_test_MainActivity_runScript(
        JNIEnv *env,
        jobject /* this */) {
    // Initialize V8.
    v8::V8::InitializeICUDefaultLocation(nullptr);
    v8::V8::InitializeExternalStartupData(nullptr);
    v8::Platform* platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);
    v8::V8::Initialize();

    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
            v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);

        // Create a stack-allocated handle scope.
        v8::HandleScope handle_scope(isolate);

        // Create a new context.
//        v8::Local<v8::Context> context = v8::Context::New(isolate);
        v8::Local<v8::Context> context = CreateShellContext(isolate);

        // Enter the context for compiling and running the hello world script.
        v8::Context::Scope context_scope(context);

        {
            InspectorClient inspectorClient(context, true);


            // Create a string containing the JavaScript source code.
            v8::Local<v8::String> source =
                    v8::String::NewFromUtf8(isolate,
                                            "function receive(message){print(message);}; send({\"id\":0, \"method\": \"Runtime.enable\"})",
                                            v8::NewStringType::kNormal)
                            .ToLocalChecked();
//        v8::Local<v8::String> source =
//                v8::String::NewFromUtf8(isolate, "print(123)",
//                                        v8::NewStringType::kNormal)
//                        .ToLocalChecked();

            // Compile the source code.
            v8::Local<v8::Script> script =
                    v8::Script::Compile(context, source).ToLocalChecked();

            // Run the script to get the result.
            v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

            // Convert the result to an UTF8 string and print it.
            v8::String::Utf8Value utf8(isolate, result);
        }
//        LOGI("v8engine %s", *utf8);
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete platform;
    delete create_params.array_buffer_allocator;
    std::string hello = "Hello from C";
    return env->NewStringUTF(hello.c_str());
}

void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::String::Utf8Value str(args[0]);
    const char* cstr = ToCString(str);
    LOGI("v8engine print %s", cstr);
}

v8::Local<v8::Context> CreateShellContext(v8::Isolate* isolate) {

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    // Bind the global 'print' function to the C++ Print callback.
    global->Set(
            v8::String::NewFromUtf8(isolate, "print", v8::NewStringType::kNormal)
                    .ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, Print));
    return v8::Context::New(isolate, NULL, global);
}


//extern "C" JNIEXPORT jstring
//JNICALL
//Java_com_example_zhanfang_test_MainActivity_runScript(
//        JNIEnv *env,
//        jobject /* this */) {
//    V8Engine v8engine;
//    v8engine.hello();
//    v8engine.destory();
//}

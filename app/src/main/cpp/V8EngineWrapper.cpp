//
// Created by zhanfang on 2021/4/28.
//

#include "V8EngineWrapper.h"
#include "log/os-android.h"
#include "console/Console.h"
#include "libplatform/libplatform.h"
#include "inspector/InspectorClient.h"
#include "canvas/CanvasContext2d.h"
#include "ArgConverter.h"

#include <nan.h>

V8EngineWrapper::V8EngineWrapper(JNIEnv *env, jobject obj) {

}

V8EngineWrapper::~V8EngineWrapper() {}

void V8EngineWrapper::initialize(jlong threadId) {
    // Initialize V8.
    LOGD("init v8");
    mThreadId = threadId;

    v8::V8::InitializeICU();
    v8::Platform* platform_ = v8::platform::CreateDefaultPlatform();
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
    Engine::Context2d::Initialize(bindings);
    global->DefineOwnProperty(context, Nan::New("bindings").ToLocalChecked(), bindings);

    // attach the context to the persistent context, to avoid V8 GC-ing it
    v8::Persistent<v8::Context> mPersistentContext;
    mPersistentContext.Reset(mIsolate, context);

    InspectorClient::GetInstance()->init();
}

void V8EngineWrapper::require(std::string src, std::string filename) {
    v8::Isolate::Scope isolate_scope(mIsolate);
    v8::HandleScope handle_scope(mIsolate);
    v8::Local<v8::Context> context = mIsolate->GetCurrentContext();

    auto source = Nan::New(src).ToLocalChecked();
    auto originName = "file:/" + filename;

    LOGD("require %s", originName.c_str());

    v8::TryCatch tc(mIsolate);

    v8::Local<v8::Script> script;
    v8::ScriptOrigin origin(Nan::New(originName).ToLocalChecked());

    InspectorClient::GetInstance()->scheduleBreak();

    v8::Script::Compile(context, source, &origin).ToLocal(&script);

    if (!script.IsEmpty()) {
        v8::Local<v8::Value> result;
        script->Run(context).ToLocal(&result);
    }
}

jstring V8EngineWrapper::runScript(jstring sourceScript) {
    v8::Isolate::Scope isolate_scope(mIsolate);
    v8::HandleScope handle_scope(mIsolate);

    v8::TryCatch tryCatch(mIsolate);

    // Enter the context for compiling and running the hello world script.
    v8::Local<v8::Context> context = mIsolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    v8::Local<v8::String> source = tns::ArgConverter::jstringToV8String(mIsolate, sourceScript);

    // Compile the source code.
    v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();

    // Run the script to get the result.
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
    return tns::ArgConverter::ConvertToJavaString(result);
}
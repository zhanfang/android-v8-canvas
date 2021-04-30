//
// Created by zhanfang on 2021/4/28.
//

#include "V8EngineWrapper.h"
#include "log/os-android.h"
#include "console/Console.h"
#include "inspector/InspectorClient.h"
#include "canvas/CanvasContext2d.h"
#include "ArgConverter.h"
#include <nan.h>

using namespace v8;

#define SETUP(isolate)\
    Isolate::Scope isolateScope(isolate);\
    HandleScope handle_scope(isolate);\
    Local<Context> context = isolate->GetCurrentContext();\
    Context::Scope context_scope(context);


static void jsWindowObjectAccessor(Local<String> property, const v8::PropertyCallbackInfo<Value>& info) {
  info.GetReturnValue().Set(info.GetIsolate()->GetCurrentContext()->Global());
}

class MethodDescriptor {
public:
    jlong methodID;
    jlong v8EnginePtr;
    Persistent<External> obj;
};

V8EngineWrapper::V8EngineWrapper(JNIEnv *env, jobject obj) {

}

V8EngineWrapper::~V8EngineWrapper() {}

void V8EngineWrapper::initialize(jstring globalAlias, jlong threadId) {
    // Initialize V8.
    LOGD("init v8");
    mThreadId = threadId;

    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    mIsolate = v8::Isolate::New(create_params);

    v8::Isolate::Scope isolate_scope(mIsolate);
    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(mIsolate);

    Local<ObjectTemplate> globalObject = ObjectTemplate::New(mIsolate);
    if (globalAlias != nullptr) {
        Local<String> alias = tns::ArgConverter::jstringToV8String(mIsolate, globalAlias);
        globalObject->SetAccessor(alias, jsWindowObjectAccessor);
    }

    Local<Context> context = Context::New(mIsolate, nullptr, globalObject);
    // attach the context to the persistent context, to avoid V8 GC-ing it
    context_.Reset(mIsolate, context);
    globalObject_ = new Persistent<Object>;
    globalObject_->Reset(mIsolate, context->Global()->GetPrototype()->ToObject(context).ToLocalChecked());

    context->Enter();

    Local<Object> obj = Local<Object>::New(mIsolate, *globalObject_);

    // console
    v8::Local<v8::Object> console = Console::createConsole(context, nullptr, 1024);
    obj->Set(context, Nan::New("console").ToLocalChecked(), console);

    // canvas
    v8::Local<v8::Object> bindings = v8::Object::New(mIsolate);
    Engine::Context2d::Initialize(bindings);
    obj->Set(context, Nan::New("bindings").ToLocalChecked(), bindings);

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

jlong V8EngineWrapper::registerJavaMethod(jlong objectHandle, jstring functionName, jboolean voidMethod) {
    SETUP(mIsolate);
    v8::Local<v8::Object> object = v8::Local<v8::Object>::New(mIsolate, *reinterpret_cast<v8::Persistent<v8::Object>*>(objectHandle));
    Local<v8::String> v8FunctionName = tns::ArgConverter::jstringToV8String(mIsolate, functionName);
    mIsolate->IdleNotificationDeadline(1);
    MethodDescriptor* md = new MethodDescriptor();
    Local<External> ext = External::New(mIsolate, md);

    md->methodID = reinterpret_cast<jlong>(md);
    md->v8EnginePtr = reinterpret_cast<jlong>(this);

    md->obj.Reset(mIsolate, ext);
    md->obj.SetWeak();

    return md->methodID;
}
//
// Created by zhanfang on 2021/4/28.
//

#include "V8Runtime.h"
#include "log/os-android.h"
#include "console/Console.h"
#include "inspector/InspectorClient.h"
#include "canvas/CanvasContext2d.h"
#include "ArgConverter.h"
#include <nan.h>

using namespace v8;
using namespace Nan;

#define SETUP(isolate)\
    Locker locker(isolate);\
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
    jlong v8RuntimePtr;
    Persistent<External> obj;
};

V8Runtime::V8Runtime(JNIEnv *env, jobject obj) {
    env_ = env;
}

V8Runtime::~V8Runtime() {}

void V8Runtime::createIsolate(jstring globalAlias) {
    // Initialize V8.
    LOGD("create v8 isolate");

    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    mIsolate = v8::Isolate::New(create_params);
    Locker locker(mIsolate);

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

void V8Runtime::createInspector() {
    //
}

void V8Runtime::require(std::string src, std::string filename) {
    SETUP(mIsolate);

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

jstring V8Runtime::runScript(jstring sourceScript) {
    SETUP(mIsolate);

    v8::TryCatch tryCatch(mIsolate);

    // Create a string containing the JavaScript source code.
    v8::Local<v8::String> source = tns::ArgConverter::jstringToV8String(mIsolate, sourceScript);

    // Compile the source code.
    v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();

    // Run the script to get the result.
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
    return tns::ArgConverter::ConvertToJavaString(result);
}

jlong V8Runtime::initNewV8Object() {
    SETUP(mIsolate);
    Local<Object> object = Object::New(mIsolate);
    Persistent<Object>* container = new Persistent<Object>;
    container->Reset(mIsolate, object);
    return reinterpret_cast<jlong>(container);
}

void objectCallback(const NAN_METHOD_ARGS_TYPE args) {
    Local<External> data = Local<External>::Cast(args.Data());
    void *methodDescriptorPtr = data->Value();
    MethodDescriptor* md = static_cast<MethodDescriptor*>(methodDescriptorPtr);
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr);
    Isolate* isolate = runtime->getIsolate();
    SETUP(isolate);

}

jlong V8Runtime::registerJavaMethod(jstring functionName, jboolean voidMethod) {
    SETUP(mIsolate);

    v8::FunctionCallback callback = objectCallback;

    Local<Object> globalObject = Local<Object>::New(mIsolate, *globalObject_);
    Local<v8::String> v8FunctionName = tns::ArgConverter::jstringToV8String(mIsolate, functionName);
    mIsolate->IdleNotificationDeadline(1);

    MethodDescriptor* md = new MethodDescriptor();
    Local<External> ext = External::New(mIsolate, md);

    md->methodID = reinterpret_cast<jlong>(md);
    md->v8RuntimePtr = reinterpret_cast<jlong>(this);

    MaybeLocal<Function> func = Function::New(context, callback, ext);

    if (!func.IsEmpty()) {
        globalObject->Set(context, v8FunctionName, func.ToLocalChecked());
    }

    md->obj.Reset(mIsolate, ext);
    md->obj.SetWeak();

    return md->methodID;
}

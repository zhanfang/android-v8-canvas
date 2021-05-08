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

#define SETUP(runtime)\
    Isolate* isolate = runtime->mIsolate;\
    Isolate::Scope isolateScope(isolate);\
    HandleScope handle_scope(isolate);\
    Local<Context> context = Local<Context>::New(isolate, runtime->context_);\
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

jclass v8Cls = nullptr;
jclass v8ArrayCls = nullptr;
jclass undefinedV8ArrayCls = nullptr;
jclass v8ObjectCls = nullptr;

jmethodID v8ArrayInitMethodID = nullptr;
jmethodID undefinedV8ArrayInitMethodID = nullptr;
jmethodID v8ArrayGetHandleMethodID = nullptr;
jmethodID v8ObjectInitMethodID = nullptr;
jmethodID v8ObjectGetHandleMethodID = nullptr;
jmethodID v8CallObjectJavaMethodMethodID = nullptr;

jobject getResult(JNIEnv *env, const Local<Context>& context, jobject v8, Isolate* isolate, Handle<Value> &result, jint expectedType);

void V8Runtime::OnLoad() {
    JNIEnv* env = JEnv();

    //  on first creation, store  a handle to JAVA classes
    v8Cls = (jclass)env->NewGlobalRef(env->FindClass("com/example/v8engine/V8"));
    v8ArrayCls = (jclass)env->NewGlobalRef(env->FindClass("com/example/v8engine/V8Array"));
    v8ObjectCls = (jclass)env->NewGlobalRef(env->FindClass("com/example/v8engine/V8Object"));


    // Get all method IDs
    v8ArrayInitMethodID = env->GetMethodID(v8ArrayCls, "<init>", "(Lcom/example/v8engine/V8;)V");
    v8ArrayGetHandleMethodID = env->GetMethodID(v8ArrayCls, "getHandle", "()J");
    v8ObjectInitMethodID = env->GetMethodID(v8ObjectCls, "<init>", "(Lcom/example/v8engine/V8;)V");
    v8ObjectGetHandleMethodID = env->GetMethodID(v8ObjectCls, "getHandle", "()J");
    v8CallObjectJavaMethodMethodID = env->GetMethodID(v8Cls, "callObjectJavaMethod", "(JLcom/example/v8engine/V8Object;Lcom/example/v8engine/V8Array;)Ljava/lang/Object;");
}

V8Runtime::V8Runtime(JNIEnv *env, jobject obj) : env_(env){
    v8 = env->NewGlobalRef(obj);
}

V8Runtime::~V8Runtime() {}

void V8Runtime::createIsolate(jstring globalAlias) {
    // Initialize V8.
    LOGD("create v8 isolate");

    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    mIsolate = v8::Isolate::New(create_params);
//    Locker locker(mIsolate);

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

    // enter and exit
    Context::Scope context_scope(context);

    Local<Object> obj = Local<Object>::New(mIsolate, *globalObject_);

    // console
    Local<v8::Object> console = Console::createConsole(context, nullptr, 1024);
    obj->Set(context, Nan::New("console").ToLocalChecked(), console);

    // canvas
    Local<Object> bindings = Object::New(mIsolate);
    Engine::Context2d::Initialize(bindings);
    obj->Set(context, Nan::New("bindings").ToLocalChecked(), bindings);
    InspectorClient::GetInstance()->init();
}

void V8Runtime::createInspector() {
    //
}

void V8Runtime::require(std::string src, std::string filename) {
    SETUP(this);

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
    SETUP(this);

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
    SETUP(this);
    Local<Object> object = Object::New(mIsolate);
    Persistent<Object>* container = new Persistent<Object>;
    container->Reset(mIsolate, object);
    return reinterpret_cast<jlong>(container);
}

jobject createParameterArray(JNIEnv* env, const Local<Context>& context, Isolate* isolate, jobject v8, int size, const NAN_METHOD_ARGS_TYPE args) {
    // V8Array result = new V8Array(v8);
    jobject result = env->NewObject(v8ArrayCls, v8ArrayInitMethodID, v8);
    // objectHandle = result.getHandle();
    jlong objectHandle = env->CallLongMethod(result, v8ArrayGetHandleMethodID);
    Handle<Object> params = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
    for (int i = 0; i < size; ++i) {
        Maybe<bool> unusedResult = params->Set(context, static_cast<uint32_t>(i), args[i]);
        unusedResult.ToChecked();
    }

    return result;
}

void objectCallback(const NAN_METHOD_ARGS_TYPE args) {
    Local<External> data = Local<External>::Cast(args.Data());
    void *methodDescriptorPtr = data->Value();
    MethodDescriptor* md = static_cast<MethodDescriptor*>(methodDescriptorPtr);
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr);
    SETUP(runtime);
    JNIEnv* env = JEnv();

    Handle<Value> receiver = args.This();
    jobject jreceiver = nullptr;
    jobject params = createParameterArray(env, context, isolate, runtime->v8, args.Length(), args);

    jobject resultObject = env->CallObjectMethod(runtime->v8, v8CallObjectJavaMethodMethodID, md->methodID, jreceiver, params);

}

jlong V8Runtime::registerJavaMethod(jlong objectHandle, jstring functionName, jboolean voidMethod) {
    SETUP(this);

    v8::FunctionCallback callback = objectCallback;

    Local<Object> globalObject = Local<Object>::New(mIsolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
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

jstring V8Runtime::toString(jlong objectHandle) {
    SETUP(this);
    Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
    String::Value unicodeString(isolate, object);

    return env_->NewString(*unicodeString, unicodeString.length());
}

jobject V8Runtime::arrayGet(jint expectedType, jlong arrayHandle, jint index) {
    SETUP(this);
    Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
    Handle<Value> result = array->Get(context, static_cast<uint32_t>(index)).ToLocalChecked();
    return getResult(env_, context, v8, isolate, result, expectedType);
}

jobject getResult(JNIEnv *env, const Local<Context>& context, jobject v8, Isolate* isolate, Handle<Value> &result, jint expectedType) {
    if (result->IsUndefined() && expectedType == V8_ARRAY) {

    } else if (result->IsUndefined() && (expectedType == V8_OBJECT || expectedType == V8_NULL)) {

    } else if (result->IsInt32()) {

    } else if (result->IsNumber()) {

    } else if (result->IsBoolean()) {

    } else if (result->IsString()) {
        String::Value unicodeString(isolate, result->ToString(context).ToLocalChecked());
        return env->NewString(*unicodeString, unicodeString.length());
    } else if (result->IsFunction()) {

    } else if (result->IsArray()) {

    } else if (result->IsTypedArray()) {

    } else if (result->IsArrayBuffer()) {

    } else if (result->IsObject()) {
        jobject objectResult = env->NewObject(v8ObjectCls, v8ObjectInitMethodID, v8);
        jlong resultHandle = env->CallLongMethod(objectResult, v8ObjectGetHandleMethodID);
        reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(context).ToLocalChecked());
        return objectResult;
    }

    return nullptr;
}


//
// Created by zhanfang on 2021/6/9.
//

#include "V8Engine.h"
#include "os-android.h"
#include "nan.h"
#include "console/Console.h"

using namespace std;
using namespace v8;

#define SETUP(runtime)\
    Isolate* isolate = runtime->mIsolate;\
    Isolate::Scope isolateScope(isolate);\
    HandleScope handle_scope(isolate);\
    Local<Context> context = Local<Context>::New(isolate, runtime->context_);\
    Context::Scope context_scope(context);

static void jsWindowObjectAccessor(Local<String> property, const v8::PropertyCallbackInfo<Value>& info) {
    info.GetReturnValue().Set(info.GetIsolate()->GetCurrentContext()->Global());
}

void V8Engine::createIsolate(const string globalAlias) {
    LOGD("create v8 isolate");

    // Create a new Isolate and make it the current one.
    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    mIsolate = Isolate::New(create_params);

    Isolate::Scope isolate_scope(mIsolate);
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(mIsolate);

    Local<ObjectTemplate> globalObject = ObjectTemplate::New(mIsolate);
    if (!globalAlias.empty()) {
        Local<String> alias = Nan::New(globalAlias).ToLocalChecked();
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
}

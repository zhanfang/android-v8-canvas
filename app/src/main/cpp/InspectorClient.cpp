#include <v8.h>
#include "InspectorClient.h"
#include "InspectorFrontend.h"
#include <os-android.h>
#include "v8_inspector/src/inspector/v8-inspector-impl.h"

#ifndef DCHECK
#define DCHECK(condition) assert(condition)
#endif

#ifndef CHECK
#define CHECK(condition) assert(condition)
#endif

using namespace v8;
using namespace v8_inspector;

enum {
    kInspectorClientIndex
};

InspectorClient::InspectorClient(Local<Context> context, bool connect) {
    if (!connect) return;
    isolate_ = context->GetIsolate();
    channel_.reset(new InspectorFrontend(context));
    inspector_ = v8_inspector::V8Inspector::create(isolate_, this);
    session_ =
            inspector_->connect(1, channel_.get(), v8_inspector::StringView());
    context->SetAlignedPointerInEmbedderData(kInspectorClientIndex, this);
    inspector_->contextCreated(v8_inspector::V8ContextInfo(
            context, kContextGroupId, v8_inspector::StringView()));

    Local<Value> function =
            FunctionTemplate::New(isolate_, SendInspectorMessage)
                    ->GetFunction(context)
                    .ToLocalChecked();
    Local<String> function_name =
            String::NewFromUtf8(isolate_, "send", NewStringType::kNormal)
                    .ToLocalChecked();
    CHECK(context->Global()->Set(context, function_name, function).FromJust());

    context_.Reset(isolate_, context);
}

InspectorClient::~InspectorClient() {}

v8_inspector::V8InspectorSession* InspectorClient::GetSession(Local<Context> context) {
    InspectorClient* inspector_client = static_cast<InspectorClient*>(
            context->GetAlignedPointerFromEmbedderData(kInspectorClientIndex));
    return inspector_client->session_.get();
}

Local<Context> InspectorClient::ensureDefaultContextInGroup(int group_id) {
    DCHECK(isolate_);
    DCHECK_EQ(kContextGroupId, group_id);
    return context_.Get(isolate_);
}

void InspectorClient::SendInspectorMessage(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    args.GetReturnValue().Set(Undefined(isolate));
    Local<String> message = args[0]->ToString(context).ToLocalChecked();
    v8_inspector::V8InspectorSession* session = InspectorClient::GetSession(context);
    int length = message->Length();
    std::unique_ptr<uint16_t[]> buffer(new uint16_t[length]);
    message->Write(buffer.get(), 0, length);
    v8_inspector::StringView message_view(buffer.get(), length);
    session->dispatchProtocolMessage(message_view);
    args.GetReturnValue().Set(True(isolate));
}
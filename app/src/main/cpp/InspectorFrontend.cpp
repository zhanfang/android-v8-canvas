//
// Created by Zhan,Fang on 2018/9/14.
//

#include "InspectorFrontend.h"

InspectorFrontend::InspectorFrontend(Local<Context> context) {
    isolate_ = context->GetIsolate();
    context_.Reset(isolate_, context);
}

void InspectorFrontend::sendResponse(
        int callId,
        std::unique_ptr<v8_inspector::StringBuffer> message) {
    Send(message->string());
}

void InspectorFrontend::sendNotification(
        std::unique_ptr<v8_inspector::StringBuffer> message) {
    Send(message->string());
}

void InspectorFrontend::flushProtocolNotifications() {}

void InspectorFrontend::Send(const v8_inspector::StringView& string) {
    v8::Isolate::AllowJavascriptExecutionScope allow_script(isolate_);
    int length = static_cast<int>(string.length());
    Local<String> message =
            (string.is8Bit()
             ? v8::String::NewFromOneByte(
                            isolate_,
                            reinterpret_cast<const uint8_t*>(string.characters8()),
                            v8::NewStringType::kNormal, length)
             : v8::String::NewFromTwoByte(
                            isolate_,
                            reinterpret_cast<const uint16_t*>(string.characters16()),
                            v8::NewStringType::kNormal, length))
                    .ToLocalChecked();
    Local<String> callback_name =
            v8::String::NewFromUtf8(isolate_, "receive", v8::NewStringType::kNormal)
                    .ToLocalChecked();
    Local<Context> context = context_.Get(isolate_);
    Local<Value> callback =
            context->Global()->Get(context, callback_name).ToLocalChecked();
    if (callback->IsFunction()) {
        v8::TryCatch try_catch(isolate_);
        Local<Value> args[] = {message};
    }
}
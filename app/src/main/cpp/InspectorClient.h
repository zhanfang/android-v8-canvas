//
// Created by Zhan,Fang on 2018/9/14.
//

#ifndef INSPECTORCLIENT_H_
#define INSPECTORCLIENT_H_

#include <v8.h>
#include <jni.h>
#include <v8-inspector.h>

using namespace v8;
using namespace v8_inspector;

class InspectorClient : public v8_inspector::V8InspectorClient {
private:
    static v8_inspector::V8InspectorSession* GetSession(Local<Context> context);
    Local<Context> ensureDefaultContextInGroup(int group_id) override;

    static void SendInspectorMessage(
            const v8::FunctionCallbackInfo<v8::Value>& args);
    static const int kContextGroupId = 1;

    std::unique_ptr<v8_inspector::V8Inspector> inspector_;
    std::unique_ptr<v8_inspector::V8InspectorSession> session_;
    std::unique_ptr<v8_inspector::V8Inspector::Channel> channel_;
    Global<Context> context_;
    v8::Isolate* isolate_;

public:
    InspectorClient(Local<Context> context, bool connect);

    virtual ~InspectorClient();
};


#endif /* JSV8INSPECTORCLIENT_H_ */

#include <v8.h>
#include <include/libplatform/libplatform.h>
#include <assert.h>
#include "ArgConverter.h"
#include "InspectorClient.h"
#include "JniLocalRef.h"
#include "log/os-android.h"
#include <v8_inspector/src/v8.h>

using namespace std;
using namespace tns;

using namespace v8;
using namespace v8_inspector;

InspectorClient::InspectorClient(v8::Isolate* isolate)
        : isolate_(isolate),
          inspector_(nullptr),
          session_(nullptr),
          connection(nullptr),
          context_(),
          running_nested_loop_(false),
          isConnected(false) {
    JEnv env;

    inspectorClass = env.FindClass("com/example/v8engine/V8Inspector");
    assert(inspectorClass != nullptr);

    sendMethod = env.GetStaticMethodID(inspectorClass, "send", "(Ljava/lang/Object;Ljava/lang/String;)V");
    assert(sendMethod != nullptr);

    sendToDevToolsConsoleMethod = env.GetStaticMethodID(inspectorClass, "sendToDevToolsConsole", "(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/String;)V");
    assert(sendToDevToolsConsoleMethod != nullptr);

    getInspectorMessageMethod = env.GetStaticMethodID(inspectorClass, "getInspectorMessage", "(Ljava/lang/Object;)Ljava/lang/String;");
    assert(getInspectorMessageMethod != nullptr);

}

void InspectorClient::connect(jobject connection) {
    JEnv env;
    this->connection = env.NewGlobalRef(connection);
    this->isConnected = true;
}

void InspectorClient::scheduleBreak() {
    Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handleScope(isolate_);

    this->session_->schedulePauseOnNextStatement(v8_inspector::StringView(), v8_inspector::StringView());
}

void InspectorClient::createInspectorSession(v8::Isolate* isolate, const v8::Local<v8::Context>& context) {
    LOGD("createInspectorSession");
    session_ = inspector_->connect(InspectorClient::contextGroupId, this, v8_inspector::StringView());
}

void InspectorClient::disconnect() {
    if (this->connection == nullptr) {
        return;
    }

    Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handleScope(isolate_);

    session_->resume();
    session_.reset();

    JEnv env;
    env.DeleteGlobalRef(this->connection);
    this->connection = nullptr;
    this->isConnected = false;

    this->createInspectorSession(isolate_, InspectorClient::PersistentToLocal(isolate_, context_));
}

void InspectorClient::dispatchMessage(const std::string& message) {
    Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handleScope(isolate_);
    v8::Local<v8::Context> context = isolate_->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    this->doDispatchMessage(isolate_, message);
}

void InspectorClient::runMessageLoop() {
    if (running_nested_loop_) {
        return;
    }

    JEnv env;

    terminated_ = false;
    running_nested_loop_ = true;
    while (!terminated_) {
        JniLocalRef msg(env.CallStaticObjectMethod(inspectorClass, getInspectorMessageMethod, this->connection));
        if (!msg.IsNull()) {
            auto inspectorMessage = ArgConverter::jstringToString(msg);
            this->doDispatchMessage(this->isolate_, inspectorMessage);
        }

        while (v8::platform::PumpMessageLoop(v8::internal::V8::GetCurrentPlatform(), isolate_)) {}
    }
    terminated_ = false;
    running_nested_loop_ = false;
}

void InspectorClient::runMessageLoopOnPause(int context_group_id) {
    this->waiting_for_resume_ = true;
    this->runMessageLoop();
}

void InspectorClient::waitForFrontend() {
    this->waiting_for_frontend_ = true;
    this->runMessageLoop();
}

void InspectorClient::quitMessageLoopOnPause() {
    terminated_ = true;
}

v8::Local<v8::Context> InspectorClient::ensureDefaultContextInGroup(int contextGroupId) {
    v8::Local<v8::Context> context = PersistentToLocal(isolate_, context_);
    return context;
}

void InspectorClient::doDispatchMessage(v8::Isolate* isolate, const std::string& message) {
    if (session_ == nullptr) {
        return;
    }

    const v8_inspector::String16 msg = v8_inspector::String16::fromUTF8(message.c_str(), message.length());
    v8_inspector::StringView message_view = toStringView(msg);
    session_->dispatchProtocolMessage(message_view);
}

void InspectorClient::sendResponse(int callId, std::unique_ptr<StringBuffer> message) {
    sendNotification(std::move(message));
}

static v8_inspector::String16 ToString16(const v8_inspector::StringView& string) {
    if (string.is8Bit()) {
        return v8_inspector::String16(reinterpret_cast<const char*>(string.characters8()), string.length());
    }

    return v8_inspector::String16(reinterpret_cast<const uint16_t*>(string.characters16()), string.length());
}

void InspectorClient::sendNotification(std::unique_ptr<StringBuffer> message) {
    if (inspectorClass == nullptr || this->connection == nullptr) {
        return;
    }

    v8_inspector::String16 msg = ToString16(message->string());

    JEnv env;
    // TODO: Pete: Check if we can use a wide (utf 16) string here
    JniLocalRef str(env.NewStringUTF(msg.utf8().c_str()));
    env.CallStaticVoidMethod(inspectorClass, sendMethod, this->connection, (jstring) str);
}

void InspectorClient::flushProtocolNotifications() {}

template<class TypeName>
inline v8::Local<TypeName> StrongPersistentToLocal(const v8::Persistent<TypeName>& persistent) {
    return *reinterpret_cast<v8::Local<TypeName> *>(const_cast<v8::Persistent<TypeName> *>(&persistent));
}

template<class TypeName>
inline v8::Local<TypeName> WeakPersistentToLocal(v8::Isolate* isolate, const v8::Persistent<TypeName>& persistent) {
    return v8::Local<TypeName>::New(isolate, persistent);
}

template<class TypeName>
inline v8::Local<TypeName> InspectorClient::PersistentToLocal(v8::Isolate* isolate, const v8::Persistent<TypeName>& persistent) {
    if (persistent.IsWeak()) {
        return WeakPersistentToLocal(isolate, persistent);
    } else {
        return StrongPersistentToLocal(persistent);
    }
}

void InspectorClient::init() {
    if (inspector_ != nullptr) {
        return;
    }
    LOGD("inspectorClient init");

    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);
    v8::Local<v8::Context> context = isolate_->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    inspector_ = V8Inspector::create(isolate_, this);
    inspector_->contextCreated(v8_inspector::V8ContextInfo(context, InspectorClient::contextGroupId, v8_inspector::StringView()));


    this->createInspectorSession(isolate_, context);
}

InspectorClient* InspectorClient::GetInstance() {
    if (instance == nullptr) {
        LOGD("new InspectorClient");
        instance = new InspectorClient(Isolate::GetCurrent());
    }

    return instance;
}

void InspectorClient::sendToFrontEndCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {

}

void InspectorClient::consoleLogCallback(const string& message, const string& logLevel) {
//    if (!inspectorIsConnected()) {
//        return;
//    }
//
//    auto isolate = Runtime::GetRuntime(0)->GetIsolate();
//    auto stack = v8::StackTrace::CurrentStackTrace(isolate, 1, v8::StackTrace::StackTraceOptions::kDetailed);
//
//    auto frame = stack->GetFrame(0);
//
//    // will be no-op in non-debuggable builds
//    v8_inspector::V8LogAgentImpl::EntryAdded(message, logLevel, ArgConverter::ConvertToString(frame->GetScriptNameOrSourceURL()), frame->GetLineNumber());
}

void InspectorClient::attachInspectorCallbacks(Isolate* isolate,
                                                   Local<ObjectTemplate>& globalObjectTemplate) {
    v8::HandleScope scope(isolate);

    auto inspectorJSObject = ObjectTemplate::New(isolate);

    inspectorJSObject->SetAccessor(ArgConverter::ConvertToV8String(isolate, "isConnected"), InspectorClient::InspectorIsConnectedGetterCallback);

    globalObjectTemplate->Set(ArgConverter::ConvertToV8String(isolate, "__inspector"), inspectorJSObject);
}

void InspectorClient::InspectorIsConnectedGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    info.GetReturnValue().Set(InspectorClient::GetInstance()->isConnected);
}

InspectorClient* InspectorClient::instance = nullptr;
jclass InspectorClient::inspectorClass = nullptr;
jmethodID InspectorClient::sendMethod = nullptr;
jmethodID InspectorClient::sendToDevToolsConsoleMethod = nullptr;
jmethodID InspectorClient::getInspectorMessageMethod = nullptr;
int InspectorClient::contextGroupId = 1;




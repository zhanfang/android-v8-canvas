#include <v8.h>
#include <assert.h>

#include "ArgConverter.h"
#include "InspectorClient.h"
#include "v8_inspector/src/inspector/v8-inspector-impl.h"

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

    inspectorClass = env.FindClass("com/example/inspector/V8Inspector");
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
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate_, mPersistentContext);
    v8::Context::Scope context_scope(context);

//    Context::Scope context_scope(isolate_->GetCurrentContext());

    this->doDispatchMessage(isolate_, message);
}

void InspectorClient::runMessageLoopOnPause(int context_group_id) {
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

        while (v8::platform::PumpMessageLoop(Runtime::platform, isolate_)) {
        }
    }
    terminated_ = false;
    running_nested_loop_ = false;
}

void InspectorClient::quitMessageLoopOnPause() {
    terminated_ = true;
}

v8::Local<v8::Context> JsV8InspectorClient::ensureDefaultContextInGroup(int contextGroupId) {
    v8::Local<v8::Context> context = PersistentToLocal(isolate_, context_);
    return context;
}

void JsV8InspectorClient::doDispatchMessage(v8::Isolate* isolate, const std::string& message) {
    if (session_ == nullptr) {
        return;
    }

    const v8_inspector::String16 msg = v8_inspector::String16::fromUTF8(message.c_str(), message.length());
    v8_inspector::StringView message_view = toStringView(msg);
    session_->dispatchProtocolMessage(message_view);
}

void JsV8InspectorClient::sendResponse(int callId, std::unique_ptr<StringBuffer> message) {
    sendNotification(std::move(message));
}

static v8_inspector::String16 ToString16(const v8_inspector::StringView& string) {
    if (string.is8Bit()) {
        return v8_inspector::String16(reinterpret_cast<const char*>(string.characters8()), string.length());
    }

    return v8_inspector::String16(reinterpret_cast<const uint16_t*>(string.characters16()), string.length());
}

void JsV8InspectorClient::sendNotification(std::unique_ptr<StringBuffer> message) {
    if (inspectorClass == nullptr || this->connection == nullptr) {
        return;
    }

    v8_inspector::String16 msg = ToString16(message->string());

    JEnv env;
    // TODO: Pete: Check if we can use a wide (utf 16) string here
    JniLocalRef str(env.NewStringUTF(msg.utf8().c_str()));
    env.CallStaticVoidMethod(inspectorClass, sendMethod, this->connection, (jstring) str);
}

void JsV8InspectorClient::flushProtocolNotifications() {
}

template<class TypeName>
inline v8::Local<TypeName> StrongPersistentToLocal(const v8::Persistent<TypeName>& persistent) {
    return *reinterpret_cast<v8::Local<TypeName> *>(const_cast<v8::Persistent<TypeName> *>(&persistent));
}

template<class TypeName>
inline v8::Local<TypeName> WeakPersistentToLocal(v8::Isolate* isolate, const v8::Persistent<TypeName>& persistent) {
    return v8::Local<TypeName>::New(isolate, persistent);
}

template<class TypeName>
inline v8::Local<TypeName> JsV8InspectorClient::PersistentToLocal(v8::Isolate* isolate, const v8::Persistent<TypeName>& persistent) {
    if (persistent.IsWeak()) {
        return WeakPersistentToLocal(isolate, persistent);
    } else {
        return StrongPersistentToLocal(persistent);
    }
}

void JsV8InspectorClient::init() {
    if (inspector_ != nullptr) {
        return;
    }

    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate_, mPersistentContext);
    v8::Context::Scope context_scope(context);


//    v8::Local<Context> context = isolate_->GetCurrentContext();

    inspector_ = V8Inspector::create(isolate_, this);

    inspector_->contextCreated(v8_inspector::V8ContextInfo(context, JsV8InspectorClient::contextGroupId, v8_inspector::StringView()));

//    v8::Persistent<v8::Context> persistentContext(isolate_, JsV8InspectorClient::PersistentToLocal(isolate_, context_));
//    context_.Reset(isolate_, persistentContext);

    mPersistentContext.Reset(isolate_, context);

    this->createInspectorSession(isolate_, context);
}

JsV8InspectorClient* JsV8InspectorClient::GetInstance() {
    if (instance == nullptr) {
        instance = new JsV8InspectorClient(mIsolate);
    }

    return instance;
}

void JsV8InspectorClient::sendToFrontEndCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if ((instance == nullptr) || (instance->connection == nullptr)) {
        return;
    }

    try {
        if ((args.Length() > 0) && args[0]->IsString()) {
            std::string message = ArgConverter::ConvertToString(args[0]->ToString());

            std::string level = "log";
            if (args.Length() > 1  && args[1]->IsString()) {
                level = ArgConverter::ConvertToString(args[1]->ToString());
            }

            JEnv env;
            JniLocalRef str(env.NewStringUTF(message.c_str()));
            JniLocalRef lev(env.NewStringUTF(level.c_str()));
            env.CallStaticVoidMethod(inspectorClass, sendToDevToolsConsoleMethod, instance->connection, (jstring) str, (jstring)lev);
        }
    } catch (NativeScriptException& e) {
        e.ReThrowToV8();
    } catch (std::exception e) {
        stringstream ss;
        ss << "Error: c++ exception: " << e.what() << endl;
        NativeScriptException nsEx(ss.str());
        nsEx.ReThrowToV8();
    } catch (...) {
        NativeScriptException nsEx(std::string("Error: c++ exception!"));
        nsEx.ReThrowToV8();
    }
}

void InspectorClient::consoleLogCallback(const string& message, const string& logLevel) {
    if (!inspectorIsConnected()) {
        return;
    }

    auto isolate = Runtime::GetRuntime(0)->GetIsolate();
    auto stack = v8::StackTrace::CurrentStackTrace(isolate, 1, v8::StackTrace::StackTraceOptions::kDetailed);

    auto frame = stack->GetFrame(0);

    // will be no-op in non-debuggable builds
    v8_inspector::V8LogAgentImpl::EntryAdded(message, logLevel, ArgConverter::ConvertToString(frame->GetScriptNameOrSourceURL()), frame->GetLineNumber());
}

void InspectorClient::attachInspectorCallbacks(Isolate* isolate,
                                                   Local<ObjectTemplate>& globalObjectTemplate) {
    v8::HandleScope scope(isolate);

    auto inspectorJSObject = ObjectTemplate::New(isolate);

    inspectorJSObject->Set(ArgConverter::ConvertToV8String(isolate, "responseReceived"), FunctionTemplate::New(isolate, NetworkDomainCallbackHandlers::ResponseReceivedCallback));
    inspectorJSObject->Set(ArgConverter::ConvertToV8String(isolate, "requestWillBeSent"), FunctionTemplate::New(isolate, NetworkDomainCallbackHandlers::RequestWillBeSentCallback));
    inspectorJSObject->Set(ArgConverter::ConvertToV8String(isolate, "dataForRequestId"), FunctionTemplate::New(isolate, NetworkDomainCallbackHandlers::DataForRequestIdCallback));
    inspectorJSObject->Set(ArgConverter::ConvertToV8String(isolate, "loadingFinished"), FunctionTemplate::New(isolate, NetworkDomainCallbackHandlers::LoadingFinishedCallback));
    inspectorJSObject->SetAccessor(ArgConverter::ConvertToV8String(isolate, "isConnected"), JsV8InspectorClient::InspectorIsConnectedGetterCallback);

    inspectorJSObject->Set(ArgConverter::ConvertToV8String(isolate, "documentUpdated"), FunctionTemplate::New(isolate, DOMDomainCallbackHandlers::DocumentUpdatedCallback));
    inspectorJSObject->Set(ArgConverter::ConvertToV8String(isolate, "childNodeInserted"), FunctionTemplate::New(isolate, DOMDomainCallbackHandlers::ChildNodeInsertedCallback));
    inspectorJSObject->Set(ArgConverter::ConvertToV8String(isolate, "childNodeRemoved"), FunctionTemplate::New(isolate, DOMDomainCallbackHandlers::ChildNodeRemovedCallback));
    inspectorJSObject->Set(ArgConverter::ConvertToV8String(isolate, "attributeModified"), FunctionTemplate::New(isolate, DOMDomainCallbackHandlers::AttributeModifiedCallback));
    inspectorJSObject->Set(ArgConverter::ConvertToV8String(isolate, "attributeRemoved"), FunctionTemplate::New(isolate, DOMDomainCallbackHandlers::AttributeRemovedCallback));

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



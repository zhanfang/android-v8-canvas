//
// Created by zhanfang on 2021-01-18.
//
#include <sstream>
#include <assert.h>
#include "V8Console.h"
#include "ArgConverter.h"
#include "log/os-android.h"

using namespace tns;

v8::Local<v8::Object> Console::createConsole(v8::Local<v8::Context> context, ConsoleCallback callback, const int maxLogcatObjectSize) {
    m_callback = callback;
    m_maxLogcatObjectSize = maxLogcatObjectSize;
    v8::Context::Scope contextScope(context);
    v8::Isolate* isolate = context->GetIsolate();

    v8::Local<v8::Object> console = v8::Object::New(isolate);
    bool success = console->SetPrototype(context, v8::Object::New(isolate)).FromMaybe(false);

    assert(success);

    bindFunctionProperty(context, console, "log", logCallback);
    bindFunctionProperty(context, console, "warn", warnCallback);

    return console;
}

void Console::sendToADBLogcat(const std::string& message, android_LogPriority logPriority) {
    // limit the size of the message that we send to logcat using the predefined value in package.json
    auto messageToLog = message;
    if (messageToLog.length() > m_maxLogcatObjectSize) {
        messageToLog = messageToLog.erase(m_maxLogcatObjectSize, std::string::npos);
        messageToLog = messageToLog + "...";
    }

    // split strings into chunks of 4000 characters
    // __android_log_write can't send more than 4000 to the stdout at a time
    auto messageLength = messageToLog.length();
    int maxStringLength = 4000;

    if (messageLength < maxStringLength) {
        __android_log_write(logPriority, LOG_TAG, messageToLog.c_str());
    } else {
        for (int i = 0; i < messageLength; i += maxStringLength) {
            auto messagePart = messageToLog.substr(i, maxStringLength);

            __android_log_write(logPriority, LOG_TAG, messagePart.c_str());
        }
    }
}

const v8::Local<v8::String> buildStringFromArg(v8::Isolate* isolate, const v8::Local<v8::Value>& val) {
    v8::Local<v8::String> argString;
    if (val->IsFunction()) {
        val->ToDetailString(isolate->GetCurrentContext()).ToLocal(&argString);
    } else if (val->IsArray()) {
        auto context = isolate->GetCurrentContext();
        auto cachedSelf = val;
        auto array = val->ToObject(context).ToLocalChecked();
        auto arrayEntryKeys = array->GetPropertyNames(isolate->GetCurrentContext()).ToLocalChecked();

        auto arrayLength = arrayEntryKeys->Length();

        argString = ArgConverter::ConvertToV8String(isolate, "[");

        for (int i = 0; i < arrayLength; i++) {
            auto propertyName = arrayEntryKeys->Get(context, i).ToLocalChecked();

            auto propertyValue = array->Get(context, propertyName).ToLocalChecked();

            // avoid bottomless recursion with cyclic reference to the same array
            if (propertyValue->StrictEquals(cachedSelf)) {
                argString = v8::String::Concat(argString, ArgConverter::ConvertToV8String(isolate, "[Circular]"));
                continue;
            }

            auto objectString = buildStringFromArg(isolate, propertyValue);

            argString = v8::String::Concat(argString, objectString);

            if (i != arrayLength - 1) {
                argString = v8::String::Concat(argString, ArgConverter::ConvertToV8String(isolate, ", "));
            }
        }

        argString = v8::String::Concat(argString, ArgConverter::ConvertToV8String(isolate, "]"));
    } else if (val->IsObject()) {
        v8::Local<v8::Object> obj = val.As<v8::Object>();

        // Todo: json 类型待实现
//        argString = transformJSObject(isolate, obj);
    } else {
        val->ToDetailString(isolate->GetCurrentContext()).ToLocal(&argString);
    }

    return argString;
}

const std::string buildLogString(const v8::FunctionCallbackInfo<v8::Value>& info, int startingIndex = 0) {
    auto isolate = info.GetIsolate();

    v8::HandleScope scope(isolate);

    std::stringstream ss;

    auto argLen = info.Length();
    if (argLen) {
        for (int i = startingIndex; i < argLen; i++) {
            v8::Local<v8::String> argString;

            argString = buildStringFromArg(isolate, info[i]);

            // separate args with a space
            if (i != 0) {
                ss << " ";
            }

            ss << ArgConverter::ConvertToString(argString);
        }
    } else {
        ss << std::endl;
    }

    std::string stringResult = ss.str();

    return stringResult;
}

void Console::logCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
    std::string log = buildLogString(info);

    sendToADBLogcat(log, ANDROID_LOG_INFO);
}

void Console::warnCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
    std::string log = buildLogString(info);
    sendToADBLogcat(log, ANDROID_LOG_WARN);
}

ConsoleCallback Console::m_callback = nullptr;
int Console::m_maxLogcatObjectSize;

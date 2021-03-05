//
// Created by zhanfang on 2021-01-18.
//

#ifndef ANDROID_V8_DEMO_MASTER_CONSOLE_H
#define ANDROID_V8_DEMO_MASTER_CONSOLE_H

#include <string>
#include <android/log.h>
#include <nan.h>


typedef void (*ConsoleCallback)(v8::Isolate* isolate, const std::string& message, const std::string& logLevel);

class Console {
    public:
        static v8::Local<v8::Object> createConsole(v8::Local<v8::Context> context, ConsoleCallback callback, const int maxLogcatObjectSize);

        static void logCallback(const v8::FunctionCallbackInfo<v8::Value>& info);
        static void warnCallback(const v8::FunctionCallbackInfo<v8::Value>& info);

    private:
        static int m_maxLogcatObjectSize;
        static ConsoleCallback m_callback;

        // heavily inspired by 'createBoundFunctionProperty' of V8's v8-console.h
        static void bindFunctionProperty(v8::Local<v8::Context> context,
                                         v8::Local<v8::Object> consoleInstance,
                                         const std::string& functionName,
                                         v8::FunctionCallback callback) {
            v8::Local<v8::String> funcName = Nan::New<v8::String>(functionName).ToLocalChecked();
            v8::Local<v8::Function> func;
            if (!v8::Function::New(context, callback, consoleInstance, 0, v8::ConstructorBehavior::kThrow).ToLocal(&func)) {
                return;
            }
            func->SetName(funcName);

            consoleInstance->CreateDataProperty(context, funcName, func);
        }

        static void sendToADBLogcat(const std::string& log, android_LogPriority logPriority);
};



#endif //ANDROID_V8_DEMO_MASTER_CONSOLE_H

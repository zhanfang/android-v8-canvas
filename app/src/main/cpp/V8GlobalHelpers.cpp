//
// Created by zhanfang on 2021/3/6.
//

#include <sstream>
#include <string>


#include "V8GlobalHelpers.h"
#include "ArgConverter.h"

using namespace v8;

static std::map<v8::Isolate*, v8::Persistent<v8::Function>*> isolateToPersistentSmartJSONStringify = std::map<v8::Isolate*, v8::Persistent<v8::Function>*>();

Local<Function> GetSmartJSONStringifyFunction(Isolate* isolate) {
    auto it = isolateToPersistentSmartJSONStringify.find(isolate);
    if (it != isolateToPersistentSmartJSONStringify.end()) {
        auto smartStringifyPersistentFunction = it->second;

        return smartStringifyPersistentFunction->Get(isolate);
    }

    std::string smartStringifyFunctionScript =
            "(function () {\n"
            "    function smartStringify(object) {\n"
            "        return JSON.stringify(object);\n"
            "    }\n"
            "    return smartStringify;\n"
            "})();";

    auto source = tns::ArgConverter::ConvertToV8String(isolate, smartStringifyFunctionScript);
    auto context = isolate->GetCurrentContext();

    Local<Script> script;
    auto maybeScript = Script::Compile(context, source).ToLocal(&script);

    if (script.IsEmpty()) {
        return Local<Function>();
    }

    Local<Value> result;
    auto maybeResult = script->Run(context).ToLocal(&result);

    if (result.IsEmpty() && !result->IsFunction()) {
        return Local<Function>();
    }

    auto smartStringifyFunction = result.As<Function>();

    auto smartStringifyPersistentFunction = new Persistent<Function>(isolate, smartStringifyFunction);

    isolateToPersistentSmartJSONStringify.insert(std::make_pair(isolate, smartStringifyPersistentFunction));

    return smartStringifyPersistentFunction->Get(isolate);
}

Local<String> tns::JsonStringifyObject(Isolate* isolate, Handle<v8::Value> value, bool handleCircularReferences) {
    if (value.IsEmpty()) {
        return String::Empty(isolate);
    }

    auto context = isolate->GetCurrentContext();
    if (handleCircularReferences) {
        Local<Function> smartJSONStringifyFunction = GetSmartJSONStringifyFunction(isolate);

        if (!smartJSONStringifyFunction.IsEmpty()) {
            if (value->IsObject()) {
                v8::Local<v8::Value> resultValue;
                v8::TryCatch tc(isolate);

                Local<Value> args[] = { value->ToObject(context).ToLocalChecked() };
                auto success = smartJSONStringifyFunction->Call(context, Undefined(isolate), 1, args).ToLocal(&resultValue);

                if (success && !tc.HasCaught()) {
                    return resultValue->ToString(context).ToLocalChecked();
                }
            }
        }
    }

    v8::Local<v8::String> resultString;
    v8::TryCatch tc(isolate);
    auto success = v8::JSON::Stringify(context, value->ToObject(context).ToLocalChecked()).ToLocal(&resultString);

    if (!success && tc.HasCaught()) {
//        throw NativeScriptException(tc);
    }

    return resultString;
}
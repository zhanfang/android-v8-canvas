//
// Created by zhanfang on 2021/3/6.
//

#ifndef V8GLOBALHELPERS_H
#define V8GLOBALHELPERS_H

#include <v8.h>

namespace tns {
    v8::Local<v8::String> JsonStringifyObject(v8::Isolate* isolate, v8::Handle<v8::Value> value, bool handleCircularReferences = true);

    bool V8GetPrivateValue(v8::Isolate* isolate, const v8::Local<v8::Object>& obj, const v8::Local<v8::String>& propName, v8::Local<v8::Value>& out);

    bool V8SetPrivateValue(v8::Isolate* isolate, const v8::Local<v8::Object>& obj, const v8::Local<v8::String>& propName, const v8::Local<v8::Value>& value);
}


#endif // V8GLOBALHELPERS_H

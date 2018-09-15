//
// Created by Zhan,Fang on 2018/9/13.
//

#ifndef TEST1_V8ENGINE_H
#define TEST1_V8ENGINE_H

#include <jni.h>
#include <v8.h>
#include "InspectorClient.h"

class V8Engine {
public:
    V8Engine();
    virtual ~V8Engine();

    static V8Engine* GetInstance(v8::Isolate* isolate);

    void destory();
    void hello();

    v8::Isolate* getIsolate() const;
    v8::Local<v8::Context> getContext() const;

private:
    v8::Persistent<v8::Context> context_;
    v8::Platform* platform_;
};

#endif //TEST1_V8ENGINE_H

//
// Created by zhanfang on 2021/6/9.
//

#ifndef ANDROID_V8_CANVAS_V8ENGINE_H
#define ANDROID_V8_CANVAS_V8ENGINE_H

#include <v8.h>
#include <string>

class V8Engine {
public:
    V8Engine() = default;
    virtual ~V8Engine() {};
    
    v8::Isolate* mIsolate;

    void createIsolate(std::string globalAlias);
    void evalScript();

private:
    v8::Persistent<v8::Object>* globalObject_;
    v8::Persistent<v8::Context> context_;
};


#endif //ANDROID_V8_CANVAS_V8ENGINE_H

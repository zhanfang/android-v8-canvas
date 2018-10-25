//
// Created by Zhan,Fang on 2018/9/13.
//

#include <jni.h>
#include <v8.h>

extern std::unique_ptr<v8::Platform> platform;
extern v8::Isolate *mIsolate;
extern v8::Persistent<v8::Context> mPersistentContext;
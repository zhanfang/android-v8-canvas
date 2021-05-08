//
// Created by Zhan,Fang on 2018/10/18.
//

#include <jni.h>
#include "v8.h"
#include <string>
#include <vector>

#ifndef UTIL_H_
#define UTIL_H_

using namespace v8;

class Util {
public:
    static std::u16string ConvertFromUtf8ToUtf16(const std::string& str);

    static jobject getResult(JNIEnv *env, const Local<Context>& context, jobject &v8, Isolate* isolate, Handle<Value> &result, jint expectedType);
};

#endif /* UTIL_H_ */

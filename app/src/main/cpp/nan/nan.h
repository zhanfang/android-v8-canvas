//
// 一个简单的 nan.h，对 v8 的一些变量、方法进行封装
// Created by zhanfang on 2021/3/3.
//
#ifndef NAN_H_
#define NAN_H_

#include <assert.h>
#include <string>
#include <v8.h>
#include "JEnv.h"
#include "nan_callbacks.h"

using namespace v8;

namespace Nan {
    typedef const v8::FunctionCallbackInfo<v8::Value>& NAN_METHOD_ARGS_TYPE;
    typedef void NAN_METHOD_RETURN_TYPE;

    typedef const v8::PropertyCallbackInfo<v8::Value>& NAN_GETTER_ARGS_TYPE;
    typedef void NAN_GETTER_RETURN_TYPE;

    typedef const v8::PropertyCallbackInfo<void>& NAN_SETTER_ARGS_TYPE;
    typedef void NAN_SETTER_RETURN_TYPE;

    typedef v8::String::ExternalOneByteStringResource ExternalOneByteStringResource;

    #define NAN_METHOD(name)                                                       \
        Nan::NAN_METHOD_RETURN_TYPE name(Nan::NAN_METHOD_ARGS_TYPE info)
    #define NAN_GETTER(name)                                                       \
        Nan::NAN_GETTER_RETURN_TYPE name(                                          \
            v8::Local<v8::String> property                                         \
          , Nan::NAN_GETTER_ARGS_TYPE info)
    #define NAN_SETTER(name)                                                       \
        Nan::NAN_SETTER_RETURN_TYPE name(                                          \
            v8::Local<v8::String> property                                         \
          , v8::Local<v8::Value> value                                             \
          , Nan::NAN_SETTER_ARGS_TYPE info)

    // Nan::New 相关实现
    template <typename T> struct FactoryBase {
        typedef v8::Local<T> return_t;
    };

    template <typename T> struct MaybeFactoryBase {
        typedef v8::MaybeLocal<T> return_t;
    };

    template <typename T> struct Factory;

    template <>
    struct Factory<v8::Array> : FactoryBase<v8::Array> {
        static inline return_t New();
        static inline return_t New(int length);
    };

    template <>
    struct Factory<v8::Boolean> : FactoryBase<v8::Boolean> {
        static inline return_t New(bool value);
    };

    template <>
    struct Factory<v8::String> : MaybeFactoryBase<v8::String> {
        static inline return_t New();
        static inline return_t New(const char *value, int length = -1);
        static inline return_t New(const uint16_t *value, int length = -1);
        static inline return_t New(const std::string& value);

        static inline return_t New(v8::String::ExternalStringResource * value);
        static inline return_t New(ExternalOneByteStringResource * value);
    };


    //=== Array ====================================================================
    Factory<v8::Array>::return_t
    Factory<v8::Array>::New() {
        return v8::Array::New(v8::Isolate::GetCurrent());
    }

    Factory<v8::Array>::return_t
    Factory<v8::Array>::New(int length) {
        return v8::Array::New(v8::Isolate::GetCurrent(), length);
    }
    //=== Boolean ==================================================================

    Factory<v8::Boolean>::return_t
    Factory<v8::Boolean>::New(bool value) {
        return v8::Boolean::New(v8::Isolate::GetCurrent(), value);
    }


    //=== String ===================================================================

    Factory<v8::String>::return_t
    Factory<v8::String>::New() {
        return v8::String::Empty(v8::Isolate::GetCurrent());
    }

    Factory<v8::String>::return_t
    Factory<v8::String>::New(const char * value, int length) {
        return v8::String::NewFromUtf8(
                v8::Isolate::GetCurrent(), value, v8::NewStringType::kNormal, length);
    }

    Factory<v8::String>::return_t
    Factory<v8::String>::New(const std::string & value) {
        assert(value.size() <= INT_MAX && "string too long");
        return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
                                       value.data(), v8::NewStringType::kNormal, static_cast<int>(value.size()));
    }

    Factory<v8::String>::return_t
    Factory<v8::String>::New(const uint16_t * value, int length) {
        return v8::String::NewFromTwoByte(v8::Isolate::GetCurrent(), value,
                                          v8::NewStringType::kNormal, length);
    }

    Factory<v8::String>::return_t
    Factory<v8::String>::New(v8::String::ExternalStringResource * value) {
        return v8::String::NewExternalTwoByte(v8::Isolate::GetCurrent(), value);
    }

    Factory<v8::String>::return_t
    Factory<v8::String>::New(ExternalOneByteStringResource * value) {
        return v8::String::NewExternalOneByte(v8::Isolate::GetCurrent(), value);
    }

    // STA ==============================API========================
    template <typename T>
    typename Factory<T>::return_t
    New() {
        return Factory<T>::New();
    }

    template <typename T, typename A0>
    typename Factory<T>::return_t
    New(A0 arg0) {
        return Factory<T>::New(arg0);
    }

    template <typename T, typename A0, typename A1>
    typename Factory<T>::return_t
    New(A0 arg0, A1 arg1) {
        return Factory<T>::New(arg0, arg1);
    }

    template <typename T, typename A0, typename A1, typename A2>
    typename Factory<T>::return_t
    New(A0 arg0, A1 arg1, A2 arg2) {
        return Factory<T>::New(arg0, arg1, arg2);
    }

    template <typename T, typename A0, typename A1, typename A2, typename A3>
    typename Factory<T>::return_t
    New(A0 arg0, A1 arg1, A2 arg2, A3 arg3) {
        return Factory<T>::New(arg0, arg1, arg2, arg3);
    }
    // END ==============================API========================

    // Convenience
    template <typename T, typename M>
    inline v8::Local<T> New(v8::Persistent<T, M> const& p);

    inline
    Factory<v8::String>::return_t
    New(std::string const& value) {  // NOLINT(build/include_what_you_use)
        return New<v8::String>(value);
    }

    inline
    Factory<v8::String>::return_t
    New(const char * value, int length) {
        return New<v8::String>(value, length);
    }

# define X(NAME)                                                               \
    inline v8::Local<v8::Value> NAME(const char *msg) {                        \
      EscapableHandleScope scope(v8::Isolate::GetCurrent());                                              \
      return scope.Escape(v8::Exception::NAME(New(msg).ToLocalChecked()));     \
    }                                                                          \
                                                                               \
    inline                                                                     \
    v8::Local<v8::Value> NAME(v8::Local<v8::String> msg) {                     \
      return v8::Exception::NAME(msg);                                         \
    }                                                                          \
                                                                               \
    inline void Throw ## NAME(const char *msg) {                               \
      HandleScope scope(v8::Isolate::GetCurrent());                                                       \
      v8::Isolate::GetCurrent()->ThrowException(                               \
          v8::Exception::NAME(New(msg).ToLocalChecked()));                     \
    }                                                                          \
                                                                               \
    inline void Throw ## NAME(v8::Local<v8::String> msg) {                     \
      HandleScope scope(v8::Isolate::GetCurrent());                                                       \
      v8::Isolate::GetCurrent()->ThrowException(                               \
          v8::Exception::NAME(msg));                                           \
    }

    X(Error)
    X(RangeError)
    X(ReferenceError)
    X(SyntaxError)
    X(TypeError)
# undef X

}

#endif  // NAN_H_



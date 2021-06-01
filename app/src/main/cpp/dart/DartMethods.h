#ifndef ANDROID_V8_CANVAS_DartMethods_H
#define ANDROID_V8_CANVAS_DartMethods_H

#include <cstdint>
#include <memory>

#define FFI_EXPORT __attribute__((__visibility__("default")))

struct FFI_EXPORT NativeString {
    const uint16_t *string;
    int32_t length;

    NativeString *clone();
    void free();
};

typedef NativeString* (*PlatformBrightness)(int32_t contextId);

namespace Engine {
    struct DartMethodsPointer {
        DartMethodsPointer() = default;
        PlatformBrightness platformBrightness;
    };

    void registerDartMethods(uint64_t *methodBytes, int32_t length);
}

#endif
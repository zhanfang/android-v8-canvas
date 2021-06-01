//
// Created by zhanfang on 2021/4/27.
//

#include <cstdint>
#include "dart/DartMethods.h"
#include "log/os-android.h"

#define FFI_EXPORT_C extern "C" __attribute__((visibility("default"))) __attribute__((used))

FFI_EXPORT_C
void registerDartMethods(uint64_t *methodBytes, int32_t length) {
    Engine::registerDartMethods(methodBytes, length);
}

FFI_EXPORT_C
int native_add(int x, int y) { return x + y; }


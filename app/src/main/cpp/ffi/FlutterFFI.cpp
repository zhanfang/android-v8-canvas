//
// Created by zhanfang on 2021/4/27.
//

#include <cstdint>
#include "dart/DartMethods.h"
#include "V8Engine.h"
#include <thread>

#define FFI_EXPORT_C extern "C" __attribute__((visibility("default"))) __attribute__((used))

V8Engine* engine;

std::__thread_id flutterUiThreadId;

std::__thread_id getUiThreadId() {
    return flutterUiThreadId;
}

FFI_EXPORT_C
void registerDartMethods(uint64_t *methodBytes, int32_t length) {
    Engine::registerDartMethods(methodBytes, length);
}

FFI_EXPORT_C
void createIsolate() {
    flutterUiThreadId = std::this_thread::get_id();
    engine = new V8Engine();
    engine->createIsolate("window");
}

FFI_EXPORT_C
int native_add(int x, int y) { return x + y; }


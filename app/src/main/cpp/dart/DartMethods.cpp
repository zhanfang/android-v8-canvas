#include "DartMethods.h"
#include <memory>
#include <thread>

namespace Engine {
    std::shared_ptr<DartMethodsPointer> methodPointer = std::make_shared<DartMethodsPointer>();

    std::shared_ptr<DartMethodsPointer> getDartMethod() {
        std::__thread_id currentThread = std::this_thread::get_id();

        return methodPointer;
    }

    void registerDartMethods(uint64_t *methodBytes, int32_t length) {
        size_t i = 0;
        methodPointer->platformBrightness = reinterpret_cast<PlatformBrightness>(methodBytes[i++]);
    }
}



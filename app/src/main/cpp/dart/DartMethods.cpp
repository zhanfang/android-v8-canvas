#include "DartMethods.h"
#include <memory>

namespace Engine {
    std::shared_ptr<DartMethodsPointer> methodPointer = std::make_shared<DartMethodsPointer>();

    std::shared_ptr<DartMethodsPointer> getDartMethod() {
        return methodPointer;
    }

    void registerDartMethods(uint64_t *methodBytes, int32_t length) {
        size_t i = 0;
        methodPointer->platformBrightness = reinterpret_cast<PlatformBrightness>(methodBytes[i++]);
    }
}



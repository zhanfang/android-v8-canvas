cmake_minimum_required(VERSION 3.4.1)

# Command info: https://cmake.org/cmake/help/v3.4/command/include_directories.html
include_directories(
    src/main/cpp
    src/main/cpp/include
    src/main/cpp/v8_inspector
)

set(
    INSPECTOR_SOURCES

    src/main/cpp/InspectorClient.cpp

    src/main/cpp/v8_inspector/src/inspector/protocol/Console.cpp
    src/main/cpp/v8_inspector/src/inspector/protocol/Debugger.cpp
    src/main/cpp/v8_inspector/src/inspector/protocol/DOM.cpp
    src/main/cpp/v8_inspector/src/inspector/protocol/HeapProfiler.cpp
    src/main/cpp/v8_inspector/src/inspector/protocol/Page.cpp
    src/main/cpp/v8_inspector/src/inspector/protocol/Profiler.cpp
    src/main/cpp/v8_inspector/src/inspector/protocol/Protocol.cpp
    src/main/cpp/v8_inspector/src/inspector/protocol/Runtime.cpp
    src/main/cpp/v8_inspector/src/inspector/protocol/Schema.cpp
    src/main/cpp/v8_inspector/src/inspector/protocol/Overlay.cpp

    src/main/cpp/v8_inspector/src/inspector/utils/base64.cpp
    src/main/cpp/v8_inspector/src/inspector/utils/v8-network-request-data.cpp
    src/main/cpp/v8_inspector/src/inspector/utils/v8-page-resources.cpp
    src/main/cpp/v8_inspector/src/inspector/utils/v8-inspector-common.cpp

    src/main/cpp/v8_inspector/src/inspector/v8-page-agent-impl.cpp
    src/main/cpp/v8_inspector/src/inspector/v8-overlay-agent-impl.cpp

    src/main/cpp/v8_inspector/src/inspector/injected-script.cc
    src/main/cpp/v8_inspector/src/inspector/inspected-context.cc
    src/main/cpp/v8_inspector/src/inspector/remote-object-id.cc
    src/main/cpp/v8_inspector/src/inspector/search-util.cc
    src/main/cpp/v8_inspector/src/inspector/string-16.cc
    src/main/cpp/v8_inspector/src/inspector/string-util.cc
    src/main/cpp/v8_inspector/src/inspector/v8-console.cc
    src/main/cpp/v8_inspector/src/inspector/v8-console-agent-impl.cc
    src/main/cpp/v8_inspector/src/inspector/v8-console-message.cc
    src/main/cpp/v8_inspector/src/inspector/v8-debugger.cc
    src/main/cpp/v8_inspector/src/inspector/v8-debugger-agent-impl.cc
    src/main/cpp/v8_inspector/src/inspector/v8-debugger-script.cc
    src/main/cpp/v8_inspector/src/inspector/v8-function-call.cc
    src/main/cpp/v8_inspector/src/inspector/v8-heap-profiler-agent-impl.cc
    src/main/cpp/v8_inspector/src/inspector/v8-injected-script-host.cc
    src/main/cpp/v8_inspector/src/inspector/v8-inspector-impl.cc
    src/main/cpp/v8_inspector/src/inspector/v8-inspector-session-impl.cc
    src/main/cpp/v8_inspector/src/inspector/v8-internal-value-type.cc
    src/main/cpp/v8_inspector/src/inspector/v8-profiler-agent-impl.cc
    src/main/cpp/v8_inspector/src/inspector/v8-regex.cc
    src/main/cpp/v8_inspector/src/inspector/v8-runtime-agent-impl.cc
    src/main/cpp/v8_inspector/src/inspector/v8-schema-agent-impl.cc
    src/main/cpp/v8_inspector/src/inspector/v8-stack-trace-impl.cc
    src/main/cpp/v8_inspector/src/inspector/v8-value-utils.cc
    src/main/cpp/v8_inspector/src/inspector/wasm-translation.cc
)

add_library(
    # Sets the name of the library. When it's built you can find it with lib prefix libNativeScript.so
    v8.engine

    # Sets the library as a shared library.
    SHARED

    # Runtime source
    src/main/cpp/Util.cpp
    src/main/cpp/ArgConverter.cpp
    src/main/cpp/AssetExtractor.cpp
    src/main/cpp/File.cpp
    src/main/cpp/JEnv.cpp
    src/main/cpp/JType.cpp
    src/main/cpp/com_inspector_runtime.cpp
    src/main/cpp/com_inspector_AssetExtractor.cpp

    # V8 inspector source files will be included
    ${INSPECTOR_SOURCES}

    src/main/cpp/V8Engine.cpp

)

set_target_properties( v8.engine PROPERTIES LINK_FLAGS -Wl,--allow-multiple-definition )

target_link_libraries( v8.engine ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libzip.a )
target_link_libraries( v8.engine ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_base.a )
target_link_libraries( v8.engine ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_init.a )
target_link_libraries( v8.engine ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_initializers.a )
target_link_libraries( v8.engine ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_libplatform.a )
target_link_libraries( v8.engine ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_libsampler.a )
target_link_libraries( v8.engine ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_libbase.a )
target_link_libraries( v8.engine ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_snapshot.a )

find_library( system-log log )
find_library( system-android android )
find_library( system-dl dl )
find_library( system-atomic atomic ) # TODO: plamen5kov: can't be found in ndk for some reasong ... look at it later (maybe deprecated in newer NDK versions)
find_library( system-z z )
find_library( log-lib log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(v8.engine ${system-log} ${system-android} ${system-dl} ${system-z} ${log-lib} )


## skia
#file(WRITE "/Users/zhanfang/Documents/code/chrome/skia/out/config/empty.cpp")
#execute_process(COMMAND
#  ninja -C "/Users/zhanfang/Documents/code/chrome/skia/out/config/" build.ninja
#  RESULT_VARIABLE ninja_result)
#if (ninja_result)
#  message(WARNING "Regeneration failed running ninja: ${ninja_result}")
#endif()
#include("/Users/zhanfang/Documents/code/chrome/skia/out/config/CMakeLists.ext")

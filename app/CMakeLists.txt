# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Command info: https://cmake.org/cmake/help/v3.4/command/include_directories.html
include_directories(
    src/main/cpp
    src/main/cpp/include
    src/main/cpp/v8_inspector

    # hack to find some libraries from the ndk
    ${ANDROID_NDK_ROOT}/sysroot/usr/include/${ARCH_INCLUDE_DIR}/
)

if ( NOT OPTIMIZED_BUILD OR OPTIMIZED_WITH_INSPECTOR_BUILD )
    # When building in Release mode we do not include the V8 inspector sources
    add_definitions(-DAPPLICATION_IN_DEBUG)

    set(
        INSPECTOR_SOURCES

        src/main/cpp/v8_inspector/src/inspector/protocol/Console.cpp
        src/main/cpp/v8_inspector/src/inspector/protocol/Debugger.cpp
        src/main/cpp/v8_inspector/src/inspector/protocol/HeapProfiler.cpp
        src/main/cpp/v8_inspector/src/inspector/protocol/Profiler.cpp
        src/main/cpp/v8_inspector/src/inspector/protocol/Protocol.cpp
        src/main/cpp/v8_inspector/src/inspector/protocol/Runtime.cpp
        src/main/cpp/v8_inspector/src/inspector/protocol/Schema.cpp

        src/main/cpp/v8_inspector/src/inspector/utils/base64.cpp
        src/main/cpp/v8_inspector/src/inspector/utils/v8-network-request-data.cpp

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
else()
    # Debug builds will include the V8 inspector sources
    set(INSPECTOR_SOURCES)
endif()

add_library(
    # Sets the name of the library. When it's built you can find it with lib prefix libNativeScript.so
    v8.engine

    # Sets the library as a shared library.
    SHARED

    # Runtime source
    src/main/cpp/V8Engine.cpp
    src/main/cpp/Util.cpp
    src/main/cpp/ArgConverter.cpp
    src/main/cpp/JEnv

    # V8 inspector source files will be included only in Release mode
    ${INSPECTOR_SOURCES}
)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( v8.engine ${log-lib} ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libzip.a )
target_link_libraries( v8.engine ${log-lib} ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_base.a )
target_link_libraries( v8.engine ${log-lib} ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_init.a )
target_link_libraries( v8.engine ${log-lib} ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_initializers.a )
target_link_libraries( v8.engine ${log-lib} ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_libplatform.a )
target_link_libraries( v8.engine ${log-lib} ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_libsampler.a )
target_link_libraries( v8.engine ${log-lib} ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_libbase.a )
target_link_libraries( v8.engine ${log-lib} ${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/libv8_snapshot.a )



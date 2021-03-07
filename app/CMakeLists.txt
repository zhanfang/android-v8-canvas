cmake_minimum_required(VERSION 3.4.1)


#--------------------------------------------------
# 设置参数
#--------------------------------------------------
set(SKIA_SO_FILE    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/skia/libs/${ANDROID_ABI}/libskia.so)

set(
    INSPECTOR_SOURCES

        src/main/cpp/inspector/InspectorClient.cpp

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

#--------------------------------------------------
# Includes
#--------------------------------------------------
include_directories(
    # skia
    third_party/skia
    third_party/skia/include
    third_party/skia/include/android
    third_party/skia/include/atlastext
    third_party/skia/include/c
    third_party/skia/include/codec
    third_party/skia/include/config
    third_party/skia/include/core
    third_party/skia/include/effects
    third_party/skia/include/pathops
    third_party/skia/include/ports
    third_party/skia/include/encode
    third_party/skia/include/gpu
    third_party/skia/include/private
    third_party/skia/include/svg
    third_party/skia/include/ports
    third_party/skia/include/third_party
    third_party/skia/include/utils

    # v8
    third_party/v8
    third_party/v8/include

    src/main/cpp
    src/main/cpp/v8_inspector
    src/main/cpp/canvas
    src/main/cpp/nan
)


#--------------------------------------------------
# Source Skia
#--------------------------------------------------
add_library(
    # Sets the name of the library.
    skia.engine
    # Sets the library as a shared library.
    SHARED
    IMPORTED
)

set_target_properties(
    skia.engine
    PROPERTIES
    IMPORTED_LOCATION
    ${SKIA_SO_FILE}
)

#--------------------------------------------------
# Source V8
#--------------------------------------------------
add_library(
    # Sets the name of the library.
    v8.engine

    # Sets the library as a shared library.
    SHARED

    # Runtime source
    src/main/cpp/Util.cpp
    src/main/cpp/AssetExtractor.cpp
    src/main/cpp/ArgConverter.cpp
    src/main/cpp/File.cpp
    src/main/cpp/JEnv.cpp
    src/main/cpp/JType.cpp
    src/main/cpp/com_inspector_runtime.cpp
    src/main/cpp/com_inspector_AssetExtractor.cpp
        src/main/cpp/console/Console.cpp
    src/main/cpp/V8GlobalHelpers.cpp

    # canvas
    src/main/cpp/canvas/CanvasEngine.cpp
    src/main/cpp/canvas/Canvas.cpp
    src/main/cpp/canvas/CanvasContext2d.cpp

    # V8 inspector source files will be included
    ${INSPECTOR_SOURCES}

    src/main/cpp/V8Engine.cpp
)

#--------------------------------------------------
# other dependencies
#--------------------------------------------------
find_library( system-log log )
find_library( system-dl dl )
find_library( system-z z )
find_library( log-lib log )
find_library( opensl OpenSLES )


#--------------------------------------------------
# link
#--------------------------------------------------

target_link_libraries(
    v8.engine

    # 顺序不能变化
    ${PROJECT_SOURCE_DIR}/third_party/v8/libs/${ANDROID_ABI}/libzip.a
    ${PROJECT_SOURCE_DIR}/third_party/v8/libs/${ANDROID_ABI}/libv8_base.a
    ${PROJECT_SOURCE_DIR}/third_party/v8/libs/${ANDROID_ABI}/libv8_init.a
    ${PROJECT_SOURCE_DIR}/third_party/v8/libs/${ANDROID_ABI}/libv8_initializers.a
    ${PROJECT_SOURCE_DIR}/third_party/v8/libs/${ANDROID_ABI}/libv8_libplatform.a
    ${PROJECT_SOURCE_DIR}/third_party/v8/libs/${ANDROID_ABI}/libv8_libsampler.a
    ${PROJECT_SOURCE_DIR}/third_party/v8/libs/${ANDROID_ABI}/libv8_libbase.a
    ${PROJECT_SOURCE_DIR}/third_party/v8/libs/${ANDROID_ABI}/libv8_snapshot.a

    ${system-log}
    ${system-dl}
    ${system-z}

    skia.engine
    -ljnigraphics
    -landroid
    ${opensl}

    ${log-lib}
)

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

add_library(
    # Sets the name of the library. When it's built you can find it with lib prefix libNativeScript.so
    v8.engine

    # Sets the library as a shared library.
    SHARED

    # Runtime source
    src/main/cpp/V8Engine.cpp
    src/main/cpp/InspectorClient.cpp
    src/main/cpp/InspectorFrontend.cpp

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



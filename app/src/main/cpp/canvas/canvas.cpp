#include "jni.h"
#include "../log/os-android.h"

#include <SkBitmap.h>


extern "C" JNIEXPORT void Java_com_example_zhanfang_test_Canvas_demo(JNIEnv* env, jobject obj) {
    LOGD("canvas demo");
}

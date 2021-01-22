#include "../log/os-android.h"

#include <jni.h>
#include <SkBitmap.h>
#include <include/core/SkFont.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/core/SkRect.h>
#include <include/core/SkColor.h>
#include <include/core/SkGraphics.h>

#include <android/bitmap.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>


extern "C" JNIEXPORT void JNICALL
Java_com_example_zhanfang_test_Canvas_render(JNIEnv* env, jclass obj, jobject jSurface, jint width, jint height) {
    LOGD("canvas render start");
     ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, jSurface);
}

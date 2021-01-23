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
    // 获取与 Surface 对应的 ANativeWindow 对象
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, jSurface);
    // 设置 buffer 的尺寸和格式
    ANativeWindow_setBuffersGeometry(nativeWindow, width, height, WINDOW_FORMAT_RGBA_8888);

    ANativeWindow_Buffer *buffer = new ANativeWindow_Buffer();

    ANativeWindow_lock(nativeWindow, buffer, 0);

    int bpr = buffer->stride * 4;
    SkBitmap bitmap;
    SkImageInfo imageInfo = SkImageInfo::MakeS32(buffer->width, buffer->height, SkAlphaType::kPremul_SkAlphaType);

    bitmap.setInfo(imageInfo, bpr);
    bitmap.setPixels(buffer->bits);

    SkCanvas *bg = new SkCanvas(bitmap);

    // 绘制矩形，并填充颜色
    SkPaint paint;
    paint.setColor(SK_ColorBLUE);
    SkRect rect;
    rect.set(SkIRect::MakeWH(width, height));
    bg->drawRect(rect, paint);

    // 绘制文字，并设置位置 & 字体大小
    SkPaint paint2;
    paint2.setColor(SK_ColorWHITE);
    const char *str = "Hello Surface Skia";
    SkFont skfont(SkTypeface::MakeDefault(),100);
    bg->drawString(str,100,100,skfont,paint2);

    LOGD("after native_window stride:%d,width:%d,height:%d,format:%d",
         buffer->stride, buffer->width, buffer->height, buffer->format);

    ANativeWindow_unlockAndPost(nativeWindow);
}

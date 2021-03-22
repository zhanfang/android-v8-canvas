//
// Created by zhanfang on 2021/3/18.
//

#ifndef ANDROID_V8_DEMO_MASTER_GLWINDOWCONTEXT_H
#define ANDROID_V8_DEMO_MASTER_GLWINDOWCONTEXT_H

#include <android/native_window.h>
#include <gpu/GrDirectContext.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

namespace Engine {
    class GLWindowContext : public SkRefCnt {
    public:
        GLWindowContext(ANativeWindow* window, int width, int height);
        sk_sp<SkSurface> getBackbufferSurface();
        void swapBuffers();
    private:
        ANativeWindow* mNativeWindow = nullptr;
        void onResize(int width, int height);
        void initContext();
        sk_sp<const GrGLInterface> initGLInterface();

        int mHeight = 0;
        int mWidth = 0;

        sk_sp<GrDirectContext> mGrContext;
        sk_sp<const GrGLInterface> mGrGLInterface;
        sk_sp<SkSurface> mSurface;

        // 实际显示设备的抽象
        EGLDisplay mEGLDisplay;
        // 存储 OpenGL ES绘图的一些状态信息
        EGLContext mEGLContext;
        // 存储图像的内存区域
        EGLSurface mEGLSurface;
        // 渲染表面的配置信息
        EGLConfig mEGLConfig;

        // parameters obtained from the native window
        // Note that the platform .cpp file is responsible for
        // initializing fSampleCount and fStencilBits!
        int mSampleCount = 1;
        int mStencilBits = 0;

    };
}



#endif //ANDROID_V8_DEMO_MASTER_GLWINDOWCONTEXT_H

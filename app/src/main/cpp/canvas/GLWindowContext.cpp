//
// Created by zhanfang on 2021/3/18.
//

#include <include/core/SkTypes.h>
#include <gpu/gl/GrGLInterface.h>
#include "GLWindowContext.h"
#include <SkSurface.h>
#include <GLES2/gl2.h>
#include <SkTypes.h>

using namespace Engine;

GLWindowContext::GLWindowContext(ANativeWindow *window, int width, int height)
    : mNativeWindow(window){
    // 设置 NativeWindow buffer
    onResize(width, height);
    ANativeWindow_acquire(window);
    // 初始化 EGL，并建立 GrContext
    initContext();
}

void GLWindowContext::onResize(int width, int height) {
    SkAssertResult(mNativeWindow);
    mHeight = height;
    mWidth = width;

    // 设置 buffer 的尺寸和格式
    int error = ANativeWindow_setBuffersGeometry(mNativeWindow, width, height, WINDOW_FORMAT_RGBA_8888);
    SkAssertResult(error == 0);
}

void GLWindowContext::initContext() {
    mGrGLInterface = initGLInterface();
    mGrContext = GrDirectContext::MakeGL(mGrGLInterface);
}

sk_sp<const GrGLInterface> GLWindowContext::initGLInterface() {
    // 1. 选择显示设备，一般都是选择默认的显示设备，也就是手机屏幕
    mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    SkASSERT(mEGLDisplay != EGL_NO_DISPLAY);

    // 2. 初始化 EGL
    EGLint majorVersion;
    EGLint minorVersion;
    SkAssertResult(eglInitialize(mEGLDisplay, &majorVersion, &minorVersion));

//    SkAssertResult(eglBindAPI(EGL_OPENGL_ES_API));

    // 3. 确定可用的 Surface 配置
    EGLint numConfigs = 0;
    const EGLint configAttribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };
    SkAssertResult(eglChooseConfig(mEGLDisplay, configAttribs, &mEGLConfig, 10, &numConfigs));
    SkASSERT(numConfigs > 0);

    // 4. 创建渲染上下文
    static const EGLint kEGLContextAttribsForOpenGLES[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    mEGLContext = eglCreateContext(
            mEGLDisplay, mEGLConfig, EGL_NO_CONTEXT/*shared_context*/, kEGLContextAttribsForOpenGLES);
    SkASSERT(EGL_NO_CONTEXT != mEGLContext);

    // 5. 创建 Surface
    mEGLSurface = eglCreateWindowSurface(mEGLDisplay, mEGLConfig, mNativeWindow, nullptr);
    SkASSERT(EGL_NO_SURFACE != mEGLSurface);

    // 6. 绑定上下文
    SkAssertResult(eglMakeCurrent(mEGLDisplay, mEGLSurface, mEGLSurface, mEGLContext));

    glClearStencil(0);
    glClearColor(255, 0, 0, 0);
    glStencilMask(0xffffffff);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    eglGetConfigAttrib(mEGLDisplay, mEGLConfig, EGL_STENCIL_SIZE, &mStencilBits);
    eglGetConfigAttrib(mEGLDisplay, mEGLConfig, EGL_SAMPLES, &mSampleCount);

    return GrGLMakeNativeInterface();
}

sk_sp<SkSurface> GLWindowContext::getBackbufferSurface() {
    SkASSERT(mGrGLInterface);
    if(!mSurface) {
        SkASSERT(mGrContext);
        GrGLint frame_buffer;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &frame_buffer);

        GrGLFramebufferInfo fbInfo;
        fbInfo.fFBOID = frame_buffer;
        fbInfo.fFormat = 0x8058; //GR_GL_RGBA8;

        GrBackendRenderTarget backendRT(mWidth,
                                        mHeight,
                                        mSampleCount,
                                        mStencilBits,
                                        fbInfo);
        mSurface = SkSurface::MakeFromBackendRenderTarget(mGrContext.get(), backendRT,
                                                          kBottomLeft_GrSurfaceOrigin,
                                                          kRGBA_8888_SkColorType,
                                                          nullptr,
                                                          nullptr);

//        // Texture. need the EGLSurface need, but just represent to EGLSurface.
//        // setup the EGLContext. need the ANativeWindow
//        SkImageInfo imageInfo = SkImageInfo::MakeN32(mWidth, mHeight, SkAlphaType::kPremul_SkAlphaType);
//        mSurface = SkSurface::MakeRenderTarget(mGrContext.get(), SkBudgeted::kYes, imageInfo, 0,
//                                               nullptr);
    }
    return mSurface;
}


void GLWindowContext::swapBuffers() {
    if(mEGLDisplay && mEGLContext && mEGLSurface) {
        eglSwapBuffers(mEGLDisplay, mEGLSurface);
    }
}

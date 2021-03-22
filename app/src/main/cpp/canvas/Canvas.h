//
// Created by zhanfang on 2021/3/3.
//
#ifndef CANVAS_H_
#define CANVAS_H_

#include <SkCanvas.h>
#include <SkSurface.h>
#include <v8.h>
#include <nan/node_object_wrap.h>
#include <android/native_window.h>
#include "GLWindowContext.h"


namespace Engine {
    class Canvas {
    public:
        Canvas(size_t width, size_t height, ANativeWindow* window);

        SkCanvas* sk_canvas() {
            return sk_surface() ? sk_surface()->getCanvas() : nullptr;
        }

        SkSurface* sk_surface() {
            return mGLWindowContext->getBackbufferSurface().get();
        }

        void flush();

    private:
        ~Canvas();
        void init(ANativeWindow *window);
        // SkCanvas is managed and owned by SkSurface, and is deleted when SkSurface is deleted.
        sk_sp<GLWindowContext> mGLWindowContext;


        SkCanvas *mCanvas;
        size_t mWidth;
        size_t mHeight;
    };
}

#endif



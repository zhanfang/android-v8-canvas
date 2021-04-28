//
// Created by zhanfang on 2021/3/3.
//

#include "Canvas.h"

using namespace Engine;

Canvas* Canvas::globalCanvas;

Canvas::Canvas(size_t width, size_t height, ANativeWindow *window)
    : mWidth(width), mHeight(height){
    init(window);
}

Canvas::~Canvas() {
    // SkCanvas is managed and owned by SkSurface, and is deleted when SkSurface is deleted.
}

void Canvas::init(ANativeWindow *window) {
    mGLWindowContext.reset(new GLWindowContext(window, mWidth, mHeight));
}

void Canvas::flush(){
    if(mGLWindowContext) {
        mGLWindowContext->swapBuffers();
    }
}

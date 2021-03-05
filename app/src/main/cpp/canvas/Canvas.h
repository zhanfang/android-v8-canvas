//
// Created by zhanfang on 2021/3/3.
//
#ifndef CANVAS_H_
#define CANVAS_H_

#include <SkCanvas.h>
#include <v8.h>

class Canvas {
    public:
        Canvas(SkCanvas *canvas);
    private:
        SkCanvas *_canvas;
};

#endif



//
// Created by zhanfang on 2021/3/3.
//
#include <v8.h>
#include <SkCanvas.h>
#include "nan/nan.h"
#include "Canvas.h"


class Context2d {
    public:
        Context2d(SkCanvas *canvas);

        static void Initialize(v8::Local<v8::Object> bindings);
        static NAN_METHOD(New);
        static NAN_METHOD(FillRect);
        static NAN_GETTER(GetFillStyle);
        static NAN_SETTER(SetFillStyle);


    private:
        ~Context2d();
        SkCanvas *_canvas;

        static SkCanvas *skCanvas;
};


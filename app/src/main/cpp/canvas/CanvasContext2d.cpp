//
// Created by zhanfang on 2021/3/3.
//
#include <SkRect.h>

#include "CanvasContext2d.h"
#include "log/os-android.h"


using namespace v8;

/*
 * Rectangle arg assertions.
 */
#define RECT_ARGS \
  double args[4]; \
  if(!checkArgs(info, args, 4)) \
    return; \
  double x = args[0]; \
  double y = args[1]; \
  double width = args[2]; \
  double height = args[3];

inline static bool checkArgs(const v8::FunctionCallbackInfo<Value> &info, double *args, int argsNum, int offset = 0){
    int argsEnd = offset + argsNum;
    bool areArgsValid = true;

    for (int i = offset; i < argsEnd; i++) {
        double val = info[i]->NumberValue();

        if (areArgsValid) {
            if (val != val ||
                val == std::numeric_limits<double>::infinity() ||
                val == -std::numeric_limits<double>::infinity()) {
                // We should continue the loop instead of returning immediately
                // See https://html.spec.whatwg.org/multipage/canvas.html

                areArgsValid = false;
                continue;
            }

            args[i - offset] = val;
        }
    }

    return areArgsValid;
}

Context2d::Context2d(SkCanvas *canvas) {
    _canvas = canvas;
}

Context2d::~Context2d() {

}

void Context2d::Initialize(v8::Local<v8::ObjectTemplate> target) {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<FunctionTemplate> ctor = FunctionTemplate::New(isolate, Context2d::New);
    ctor->InstanceTemplate()->SetInternalFieldCount(1);
    ctor->SetClassName(Nan::New("CanvasRenderingContext2D").ToLocalChecked());

    // Prototype
    Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
    proto->Set(Nan::New("fillRect").ToLocalChecked(), FunctionTemplate::New(Isolate::GetCurrent(), FillRect));
    proto->SetAccessor(Nan::New("fillStyle").ToLocalChecked(), GetFillStyle, SetFillStyle);

    Local<Context> ctx = isolate->GetCurrentContext();
    target->Set(Nan::New("CanvasRenderingContext2D").ToLocalChecked(), ctor);
}

NAN_METHOD(Context2d::New) {
//    v8::String info[0]
}

NAN_METHOD(Context2d::FillRect) {
    RECT_ARGS;
    if (0 == width || 0 == height) return;
    SkRect r = SkRect::MakeXYWH(x, y, width, height);
//    SkCanvas *canvas = info.This();
//    _canvas->drawRect(r, m_fillPaint );
}

NAN_GETTER(Context2d::GetFillStyle) {

}

NAN_SETTER(Context2d::SetFillStyle) {

}

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

void Context2d::Initialize(v8::Local<v8::Object> bindings) {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<FunctionTemplate> ctor = FunctionTemplate::New(isolate, Context2d::New);
    ctor->InstanceTemplate()->SetInternalFieldCount(1);
    ctor->SetClassName(Nan::New("CanvasRenderingContext2D").ToLocalChecked());

    // Prototype
    Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
    proto->Set(Nan::New("fillRect").ToLocalChecked(), FunctionTemplate::New(Isolate::GetCurrent(), FillRect));
    proto->SetAccessor(Nan::New("fillStyle").ToLocalChecked(), GetFillStyle, SetFillStyle);

    bindings->Set(Nan::New("Context2D").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(Context2d::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Class constructors cannot be invoked without 'new'");
    }

    if (!info[0]->IsString())
        return Nan::ThrowTypeError("args should be string");

}

NAN_METHOD(Context2d::FillRect) {
    RECT_ARGS;
    if (0 == width || 0 == height) return;
    SkRect r = SkRect::MakeXYWH(x, y, width, height);
//    SkCanvas *canvas = info.This();
//    _canvas->drawRect(r, m_fillPaint );
}

NAN_GETTER(Context2d::GetFillStyle) {
    LOGD("343333");
}

NAN_SETTER(Context2d::SetFillStyle) {
    if (value->IsString()) {
        LOGD("1234");
    } else if (value->IsObject()) {

    }
}

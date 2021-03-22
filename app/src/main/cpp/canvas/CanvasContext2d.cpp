//
// Created by zhanfang on 2021/3/3.
//
#include <SkRect.h>

#include "CanvasContext2d.h"
#include "log/os-android.h"
#include "jni/V8Engine.h"
#include "GraphicsTypes.h"
#include "ArgConverter.h"

using namespace v8;
using namespace tns;
using namespace Engine;

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

Context2d::Context2d(SkCanvas* canvas) {
    m_skCanvas = canvas;
    // 设置样式
    m_fillPaint.setStyle(SkPaint::kFill_Style);
    // 设置抗齿距
    m_fillPaint.setAntiAlias(true);
    m_strokePaint.setStyle(SkPaint::kStroke_Style);
    m_strokePaint.setAntiAlias(true);
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
    proto->Set(Nan::New("strokeRect").ToLocalChecked(), FunctionTemplate::New(Isolate::GetCurrent(), StrokeRect));
    proto->Set(Nan::New("clearRect").ToLocalChecked(), FunctionTemplate::New(Isolate::GetCurrent(), ClearRect));
    proto->SetAccessor(Nan::New("fillStyle").ToLocalChecked(), GetFillStyle, SetFillStyle);

    bindings->Set(Nan::New("Context2D").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(Context2d::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Class constructors cannot be invoked without 'new'");
    }

    if (!info[0]->IsString())
        return Nan::ThrowTypeError("args should be string");

    SkCanvas* skCanvas = canvas->sk_canvas();

    Context2d *context = new Context2d(skCanvas);
    context->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Context2d::FillRect) {
    RECT_ARGS;
    if (0 == width || 0 == height) return;
    Context2d *context = node::ObjectWrap::Unwrap<Context2d>(info.This());
    SkRect r = SkRect::MakeXYWH(x, y, width, height);

    context->m_skCanvas->drawRect(r, context->m_fillPaint);
    context->m_skCanvas->flush();
}

NAN_METHOD(Context2d::StrokeRect) {
    RECT_ARGS;
    if (0 == width || 0 == height) return;
    Context2d *context = node::ObjectWrap::Unwrap<Context2d>(info.This());
    SkRect r = SkRect::MakeXYWH(x, y, width, height);

    context->m_skCanvas->drawRect(r, context->m_strokePaint);
}

NAN_METHOD(Context2d::ClearRect) {
    RECT_ARGS;
    if (0 == width || 0 == height) return;
    SkRect r = SkRect::MakeXYWH(x, y, width, height);

    Context2d *context = node::ObjectWrap::Unwrap<Context2d>(info.This());
    SkPaint paint(context->m_fillPaint);
    paint.setBlendMode(SkBlendMode::kClear);
    context->m_skCanvas->drawRect(r, paint);
}

NAN_GETTER(Context2d::GetFillStyle) {
    Context2d *context = node::ObjectWrap::Unwrap<Context2d>(info.This());

    info.GetReturnValue().Set(context->m_style);
}

NAN_SETTER(Context2d::SetFillStyle) {
    Context2d *context = node::ObjectWrap::Unwrap<Context2d>(info.This());
    if (value->IsString()) {
        Local<String> str = value->ToString();
        SkColor color = Engine::to_color(ArgConverter::ConvertToString(str));
        context->m_fillPaint.setColor(color);
    } else if (value->IsObject()) {


    }

    context->m_style = value;
}

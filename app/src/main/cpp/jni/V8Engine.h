//
// Created by Zhan,Fang on 2018/9/13.
//

#include <jni.h>
#include <v8.h>
#include "libplatform/libplatform.h"
#include <android/native_window.h>
#include <include/core/SkCanvas.h>

extern v8::Platform* platform_;
extern v8::Persistent<v8::Context> mPersistentContext;
extern ANativeWindow *nativeWindow;

extern SkCanvas *skCanvas;
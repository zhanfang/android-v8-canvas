package com.example.v8engine;

import android.view.Surface;

public class V8 {

    /**
     * 创建 c++ 的 V8Engine
     * @return nativePtr
     */
    public static native long NewV8Engine();

    public static native void initV8(long nativeV8Engine, long threadId);

    public static native void onSurfaceCreate(Surface jSurface, int width, int height);

    public static native void connect(Object connection);

    public static native void waitForFrontend();

    public static native void scheduleBreak();

    public static native void disconnect();

    public static native void dispatchMessage(String message);

    public static native void require(long nativeV8Engine, String filePath);

    public static native String runScript(long nativeV8Engine, String script);

}

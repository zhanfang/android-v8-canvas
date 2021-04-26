package com.example.v8engine;

import android.view.Surface;

public class V8 {
    static {
        System.loadLibrary("v8.engine");
    }

    public static native void initV8();

    public static native void onSurfaceCreate(Surface jSurface, int width, int height);

    public static native void connect(Object connection);

    public static native void waitForFrontend();

    public static native void scheduleBreak();

    public static native void disconnect();

    public static native void dispatchMessage(String message);

    public static native void require(String filePath);

}

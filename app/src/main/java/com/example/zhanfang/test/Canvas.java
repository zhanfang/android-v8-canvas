package com.example.zhanfang.test;

import android.view.Surface;

public class Canvas {
    public static native void init(Surface surface, int width, int height);
    public static native void initGl(Surface surface, int width, int height);
}

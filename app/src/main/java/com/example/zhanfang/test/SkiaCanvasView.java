package com.example.zhanfang.test;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.example.v8engine.V8;
import com.example.v8engine.V8EngineCache;

/**
 * TODO: document your custom view class.
 */
public class SkiaCanvasView extends SurfaceView implements SurfaceHolder.Callback {
    private SurfaceHolder mHolder;
    private boolean mIsDrawing = false;

    public SkiaCanvasView(Context context) {
        super(context);
        init();
    }

    public SkiaCanvasView(Context context, AttributeSet attrs) {
        super(context, attrs);
        Log.d("skia", "inited");
        init();
    }

    public SkiaCanvasView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init();
    }

    private void init() {
        mHolder = getHolder();
        mHolder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d("skia", "surfaceCreated");
        V8EngineCache.getInstance().get("v8_engine").runOnJSThread(new Runnable() {
            @Override
            public void run() {
                V8.onSurfaceCreate(mHolder.getSurface(), getWidth(), getHeight());
            }
        });
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d("skia", "surfaceChanged");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d("skia", "surfaceDestroyed");
        mIsDrawing = false;
    }

}
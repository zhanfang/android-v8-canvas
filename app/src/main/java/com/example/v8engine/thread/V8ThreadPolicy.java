package com.example.v8engine.thread;

import android.os.Handler;
import android.os.Looper;

import com.example.v8engine.V8Engine;

public class V8ThreadPolicy implements IV8ThreadPolicy {

    private V8Engine engine;
    private Thread jsThread;
    private Handler handler;

    /**
     * 构造函数
     * @param engine
     */
    public V8ThreadPolicy(V8Engine engine) {
        this.engine = engine;
    }

    @Override
    public void startV8Thread() {
        if (jsThread == null) {
            jsThread = new Thread(new V8EngineRunnable());
            jsThread.start();
        }
    }

    @Override
    public void runDelegateRunnable(Runnable runnable) {
        if (handler == null) {
            return;
        }
        handler.post(runnable);
    }

    @Override
    public void runDelegateRunnable(Runnable runnable, long delay) {
        if (handler == null) {
            return;
        }
        handler.postDelayed(runnable, delay);
    }

    @Override
    public Thread getThread() {
        if (handler != null) {
            return handler.getLooper().getThread();
        }
        return null;
    }

    /**
     * This Runnable is the main loop of a separate Thread where v8 code is executed.
     */
    class V8EngineRunnable implements Runnable {

        @Override
        public void run() {
            Looper.prepare();
            handler = new Handler();
            engine.startEngineInternal();
            Looper.loop();
        }
    }
}

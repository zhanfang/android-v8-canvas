package com.example.v8engine;

import android.util.Log;
import android.webkit.ValueCallback;

import com.example.v8engine.thread.V8ThreadPolicy;

public class V8Engine implements IJSRuntime {

    private V8 v8;
    private String TAG = "JavaV8";
    private V8ThreadPolicy v8ThreadPolicy;

    public static void loadLib() {
        // 加载 so 库文件
        System.loadLibrary("v8.engine");
    }

    /**
     * 构造函数
     */
    public V8Engine() {
        v8ThreadPolicy = new V8ThreadPolicy(this);
    }

    /**
     * 启动 v8 线程
     */
    public synchronized void startEngine() {
        if (v8ThreadPolicy != null) {
            v8ThreadPolicy.startV8Thread();
        }
    }

    /**
     * 启动引擎，执行 JS Bundle 文件
     */
    public void startEngineInternal() {
        Log.d(TAG, "v8 thread start in " + Thread.currentThread().getId());
        v8 = V8.createRuntime();
        registerJavaMethod(new JavaCallback() {
            @Override
            public void invoke(String receiver, int parameters) {
                Log.d(TAG, receiver);
            }
        }, "call_to_java");
    }

    public void requireJSFile(final String filePath) {
        runOnJSThread(new Runnable() {
            @Override
            public void run() {
                v8.require(filePath);
            }
        });
    }

    @Override
    public void runOnJSThread(Runnable runnable) {
        if(v8ThreadPolicy != null) {
            v8ThreadPolicy.runDelegateRunnable(runnable);
        }
    }

    @Override
    public void postOnJSThread(Runnable runnable) {
        if(v8ThreadPolicy != null) {
            v8ThreadPolicy.runDelegateRunnable(runnable);
        }
    }

    @Override
    public void postOnJSThread(Runnable runnable, long delayMillis) {
        if(v8ThreadPolicy != null) {
            v8ThreadPolicy.runDelegateRunnable(runnable, delayMillis);
        }
    }

    /**
     * 在 js 线程中执行 js，执行完毕调用回调
     * @param js
     * @param resultCb
     */
    public void evalJavascript(final String js, final ValueCallback<String> resultCb) {
        runOnJSThread(new Runnable() {
            @Override
            public void run() {
                evalJavascriptImpl(js, resultCb);
            }
        });
    }

    /**
     * eval js 实现
     * @param js
     * @param resultCb
     */
    private void evalJavascriptImpl(final String js, final ValueCallback<String> resultCb) {
        String res = v8.runScript(js);
        if (resultCb != null) {
            resultCb.onReceiveValue(res);
        }
    }

    /**
     * Register a Java method as a JavaScript function. When the JS Function is invoked
     * the Java method will be called.
     *
     * @param callback The JavaCallback to call when the JSFunction is invoked.
     * @param jsFunctionName The name of the JSFunction.
     *
     */
    public void registerJavaMethod(final JavaCallback callback, final String jsFunctionName) {
        v8.registerJavaMethod(callback, jsFunctionName);
    }
}

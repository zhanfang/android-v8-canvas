package com.example.square;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.example.v8engine.JavaCallback;
import com.example.v8engine.V8Array;
import com.example.v8engine.V8Engine;
import com.example.v8engine.V8EngineCache;
import com.example.v8engine.V8Object;

import org.json.JSONException;
import org.json.JSONObject;

import io.flutter.embedding.engine.FlutterEngine;
import io.flutter.plugin.common.MethodChannel;

/**
 * 用于建立 js 与 flutter 关联的 engine
 */
public class SquareEngine {

    private String TAG = "SquareEngine";

    public V8Engine v8Engine;
    private MethodChannel methodChannel;
    private Handler mainHandler;
    private String CHANNEL_TYPE = "square";

    public SquareEngine(FlutterEngine flutterEngine) {
        v8Engine = V8EngineCache.getInstance().get("v8_engine");
        v8Engine.startEngine();
        methodChannel = new MethodChannel(flutterEngine.getDartExecutor().getBinaryMessenger(), CHANNEL_TYPE);

        this.mainHandler = new Handler(Looper.getMainLooper());
    }

    public void start(String filePath) {
        registerMethodCall();
        v8Engine.requireJSFile(filePath);
    }

    /**
     * 注册 js 与 java 与 flutter 的通信调用方法
     */
    private void registerMethodCall() {
        v8Engine.registerJavaMethod(new JavaCallback() {
            @Override
            public Object invoke(V8Object receiver, V8Array parameters) {
                Log.d(TAG, "js call native: " + parameters.get(0).toString());
                return null;
            }
        }, "call_to_java");

        v8Engine.registerJavaMethod(new JavaCallback() {
            @Override
            public Object invoke(V8Object receiver, V8Array parameters) {
                final String paramStr = parameters.get(0).toString();
                Log.d(TAG, "js call flutter: " + paramStr);
                mainHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        methodChannel.invokeMethod("methodChannel_js_call_flutter", paramStr);
                    }
                });
                return null;
            }
        }, "call_to_flutter");
    }


}

package com.example.zhanfang.test;

import android.app.Application;
import android.content.Context;

import com.example.v8engine.V8Engine;
import com.example.v8engine.V8EngineCache;

import java.io.File;

import io.flutter.embedding.engine.FlutterEngine;
import io.flutter.embedding.engine.FlutterEngineCache;
import io.flutter.embedding.engine.dart.DartExecutor;

public class MainApplication extends Application {

    private static MainApplication instance = null;

    public MainApplication() {
        super();
        instance = this;
    }

    public void onCreate() {
        super.onCreate();

        // 加载 so 库文件
        System.loadLibrary("v8.engine");

        createJSDir();

        V8Engine v8Engine = new V8Engine();
        V8EngineCache.getInstance().put("v8_engine", v8Engine);

        FlutterEngine flutterEngine = new FlutterEngine(this);

        // Start executing Dart code to pre-warm the FlutterEngine.
        flutterEngine.getDartExecutor().executeDartEntrypoint(
                DartExecutor.DartEntrypoint.createDefault()
        );

        // Cache the FlutterEngine to be used by FlutterActivity.
        FlutterEngineCache
                .getInstance()
                .put("flutter_engine", flutterEngine);
    }

    public static MainApplication getInstance() {
        return instance;
    }

    public static Context getContext() {
        return instance.getApplicationContext();
    }

    /**
     * 移动前端源文件
     */
    private void createJSDir() {
        DefaultExtractPolicy extractPolicy = new DefaultExtractPolicy();
        AssetExtractor aE = new AssetExtractor(null);
        String outputDir = getFilesDir().getPath() + File.separator;

        boolean removePreviouslyInstalledAssets = true;
        aE.extractAssets(this, "app", outputDir, extractPolicy, removePreviouslyInstalledAssets);
        extractPolicy.setAssetsThumb(this);
    }
}

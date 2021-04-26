package com.example.zhanfang.test;

import android.app.Activity;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import io.flutter.Log;
import io.flutter.embedding.android.ExclusiveAppComponent;
import io.flutter.embedding.android.FlutterView;
import io.flutter.embedding.engine.FlutterEngine;
import io.flutter.embedding.engine.FlutterEngineCache;
import io.flutter.embedding.engine.dart.DartExecutor;
import io.flutter.plugin.platform.PlatformPlugin;

public class MyFlutterActivity extends AppCompatActivity implements ExclusiveAppComponent {

    private String TAG = "MyFlutterActivity";
    private FlutterEngine engine;
    private PlatformPlugin platformPlugin;
    private FlutterView flutterView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        engine = FlutterEngineCache.getInstance().get("flutter_engine");

        if (engine == null) {
            engine = new FlutterEngine(this);
            engine.getDartExecutor().executeDartEntrypoint(DartExecutor.DartEntrypoint.createDefault());
        }

        engine.getActivityControlSurface().attachToActivity((ExclusiveAppComponent<Activity>) this, this.getLifecycle());

        flutterView = new FlutterView(this);

        flutterView.attachToFlutterEngine(engine);

        platformPlugin = new PlatformPlugin(this, engine.getPlatformChannel());

        setContentView(flutterView);
    }

    @Override
    protected void onResume() {
        super.onResume();
        engine.getLifecycleChannel().appIsResumed();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "Activity destroy");
        release();
    }


    @Override
    public void detachFromFlutterEngine() {
        Log.d(TAG, "FlutterActivity " + this + " connection to the engine " + engine + " evicted by another attaching activity");
        release();
    }

    @NonNull
    @Override
    public Object getAppComponent() {
        return this;
    }

    private void release() {
        // Release Flutter's control of UI such as system chrome.
        platformPlugin.destroy();
        // Detach rendering pipeline.
        flutterView.detachFromFlutterEngine();
        // Plugins are no longer attached to an activity.
        engine.getActivityControlSurface().detachFromActivity();
        // Set Flutter's application state to detached.
        engine.getLifecycleChannel().appIsDetached();

        platformPlugin = null;
        flutterView = null;
    }
}

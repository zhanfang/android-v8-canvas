package com.example.zhanfang.test;

import androidx.activity.ComponentActivity;
import androidx.lifecycle.LifecycleObserver;

import io.flutter.embedding.android.ExclusiveAppComponent;
import io.flutter.embedding.android.FlutterView;
import io.flutter.embedding.engine.FlutterEngine;
import io.flutter.plugin.platform.PlatformPlugin;

/**
 *  This is an application-specific wrapper class that exists to expose the intersection of an
 *  application's active activity and an application's visible view to a [FlutterEngine] for
 *  rendering.
 *
 *  Omitted features from the [io.flutter.embedding.android.FlutterActivity] include:
 *   * **State restoration**. If you're integrating at the view level, you should handle activity
 *      state restoration yourself.
 *   * **Engine creations**. At this level of granularity, you must make an engine and attach.
 *      and all engine features like initial route etc must be configured on the engine yourself.
 *   * **Splash screens**. You must implement it yourself. Read from
 *     `addOnFirstFrameRenderedListener` as needed.
 *   * **Transparency, surface/texture**. These are just [FlutterView] level APIs. Set them on the
 *      [FlutterView] directly.
 *   * **Intents**. This doesn't do any translation of intents into actions in the [FlutterEngine].
 *      you must do them yourself.
 *   * **Back buttons**. You must decide whether to send it to Flutter via
 *      [FlutterEngine.getNavigationChannel.popRoute()], or consume it natively. Though that
 *      decision may be difficult due to https://github.com/flutter/flutter/issues/67011.
 *   * **Low memory signals**. You're strongly encouraged to pass the low memory signals (such
 *      as from the host `Activity`'s `onTrimMemory` callbacks) to the [FlutterEngine] to let
 *      Flutter and the Dart VM cull its own memory usage.
 *
 * Your own [FlutterView] integrating application may need a similar wrapper but you must decide on
 * what the appropriate intersection between the [FlutterView], the [FlutterEngine] and your
 * `Activity` should be for your own application.
 */
public class FlutterViewEngine implements LifecycleObserver {
    private FlutterEngine engine;
    private ComponentActivity activity;
    private FlutterView flutterView;
    private PlatformPlugin platformPlugin;

    FlutterViewEngine(FlutterEngine engine) {
        this.engine = engine;
    }

    /**
     * This is the intersection of an available activity and of a visible [FlutterView]. This is
     * where Flutter would start rendering.
     */
    private void hookActivityAndView() {
//        platformPlugin = new PlatformPlugin(activity, engine.getPlatformChannel());
//        // 关联 activity 控制 flutter 执行 dart 代码及绘制的句柄
//        engine.getActivityControlSurface().attachToActivity(activity, activity.getLifecycle());
    }


    /**
     * Signal that a host `Activity` is now ready. If there is no [FlutterView] instance currently
     * attached to the view hierarchy and visible, Flutter is not yet rendering.
     *
     * You can also choose at this point whether to notify the plugins that an `Activity` is
     * attached or not. You can also choose at this point whether to connect a Flutter
     * [PlatformPlugin] at this point which allows your Dart program to trigger things like
     * haptic feedback and read the clipboard. This sample arbitrarily chooses no for both.
     */
    public void attachToActivity(ComponentActivity activity) {
        this.activity = activity;
        if (flutterView != null) {
            hookActivityAndView();
        }
    }

    /**
     * Signal that a [FlutterView] instance is created and attached to a visible Android view
     * hierarchy.
     *
     * If an `Activity` was also previously provided, this puts Flutter into the rendering state
     * for this [FlutterView]. This also connects this wrapper class to listen to the `Activity`'s
     * lifecycle to pause rendering when the activity is put into the background while the
     * view is still attached to the view hierarchy.
     */
    public void attachFlutterView(FlutterView flutterView) {
        this.flutterView = flutterView;
        if (activity != null) {
            hookActivityAndView();
        }
    }
}

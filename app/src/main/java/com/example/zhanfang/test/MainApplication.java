package com.example.zhanfang.test;

import android.app.Application;
import android.content.Context;

import com.example.v8engine.V8;

public class MainApplication extends Application {

    private static MainApplication instance = null;

    public MainApplication() {
        super();
        instance = this;
    }

    public void onCreate() {
        super.onCreate();
        instance = this;
        V8.initV8();
    }

    public static MainApplication getInstance() {
        return instance;
    }

    public static Context getContext() {
        return instance.getApplicationContext();
    }

}

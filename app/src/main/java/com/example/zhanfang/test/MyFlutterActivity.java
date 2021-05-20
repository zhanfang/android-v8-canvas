package com.example.zhanfang.test;

import android.os.Bundle;
import android.os.Handler;

import androidx.annotation.Nullable;

import com.example.square.SquareActivity;
import com.example.square.SquareEngine;

public class MyFlutterActivity extends SquareActivity {

    private String TAG = "MyFlutterActivity";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final SquareEngine squareEngine = new SquareEngine(engine);
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                squareEngine.start("/data/data/com.example.zhanfang.test/files/app/app.js");
            }
        }, 3000);

    }
}

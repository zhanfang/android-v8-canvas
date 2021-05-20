package com.example.zhanfang.test;

import android.app.Application;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;

import com.example.v8engine.V8Engine;
import com.example.v8engine.V8EngineCache;
import com.example.v8engine.V8Inspector;

import androidx.appcompat.app.AppCompatActivity;

import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.webkit.ValueCallback;
import android.widget.Button;

import java.io.IOException;


public class MainActivity extends AppCompatActivity {
    private V8Engine v8Engine;
    private String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        v8Engine = V8EngineCache.getInstance().get("v8_engine");

//        v8Engine.startEngine();

        // 测试 v8 功能
        Button button = (Button) findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                v8Engine.requireJSFile("/data/data/com.example.zhanfang.test/files/app/test.js");
            }
        });

//        final Application app = MainApplication.getInstance();
//        // v8 inspector 相关
//        final Handler handler = new Handler();
//        new Thread(new Runnable() {
//            @Override
//            public void run() {
//                try {
//                    V8Inspector v8Inspector = new V8Inspector(
//                            app.getFilesDir().getAbsolutePath(),
//                            app.getPackageName(),
//                            handler);
//                    v8Inspector.start();
//
//                    v8Inspector.waitForDebugger(false);
//
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
//            }
//        }).start();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public void btnClick(View v) {
        Intent intent = new Intent(this, MyFlutterActivity.class);
        startActivity(intent);
    }

    public void callToJavaClick(View v) {
        v8Engine.evalJavascript("var a = {a: 1}; call_to_java(a)", new ValueCallback<String>() {
            @Override
            public void onReceiveValue(String value) {
                Log.d(TAG, "success:" + value);
            }
        });
    }
}

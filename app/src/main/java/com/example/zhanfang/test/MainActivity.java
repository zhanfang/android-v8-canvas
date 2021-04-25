package com.example.zhanfang.test;

import android.app.Application;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.view.Surface;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;

import java.io.File;
import java.io.IOException;

import io.flutter.embedding.android.FlutterActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 移动前端源文件
        final Application app = MainApplication.getInstance();

        DefaultExtractPolicy extractPolicy = new DefaultExtractPolicy();
        AssetExtractor aE = new AssetExtractor(null);
        String outputDir = app.getFilesDir().getPath() + File.separator;

        boolean removePreviouslyInstalledAssets = true;
        aE.extractAssets(app, "app", outputDir, extractPolicy, removePreviouslyInstalledAssets);
        extractPolicy.setAssetsThumb(app);

        // 测试 v8 功能
        Button button = (Button) findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                V8.require("/data/data/com.example.zhanfang.test/files/app/test.js");
            }
        });

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);

        // v8 inspector 相关
        final Handler handler = new Handler();
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    V8Inspector v8Inspector = new V8Inspector(
                            app.getFilesDir().getAbsolutePath(),
                            app.getPackageName(),
                            handler);
                    v8Inspector.start();

                    v8Inspector.waitForDebugger(false);

                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();
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
        startActivity(
                FlutterActivity.createDefaultIntent(this)
        );
    }
}

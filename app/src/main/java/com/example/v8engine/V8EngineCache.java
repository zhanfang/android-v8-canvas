package com.example.v8engine;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.HashMap;
import java.util.Map;

public class V8EngineCache {
    private static V8EngineCache instance;

    private final Map<String, V8Engine> cachedEngine = new HashMap<>();

    @NonNull
    public static V8EngineCache getInstance() {
        if (instance == null) {
            instance = new V8EngineCache();
        }
        return instance;
    }

    public void put(@NonNull String engineId, @Nullable V8Engine engine) {
        if(engine != null) {
            cachedEngine.put(engineId, engine);
        }
        else {
            cachedEngine.remove(engineId);
        }
    }

    public V8Engine get(@NonNull String engineId) {
        return cachedEngine.get(engineId);
    }
}

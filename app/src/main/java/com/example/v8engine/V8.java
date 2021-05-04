package com.example.v8engine;

import android.view.Surface;

import java.util.LinkedList;

public class V8 extends V8Object {

    private static int runtimeCounter = 0;
    private boolean released = false;
    private long v8EnginePtr;
    private V8Locker locker;
    private long                         objectReference         = 0;
    private LinkedList<ReferenceHandler> referenceHandlers       = new LinkedList<ReferenceHandler>();

    protected V8(V8 v8) {
        super(v8);
    }

    protected V8(final String globalAlias) {
        super(null);
        v8EnginePtr = NewV8Engine();
        createIsolate(v8EnginePtr, globalAlias);
        locker = new V8Locker(this);
    }

    public static V8 createRuntime() {
        return createRuntime(null);
    }

    public static V8 createRuntime(final String globalAlias) {
        V8 runtime = new V8(globalAlias);
        runtimeCounter++;
        return runtime;
    }

    public void require(String filePath) {
        _require(v8EnginePtr, filePath);
    }

    public String runScript(String script) {
        return  _runScript(v8EnginePtr, script);
    }

    public void registerCallback(final Object object, final String jsFunctionName, final boolean includeReceiver) {
        _registerJavaMethod(v8EnginePtr, jsFunctionName, false);
    }

    long getV8EnginePtr() {
        return v8EnginePtr;
    }

    void checkThread() {
        locker.checkThread();
        if (isReleased()) {
            throw new Error("Runtime disposed error");
        }
    }

    void addObjRef(final V8Value reference) {
        objectReference++;
        if (!referenceHandlers.isEmpty()) {
            notifyReferenceCreated(reference);
        }
    }

    void releaseObjRef(final V8Value reference) {
        if (!referenceHandlers.isEmpty()) {
            notifyReferenceDisposed(reference);
        }
        objectReference--;
    }

    private void notifyReferenceCreated(final V8Value object) {
        for (ReferenceHandler referenceHandler : referenceHandlers) {
            referenceHandler.v8HandleCreated(object);
        }
    }

    private void notifyReferenceDisposed(final V8Value object) {
        for (ReferenceHandler referenceHandler : referenceHandlers) {
            referenceHandler.v8HandleDisposed(object);
        }
    }

    /**
     * 创建 c++ 的 V8Engine
     * @return nativePtr
     */
    private native long NewV8Engine();

    private native void createIsolate(final long nativeV8Engine, String globalAlias);

    private native void _require(final long nativeV8Engine, String filePath);

    private native String _runScript(final long nativeV8Engine, String script);

    private native long _registerJavaMethod(final long nativeV8Engine, final String functionName, final boolean voidMethod);

    public native long initNewV8Object(final long nativeV8Engine);

    protected native int getType(final long nativeV8Engine, final long objectHandle);

    protected native void acquireLock(final long nativeV8Engine);
    protected native void releaseLock(final long nativeV8Engine);

    protected native void release(final long nativeV8Engine, final long objectHandle);

    public static native void onSurfaceCreate(Surface jSurface, int width, int height);
}

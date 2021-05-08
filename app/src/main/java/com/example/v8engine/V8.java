package com.example.v8engine;

import android.view.Surface;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

/**
 * An isolated V8Runtime. All JavaScript execution must exist
 * on a single runtime, and data is not shared between runtimes.
 * A runtime must be created and released when finished.
 *
 * All access to a runtime must come from the same thread, unless
 * the thread explicitly gives up control using the V8Locker.
 *
 * A public static factory method can be used to create the runtime.
 *
 * V8 runtime = V8.createV8Runtime();
 *
 */
public class V8 extends V8Object {

    private static int runtimeCounter = 0;
    private long v8EnginePtr;
    private V8Locker locker;
    private long                         objectReference         = 0;
    private LinkedList<ReferenceHandler> referenceHandlers       = new LinkedList<ReferenceHandler>();

    private Map<Long, MethodDescriptor> functionRegistry = new HashMap<Long, MethodDescriptor>();

    private class MethodDescriptor {
        JavaCallback callback;
    }

    protected V8(V8 v8) {
        super(v8);
    }

    protected V8(final String globalAlias) {
        super(null);
        released = false;
        v8EnginePtr = _createIsolate(v8EnginePtr, globalAlias);
        locker = new V8Locker(this);
        checkThread();
        objectHandle = _getGlobalObject(v8EnginePtr);
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

    protected long initNewV8Object(final long v8RuntimePtr) {
        return  _initNewV8Object(v8RuntimePtr);
    }

    protected String toString(final long v8RuntimePtr, final long objectHandle) {
        return _toString(v8RuntimePtr, objectHandle);
    }

    protected Object arrayGet(final long v8RuntimePtr, final int expectedType, final long arrayHandle, final int index) {
        return _arrayGet(v8RuntimePtr, expectedType, arrayHandle, index);
    }

    // ---------------------- register and call java method STRAT--------------------------

    public void registerCallback(final JavaCallback callback, final long objectHandle, final String jsFunctionName) {
        long methodId = _registerJavaMethod(v8EnginePtr, objectHandle, jsFunctionName, false);
        createAndRegisterMethodDescriptor(callback, methodId);
    }

    public void registerCallback(final Object object, final String jsFunctionName, final boolean includeReceiver) {
        // _registerJavaMethod(v8EnginePtr, jsFunctionName, false);
    }

    void createAndRegisterMethodDescriptor(final JavaCallback callback, final long methodID) {
        MethodDescriptor methodDescriptor = new MethodDescriptor();
        methodDescriptor.callback = callback;
        functionRegistry.put(methodID, methodDescriptor);
    }

    protected Object callObjectJavaMethod(final long methodId, final V8Object receiver, final V8Array params) {
        MethodDescriptor methodDescriptor = functionRegistry.get(methodId);
        if (methodDescriptor.callback != null) {
            return methodDescriptor.callback.invoke(receiver, params);
        }
        return null;
    }

    // ---------------------- register and call java method END--------------------------

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

    // ---------------------------- native method -----------------------------------

    private native long _createIsolate(final long v8RuntimePtr, String globalAlias);

    private native long _getGlobalObject(final long v8RuntimePtr);

    private native void _require(final long v8RuntimePtr, String filePath);

    private native String _runScript(final long v8RuntimePtr, String script);

    private native long _registerJavaMethod(final long v8RuntimePtr, final long objectHandle, final String functionName, final boolean voidMethod);

    private native long _initNewV8Object(final long v8RuntimePtr);

    private native String _toString(final long v8RuntimePtr, final long objectHandle);

    private native Object _arrayGet(long v8RuntimePtr, int expectedType, long arrayHandle, int index);

    protected native int getType(final long v8RuntimePtr, final long objectHandle);

    protected native void acquireLock(final long v8RuntimePtr);
    protected native void releaseLock(final long v8RuntimePtr);

    protected native void release(final long v8RuntimePtr, final long objectHandle);

    public static native void onSurfaceCreate(Surface jSurface, int width, int height);
}

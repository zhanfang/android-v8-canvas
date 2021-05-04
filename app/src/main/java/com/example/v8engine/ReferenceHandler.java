package com.example.v8engine;

/**
 * Callback used to track when native handles are created and released.
 */
public interface ReferenceHandler {
    /**
     * Called when a native handle is first created. The V8Value
     * referenced by that handle is passed as a parameter.
     *
     * @param object The V8Value referenced by the handle
     */
    public void v8HandleCreated(V8Value object);

    /**
     * Called when a native handle is released. The V8Value
     * referenced by that handle is passed as a parameter.
     *
     * @param object The V8Value referenced by the handle
     */
    public void v8HandleDisposed(V8Value object);
}

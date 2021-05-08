package com.example.v8engine;

/**
 * A V8Value that represents a JavaScript array.
 * JavaScript Arrays contain elements by index, but
 * can also contain elements by 'key' which is why V8Array
 * extends V8Object.
 */
public class V8Array extends V8Object {

    public V8Array(V8 v8) {
        super(v8);
        v8.checkThread();
    }

    protected V8Array(final V8 v8, final Object data) {
        super(v8, data);
    }

    /**
     * Gets the value at a given index as a Java Object. Primitives are boxed.
     *
     * @param index The index to get the value at.
     *
     * @return The value at the given index.
     */
    public Object get(final int index) {
        v8.checkThread();
        checkReleased();
        return v8.arrayGet(v8.getV8EnginePtr(), V8_OBJECT, objectHandle, index);
    }
}

package com.example.v8engine;

/**
 * A V8Value that represents a JavaScript array.
 * JavaScript Arrays contain elements by index, but
 * can also contain elements by 'key' which is why V8Array
 * extends V8Object.
 */
public class V8Array extends V8Object {
    protected V8Array(V8 v8) {
        super(v8);
    }
}

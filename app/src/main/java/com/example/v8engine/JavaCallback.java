package com.example.v8engine;

/**
 * Classes that implement this interface provide a method
 * which can be invoked from JavaScript. The method can return
 * a result.
 *
 * After creating an instance of a class that implements this
 * interface it can be registered as a Callback on a V8Object.
 */
public interface JavaCallback {
    /**
     * Called when a JS Function invokes a the registered Java
     * method.
     *
     * @param receiver The V8Object that the function was called on.
     * @param parameters The parameters passed to the JS Function. The
     * parameter array does not need to be released, by any objects accessed
     * from the array must be.
     *
     * @return A result that should be passed back to JavaScript. The
     * result must be either an Integer, Double, Boolean, String or V8Value.
     */
    public Object invoke(V8Object receiver, V8Array parameters);
}

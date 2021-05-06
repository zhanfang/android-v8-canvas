package com.example.v8engine;

/**
 * The concrete class for all V8 Objects. V8Objects are
 * JavaScript objects accessible in java. Specialized
 * subclasses exist for V8Arrays and V8Functions.
 *
 * V8Object are JavaScript object with key value pairs.
 * Specific get methods exist to access values as primitives.
 * General get methods also exist, which return Java Objects
 * and can be casted to the correct subclass.
 *
 * V8Object have native resources and must be released
 * when they are no longer need in Java.
 */
public class V8Object extends V8Value{
    protected V8Object(V8 v8) {
        super(v8);
    }

    /**
     * Register a Java method as a JavaScript function. When the JS Function is invoked
     * the Java method will be called.
     *
     * @param callback The JavaCallback to call when the JSFunction is invoked.
     * @param jsFunctionName The name of the JSFunction.
     *
     * @return The receiver.
     */
    public V8Object registerJavaMethod(final JavaCallback callback, final String jsFunctionName) {
        v8.checkThread();
        checkReleased();
        v8.registerCallback(callback, getHandle(), jsFunctionName);
        return this;
    }

    /**
     * Register a Java method reflectively given it's name a signature. The option to include
     * the JS Object in the callback can be specified by setting includeReceiver true.
     *
     * @param object The Java Object on which the method is defined.
     * @param methodName The name of the method to register.
     * @param jsFunctionName The name of the JavaScript function to register the
     * method with.
     * @param parameterTypes The parameter types of the method.
     * @param includeReceiver True if the first parameter should include the JS Object,
     * false otherwise.
     *
     * @return The receiver.
     */
    public V8Object registerJavaMethod(final Object object, final String methodName, final String jsFunctionName, final Class<?>[] parameterTypes, final boolean includeReceiver) throws NoSuchMethodException {
        v8.checkThread();
        checkReleased();
        v8.registerCallback(object, jsFunctionName, includeReceiver);
        return this;
    }
}

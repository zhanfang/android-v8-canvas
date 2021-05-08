package com.example.v8engine;

import java.io.Closeable;

/**
 * A base class for all V8 resources. V8 resources must
 * be closed/released. The rules for releasing resources is as
 * follows:
 *
 * 1. If you created it, you must close it, with one exception;
 *    if the object is being passed pack via a return statement,
 *    the system will release it for you.
 *
 * 2. If the system created it, you don’t need to worry about it,
 *    with one caveat; if the object was returned to you as a
 *    result of a method call, you must close it.
 */
abstract public class V8Value implements Closeable {

    public static final int NULL                         = 0;
    public static final int UNKNOWN                      = 0;
    public static final int INTEGER                      = 1;
    public static final int INT_32_ARRAY                 = 1;
    public static final int DOUBLE                       = 2;
    public static final int FLOAT_64_ARRAY               = 2;
    public static final int BOOLEAN                      = 3;
    public static final int STRING                       = 4;
    public static final int V8_ARRAY                     = 5;
    public static final int V8_OBJECT                    = 6;
    public static final int V8_FUNCTION                  = 7;
    public static final int V8_TYPED_ARRAY               = 8;
    public static final int BYTE                         = 9;
    public static final int INT_8_ARRAY                  = 9;
    public static final int V8_ARRAY_BUFFER              = 10;
    public static final int UNSIGNED_INT_8_ARRAY         = 11;
    public static final int UNSIGNED_INT_8_CLAMPED_ARRAY = 12;
    public static final int INT_16_ARRAY                 = 13;
    public static final int UNSIGNED_INT_16_ARRAY        = 14;
    public static final int UNSIGNED_INT_32_ARRAY        = 15;
    public static final int FLOAT_32_ARRAY               = 16;
    public static final int UNDEFINED                    = 99;

    protected V8      v8;
    protected long    objectHandle;
    protected boolean released = true;

    protected V8Value(final V8 v8) {
        if (v8 == null) {
            this.v8 = (V8) this;
        }
        else {
            this.v8 = v8;
        }
    }

    protected void initialize(final long runtimePtr, final Object data) {
        long objectHandle = v8.initNewV8Object(runtimePtr);
        released = false;
        addObjectReference(objectHandle);
    }

    protected void addObjectReference(final long objectHandle) throws Error {
        this.objectHandle = objectHandle;
        try {
            v8.addObjRef(this);
        } catch (Error e) {
            close();
            throw e;
        } catch (RuntimeException e) {
            close();
            throw e;
        }
    }

    /**
     * Gets the runtime this Value was created on.
     *
     * @return Returns the V8 runtime this value is associated with.
     */
    public V8 getRuntime() {
        return v8;
    }

    /**
     * Returns a string representation of the V8 Type.
     * @param type Type to return as a string. See constants in V8Value.
     * @return The V8Value type as a string.
     */
    public static String getStringRepresentation(final int type) {
        switch (type) {
            case NULL:
                return "Null";
            case INTEGER:
                return "Integer";
            case DOUBLE:
                return "Double";
            case BOOLEAN:
                return "Boolean";
            case STRING:
                return "String";
            case V8_ARRAY:
                return "V8Array";
            case V8_OBJECT:
                return "V8Object";
            case V8_FUNCTION:
                return "V8Function";
            case V8_TYPED_ARRAY:
                return "V8TypedArray";
            case BYTE:
                return "Byte";
            case V8_ARRAY_BUFFER:
                return "V8ArrayBuffer";
            case UNSIGNED_INT_8_ARRAY:
                return "UInt8Array";
            case UNSIGNED_INT_8_CLAMPED_ARRAY:
                return "UInt8ClampedArray";
            case INT_16_ARRAY:
                return "Int16Array";
            case UNSIGNED_INT_16_ARRAY:
                return "UInt16Array";
            case UNSIGNED_INT_32_ARRAY:
                return "UInt32Array";
            case FLOAT_32_ARRAY:
                return "Float32Array";
            case UNDEFINED:
                return "Undefined";
            default:
                throw new IllegalArgumentException("Invalid V8 type: " + type);
        }
    }

    /**
     * Returns the 'type' of this V8Value. The available types are defined
     * as constants in {@link V8Value}. Only types that inherit from
     * {@link V8Value} can be returned here.
     *
     * @return The 'type of this V8Value.
     */
    public int getV8Type() {
        v8.checkThread();
        return v8.getType(v8.getV8EnginePtr(), getHandle());
    }

    protected long getHandle() {
        checkReleased();
        return objectHandle;
    }

    public boolean isUndefined() {
        return false;
    }

    public String toString() {
        v8.checkThread();
        return v8.toString(v8.getV8EnginePtr(), getHandle());
    }

    /**
     * Determine if the native resources have been released. Once released
     * a V8 Value can no longer be used.
     *
     * @return Returns true if this object has been released, false otherwise.
     */
    public boolean isReleased() {
        return released;
    }

    protected void checkReleased() {
        if (released) {
            throw new IllegalStateException("Object released");
        }
    }

    @Override
    public void close() {
        if (!released) {
            try {
                v8.releaseObjRef(this);
            } finally {
                released = true;
                v8.release(v8.getV8EnginePtr(), objectHandle);
            }
        }
    }
}

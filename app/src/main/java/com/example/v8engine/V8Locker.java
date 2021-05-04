package com.example.v8engine;

/**
 * Represents a lock for a V8Runtime that can be moved between
 * threads. When instantiated, the lock is automatically assigned
 * to the current thread. If another thread wishes to acquire the
 * lock, it must first be released.
 */
public class V8Locker {

    private Thread thread = null;
    private boolean released = false;
    private V8 runtime;

    V8Locker(final V8 runtime) {
        this.runtime = runtime;
    }

    /**
     * Acquire the lock if it's currently not acquired by another
     * thread. If it's currently held by another thread, an
     * Error will be thrown.
     */
    public synchronized void acquire() {
        if ((thread != null) && (thread != Thread.currentThread())) {
            throw new Error("Invalid V8 thread access: current thread is " + Thread.currentThread() + " while the locker has thread " + thread);
        }
        else if (thread == Thread.currentThread()) {
            return;
        }
        runtime.acquireLock(runtime.getV8EnginePtr());
        thread = Thread.currentThread();
        released = false;
    }

    /**
     * Release the lock if it's currently held by the calling thread.
     * If the current thread does not hold the lock, and error will be
     * thrown. If no thread holds the lock then nothing will happen.
     */
    public synchronized void release() {
        if ((released && (thread == null)) || runtime.isReleased()) {
            return;
        }
        checkThread();
        runtime.releaseLock(runtime.getV8EnginePtr());
        thread = null;
        released = true;
    }

    /**
     * Checks if the locker has access to the current thread.
     * If the locker holds a different thread, than an Error
     * is thrown.
     */
    public void checkThread() {
        if (released && (thread == null)){
            throw new Error("Invalid V8 thread access: the locker has been released!");
        }
        if ((thread != Thread.currentThread())) {
            throw new Error("Invalid V8 thread access: current thread is " + Thread.currentThread() + " while the locker has thread " + thread);
        }
    }

    /**
     * Check if the current thread holds this lock.
     *
     * @return Returns true if the current thread holds the lock,
     * false otherwise.
     */
    public boolean hasLock() {
        return thread == Thread.currentThread();
    }
}

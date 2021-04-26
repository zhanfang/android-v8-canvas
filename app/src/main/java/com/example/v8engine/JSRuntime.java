package com.example.v8engine;

/**
 * JS 运行时抽象
 */
public interface JSRuntime {

    /**
     * 在JS线程执行，若当前线程为非JS线程则执行postOnJSThread
     *
     * @param runnable
     */
    public void runOnJSThread(Runnable runnable);

    /**
     * post到JS线程执行
     *
     * @param runnable runnable
     */
    public void postOnJSThread(Runnable runnable);

    /**
     * post到JS线程执行
     *
     * @param runnable runnable
     * @param delayMillis
     */
    public void postOnJSThread(Runnable runnable, long delayMillis);
}

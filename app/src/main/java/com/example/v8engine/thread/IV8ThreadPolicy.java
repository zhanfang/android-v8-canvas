package com.example.v8engine.thread;

/**
 * v8 线程代理策略
 */
public interface IV8ThreadPolicy {

    /**
     * 启动 v8 engine
     */
    void startV8Thread();

    /**
     * 执行 runnable
     * @param runnable
     */
    void runDelegateRunnable(Runnable runnable);

    /**
     * 延迟执行 runnable
     * @param runnable
     * @param delay
     */
    void runDelegateRunnable(Runnable runnable, long delay);

    /**
     * 获取当前工作线程
     * @return
     */
    Thread getThread();
}

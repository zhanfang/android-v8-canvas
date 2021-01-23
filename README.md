# android-v8-demo
一个将 v8 嵌入 android 的 demo，并且集成了 skia 能力。

## 环境配置
- Android Stadio 4.1.2 版本
- gradle 6.5 版本
- CMake 3.10.x
- Android SDK 28，完成相关配置（local.properties）
- NDK 版本选择 r21 及以下版本，完成相关配置 (local.properties)

## 动态库编译
- 需要编译 skia 的 so 文件
- 需要编译 v8 的 so 文件

## 开启 v8 inspector
此项目在 v8 初始化阶段已默认开启 inspector，但是需要把 unix domain socket 端口映射到本地端口才可进行调试，需要进行以下步骤
```bash
adb shell cat /proc/net/unix | grep --text  _devtools_remote
```
通过执行上面的命令，会得到类似下面的结果
```
0000000000000000: 00000002 00000000 00010000 0001 01 91869 @com.example.zhanfang.test_devtools_remote
```
其中 com.example.zhanfang.test_devtools_remote 就是 localabstract 端口号。我们需要通过 adb 命令把这个端口转发到本地 PC 的 TCP 端口上
```
adb forward tcp:4000 localabstract:com.example.zhanfang.test_devtools_remote
```
forward 转发之后就可以用浏览器打开 http://localhost:4000/json 查看到被调试的页面基础信息，其中 webSocketDebuggerUrl 字段是用于远程调试连接的 websocket

** 由于此项目使用的 v8 版本较旧，需要使用 1.2 版本的 ide-devtools-frontend **
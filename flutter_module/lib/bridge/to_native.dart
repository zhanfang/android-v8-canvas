import 'dart:ffi';

final DynamicLibrary nativeLib = DynamicLibrary.open("libv8.engine.so");

final int Function(int x, int y) nativeAdd = nativeLib
    .lookup<NativeFunction<Int8 Function(Int8, Int8)>>("native_add")
    .asFunction();

final void Function(String globalAlias) _createIsolate = nativeLib.lookup(NativeFunction<Void Function()>('createIsolate'))
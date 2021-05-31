import 'package:flutter/material.dart';
import 'dart:ffi';

import 'package:flutter_module/core/dynamic_app.dart';
import 'package:flutter_module/widget/main.dart';

final DynamicLibrary nativeLib = DynamicLibrary.open("libffi.so");

final int Function(int x, int y) nativeAdd = nativeLib
    .lookup<NativeFunction<Int8 Function(Int8, Int8)>>("native_add")
    .asFunction();

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  DynamicApp app = DynamicApp();
  // 注册基础组件
  registerBasicWidgets();

  return runApp(DynamicAppRoot());
}

import 'package:flutter/material.dart';
import 'package:flutter_module/bridge/from_native.dart';
import 'package:flutter_module/bridge/to_native.dart';

import 'package:flutter_module/core/dynamic_app.dart';
import 'package:flutter_module/widget/main.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  DynamicApp app = DynamicApp();
  // 注册基础组件
  registerBasicWidgets();
  // 注册 navtive binding 函数
  registerDartMethodsToCpp();

  nativeAdd(1, 2);

  return runApp(DynamicAppRoot());
}

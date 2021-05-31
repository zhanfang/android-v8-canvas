import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_module/core/dynamic_model.dart';
import 'package:flutter_module/core/dynamic_page.dart';
import 'package:flutter_module/utils/stack_list.dart';

typedef void DynamicChannelFn(dynamic params);

/// 拦截器的构建方法类型
typedef ModelBuilder = Widget Function(
    DynamicModel model, BuildContext context);

class DynamicApp {
  static DynamicApp _instance;
  MethodChannel _channel;
  Map<String, DynamicChannelFn> _channelFuncs = {};
  Map<String, Map<String, ModelBuilder>> widgets = {};

  /// appBar高度
  double appBarHeight = kToolbarHeight;

  /// context 栈
  StackList<BuildContext> contextStack = StackList();

  // 单例模式
  factory DynamicApp() => DynamicApp.getInstance();

  static DynamicApp getInstance() {
    if (_instance == null) {
      _instance = DynamicApp._();
      _instance.registerFuncs({
        'updateWidget': (params) {
          _instance.updateWidget(params);
        },
        'pushPage': (params) {
          _instance.pushPage('defalut', params);
        }
      });
      _instance.setupChannel();
    }

    return _instance;
  }

  DynamicApp._();

  registerFuncs(Map<String, DynamicChannelFn> funcs) {
    for (String key in funcs.keys) {
      this._channelFuncs[key] = funcs[key];
    }
  }

  setupChannel() {
    _channel = MethodChannel('square');
    // ignore: missing_return
    _channel.setMethodCallHandler((MethodCall call) {
      if (call.method == 'methodChannel_js_call_flutter') {
        final dynamic args = jsonDecode(call.arguments);
        final String method = args['method'];
        final dynamic params = args['params'];

        Function func = _channelFuncs[method];
        func?.call(params);
      }
    });
  }

  void registerWidget(
    String widgetName,
    ModelBuilder constructor,
  ) {
    widgets[widgetName] = {
      'constructor': constructor,
    };
  }

  ModelBuilder getWidget(String widgetName) {
    if (widgets[widgetName] == null) {
      return null;
    }
    return widgets[widgetName]['constructor'];
  }

  /// 获取当前显示页面的 context
  BuildContext get context => contextStack.last;

  void pushPage(String pageName, Map<String, dynamic> pageData) {
    Navigator.push(context, MaterialPageRoute(builder: (context) {
      return DynamicPage(pageName, pageData);
    }));
  }

  void popPage() {
    Navigator.pop(context);
    contextStack.pop();
  }

  void updateWidget(dynamic params) {
    print(params);
    // pushPage('test', params);
  }
}

class DynamicMainApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return DynamicPage('home', {});
  }
}

class DynamicAppRoot extends StatefulWidget {
  @override
  State<StatefulWidget> createState() {
    return _DynamicAppRootState();
  }
}

class _DynamicAppRootState extends State<DynamicAppRoot> {
  Key key = UniqueKey();

  @override
  Widget build(BuildContext context) {
    return Material(
      key: key,
      child: WillPopScope(
        child: Stack(
          textDirection: TextDirection.ltr,
          children: <Widget>[
            MaterialApp(
              color: Colors.white,
              title: '123',
              theme: ThemeData(
                primarySwatch: Colors.blue,
              ),
              debugShowCheckedModeBanner: false,
              home: DynamicMainApp(),
            ),
            Container(),
          ],
        ),
        onWillPop: () {
          return Future.value(false);
        },
      ),
    );
  }
}

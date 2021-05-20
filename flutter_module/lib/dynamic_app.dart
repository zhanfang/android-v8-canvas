

import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_module/dynamic_page.dart';

typedef void DynamicChannelFn(dynamic params);

class DynamicApp {
  MethodChannel _channel;
  Map<String, DynamicChannelFn> _channelFuncs = {};
  
  DynamicApp() {
    registerFuncs({
      'updateWidget': (params) {
        this.updateWidget(params);
      }
    });
    setupChannel();
  }

  registerFuncs(Map<String, DynamicChannelFn> funcs) {
    for(String key in funcs.keys) {
      this._channelFuncs[key] = funcs[key];
    }
  }

  setupChannel() {
    DynamicApp self = this;
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

  void pushPage(@required String pageName, @required Map<String, dynamic> pageData) {
    DynamicPage page = DynamicPage(pageName, pageData);
  }

  void updateWidget(dynamic params) {
    print(params);
  }
}
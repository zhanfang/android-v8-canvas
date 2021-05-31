import 'package:flutter/material.dart';
import 'package:flutter_module/core/dynamic_widget.dart';

import 'dynamic_app.dart';

/// js端传输过来的 jsonData 转换为的 model
/// 具有层级关系，可以递归查找
class DynamicModel {
  String name;

  /// widgetId
  String widgetId;

  /// 当前model对应的widget是否是stateful
  bool isStateful;

  /// 当前model对应的widget的key
  Key key;

  /// 当前widget对应的props
  Map<String, dynamic> props = {};

  /// props的属性为DynamicModel时构建出的值
  Map<String, dynamic> buildProps = {};

  // 父节点 model
  DynamicModel parent;

  /// 判断 data 是否是一个组件节点
  static bool isModelMap(data) {
    return (data is Map) && data['type'] != null;
  }

  static bool isModelList(data) {
    return (data is List) &&
        (data.length > 0) &&
        ((data.first is DynamicModel) || isModelMap(data.first));
  }

  DynamicModel(Map<String, dynamic> vdom, BuildContext context) {
    name = vdom['type'];
    isStateful = vdom['isStateful'] == true;

    Map<String, dynamic> tempProps = vdom['props'];
    if (tempProps == null) return;
    print(vdom);

    for (String key in tempProps.keys) {
      dynamic data = tempProps[key];
      if (isModelMap(data)) {
        DynamicModel model = DynamicModel(data, context);
        props[key] = model;
        buildProps[key] = getWidgetClass(model, context);
      } else {
        props[key] = data;
      }
    }

    if (vdom['children'] != null) {
      List<Widget> children = [];
      for (var i = 0; i < vdom['children'].length; i++) {
        DynamicModel model = DynamicModel(vdom['children'][i], context);
        children.add(getWidgetClass(model, context));
      }
      buildProps['children'] = children;
    }
  }

  Widget getWidgetClass(DynamicModel model, BuildContext context) {
    ModelBuilder widgetConstructor = DynamicApp().getWidget(model.name);
    Widget widget;
    if (widgetConstructor == null) {
      widget = Container();
    } else {
      widget = widgetConstructor(model, context);
    }

    return model.isStateful
        ? model.name != 'AppBar'
            ? DynamicWidget(model, widget)
            : DynamicPreferredSizeWidget(model, widget)
        : widget;
  }
}

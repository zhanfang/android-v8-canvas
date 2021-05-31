import 'package:flutter/material.dart';
import 'package:flutter_module/core/dynamic_app.dart';
import 'package:flutter_module/core/dynamic_model.dart';

class DynamicPage extends StatefulWidget {
  final String pageName;
  final Map<String, dynamic> pageData;

  DynamicPage(this.pageName, this.pageData);

  @override
  DynamicPageState createState() {
    return DynamicPageState();
  }
}

class DynamicPageState extends State<DynamicPage> {
  String pageName;
  Map<String, dynamic> pageData;
  Widget widgetInstance;

  @override
  void initState() {
    super.initState();
    pageName = widget.pageName;
    pageData = widget.pageData;

    widgetInstance = buildWidget(pageData);
  }

  @override
  Widget build(BuildContext context) {
    DynamicApp().contextStack.push(context);
    return Material(
      color: Colors.transparent,
      child: widgetInstance ??
          Scaffold(
            backgroundColor: Colors.white,
          ),
    );
  }

  /// 根据 vdom 构建 widget tree
  /// 1. 转换 vdom props 为 dynamicModel
  /// 2. 根据 dynamicModel 生成 Widget class
  /// 3. 组装成 Widget tree
  Widget buildWidget(Map<String, dynamic> pageData) {
    DynamicModel model = DynamicModel(pageData, context);
    ModelBuilder widgetConstructor = DynamicApp().getWidget(pageData['type']);
    if (widgetConstructor == null) {
      return Container();
    }
    return widgetConstructor(model, context);
  }
}

import 'package:flutter/material.dart';
import 'package:flutter_module/core/dynamic_model.dart';
import 'package:flutter_module/widget/widget_basic.dart';

class PageWidget extends BasicWidget {
  final DynamicModel model;
  final PreferredSizeWidget appBar;
  final List<Widget> children;
  final Color backgroundColor;
  final bool scrollable;

  PageWidget(
    this.model, {
    Key key,
    this.appBar,
    this.children,
    this.backgroundColor = Colors.white,
    this.scrollable = false,
  }) : super(model, key: key);

  @override
  Widget buildMainWidget(BuildContext context) {
    return Scaffold(
      appBar: appBar,
      body: Container(
        constraints: BoxConstraints.expand(),
        child: Container(
          alignment: Alignment.topLeft,
          child: buildContentWidget(context),
        ),
      ),
    );
  }

  Widget buildContentWidget(BuildContext context) {
    if (children.length == 0) return null;
    if (children.length == 1) {
      // Widget child = children.first;
    }

    return Column(
      children: children,
      mainAxisAlignment: MainAxisAlignment.start,
      crossAxisAlignment: CrossAxisAlignment.start,
    );
  }
}

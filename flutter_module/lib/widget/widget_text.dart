import 'package:flutter/material.dart';
import 'package:flutter_module/core/dynamic_model.dart';
import 'package:flutter_module/widget/widget_basic.dart';

class TextWidget extends BasicWidget {
  final String text;
  final List<TextSpan> children;
  final TextStyle style;
  final TextAlign align;
  final bool wrap;
  final TextOverflow overflow;
  final int maxLines;

  TextWidget(
    DynamicModel model, {
    Key key,
    this.text,
    this.children,
    this.style,
    this.align = TextAlign.left,
    this.wrap = true,
    this.overflow,
    this.maxLines,
  }) : super(model, key: key);

  @override
  Widget buildMainWidget(BuildContext context) {
    return Text.rich(
      TextSpan(text: text ?? '', children: children),
      style: style,
      textAlign: align,
      softWrap: wrap,
      overflow: overflow,
      maxLines: maxLines,
      textScaleFactor: 1.0,
    );
  }
}

import 'package:flutter/material.dart';
import 'package:flutter_module/core/dynamic_model.dart';
import 'package:flutter_module/utils/util.dart';

class BasicWidget extends StatelessWidget {
  final DynamicModel model;
  final double _width;
  final double _height;
  final double _minWidth;
  final double _minHeight;
  final double _maxWidth;
  final double _maxHeight;
  final int _flex;
  final Map<String, double> _absolute;
  final EdgeInsets _padding;
  final EdgeInsets _margin;
  final Color _tintColor;
  final Color _backgroundColor;
  final bool _disabled;
  final double _opacity;

  BasicWidget(
    this.model, {
    Key key,
    double width,
    double height,
    double minWidth,
    double minHeight,
    double maxWidth,
    double maxHeight,
    int flex,
    Map<String, double> absolute,
    EdgeInsets padding,
    EdgeInsets margin,
    Color tintColor,
    Color backgroundColor,
    Gradient backgroundGradient,
    Border border,
    BorderRadius borderRadius,
    List<BoxShadow> boxShadow,
    bool disabled = false,
    double opacity = 1.0,
    double tapOpacity,
    Matrix4 transform,
  })  : _width = model != null
            ? Util.getDouble(model.props['width']) ?? width
            : width,
        _height = model != null
            ? Util.getDouble(model.props['height']) ?? height
            : height,
        _minWidth = model != null
            ? Util.getDouble(model.props['minWidth']) ?? minWidth
            : minWidth,
        _minHeight = model != null
            ? Util.getDouble(model.props['minHeight']) ?? minHeight
            : minHeight,
        _maxWidth = model != null
            ? Util.getDouble(model.props['maxWidth']) ?? maxWidth
            : maxWidth,
        _maxHeight = model != null
            ? Util.getDouble(model.props['maxHeight']) ?? maxHeight
            : maxHeight,
        _flex = model != null ? Util.getInt(model.props['flex']) ?? flex : flex,
        _absolute = model != null
            ? Util.getAbsolute(model.props['absolute'])
            : absolute,
        _padding = model != null &&
                model.name != 'ListView' &&
                model.name != 'ScrollView'
            ? Util.getEdgeInsets(model.props['padding']) ?? padding
            : padding,
        _margin = model != null
            ? Util.getEdgeInsets(model.props['margin']) ?? margin
            : margin,
        _tintColor = model != null
            ? Util.getColor(model.props['tintColor']) ?? tintColor
            : tintColor,
        _backgroundColor = model != null
            ? Util.getColor(model.props['backgroundColor']) ?? backgroundColor
            : backgroundColor,
        _disabled = model != null
            ? (Util.getBoolean(model.props['disabled']))
            : disabled,
        _opacity = model != null
            ? (Util.getDouble(
                  model.props['opacity'],
                  range: Range(min: 0, max: 1),
                ) ??
                opacity)
            : 1.0,
        super(key: key ?? (model != null ? model.key : null));

  @override
  Widget build(BuildContext context) {
    return buildLayoutWidget(context);
  }

  Widget buildLayoutWidget(BuildContext context) {
    if (_absolute != null) {
      return Positioned(
        top: _absolute['top'],
        bottom: _absolute['bottom'],
        left: _absolute['left'],
        right: _absolute['right'],
        child: buildTouchWidget(context),
      );
    } else if (_flex != null) {
      return Expanded(
        flex: _flex,
        child: buildTouchWidget(context),
      );
    }
    return buildTouchWidget(context);
  }

  Widget buildTouchWidget(BuildContext context) {
    BoxConstraints constraint = BoxConstraints(
      minWidth: _minWidth ?? 0,
      minHeight: _minHeight ?? 0,
      maxWidth: _maxWidth ?? double.infinity,
      maxHeight: _maxHeight ?? double.infinity,
    );

    BoxDecoration decoration =
        BoxDecoration(color: _backgroundColor ?? Colors.transparent);

    Widget styleWidget = Container(
      margin: _margin,
      padding: _padding,
      width: _width,
      height: _height,
      constraints: constraint,
      decoration: decoration,
      child: buildMainWidget(context),
    );

    return Opacity(
      opacity: _opacity,
      child: styleWidget,
    );
  }

  Widget buildMainWidget(BuildContext context) {
    return Container();
  }
}

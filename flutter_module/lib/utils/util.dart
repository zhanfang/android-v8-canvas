import 'dart:io';

import 'package:flutter/material.dart';

/// 数字范围类型
/// 闭区间
/// [Range(min: 0, max: 1)] 表示 [0, 1]
class Range {
  final num min;
  final num max;

  Range({this.min, this.max});

  /// 判断目标数字是否在当前范围内
  /// 在范围内则返回当前数字
  /// 不在范围内则返回边界数字
  convertNumber(num value) {
    if (min != null && value < min) return min;
    if (max != null && value > max) return max;
    return value;
  }
}

class Util {
  static bool _isAndroid = Platform.isAndroid;
  static bool get isAndroid => _isAndroid;

  static int getInt(dynamic value) {
    return (value is num) ? value.toInt() : null;
  }

  static double getDouble(dynamic value, {Range range}) {
    if (value is num)
      return range == null
          ? value.toDouble()
          : range.convertNumber(value).toDouble();
    return null;
  }

  /// 将 value 转换为 boolean
  /// [nullIsTrue] 当 value 为 null 时是否返回 true
  static bool getBoolean(dynamic value, {bool nullIsTrue = false}) {
    if ((value == null && !nullIsTrue) ||
        value == '' ||
        value == 0 ||
        value == false) return false;
    return true;
  }

  static String getString(dynamic value) {
    if (value is String) return value;
    if (value != null) return value.toString();
    return null;
  }

  static FontWeight getTextWeight(dynamic value) {
    if (!(value is String)) return null;
    switch (value as String) {
      case 'normal':
        return FontWeight.normal;
      case 'light':
        return FontWeight.w300;
      case 'bold':
        return FontWeight.w600;
      case 'bolder':
        return FontWeight.w700;
      default:
        return null;
    }
  }

  static bool _isShadowMap(dynamic value) {
    if (!(value is Map)) return false;
    if (value['color'] != null ||
        value['offsetX'] != null ||
        value['offsetY'] != null ||
        value['blur'] != null) return true;
    return false;
  }

  static List<Shadow> getTextShadow(dynamic value) {
    if (!Util._isShadowMap(value)) return null;
    return [
      Shadow(
        color: Util.getColor(value['color']) ?? Colors.black,
        offset: Offset(
          Util.getDouble(value['offsetX']) ?? 0,
          Util.getDouble(value['offsetY']) ?? 0,
        ),
        blurRadius: Util.getDouble(value['blur']) ?? 0,
      )
    ];
  }

  static TextDecoration getTextDecoration(dynamic value) {
    if (!(value is String)) return null;
    switch (value as String) {
      case 'none':
        return TextDecoration.none;
      case 'lineThrough':
        return TextDecoration.lineThrough;
      case 'overline':
        return TextDecoration.overline;
      case 'underline':
        return TextDecoration.underline;
      default:
        return null;
    }
  }

  static TextStyle getTextStyle(dynamic value) {
    if (!(value is Map)) return null;
    double size = Util.getDouble(value['size']);
    FontWeight weight = Util.getTextWeight(value['weight']);
    List<Shadow> shadow = Util.getTextShadow(value['shadow']);
    TextDecoration decoration = Util.getTextDecoration(value['decoration']);
    Color color = Util.getColor(value['color']);
    Color backgroundColor = Util.getColor(value['backgroundColor']);
    double letterSpacing = Util.getDouble(value['letterSpacing']);
    double wordSpacing = Util.getDouble(value['wordSpacing']);
    double lineHeight = Util.getDouble(value['lineHeight']);

    return TextStyle(
      fontSize: size,
      fontWeight: weight,
      shadows: shadow,
      color: color,
      backgroundColor: backgroundColor,
      letterSpacing: letterSpacing,
      wordSpacing: wordSpacing,
      decoration: decoration,
      decorationColor: color,
      height: lineHeight,
    );
  }

  static TextSpan getTextSpan(dynamic value) {
    if (!(value is Map)) return null;
    if (value['text'] == null) return null;
    return TextSpan(
        text: Util.getSafeString(value['text']),
        style: Util.getTextStyle(value));
  }

  // 过滤不可见 Unicode 码
  // 否则会导致页面字体错乱
  static String getSafeString(dynamic value) {
    String res = Util.getString(value);
    if (res == null) return res;
    return res.replaceAll(RegExp('[\u0000-\u001F\u007F-\u00A0]'), '');
  }

  static TextAlign getTextAlign(dynamic value) {
    if (!(value is String)) return TextAlign.left;
    switch (value as String) {
      case 'left':
        return TextAlign.left;
      case 'right':
        return TextAlign.right;
      case 'center':
        return TextAlign.center;
      default:
        return TextAlign.left;
    }
  }

  static TextOverflow getTextOverflow(dynamic value) {
    if (!(value is String)) return null;
    switch (value as String) {
      case 'ellipsis':
        return TextOverflow.ellipsis;
      case 'visible':
        return TextOverflow.visible;
      case 'clip':
        return TextOverflow.clip;
      case 'fade':
        return TextOverflow.fade;
      default:
        return null;
    }
  }

  static List getList(dynamic value) {
    if (value == null || value is List) return value;
    return [value];
  }

  static List<Widget> getWidgetList(dynamic value) {
    if (!(value is List)) return [];
    List<Widget> target = [];
    for (int i = 0; i < value.length; i++) {
      if (value[i] is Widget) {
        Widget widget = value[i];
        target.add(widget);
      }
    }
    return target;
  }

  static Map<String, double> getAbsolute(dynamic value) {
    if (value is num) {
      double v = value.toDouble();
      return {
        'top': v,
        'bottom': v,
        'left': v,
        'right': v,
      };
    }
    if (value is Map)
      return {
        'top': (value['top'] != null && value['top'] is num)
            ? value['top'].toDouble()
            : null,
        'bottom': (value['bottom'] != null && value['bottom'] is num)
            ? value['bottom'].toDouble()
            : null,
        'left': (value['left'] != null && value['left'] is num)
            ? value['left'].toDouble()
            : null,
        'right': (value['right'] != null && value['right'] is num)
            ? value['right'].toDouble()
            : null,
      };
    return null;
  }

  static Color getColor(dynamic value) {
    return (value is int) && value >= 0x00000000 && value <= 0xffffffff
        ? Color(value)
        : null;
  }

  static Color getRGBAColor(dynamic value) {
    return (value is List)
        ? Color.fromRGBO(value[0], value[1], value[2], value[3] * 1.0)
        : null;
  }

  static EdgeInsets getEdgeInsets(dynamic value) {
    if (value is num) return EdgeInsets.all(value.toDouble());
    if (!(value is Map)) return null;
    return EdgeInsets.fromLTRB(
        value['left']?.toDouble() ?? 0,
        value['top']?.toDouble() ?? 0,
        value['right']?.toDouble() ?? 0,
        value['bottom']?.toDouble() ?? 0);
  }
}

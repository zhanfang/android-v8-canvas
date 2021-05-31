import 'package:flutter/material.dart';
import 'package:flutter_module/core/dynamic_app.dart';
import 'package:flutter_module/core/dynamic_model.dart';
import 'package:flutter_module/utils/util.dart';
import 'package:flutter_module/widget/widget_app_bar.dart';
import 'package:flutter_module/widget/widget_page.dart';
import 'package:flutter_module/widget/widget_text.dart';

class WidgetMap {
  TextWidget textConstructor(DynamicModel model, BuildContext context) {
    Map<String, dynamic> props = model.props;

    print(props);

    return TextWidget(
      model,
      text: Util.getSafeString(props['text']),
      children: getTextSpanList(Util.getList(props['richText'])),
      style: Util.getTextStyle(props),
      align: Util.getTextAlign(props['align']),
      wrap: Util.getBoolean(props['wrap'], nullIsTrue: true),
      overflow: Util.getTextOverflow(props['overflow']),
      maxLines: Util.getInt(props['maxLines']),
    );
  }

  List<TextSpan> getTextSpanList(List list) {
    if (list == null) return null;
    List<TextSpan> res = [];
    for (dynamic item in list) {
      TextSpan span = Util.getTextSpan(item);
      if (span != null) res.add(span);
    }
    return res;
  }

  PageWidget pageConstructor(DynamicModel model, BuildContext context) {
    Map<String, dynamic> props = model.props;
    Map<String, dynamic> buildProps = model.buildProps;

    return PageWidget(
      model,
      appBar: buildProps['appBar'] as PreferredSizeWidget,
      children: Util.getWidgetList(buildProps['children']),
      backgroundColor: Util.getColor(props['backgroundColor']) ?? Colors.white,
    );
  }

  AppBarWidget appBarConstructor(DynamicModel model, BuildContext context) {
    Map<String, dynamic> props = model.props;
    Map<String, dynamic> buildProps = model.buildProps;

    bool isLight = Util.getBoolean(props['statusTextLight']);
    Color themeColor = Util.getRGBAColor(props['titleColor']) ??
        (isLight ? Colors.white : Colors.black);

    return AppBarWidget(
      model,
      key: model.key,
      elevation: Util.getBoolean(props['elevation']),
      isLight: isLight,
      backgroundColor: Util.getColor(props['backgroundColor']) ?? Colors.white,
      centerTitle: Util.getBoolean(props['centerTitle'], nullIsTrue: true),
      leading: (!Util.getBoolean(props['showLeadingButton'], nullIsTrue: true)
          ? null
          : AppBarBackButton(color: themeColor)),
      title: buildProps['titleView'] ??
          AppBarText(
              props['title'] ?? '',
              TextStyle(
                color: themeColor,
                fontSize: Util.getDouble(props['titleSize']),
                fontWeight: Util.getTextWeight(props['titleWeight']),
              )),
    );
  }
}

void registerBasicWidgets() {
  DynamicApp app = DynamicApp();
  WidgetMap widgetMap = WidgetMap();
  app.registerWidget('text', widgetMap.textConstructor);
  app.registerWidget('page', widgetMap.pageConstructor);
  app.registerWidget('appBar', widgetMap.appBarConstructor);
}

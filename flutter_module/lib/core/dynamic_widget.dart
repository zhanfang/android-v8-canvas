import 'package:flutter/material.dart';
import 'package:flutter_module/core/dynamic_model.dart';

class DynamicWidget extends StatefulWidget {
  final DynamicModel model;
  final Widget widgetInstance;

  DynamicWidget(this.model, this.widgetInstance);

  @override
  State<StatefulWidget> createState() {}
}

class DynamicWidgetState extends State<DynamicWidget> {
  Widget widgetInstance;

  DynamicWidgetState(this.widgetInstance);

  @override
  Widget build(BuildContext context) {
    return widgetInstance;
  }

  void updateWidget(Widget newWidget) {
    if (widgetInstance == newWidget) return;

    setState(() {
      widgetInstance = newWidget;
    });
  }
}

class DynamicPreferredSizeWidget extends DynamicWidget
    implements PreferredSizeWidget {
  DynamicPreferredSizeWidget(DynamicModel model, Widget widgetInstance)
      : super(model, widgetInstance);
  @override
  Size get preferredSize => widgetInstance == null
      ? null
      : (widgetInstance as PreferredSizeWidget).preferredSize;
}

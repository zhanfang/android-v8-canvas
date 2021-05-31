import 'package:flutter/material.dart';
import 'package:flutter_module/core/dynamic_app.dart';
import 'package:flutter_module/core/dynamic_model.dart';
import 'package:flutter_module/widget/widget_basic.dart';
import 'package:flutter_module/widget/widget_text.dart';

class AppBarWidget extends BasicWidget implements PreferredSizeWidget {
  final DynamicModel model;
  final bool elevation;
  final bool isLight;
  final Color backgroundColor;
  final bool centerTitle;
  final Widget title;
  final Widget leading;
  final List<Widget> actions;

  AppBarWidget(
    this.model, {
    Key key,
    this.elevation = false,
    this.isLight = true,
    this.backgroundColor,
    this.centerTitle = true,
    this.title,
    this.leading,
    this.actions,
  }) : super(model, key: key);

  @override
  Size get preferredSize => Size.fromHeight(DynamicApp().appBarHeight);

  @override
  Widget buildMainWidget(BuildContext context) {
    return PreferredSize(
      key: model.key,
      preferredSize: preferredSize,
      child: AppBar(
        automaticallyImplyLeading: false,
        elevation: elevation ? 1 : 0,
        centerTitle: centerTitle, //标题是否居中
        backgroundColor: backgroundColor,
        brightness: isLight ? Brightness.dark : Brightness.light,
        leading: leading, //导航栏最左侧Widget，常见为抽屉菜单按钮或返回按钮。
        actions: actions,
        title: title,
        titleSpacing: 0,
      ),
    );
  }
}

class AppBarBackButton extends StatelessWidget {
  final Color color;

  AppBarBackButton({Key key, this.color}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      child: Container(
        width: 44,
        height: 44,
        padding: EdgeInsets.only(left: 10),
        alignment: Alignment.centerLeft,
        child: Icon(
          Icons.arrow_back_ios,
          color: color,
        ),
      ),
      onTap: () {
        DynamicApp().popPage();
      },
    );
  }
}

class AppBarText extends StatefulWidget {
  final String text;
  final TextStyle style;

  AppBarText(this.text, this.style);

  @override
  State<StatefulWidget> createState() {
    return AppBarTextState(text, style);
  }
}

class AppBarTextState extends State<AppBarText> {
  String text = '';
  TextStyle style;

  AppBarTextState(this.text, this.style);

  @override
  Widget build(BuildContext context) {
    return TextWidget(
      null,
      text: text,
      style: style,
      maxLines: 1,
    );
  }
}



import 'package:flutter/material.dart';

class DynamicPage extends StatefulWidget {
  final String pageName;
  final Map<String, dynamic> pageData;

  DynamicPage(this.pageName, this.pageData);
  
  @override
  State<StatefulWidget> createState() {
    return DynamicPageState();
  }
}

class DynamicPageState extends State<DynamicPage> {
  @override
  Widget build(BuildContext context) {
    // TODO: implement build

  }
  
}
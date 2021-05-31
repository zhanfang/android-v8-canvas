
class StackList<T> {
  List<T> stack = [];

  int get length => stack.length;
  bool get isEmpty => stack.isEmpty;
  T get last => length > 0 ? stack.last : null;
  set last(T item) {
    if (length > 0) stack.last = item;
  }

  T get first => length > 0 ? stack.first : null;
  set first(T item) {
    if (length > 0) stack.first = item;
  }

  T operator [](int index) => stack[index];
  void operator []=(int index, T item) => replace(item, index);

  /// 向栈中添加元素
  void push(T item) {
    stack.add(item);
  }

  /// 推出栈尾元素
  T pop() {
    return stack.length > 0 ? stack.removeLast() : null;
  }

  /// 替换栈中元素
  T replace(T item, int index) {
    if (index < length) {
      T temp = stack[index];
      if (temp != item) stack[index] = item;
      return temp;
    }
    return null;
  }

  /// 判断当前栈是否含有某个元素
  bool has(T item) {
    return stack.contains(item);
  }
}
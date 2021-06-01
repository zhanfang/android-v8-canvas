import 'dart:ffi';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';

// An native struct can be directly convert to javaScript String without any conversion cost.
class NativeString extends Struct {
  Pointer<Uint16> string;

  @Int32()
  int length;
}

Pointer<Uint16> _stringToUint16(String string) {
  final units = string.codeUnits;
  final Pointer<Uint16> result = allocate<Uint16>(count: units.length);
  final Uint16List nativeString = result.asTypedList(units.length);
  nativeString.setAll(0, units);
  return result;
}

Pointer<NativeString> stringToNativeString(String string) {
  assert(string != null);
  Pointer<NativeString> nativeString = allocate<NativeString>();
  nativeString.ref.string = _stringToUint16(string);
  nativeString.ref.length = string.length;
  return nativeString;
}

String uint16ToString(Pointer<Uint16> pointer, int length) {
  return String.fromCharCodes(pointer.asTypedList(length));
}

String nativeStringToString(Pointer<NativeString> pointer) {
  return uint16ToString(pointer.ref.string, pointer.ref.length);
}

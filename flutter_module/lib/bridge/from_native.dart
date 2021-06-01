import 'dart:ffi';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';
import 'package:flutter_module/bridge/util.dart';

final DynamicLibrary nativeLib = DynamicLibrary.open("libffi.so");

final int Function(int x, int y) nativeAdd = nativeLib
    .lookup<NativeFunction<Int8 Function(Int8, Int8)>>("native_add")
    .asFunction();

/// Steps for using dart:ffi to call a Dart function from C:
/// 1. Import dart:ffi.
/// 2. Create a typedef with the FFI type signature of the Dart function.
/// 3. Create a typedef for the variable that you’ll use when calling the
///    Dart function.
/// 4. Open the dynamic library that register in the C.
/// 5. Get a reference to the C function, and put it into a variable.
/// 6. Call from C.

typedef Native_PlatformBrigthness = Pointer<NativeString> Function();

Pointer<NativeString> _platformBrightness() {
  print('1111111');
}

final Pointer<NativeFunction<Native_PlatformBrigthness>>
    _nativePlatformBrightness = Pointer.fromFunction(_platformBrightness);

final List<int> _dartNativeMethods = [_nativePlatformBrightness.address];

typedef Native_RegisterDartMethods = Void Function(
    Pointer<Uint64> methodBytes, Int32 length);
typedef Dart_RegisterDartMethods = void Function(
    Pointer<Uint64> methodBytes, int length);

final Dart_RegisterDartMethods _registerDartMethods = nativeLib
    .lookup<NativeFunction<Native_RegisterDartMethods>>('registerDartMethods')
    .asFunction();

void registerDartMethodsToCpp() {
  Pointer<Uint64> bytes = allocate<Uint64>(count: _dartNativeMethods.length);
  Uint64List nativeMethodList = bytes.asTypedList(_dartNativeMethods.length);
  nativeMethodList.setAll(0, _dartNativeMethods);
  _registerDartMethods(bytes, _dartNativeMethods.length);
}

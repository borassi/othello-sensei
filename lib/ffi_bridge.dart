import 'dart:ffi';
import 'dart:io';

import 'package:othello_sensei_flutter/ffi_engine.dart';

final FFIEngine ffiEngine = FFIEngine(
  Platform.isMacOS || Platform.isIOS ? DynamicLibrary.process() // macos and ios
        : (DynamicLibrary.open(Platform.isWindows // windows
        ? 'main.dll'
        : 'libmain.so'))
);

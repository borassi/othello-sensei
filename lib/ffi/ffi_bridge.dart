/*
 * Copyright (c) 2023-2024. Michele Borassi
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

import 'dart:ffi';
import 'dart:io';

import 'package:othello_sensei/ffi/ffi_engine.dart';

DynamicLibrary getDynamicLibrary() {
  if (Platform.isMacOS || Platform.isIOS) {
    return DynamicLibrary.process();
  } else if (Platform.isWindows) {
    return DynamicLibrary.open('api.dll');
  } else {
    return DynamicLibrary.open('libui.so');
  }
}

final FFIEngine ffiEngine = FFIEngine(getDynamicLibrary());

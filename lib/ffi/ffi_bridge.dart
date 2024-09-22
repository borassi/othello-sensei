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

import 'ffi_cpu_adapter.dart';

DynamicLibrary getDynamicLibrary() {
  if (Platform.isAndroid) {
    return DynamicLibrary.open('libui.so');
  }

  if (Platform.isMacOS || Platform.isIOS) {
    return DynamicLibrary.process();
  } else if (Platform.isWindows) {
    FFICpuSupportedFeatures supportedFeatures = FFICpuSupportedFeatures(DynamicLibrary.open('cpu_adapter_win.dll'));
    if (supportedFeatures.CPUHasBMI2()) {
      try {
        return DynamicLibrary.open('ui_win-bmi2.dll');
      } catch (invalidArgumentException) {
        // Run locally, didn't copy the file.
      }
    }
    if (supportedFeatures.CPUHasPopcnt()) {
      try {
        return DynamicLibrary.open('ui_win-popcnt.dll');
      } catch (invalidArgumentException) {
        // Run locally, didn't copy the file.
      }
    }
    return DynamicLibrary.open('ui_win.dll');
  } else if (Platform.isLinux) {
    FFICpuSupportedFeatures supportedFeatures = FFICpuSupportedFeatures(DynamicLibrary.open('libcpu_adapter.so'));
    if (supportedFeatures.CPUHasBMI2()) {
      try {
        return DynamicLibrary.open('libui-bmi2.so');
      } catch (invalidArgumentException) {
        // Run locally, didn't copy the file.
      }
    }
    if (supportedFeatures.CPUHasPopcnt()) {
      try {
        return DynamicLibrary.open('libui-popcnt.so');
      } catch (invalidArgumentException) {
        // Run locally, didn't copy the file.
      }
    }
  }
  return DynamicLibrary.open('libui.so');
}

final FFIEngine ffiEngine = FFIEngine(getDynamicLibrary());

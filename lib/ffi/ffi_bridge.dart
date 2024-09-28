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

import 'ffi_cpu_adapter.dart';

enum CpuType {
  generic,
  noFeature,
  popcnt,
  bmi2,
}

(DynamicLibrary, CpuType) getDynamicLibrary() {
  if (Platform.isAndroid) {
    return (DynamicLibrary.open('libui.so'), CpuType.generic);
  }

  FFICpuSupportedFeatures supportedFeatures;
  String cpuNamePattern;

  if (Platform.isMacOS || Platform.isIOS) {
    return (DynamicLibrary.process(), CpuType.generic);
  } else if (Platform.isWindows) {
    supportedFeatures = FFICpuSupportedFeatures(
        DynamicLibrary.open('cpu_adapter_win.dll'));
    cpuNamePattern = 'ui_win{}.dll';
  } else if (Platform.isLinux) {
    supportedFeatures = FFICpuSupportedFeatures(
        DynamicLibrary.open('libcpu_adapter.so'));
    cpuNamePattern = 'libui{}.so';
  } else {
    throw UnimplementedError('Unsupported platform ${Platform.operatingSystem}');
  }

  if (supportedFeatures.CPUHasBMI2()) {
    try {
      var dynamicLibrary = DynamicLibrary.open(cpuNamePattern.replaceFirst('{}', '-bmi2'));
      return (dynamicLibrary, CpuType.bmi2);
    } catch (invalidArgumentException) {
      // Run locally, didn't copy the file.
    }
  }
  if (supportedFeatures.CPUHasPopcnt()) {
    try {
      var dynamicLibrary = DynamicLibrary.open(cpuNamePattern.replaceFirst('{}', '-popcnt'));
      return (dynamicLibrary, CpuType.popcnt);
    } catch (invalidArgumentException) {
      // Run locally, didn't copy the file.
    }
  }
  var dynamicLibrary = DynamicLibrary.open(cpuNamePattern.replaceFirst('{}', ''));
  return (dynamicLibrary, CpuType.noFeature);
}

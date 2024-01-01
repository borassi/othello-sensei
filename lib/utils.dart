/*
 * Copyright (c) 2023. Michele Borassi
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

import 'dart:async';
import 'dart:math';

extension StringExtension on String {
  String capitalize() {
    return "${this[0].toUpperCase()}${substring(1)}";
  }
}

extension FutureExtension<T> on Future<T> {
  /// Checks if the future has returned a value, using a Completer.
  bool isCompleted() {
    final completer = Completer<T>();
    then(completer.complete).catchError(completer.completeError);
    return completer.isCompleted;
  }
}

String prettyPrintDouble(double d) {
  if (d == double.infinity) {
    return "+Inf";
  } else if (d == -double.infinity) {
    return "-Inf";
  } else if (d.isNaN) {
    return "NaN";
  } else if (d.abs() < 1.0E-20) {
    return "0";
  }
  if (d < 0) {
    return "-${prettyPrintDouble(-d)}";
  }
  const List<String> suffixes = [
      "p", "n", "μ", "m", "", "k", "M", "G",
      "T", "P", "E", "Z", "Y"];
  int orderOfMagnitude = (log(d) / ln10 / 3).floor();
  // Between 1 and 999.999999
  double rescaledD = d / pow(10, 3 * orderOfMagnitude);
  if (rescaledD > 999.5) {
    orderOfMagnitude++;
    rescaledD = 1;
  }
  int suffixesPosition = orderOfMagnitude + 4;
  if (suffixesPosition >= suffixes.length || suffixesPosition < 0) {
    return d.toStringAsPrecision(1);
  }
  String suffix = suffixes[suffixesPosition];

  if (rescaledD < 9.95) {
    return "${rescaledD.toStringAsFixed(1)}$suffix";
  }
  return "${rescaledD.toStringAsFixed(0)}$suffix";
}

String camelCaseToSpaces(String s) {
  final exp = RegExp('([A-Z]+)');
  String result = s.replaceAllMapped(exp, (m) {
    String? g = m.group(0);
    if (g == null) {
      return '';
    }
    return ' ${g}';
  });
  return result.capitalize();
}
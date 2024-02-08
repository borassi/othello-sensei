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

import 'dart:math';

import 'package:flutter/services.dart';

extension StringExtension on String {
  String capitalize() {
    return "${this[0].toUpperCase()}${substring(1)}";
  }
}

// Taken from https://en.wikipedia.org/wiki/Hamming_weight.
int bitCount(int x) {
  const m1  = 0x5555555555555555; //binary: 0101...
  const m2  = 0x3333333333333333; //binary: 00110011..
  const m4  = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
  const h01 = 0x0101010101010101;

  x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
  x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
  x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
  return (x * h01) >> 56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}

class IntInputFormatter implements TextInputFormatter {
  @override
  TextEditingValue formatEditUpdate(TextEditingValue oldValue, TextEditingValue newValue) {
    try {
      int.parse(newValue.text);
    } on FormatException {
      return oldValue;
    }
    return newValue;
  }
}

class DoubleInputFormatter implements TextInputFormatter {
  @override
  TextEditingValue formatEditUpdate(TextEditingValue oldValue, TextEditingValue newValue) {
    try {
      double.parse(newValue.text);
    } on FormatException {
      return oldValue;
    }
    return newValue;
  }
}

String prettyPrintDouble(double? value) {
  if (value == null) {
    return '-';
  }
  double d = value!;
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

String moveToString(int move) {
  if (move == 255) {
    return '--';
  }
  assert(move >= 0);
  assert(move <= 63);
  return '${String.fromCharCode((7 - move % 8) + "a".codeUnitAt(0))}${8 - move ~/ 8}';
}

T maxIgnoreNaN<T extends num>(T x, T y) {
  return x.isNaN ? y : (y.isNaN ? x : max(x, y));
}

T minIgnoreNaN<T extends num>(T x, T y) {
  return x.isNaN ? y : (y.isNaN ? x : min(x, y));
}
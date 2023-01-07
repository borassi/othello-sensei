/*
 * Copyright 2022 Google LLC
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
 */

#include <chrono>
#include <ctime>
#include <iostream>
#include <locale>
#include "misc.h"

ElapsedTime::ElapsedTime() : start_(std::chrono::system_clock::now()) {}

double ElapsedTime::Get() {
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start_;
  return diff.count();
}

double GaussianCDF(double value) {
  return 0.5 * erfc(-value * sqrt(0.5));
}

double GaussianCDF(double x, double mean, double stddev) {
  return GaussianCDF((x - mean) / stddev);
}

std::string PrettyPrintDouble(double d) {
  std::locale::global(std::locale("en_US.UTF8"));
  if (d == INFINITY) {
    return "+Inf";
  } else if (d == -INFINITY) {
    return "-Inf";
  } else if (isnan(d)) {
    return "NaN";
  } else if (abs(d) < 1.E-20) {
    return "0";
  }
  if (d < 0) {
    return "-" + PrettyPrintDouble(-d);
  }
  int kSuffixLength = 13;
  std::string suffixes[] = {
      "p", "n", "μ", "m", "", "k", "M", "G",
      "T", "P", "E", "Z", "Y"};
  int orderOfMagnitude = (((int) floor(log10(d)) + 300) / 3) - 100;
  // Between 1 and 999.999999
  double rescaledL = d / pow(10, 3 * orderOfMagnitude);
  if (rescaledL > 999.5) {
    orderOfMagnitude++;
    rescaledL = 1;
  }
  int suffixesPosition = orderOfMagnitude + 4;
  char buffer[32];
  if (suffixesPosition >= kSuffixLength || suffixesPosition < 0) {
    snprintf(buffer, sizeof(buffer), "%.1E", d);
    return std::string(buffer);
  }
  std::string suffix = suffixes[suffixesPosition];

  if (rescaledL < 9.95) {
    snprintf(buffer, sizeof(buffer), "%.1f%s", rescaledL, suffix.c_str());
    return std::string(buffer);
  }
  snprintf(buffer, sizeof(buffer), "%.0f%s", rescaledL, suffix.c_str());
  return std::string(buffer);
}
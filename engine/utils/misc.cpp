/*
 * Copyright 2023 Michele Borassi
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
#include <regex>
#include <time.h>
#include "constants.h"
#include "misc.h"

ElapsedTime::ElapsedTime() : start_(std::chrono::system_clock::now()) {}

double ElapsedTime::Get() const {
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start_;
  return diff.count();
}

std::string PrettyPrintDouble(double d) {
//  std::locale::global(std::locale("en_US.UTF8"));
  assert(!isnan(d));
  if (d > DBL_MAX) {
    return "+Inf";
  } else if (d < -DBL_MAX) {
    return "-Inf";
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

std::string Indent(const std::string& s, const std::string& characters) {
  auto except_last = s.substr(0, s.size() - 1);
  return characters + std::regex_replace(
      except_last, std::regex("\n"), "\n" + characters)
      + s.substr(s.size() - 1, s.size());
}

short GetCurrentYear() {
  std::time_t t = std::time(nullptr);
  std::tm time;
  localtime_r(&t, &time);
  return 1900 + time.tm_year;
}

std::string LeftStrip(const std::string& s) {
  std::string result = s;
  result.erase(
      result.begin(),
      std::find_if(
          result.begin(),
          result.end(),
          [](unsigned char c) { return !std::isspace(c); }
      )
  );
  return result;
}

std::string RightStrip(const std::string& s) {
  std::string result = s;
  result.erase(
      std::find_if(
          result.rbegin(),
          result.rend(),
          [](unsigned char c) { return !std::isspace(c); }
      ).base(),
      result.end()
  );
  return result;
}

bool EndsWith(const std::string& s, const std::string& suffix) {
  return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
}

std::string ToLower(const std::string& s) {
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
    [](unsigned char c){ return std::tolower(c); });
  return result;
}

std::vector<std::string> Split(const std::string& s, char c, bool strip) {
  std::stringstream stream(s);
  std::string segment;
  std::vector<std::string> result;

  while(std::getline(stream, segment, c)) {
    if (strip) {
      segment = LeftStrip(RightStrip(segment));
    }
    if (!segment.empty()) {
      result.push_back(segment);
    }
  }
  return result;
}

void PrintSupportedFeatures() {
#if __POPCNT__
  std::cout << "Using intrinsic popcount\n" << std::flush;
#else
  std::cout << "Not using intrinsic popcount\n" << std::flush;
#endif
#if __BMI2__
  std::cout << "Using BMI2 instructions\n" << std::flush;
#else
  std::cout << "Not using BMI2 instructions\n" << std::flush;
#endif
}
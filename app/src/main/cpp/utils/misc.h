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

#ifndef UTILS_MISC_H
#define UTILS_MISC_H

#include <algorithm>
#include <assert.h>
#include <chrono>
#include <limits>
#include <math.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class ElapsedTime {
 public:
  ElapsedTime();
  double Get();

 private:
  std::chrono::time_point<std::chrono::system_clock> start_;
};

constexpr double ConstexprLog(double x) {
  assert(x > 0);
  if (x < 1E-32) {
    return -std::numeric_limits<double>::infinity();
  }
  if (x == std::numeric_limits<double>::infinity()) {
    return std::numeric_limits<double>::infinity();
  }
  double result = 0;
  // Move x between (little less than) 1 / sqrt(2)
  // and (little more than) sqrt(2).
  while (x < 0.701) {
    x *= 2;
    result -= 0.69314718056;
  }
  while (x > 1.5) {
    x /= 2;
    result += 0.69314718056;
  }
  // log(x) = log(1-y) = -SUM_(n=1)^INF y^n / n
  // SUM_(n=n0)^INF y^n / n <= SUM_(n=n0)^INF y^n = y^n0 / (1-y) <= 1/2^(n0-1).
  // For n0 = 31, the error is less than 10^9.
  double y = 1 - x;
  double y_n = 1;
  for (int n = 1; n < 21; ++n) {
    y_n *= y;
    result -= y_n / n;
  }
  return result;
}

constexpr double ConstexprExp(double x) {
  if (x == -std::numeric_limits<double>::infinity()) {
    return 0;
  }
  if (x == std::numeric_limits<double>::infinity()) {
    return std::numeric_limits<double>::infinity();
  }
  double multiplier = 1;
  double result = 1;
  while (x < -0.5) {
    x += 1;
    multiplier /= 2.71828182846;
  }
  while (x > 0.5) {
    x -= 1;
    multiplier *= 2.71828182846;
  }
  // exp(x) = sum(x^n / n!).
  double term = 1;
  for (int n = 1; n < 21; ++n) {
    term *= x / n;
    result += term;
  }
  return result * multiplier;
}

constexpr double ConstexprPow(double x, double y) {
  if (x == 0 && y == 0) {
    return 1;
  }
  return ConstexprExp(ConstexprLog(x) * y);
}

template<typename T>
bool Contains(std::vector<T> v, T elem) {
  return std::find(v.begin(), v.end(), elem) != v.end();
}

template<typename K, typename V>
V GetOrDefault(const std::unordered_map<K, V>& map, K key, V default_value) {
  if (auto iter = map.find(key); iter != map.end()) {
    return iter->second;
  }
  return default_value;
}

struct PairHash {
  template<class T1, class T2>
  std::size_t operator() (const std::pair<T1, T2>& pair) const {
      return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
  }
};

std::string PrettyPrintDouble(double d);

std::string Indent(const std::string& s, const std::string& characters);

#endif  // UTILS_MISC_H
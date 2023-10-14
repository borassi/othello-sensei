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

#include <cassert>
#include <math.h>
#include "../utils/functions.h"
#include "../utils/misc.h"

template<int lambda_100>
constexpr double Exponential(double x) {
  assert(x >= 0 && x <= 1);
  return exp(1 - pow(x, -lambda_100 / 100.0));
}

template<int lambda_100>
double ExpPolyLog(double x) {
  assert(x >= 0 && x <= 1);
  assert(lambda_100 >= 100 && lambda_100 <= 400);
  return -pow(-log(x), lambda_100 / 100.0);
}

template<int lambda_100, int n>
constexpr double Polynomial(double x) {
  assert(lambda_100 > 0);
  assert(lambda_100 / 100.0 <= n / (n-1));
  assert(x >= 0 && x <= 1);
  return lambda_100 / 100.0 * x - (lambda_100 / 100.0 - 1) * pow(x, n);
}

constexpr double Exponential(double x) {
//  double rescaled_x;
//  double exponent = 1.5;
//  if (x < 0.5) {
//    rescaled_x = pow(x, exponent) / pow(0.5, exponent - 1);
//  } else {
//    rescaled_x = 1 - pow(1-x, exponent) / pow(0.5, exponent - 1);
//  }
  return pow(2, 12 * (1-x)) - 1;
}

constexpr double PolyLog(double x) {
//  double rescaled_x;
//  double exponent = 2.5;
//  if (x < 0.5) {
//    rescaled_x = pow(x, exponent) / pow(0.5, exponent - 1);
//  } else {
//    rescaled_x = 1 - pow(1-x, exponent) / pow(0.5, exponent - 1);
//  }
  return pow(-log(x), 1.3);
}

constexpr double ExponentialTimesPolyLog(double x) {
  assert(x >= 0 && x <= 1);
  return -Exponential(x) * PolyLog(x);
}

class ProbCombiner {
 public:
  constexpr ProbCombiner(double (*function)(double x)): function_(function) {
    assert(f(0) == -std::numeric_limits<float>::infinity());
    assert(f(1) == 0);
  }

  constexpr double f(double x) { return function_(x); };

  constexpr double inverse(double y) {
    assert(y <= 0);
    return Inverse(function_, y, 0, 1);
  }

  constexpr double derivative(double x) {
    assert(x >= 0 && x <= 1);
    return Derivative(function_, x);
  }

 private:
  double (*function_)(double x);
};

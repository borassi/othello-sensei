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

class ProbCombiner {
 public:
  constexpr ProbCombiner(double (*function)(double x)): function_(function) {}

  constexpr double f(double x) { return function_(x); };

  constexpr double inverse(double y) {
    assert(y <= 0);
    double l = 0;
    double u = 1;
    while (u - l > 1E-12) {
      double mid = (l + u) / 2;
      double fmid = f(mid);
      if (fmid == y) {
        return mid;
      } else if (fmid < y) {
        l = mid;
      } else {
        u = mid;
      }
    }
    return (l + u) / 2;
  }

  constexpr double derivative(double x) {
    assert(x >= 0 && x <= 1);
    double epsilon = 1E-10;
    if (x < epsilon) {
      return (f(x + epsilon) - f(x)) / epsilon;
    } else if (x > 1 - epsilon) {
      return (f(x) - f(x - epsilon)) / epsilon;
    }
    return (f(x + epsilon) - f(x - epsilon)) / (2 * epsilon);
  }

 private:
  double (*function_)(double x);
};

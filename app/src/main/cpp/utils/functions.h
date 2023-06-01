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
#ifndef OTHELLO_SENSEI_FUNCTIONS_H
#define OTHELLO_SENSEI_FUNCTIONS_H

constexpr double Inverse(double (*function)(double), double y, double l, double u) {
  assert(function(l) <= y && y <= function(u));
  if (function(l) == y) {
    return l;
  } else if (function(u) == y) {
    return u;
  }
  while (u - l > 1E-14) {
    double mid = (l + u) / 2;
    double fmid = function(mid);
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

constexpr double Derivative(double (*function)(double), double x) {
  assert(x >= 0 && x <= 1);
  double epsilon = 1E-12;
  if (x < epsilon) {
    return (function(x + epsilon) - function(x)) / epsilon;
  } else if (x > 1 - epsilon) {
    return (function(x) - function(x - epsilon)) / epsilon;
  }
  return (function(x + epsilon) - function(x - epsilon)) / (2 * epsilon);
}

#endif //OTHELLO_SENSEI_FUNCTIONS_H

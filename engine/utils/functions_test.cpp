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

#include <gtest/gtest.h>
#include <math.h>
#include "functions.h"

class FunctionsTestClass : public testing::TestWithParam<double (*)(double)> {};

TEST_P(FunctionsTestClass, Inverse) {
  auto function = GetParam();
  for (double d = 0; d <= 1; d += 0.01) {
    ASSERT_NEAR(d, Inverse(function, function(d), 0, 1), 1E-8);
  }
}

TEST_P(FunctionsTestClass, Derivative) {
  double epsilon = 1E-12;
  double tolerance = 1E-3;
  auto function = GetParam();

  for (double d = 0.01; d <= 0.99; d += 0.01) {
    ASSERT_NEAR(
        (function(d + epsilon) - function(d)) / epsilon,
        Derivative(function, d),
        Derivative(function, d) * tolerance);
    ASSERT_NEAR(
        (function(d - epsilon) - function(d)) / (-epsilon),
        Derivative(function, d),
        Derivative(function, d) * tolerance);
  }
}

INSTANTIATE_TEST_SUITE_P(FunctionsTestClass,
    FunctionsTestClass,
    testing::Values(
        [](double x) { return 1 - 1 / x; },
        [](double x) { return (pow(x, 0.2) - pow(1-x, 0.2)) / 2 + 0.5; },
        [](double x) { return (pow(x, 2) - pow(1-x, 2)) / 2 + 0.5; },
        [](double x) { return -pow(-log(x), 1.5); }
    )
);
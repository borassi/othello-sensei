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
#include <gmock/gmock.h>
#include "prob_combiners.h"

TEST(ProbCombinerTest, F) {
  ProbCombiner combiner(ExpPolyLog<150>);
  EXPECT_LE(-FLT_MAX, combiner.f(0));
  EXPECT_EQ(0, combiner.f(1));
  for (double d = 0.01; d <= 1; d += 0.01) {
    EXPECT_TRUE(combiner.f(d) > combiner.f(d - 0.01));
    EXPECT_TRUE(combiner.f(d) < 0);
  }
}

TEST(ProbCombinerTest, Inverse) {
  ProbCombiner combiner(ExpPolyLog<150>);
  for (double d = 0.01; d <= 1; d += 0.01) {
    EXPECT_NEAR(d, combiner.inverse(combiner.f(d)), 1E-6);
  }
}

TEST(ProbCombinerTest, Derivative) {
  double epsilon = 1E-10;
  double tolerance = 1E-4;
  ProbCombiner combiner(ExpPolyLog<150>);

  for (double d = 0.01; d <= 0.99; d += 0.01) {
    EXPECT_NEAR(
        (combiner.f(d + epsilon) - combiner.f(d)) / epsilon,
        combiner.derivative(d),
        tolerance);
    EXPECT_NEAR(
        (combiner.f(d - epsilon) - combiner.f(d)) / (-epsilon),
        combiner.derivative(d),
        tolerance);
  }
}
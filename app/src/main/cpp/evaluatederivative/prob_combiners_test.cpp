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

#include <gtest/gmock.h>
#include <gtest/gtest.h>
#include "prob_combiners.h"

class ProbCombinerTestClass : public testing::TestWithParam<ProbCombiner> {};

TEST_P(ProbCombinerTestClass, F) {
  ProbCombiner combiner = GetParam();
  EXPECT_NEAR(0, combiner.f(0), 1E-6);
  EXPECT_NEAR(0, combiner.f(1E-6), 1E-4);
  EXPECT_NEAR(1, combiner.f(1), 1E-6);
  EXPECT_NEAR(1, combiner.f(1 - 1E-6), 1E-4);
  for (double d = 0.01; d <= 1; d += 0.01) {
    EXPECT_TRUE(combiner.f(d) > combiner.f(d - 0.01));
    EXPECT_TRUE(combiner.f(d) > 0);
    EXPECT_TRUE(combiner.f(d) < 1);
  }
}

TEST_P(ProbCombinerTestClass, Inverse) {
  ProbCombiner combiner = GetParam();
  for (double d = 0.01; d <= 1; d += 0.01) {
    EXPECT_NEAR(d, combiner.inverse(combiner.f(d)), 1E-6);
  }
}

TEST_P(ProbCombinerTestClass, Derivative) {
  double epsilon = 1E-10;
  double tolerance = 1E-4;
  ProbCombiner combiner = GetParam();
  EXPECT_NEAR(
      (combiner.f(epsilon) - combiner.f(0)) / epsilon,
      combiner.derivative(0),
      tolerance);
  EXPECT_NEAR(
      (combiner.f(1) - combiner.f(1-epsilon)) / epsilon,
      combiner.derivative(1),
      tolerance);

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

INSTANTIATE_TEST_SUITE_P(ProbCombinerTest,
    ProbCombinerTestClass,
    testing::Values(
        ProbCombiner(Exponential<-30>),
        ProbCombiner(Exponential<-100>),
        ProbCombiner(Polynomial<200, 2>),
        ProbCombiner(Polynomial<120, 2>),
        ProbCombiner(Polynomial<1, 2>),
        ProbCombiner(Polynomial<110, 11>),
        ProbCombiner(ExpPolyLog<100>),
        ProbCombiner(ExpPolyLog<200>),
        ProbCombiner(ExpPolyLog<400>)
    )
);
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
#include <limits>
#include "win_probability.h"

TEST(Gaussian, Gaussian) {
  EXPECT_NEAR(0.5, GaussianCDF(0, 0, 1), 1.E-5);

  // https://en.wikipedia.org/wiki/Standard_normal_table.
  EXPECT_NEAR(1.58655e-1, GaussianCDF(-1, 0, 1), 1.E-4);
  EXPECT_NEAR(2.27501e-2, GaussianCDF(-2, 0, 1), 1.E-4);
  EXPECT_NEAR(1.34990e-3, GaussianCDF(-3, 0, 1), 1.E-4);
  EXPECT_NEAR(3.16712e-5, GaussianCDF(-4, 0, 1), 1.E-6);
  EXPECT_NEAR(2.86652e-7, GaussianCDF(-5, 0, 1), 1.E-8);
  EXPECT_NEAR(9.86588e-10, GaussianCDF(-6, 0, 1), 1.E-10);

  for (int i = 0; i < 100; ++i) {
    EXPECT_NEAR(GaussianCDF(i / 10.0, 0, 1), 1 - GaussianCDF(-i / 10.0, 0, 1), 1E-8);
  }
}

TEST(EvaluationTest, ProbabilityToByte) {
  ASSERT_EQ(ProbabilityToByteExplicit(0), 0);
  ASSERT_EQ(ProbabilityToByteExplicit(1), kProbStep);
  ASSERT_NEAR(ByteToProbabilityExplicit(10), 0, 0.1);

  for (int i = 0; i <= kProbStep; ++i) {
    ASSERT_EQ(ProbabilityToByteExplicit(ByteToProbability((Probability) i)), i);
  }
}

TEST(EvaluationTest, DeltaToCDFOffset) {
  for (Square depth = 1; depth <= 4; ++depth) {
    for (EvalLarge delta = -128 * 8; delta <= 128 * 8; ++delta) {
      for (Square empties = 0; empties < 64; ++empties) {
        int offset = DataToCDFOffset(depth, empties, delta);
        ASSERT_LT(offset, kMaxCDFOffset);
        const auto actual = CDFOffsetToDepthEmptiesEval(offset);
        const auto& actual_depth = std::get<0>(actual);
        const auto& actual_empties = std::get<1>(actual);
        const auto& actual_delta = std::get<2>(actual);
        ASSERT_EQ(actual_depth, depth)
            << (int) depth << " " << (int) actual_depth << "\n"
            << (int) delta << " " << (int) actual_delta << "\n"
            << (int) empties << " " << (int) actual_empties << "\n"
            << offset << "\n";
        ASSERT_EQ(actual_empties, empties);
        ASSERT_EQ(actual_delta, delta)
            << (int) depth << " " << (int) actual_depth << "\n"
            << (int) delta << " " << (int) actual_delta << "\n"
            << (int) empties << " " << (int) actual_empties << "\n"
            << offset << "\n";
      }
    }
  }
}
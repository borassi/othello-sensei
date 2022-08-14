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

#include <gtest/gtest.h>
#include <math.h>
#include <limits>
#include "misc.h"

TEST(ConstexprMath, Log) {
  for (double x : {1E-30, 0.000001, 0.01, 0.5, 1.0, 1.5, 2.0, 10.0, 10000.0, 100000000.0, std::numeric_limits<double>::infinity()}) {
    EXPECT_FLOAT_EQ(ConstexprLog(x), log(x));
  }
}

TEST(ConstexprMath, Exp) {
  for (double x : {-std::numeric_limits<double>::infinity(), -10.5, -4.0, -0.01, 1E-30, 0.0, 0.1, std::numeric_limits<double>::infinity()}) {
    EXPECT_FLOAT_EQ(ConstexprExp(x), exp(x));
  }
}

TEST(ConstexprMath, Pow) {
  for (double x : {0.01, 0.1, 1.0, 2.3, 10.6}) {
    for (double y : {-5.0, -2.0, -1.5, -1.0, -0.01, 0.0, 0.01, 1.0, 10.0}) {
      EXPECT_FLOAT_EQ(ConstexprPow(x, y), pow(x, y));
    }
  }
}

TEST(Gaussian, Gaussian) {
  EXPECT_NEAR(0.5, GaussianCDF(0, 0, 1), 1.E-5);
  // 68-95-99.7 rule.
  EXPECT_NEAR((1-0.6827) / 2, GaussianCDF(-1, 0, 1), 1.E-4);
  EXPECT_NEAR((1-0.9545) / 2, GaussianCDF(-2, 0, 1), 1.E-4);
  EXPECT_NEAR((1-0.9974) / 2, GaussianCDF(-3, 0, 1), 1.E-4);
  EXPECT_NEAR((1-0.99997) / 2, GaussianCDF(-4, 0, 1), 1.E-4);
  EXPECT_NEAR(0, GaussianCDF(-5, 0, 1), 1.E-5);
  EXPECT_NEAR(0, GaussianCDF(-10, 0, 1), 1.E-5);
  EXPECT_NEAR(0, GaussianCDF(-20, 0, 1), 1.E-5);

  EXPECT_NEAR(1-(1-0.6827) / 2, GaussianCDF(1, 0, 1), 1.E-4);
  EXPECT_NEAR(1-(1-0.9545) / 2, GaussianCDF(2, 0, 1), 1.E-4);
  EXPECT_NEAR(1-(1-0.9974) / 2, GaussianCDF(3, 0, 1), 1.E-4);
  EXPECT_NEAR(1-(1-0.99997) / 2, GaussianCDF(4, 0, 1), 1.E-4);
  EXPECT_NEAR(1, GaussianCDF(5, 0, 1), 1.E-5);
  EXPECT_NEAR(1, GaussianCDF(10, 0, 1), 1.E-5);
  EXPECT_NEAR(1, GaussianCDF(20, 0, 1), 1.E-5);

  EXPECT_NEAR(1, GaussianCDF(1, 0, 0.00001), 1.E-5);
  EXPECT_NEAR((1-0.6827) / 2, GaussianCDF(4, 6, 2), 1.E-5);
}

TEST(Vector, VectorContains) {
  EXPECT_TRUE(Contains<int>({1, 2, 3}, 2));
  EXPECT_FALSE(Contains<int>({1, 2, 3}, 4));
}
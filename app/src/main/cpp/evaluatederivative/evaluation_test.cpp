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
#include "evaluation.h"

TEST(EvaluationTest, Base) {
  Evaluation e;
  e.SetLeaf(0.3, 10000, 5000);
  e.AddDescendants(10);
  EXPECT_NEAR(e.ProbGreaterEqual(), 0.3, 1.0 / 500.0);  // 1/255 precision.
  EXPECT_FLOAT_EQ(e.ProofNumber(), 10000);
  EXPECT_FLOAT_EQ(e.DisproofNumber(), 5000);
  EXPECT_FALSE(e.IsSolved());
  EXPECT_NEAR(e.MaxLogDerivative(),
              kLogDerivativeMultiplier * kLeafMultiplier * log(0.3 * 0.7),
              40);

  e.SetProved();
  EXPECT_EQ(e.ProbGreaterEqual(), 1);
  EXPECT_EQ(e.ProofNumber(), 0);
  EXPECT_EQ(e.DisproofNumber(), INFINITY);
  EXPECT_TRUE(e.IsSolved());
  EXPECT_EQ(e.MaxLogDerivative(), kLogDerivativeMinusInf);

  e.SetDisproved();
  EXPECT_EQ(e.ProbGreaterEqual(), 0);
  EXPECT_EQ(e.ProofNumber(), INFINITY);
  EXPECT_EQ(e.DisproofNumber(), 0);
  EXPECT_TRUE(e.IsSolved());
  EXPECT_EQ(e.MaxLogDerivative(), kLogDerivativeMinusInf);

  e.SetLeaf(0, 10000, 5000);
  EXPECT_EQ(e.ProbGreaterEqual(), 0);
  EXPECT_FLOAT_EQ(e.ProofNumber(), 10000);
  EXPECT_FLOAT_EQ(e.DisproofNumber(), 5000);
  EXPECT_FALSE(e.IsSolved());
  EXPECT_EQ(e.MaxLogDerivative(), kLogDerivativeMinusInf);
}
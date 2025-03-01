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
#include <limits>
#include <random>
#include "evaluation.h"

void ExpectProofNumberNear(double proof_number, double value) {
  EXPECT_NEAR(proof_number, value, value * (kBaseLogProofNumber - 1) / 2);
}

//TEST(EvaluationTest, Base) {
//  Evaluation e;
//  e.SetLeaf(0.3, 10000, 5000);
//  EXPECT_NEAR(e.ProbGreaterEqual(), 0.3, 1.0 / 500.0);  // 1/255 precision.
//  ExpectProofNumberNear(e.ProofNumber(), 10000);
//  ExpectProofNumberNear(e.DisproofNumber(), 5000);
//  EXPECT_FALSE(e.IsSolved());
//  EXPECT_NEAR(e.MaxLogDerivative(),
//              kLogDerivativeMultiplier * log(0.3 * 0.7),
//              40);
//
//  e.SetProved();
//  EXPECT_EQ(e.ProbGreaterEqual(), 1);
//  EXPECT_EQ(e.ProofNumber(), 0);
//  EXPECT_EQ(e.DisproofNumber(), INFINITY);
//  EXPECT_TRUE(e.IsSolved());
//  EXPECT_EQ(e.MaxLogDerivative(), kLogDerivativeMinusInf);
//
//  e.SetDisproved();
//  EXPECT_EQ(e.ProbGreaterEqual(), 0);
//  EXPECT_EQ(e.ProofNumber(), INFINITY);
//  EXPECT_EQ(e.DisproofNumber(), 0);
//  EXPECT_TRUE(e.IsSolved());
//  EXPECT_EQ(e.MaxLogDerivative(), kLogDerivativeMinusInf);
//
//  e.SetLeaf(0, 10000, 5000);
//  EXPECT_EQ(e.ProbGreaterEqual(), 0);
//  EXPECT_EQ(e.ProofNumber(), INFINITY);
//  ExpectProofNumberNear(e.DisproofNumber(), 5000);
//  EXPECT_FALSE(e.IsSolved());
//  EXPECT_EQ(e.MaxLogDerivative(), kLogDerivativeMinusInf);
//}

TEST(EvaluationTest, ProofNumberToByte) {
  ASSERT_EQ(ProofNumberToByte(0), 0);
  ASSERT_EQ(ProofNumberToByte(1), 1);
  ASSERT_EQ(ProofNumberToByte(kMaxProofNumber), kProofNumberStep - 1);
  ASSERT_EQ(ProofNumberToByte(DBL_MAX), kProofNumberStep);
  for (int i = 0; i <= kProofNumberStep; ++i) {
    ASSERT_EQ(ProofNumberToByte(ByteToProofNumber((PN) i)), i);
  }
  for (int i = 0; i < 1000; ++i) {
    float x = (float) exp((double) rand() / RAND_MAX * log(kMaxProofNumber));
    PN byte = ProofNumberToByte(x);
    EXPECT_NE(byte, 0);
    EXPECT_NE(byte, kProofNumberStep);
    float converted_x = ByteToProofNumber(byte);
    EXPECT_GT(converted_x, x / ((1.01 + kBaseLogProofNumber) / 2));
    EXPECT_LT(converted_x, x * ((1.01 + kBaseLogProofNumber) / 2));
  }
}



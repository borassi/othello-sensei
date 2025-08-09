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
#include <random>

#include "move_selector.h"

TEST(MoveSelector, GetMoveMultiplier) {
  double sum_multipliers = 0;
  for (int i = 0; i < 60; ++i) {
    sum_multipliers += GetMoveMultiplier(i);
  }
  EXPECT_DOUBLE_EQ(sum_multipliers, 2);
}

TEST(MoveSelector, FindBestChildNone) {
  EXPECT_EQ(FindNextMove({}, 2, DBL_MAX), nullptr);
}

class MoveSelectorTestClass : public testing::TestWithParam<double> {};

INSTANTIATE_TEST_SUITE_P(MoveSelectorTestClass, MoveSelectorTestClass,
    testing::Values(0.0, 1.0, 2.0, 3.0, 6.0));

TEST_P(MoveSelectorTestClass, FindBestChildTwo) {
  TreeNode n1;
  n1.Reset(Board("e6f4"), 1, 1);
  n1.SetLeafEval(0, 4);
  n1.UpdateLeafWeakLowerUpper(-63, 63);
  TreeNode n2;
  n2.Reset(Board("e6f6"), 1, 1);
  n2.SetLeafEval(6 * 8, 4);
  n2.UpdateLeafWeakLowerUpper(-63, 63);

  std::vector<const Node*> nodes = {&n1, &n2};
  double error = 0;
  double expected_error = GetParam();
  int n = 10000;

  for (int i = 0; i < n; ++i) {
    const Node* best = FindNextMove(nodes, expected_error, DBL_MAX);
    if (best == &n2) {
      error += 6;
    }
  }
  EXPECT_NEAR(error / n, std::min(expected_error, 3.0), 0.05);
}

TEST_P(MoveSelectorTestClass, FindBestChildThree) {
  TreeNode n1;
  n1.Reset(Board("e6f4"), 1, 1);
  n1.SetLeafEval(0, 4);
  n1.UpdateLeafWeakLowerUpper(-63, 63);
  TreeNode n2;
  n2.Reset(Board("e6f6"), 1, 1);
  n2.SetLeafEval(3 * 8, 4);
  n2.UpdateLeafWeakLowerUpper(-63, 63);
  TreeNode n3;
  n3.Reset(Board("e6d6"), 1, 1);
  n3.SetLeafEval(6 * 8, 4);
  n3.UpdateLeafWeakLowerUpper(-63, 63);

  std::vector<const Node*> nodes = {&n1, &n2, &n3};
  double expected_error = GetParam();
  int n = 20000;
  int n1_best = 0;
  int n2_best = 0;
  int n3_best = 0;

  for (int i = 0; i < n; ++i) {
    const Node* best = FindNextMove(nodes, expected_error, DBL_MAX);
    if (best == &n1) {
      ++n1_best;
    } else if (best == &n2) {
      ++n2_best;
    } else if (best == &n3) {
      ++n3_best;
    }
  }
  EXPECT_NEAR((n2_best * 3 + n3_best * 6) / (double) n, std::min(expected_error, 3.0), 0.05) << n1_best << " " << n2_best << " " << n3_best;
  if (expected_error > 0) {
    EXPECT_NEAR(n1_best / (double) n2_best, n2_best / (double) n3_best, 0.15) << n1_best << " " << n2_best << " " << n3_best;
  }
}


TEST_P(MoveSelectorTestClass, FindBestChildThreeExcludeOne) {
  TreeNode n1;
  n1.Reset(Board("e6f4"), 1, 1);
  n1.SetLeafEval(0, 4);
  n1.UpdateLeafWeakLowerUpper(-63, 63);
  TreeNode n2;
  n2.Reset(Board("e6f6"), 1, 1);
  n2.SetLeafEval(6 * 8, 4);
  n2.UpdateLeafWeakLowerUpper(-63, 63);
  TreeNode n3;
  n3.Reset(Board("e6d6"), 1, 1);
  n3.SetLeafEval(7 * 8, 4);
  n3.UpdateLeafWeakLowerUpper(-63, 63);

  std::vector<const Node*> nodes = {&n1, &n2, &n3};
  double error = 0;
  double expected_error = GetParam();
  int n = 10000;

  for (int i = 0; i < n; ++i) {
    const Node* best = FindNextMove(nodes, expected_error, 7);
    ASSERT_FALSE(best == &n3);
    if (best == &n2) {
      error += 6;
    }
  }
  EXPECT_NEAR(error / n, std::min(expected_error, 3.0), 0.05);
}
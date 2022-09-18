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
#include "tree_node.h"

// EvalGoal: +20.
//                  e6         e6
//                  +20   ->   +20
//                 30 1040   +16 +66
//               1001   10   +16 +66
//               ____|________________________________
//              |                        |            |
//             e6f4      e6f4           e6f6         e6d6
//             -20   ->  -20            -14          -12
//            10  1K   -66 -16          0   0        0  1
//          1040  30   -66 -16        INF INF      INF 1K
//          _____|______
//         |         ___|___
//       e6f4c3     |e6f4e3 |     e6f4e3 >= +22
//        +20       |  +24  | ->    +24
//        0  1K     | 30 40 |      0   0
//      INF  10     | 1K 1K |    INF INF
//     ____|_____   |_______|
//    |          |
// e6f4c3c4  e6f4c3d6 <=-14
//    -20       -16
//    0 INF    10  20
//  INF   0    1K  1K
TEST(TreeNodeTest, OneThread) {
  TreeNode e6;
  e6.Reset(Board("e6"), 0, 1);
  e6.SetLeaf(-5, 1, 1, 4, 40);
  EXPECT_EQ(e6.Lower(), -64);
  EXPECT_EQ(e6.Upper(), 64);
  EXPECT_EQ(e6.WeakLower(), -5);
  EXPECT_EQ(e6.WeakUpper(), 1);
  EXPECT_EQ(e6.GetNVisited(), 40);
  EXPECT_NEAR(e6.GetEvaluation(1).ProbGreaterEqual(), 0.5, 1.01 / kProbStep);
  EXPECT_EQ(e6.GetNVisited(), 40);
  TreeNode e6f4;
  e6f4.Reset(Board("e6f4"), 1, 1);
  e6f4.SetLeaf(-1, 5, 0, 4, 10);
  TreeNode e6f6;
  e6f6.Reset(Board("e6f6"), 1, 1);
  e6f6.SetLeaf(-1, 5, 0, 4, 10);
  TreeNode e6d6;
  e6d6.Reset(Board("e6d6"), 1, 1);
  e6d6.SetLeaf(-1, 5, -8, 4, 10);
  e6.SetChildren({&e6f4, &e6f6, &e6d6});
  #ifndef NDEBUG
  EXPECT_THROW(e6.SetChildren({&e6f4, &e6f6, &e6f6}), ChildError);
  EXPECT_THROW(e6.SetChildren({&e6f4, &e6f6}), ChildError);
  EXPECT_THROW(e6.SetChildren({&e6f4, &e6f6, &e6d6, &e6d6}), ChildError);
  #endif
  e6.UpdateFathers();
  EXPECT_EQ(e6.GetNVisited(), 40);
}
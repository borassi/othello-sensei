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

 #include "book_tree_node.h"

std::ostream& operator<<(std::ostream& stream, const BookTreeNode& b) {
  Board board = b.ToBoard();
  stream << b.Player() << " " << b.Opponent() << ": ["
         << (int) b.Lower() << " " << (int) b.Upper() << "]";
  for (int i = b.WeakLower(); i < b.WeakUpper(); i += 2) {
    float p = b.GetEvaluation(i).ProbGreaterEqual();
    if (p != 0 && p != 1) {
      stream << std::setprecision(2) << " " << i << ":" << p;
    }
  }
  stream << " (vis: " << b.GetNVisited() << ")";
  return stream;
}
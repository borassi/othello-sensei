/*
 * Copyright 2025 Michele Borassi
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

#include "../board_optimized/board_optimized.h"
#include "../board_optimized/play_move.h"

Square empties[64];
Square n_empties;
uint64_t n_visited = 0;

int EvaluateRecursive(bool passed) {
  ++n_visited;
  if (n_empties == 0) {
    return turn * disk_difference;
  }
  int best_eval = -66;
  for (int i = n_empties - 1; i >= 0; --i) {
    Square move = empties[i];
    play_move[move]();
    if (flipped) {
      empties[i] = empties[n_empties - 1];
      --n_empties;
      best_eval = std::max(best_eval, -EvaluateRecursive(false));
      Undo();
      ++n_empties;
      empties[i] = move;
    }
  }
  if (best_eval == -66) {
    if (passed) {
      return turn * disk_difference + ((turn * disk_difference) > 0 ? 1 : -1) * n_empties;
    }
    turn = -turn;
    best_eval = -EvaluateRecursive(true);
    turn = -turn;
  }
  return best_eval;
}

int Evaluate(BitPattern player, BitPattern opponent) {
  n_empties = 0;
  BitPattern empties_pattern = ~(player | opponent);
  FOR_EACH_SET_BIT(empties_pattern, square_pattern) {
    Square square = (Square) __builtin_ctzll(square_pattern);
    empties[n_empties++] = square;
  }
  BoardToPatterns(player, opponent);
  return EvaluateRecursive(false);
}
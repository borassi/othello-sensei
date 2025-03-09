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

#include "../board/get_flip.h"
#include "../board_optimized/board_optimized.h"
#include "../board_optimized/play_move.h"

Square empties[64];
uint64_t n_visited = 0;

constexpr int NextNEmpties(int n_empties) {
  return std::min(64, std::max(0, n_empties - 1));
}

template<int n_empties>
int EvaluateRecursive(bool passed) {
  ++n_visited;
  if (n_empties == 0) {
    return disk_difference;
  }
  int best_eval = -66;
  for (int i = n_empties - 1; i >= 0; --i) {
    Square move = empties[i];
    play_move[move]();
    if (flipped) {
      empties[i] = empties[n_empties - 1];
      best_eval = std::max(best_eval, -EvaluateRecursive<NextNEmpties(n_empties)>(false));
      Undo();
      empties[i] = move;
    }
  }
  if (best_eval == -66) {
    if (passed) {
      return disk_difference + (disk_difference > 0 ? 1 : -1) * n_empties;
    }
    Invert();
    best_eval = -EvaluateRecursive<n_empties>(true);
    Invert();
  }
  return best_eval;
}

int Evaluate(BitPattern player, BitPattern opponent) {
  int n_empties = 0;
  BitPattern empties_pattern = ~(player | opponent);
  FOR_EACH_SET_BIT(empties_pattern, square_pattern) {
    Square square = (Square) __builtin_ctzll(square_pattern);
    empties[n_empties++] = square;
  }
  BoardToPatterns(player, opponent);
  switch (n_empties) {
    case 0:
      return EvaluateRecursive<0>(false);
    case 1:
      return EvaluateRecursive<1>(false);
    case 2:
      return EvaluateRecursive<2>(false);
    case 3:
      return EvaluateRecursive<3>(false);
    case 4:
      return EvaluateRecursive<4>(false);
    case 5:
      return EvaluateRecursive<5>(false);
    case 6:
      return EvaluateRecursive<6>(false);
    case 7:
      return EvaluateRecursive<7>(false);
    case 8:
      return EvaluateRecursive<8>(false);
    case 9:
      return EvaluateRecursive<9>(false);
    case 10:
      return EvaluateRecursive<10>(false);
    case 11:
      return EvaluateRecursive<11>(false);
  }
  return 0;
}

template<int n_empties>
int EvaluateRecursiveStandard(BitPattern player, BitPattern opponent, bool passed) {
  ++n_visited;
  if (n_empties == 0) {
    return 2 * __builtin_popcountll(player) - 64;
  }
  int best_eval = -66;
  for (int i = n_empties - 1; i >= 0; --i) {
    Square move = empties[i];
    BitPattern flip = GetFlip(move, player, opponent);
    if (flip) {
      empties[i] = empties[n_empties - 1];
      BitPattern new_player = NewPlayer(flip, opponent);
      BitPattern new_opponent = NewOpponent(flip, player);
      best_eval = std::max(best_eval, -EvaluateRecursiveStandard<NextNEmpties(n_empties)>(new_player, new_opponent, false));
      empties[i] = move;
    }
  }
  if (best_eval == -66) {
    if (passed) {
      int delta = __builtin_popcountll(player) - __builtin_popcountll(opponent);
      return delta + ((delta > 0) - (delta < 0)) * n_empties;
    }
    best_eval = -EvaluateRecursiveStandard<n_empties>(opponent, player, true);
  }
  return best_eval;
}

int EvaluateStandard(BitPattern player, BitPattern opponent) {
  int n_empties = 0;
  BitPattern empties_pattern = ~(player | opponent);
  FOR_EACH_SET_BIT(empties_pattern, square_pattern) {
    Square square = (Square) __builtin_ctzll(square_pattern);
    empties[n_empties++] = square;
  }
  switch (n_empties) {
    case 0:
      return EvaluateRecursiveStandard<0>(player, opponent, false);
    case 1:
      return EvaluateRecursiveStandard<1>(player, opponent, false);
    case 2:
      return EvaluateRecursiveStandard<2>(player, opponent, false);
    case 3:
      return EvaluateRecursiveStandard<3>(player, opponent, false);
    case 4:
      return EvaluateRecursiveStandard<4>(player, opponent, false);
    case 5:
      return EvaluateRecursiveStandard<5>(player, opponent, false);
    case 6:
      return EvaluateRecursiveStandard<6>(player, opponent, false);
    case 7:
      return EvaluateRecursiveStandard<7>(player, opponent, false);
    case 8:
      return EvaluateRecursiveStandard<8>(player, opponent, false);
    case 9:
      return EvaluateRecursiveStandard<9>(player, opponent, false);
    case 10:
      return EvaluateRecursiveStandard<10>(player, opponent, false);
    case 11:
      return EvaluateRecursiveStandard<11>(player, opponent, false);
  }
  return 0;
}
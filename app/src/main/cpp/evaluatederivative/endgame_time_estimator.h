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

#include <math.h>
#include "evaluation.h"
#include "../board/bitpattern.h"
#include "../board/get_moves.h"

#ifndef OTHELLOSENSEI_ENDGAME_TIME_ESTIMATOR_H
#define OTHELLOSENSEI_ENDGAME_TIME_ESTIMATOR_H

constexpr double kErrorWeight = 0.1;

inline double Bound(double value) {
  return std::max(1.0, std::min(kMaxProofNumber * 0.99, value));
}

inline double ConvertProofNumber(double old, Eval old_goal, Eval new_goal) {
  assert(new_goal < old_goal);
  return Bound(exp(log(old) + 0.07 * (new_goal - old_goal)));
}

inline double ConvertDisproofNumber(double old, int old_goal, int new_goal) {
  assert(new_goal > old_goal);
  return Bound(exp(log(old) - 0.07 * (new_goal - old_goal)));
}

inline double LogProofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  BitPattern empties = ~(player | opponent);
  int n_empties = __builtin_popcountll(empties);
  EvalLarge error = approx_eval - lower;
  if (n_empties < 6) {
    return +0.0896 * 1+0.4480 * n_empties+0.0573 * GetNMoves(opponent, player)+0.2046 * GetNMoves(player, opponent)-0.0306 * error / 8;
  } else if (n_empties < 12) {
    return +0.9478 * 1+0.3839 * n_empties+0.0636 * GetNMoves(opponent, player)+0.1609 * GetNMoves(player, opponent)-0.0555 * error / 8;
  } else if (n_empties < 18) {
    return +0.9345 * 1+0.4647 * n_empties+0.2014 * GetNMoves(opponent, player)+0.1419 * GetNMoves(player, opponent)-0.0908 * error / 8;
  } else if (n_empties < 24) {
    return -1.9567 * 1+0.6265 * n_empties+0.3195 * GetNMoves(opponent, player)+0.0003 * GetNMoves(player, opponent)-0.0826 * error / 8;
  } else {
    return -2.6000 * 1+0.6429 * n_empties+0.3346 * GetNMoves(opponent, player)-0.0196 * GetNMoves(player, opponent)-0.0706 * error / 8;
  }
}

inline double ProofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return Bound(exp(LogProofNumber(player, opponent, lower, approx_eval)));
}

inline double LogDisproofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  BitPattern empties = ~(player | opponent);
  int n_empties = __builtin_popcountll(empties);
  EvalLarge error = approx_eval - lower;
  if (n_empties < 6) {
    return +0.1115 * 1+0.5576 * n_empties+0.1452 * GetNMoves(opponent, player)+0.1551 * GetNMoves(player, opponent)+0.0455 * error / 8;
  } else if (n_empties < 12) {
    return +0.8592 * 1+0.3123 * n_empties+0.2258 * GetNMoves(opponent, player)+0.2923 * GetNMoves(player, opponent)+0.0639 * error / 8;
  } else if (n_empties < 18) {
    return +1.6465 * 1+0.3654 * n_empties+0.1477 * GetNMoves(opponent, player)+0.3816 * GetNMoves(player, opponent)+0.0931 * error / 8;
  } else if (n_empties < 24) {
    return -1.0281 * 1+0.5698 * n_empties+0.0262 * GetNMoves(opponent, player)+0.4077 * GetNMoves(player, opponent)+0.0910 * error / 8;
  } else {
    return -2.9501 * 1+0.6321 * n_empties-0.0400 * GetNMoves(opponent, player)+0.4972 * GetNMoves(player, opponent)+0.0830 * error / 8;
  }
}

inline double DisproofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return Bound(exp(LogDisproofNumber(player, opponent, lower, approx_eval)));
}
#endif  // OTHELLOSENSEI_ENDGAME_TIME_ESTIMATOR_H
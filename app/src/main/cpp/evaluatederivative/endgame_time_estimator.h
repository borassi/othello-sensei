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
  return Bound(exp(log(old) + kErrorWeight * (new_goal - old_goal)));
}

inline double ConvertDisproofNumber(double old, int old_goal, int new_goal) {
  assert(new_goal > old_goal);
  return Bound(exp(log(old) - kErrorWeight * (new_goal - old_goal)));
}

inline double LogProofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  BitPattern empties = ~(player | opponent);
  int n_empties = __builtin_popcountll(empties);
  EvalLarge error = approx_eval - lower;
  return -3.2519 + 0.5526 * n_empties + 2.3102 * log(1 + GetNMoves(opponent,
                                                                player))
      -kErrorWeight / 8 * error;
}

inline double ProofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return Bound(exp(LogProofNumber(player, opponent, lower, approx_eval)));
}

inline double LogDisproofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  BitPattern empties = ~(player | opponent);
  int n_empties = __builtin_popcountll(empties);
  EvalLarge error = approx_eval - lower;
  return -3.7256 + 0.5757 * n_empties + 2.5578 * log(1 + GetNMoves(player,opponent))
      +kErrorWeight / 8 * error;
}

inline double DisproofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return Bound(exp(LogDisproofNumber(player, opponent, lower, approx_eval)));
}
#endif  // OTHELLOSENSEI_ENDGAME_TIME_ESTIMATOR_H
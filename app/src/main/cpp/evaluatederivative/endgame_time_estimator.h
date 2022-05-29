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
#include "../board/bitpattern.h"
#include "../board/get_moves.h"

double LogProofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  BitPattern empties = ~(player | opponent);
  int n_empties = __builtin_popcountll(empties);
  return -3.29 + 0.5395 * n_empties + 2.4157 * log(1 + GetNMoves(opponent, player))
      -0.0615 / 8 * (approx_eval - lower);
}

double ProofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return std::max(1.0, std::min(1E30, exp(LogProofNumber(player, opponent, lower, approx_eval))));
}

double LogDisproofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  BitPattern empties = ~(player | opponent);
  int n_empties = __builtin_popcountll(empties);
  return -3.6659 + 0.5501 * n_empties + 2.7047 * log(1 + GetNMoves(player, opponent))
      -0.0590 / 8 * (lower - approx_eval);
}

double DisproofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return std::max(1.0, std::min(1E30, exp(LogDisproofNumber(player, opponent, lower, approx_eval))));
}
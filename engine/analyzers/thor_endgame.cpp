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

#include <cassert>
#include <iostream>
#include <cmath>
#include "../board/bitpattern.h"
#include "../board/get_moves.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatealphabeta/test_evaluator.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/load_training_set.h"
#include "../utils/misc.h"

class EvaluateThor {
 public:
  EvaluateThor(const int8_t* const evals) :
      num_boards_(0),
      n_visited_(0),
      elapsed_time_(),
      evals_(evals),
      evaluator_(&hash_map_, PatternEvaluator::Factory(evals)) {
    for (const auto& board : load_test_set()) {
      if (board.Empties() != 14
          || HaveToPass(board.GetPlayer(), board.GetOpponent())) {
        continue;
      }
      boards_.push_back(board);
    }
  }

  void Run(int stop_after) {
    int alpha = kMinEvalLarge;
    int beta = kMaxEvalLarge;

    for (const EvaluatedBoard& board : boards_) {
      EvalLarge eval = evaluator_.Evaluate(board.GetPlayer(), board.GetOpponent(), board.Empties(), alpha, beta);
      assert(eval <= alpha || eval >= beta || eval == EvalToEvalLarge(board.GetEval()));
      num_boards_++;
      n_visited_ += evaluator_.GetNVisited();
      if (num_boards_ % 100 == 0) {
        Print();
        if (elapsed_time_.Get() > stop_after) {
          return;
        }
      }
    }
    Print();
  }

  NVisited GetNVisited() const { return n_visited_; }

  void Print() {
    double time = elapsed_time_.Get();
    std::cout << "After " << num_boards_ << ":\n";
    std::cout << "  Positions: " << n_visited_ << "\n";
    std::cout << "  Positions / test: " << n_visited_ / num_boards_ << "\n";
    std::cout << "  Positions / sec: " << std::fixed << n_visited_ / time
              << "\n";
    std::cout << "  Total time: " << time << "\n";
  }
 private:
  int num_boards_;
  std::vector<EvaluatedBoard> boards_;
  NVisited n_visited_;
  ElapsedTime elapsed_time_;
  EvaluatorAlphaBeta evaluator_;
  HashMap<kBitHashMap> hash_map_;
  const int8_t* const evals_;
};
//
int main() {
  const std::vector<int8_t> evals = LoadEvals();
  EvaluateThor evaluate_thor(evals.data());
  evaluate_thor.Run(20);
  evaluate_thor.Print();
  return 0;
}
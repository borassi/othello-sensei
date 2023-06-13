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

#include <cmath>
#include <iomanip>
#include <iostream>
#include "../board/bitpattern.h"
#include "../board/get_moves.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatealphabeta/test_evaluator.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/load_training_set.h"
#include "../utils/misc.h"

class Evaluator {
 public:
  virtual EvalLarge operator()(BitPattern player, BitPattern opponent) = 0;
  virtual NVisited GetNVisited() const = 0;
};

class EvaluateDepth0 : public Evaluator {
 public:
  explicit EvaluateDepth0(const int8_t* const evals) : evals_(evals) {}
  EvalLarge operator() (BitPattern player, BitPattern opponent) override {
    PatternEvaluator eval(evals_);
    eval.Setup(player, opponent);
    return eval.Evaluate();
  }
  NVisited GetNVisited() const override { return 1; }

 private:
  const int8_t* const evals_;
};

class EvaluateInDepth : public Evaluator {
 public:
  EvaluateInDepth(const int8_t* const evals, int depth) :
      depth_(depth),
      evals_(evals),
      test_evaluator_(&hash_map_, PatternEvaluator::Factory(evals)) {}

  EvalLarge operator()(BitPattern player, BitPattern opponent) override {
    return test_evaluator_.Evaluate(player, opponent, depth_);
  }
  NVisited GetNVisited() const override { return test_evaluator_.GetNVisited(); }

 private:
  DepthValue depth_;
  EvaluatorAlphaBeta test_evaluator_;
  HashMap<kBitHashMap> hash_map_;
  const int8_t* const evals_;
};

class EvaluateThor {
 public:
  EvaluateThor() :
      boards_(load_test_set()),
      sum_error_squared_(0),
      num_boards_(0),
      n_visited_(0),
      elapsed_time_() {
    for (int i = 0; i < 60; ++i) {
      positions_with_empties_[i] = 0;
      sum_error_squared_for_empty_[i] = 0;
    }
  }

  void Run(Evaluator* evaluator, int min_error_print, int stop_after) {
    sum_error_squared_ = 0;
    num_boards_ = 0;
    n_visited_ = 0;
    int i = 0;

    for (EvaluatedBoard board : boards_) {
      if (board.Empties() < 4 || board.Empties() > 60 ||
          HaveToPass(board.GetPlayer(), board.GetOpponent())) {
        continue;
      }
      EvalLarge eval = (*evaluator)(board.GetPlayer(), board.GetOpponent());
      double error = eval / 8.0 - board.GetEval();
//      if ((error > min_error_print || error < -min_error_print)
//          && (double) rand() / RAND_MAX < 0.001) {
//        PatternEvaluator eval;
//        eval.Setup(board.GetPlayer(), board.GetOpponent());
//        std::cout << board.GetBoard() << "\n";
//        double result = eval.EvaluateBase<true>() / 8.0;
//        std::cout << result << " " << board.GetEval() << "\n\n";
//      }
      sum_error_squared_ += error * error;
      positions_with_empties_[board.Empties()]++;
      sum_error_squared_for_empty_[board.Empties()] += error * error;
      num_boards_++;
      n_visited_ += evaluator->GetNVisited();
      if (++i % 5000 == 0 && elapsed_time_.Get() > stop_after) {
        Print();
        return;
      }
    }
  }

  NVisited GetNVisited() const { return n_visited_; }

  void Print() {
    double time = elapsed_time_.Get();
    std::cout << "After " << num_boards_ << ":\n";
    std::cout << "  Error: " << sqrt(sum_error_squared_ / num_boards_) << "\n";
    std::cout << "  Positions: " << n_visited_ << "\n";
    std::cout << "  Positions / test: " << n_visited_ / num_boards_ << "\n";
    std::cout << "  Positions / sec: " << std::fixed << n_visited_ / time << "\n";
    std::cout << "  Total time: " << time << "\n";
    std::cout << "  Error at empties: {";
    for (int i = 0; i < 60; ++i) {
      std::cout << std::setprecision(2) << std::max(2.0, sqrt(sum_error_squared_for_empty_[i] / positions_with_empties_[i])) << ", ";
    }
    std::cout << "}\n";
  }
 private:
  double sum_error_squared_;
  int num_boards_;
  std::vector<EvaluatedBoard> boards_;
  NVisited n_visited_;
  ElapsedTime elapsed_time_;
  int positions_with_empties_[60];
  double sum_error_squared_for_empty_[60];
};

int main() {
  const std::vector<int8_t> evals = LoadEvals();
  EvaluateDepth0 eval_depth_0(evals.data());
  EvaluateInDepth eval_in_depth(evals.data(), 4);
  EvaluateThor evaluate_thor;
  evaluate_thor.Run(&eval_in_depth, 10000, 20);
  evaluate_thor.Print();
  return 0;
}
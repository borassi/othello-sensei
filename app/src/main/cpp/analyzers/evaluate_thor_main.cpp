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

#include <iostream>
#include <cmath>
#include "../board/bitpattern.h"
#include "../board/get_moves.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluateindepth/test_evaluator.h"
#include "../utils/load_training_set.h"
#include "../utils/misc.h"

class Evaluator {
 public:
  virtual EvalLarge operator()(BitPattern player, BitPattern opponent) = 0;
  virtual NVisited GetNVisited() const = 0;
};

class EvaluateDepth0 : public Evaluator {
 public:
  EvalLarge operator() (BitPattern player, BitPattern opponent) override {
    PatternEvaluator eval;
    eval.Setup(player, opponent);
    return eval.Evaluate();
  }
  NVisited GetNVisited() const override { return 1; }
};

class EvaluateInDepth : public Evaluator {
 public:
  explicit EvaluateInDepth(int depth) :
      depth_(depth),
      depth_one_evaluator_(),
      test_evaluator_(&depth_one_evaluator_) {}

  EvalLarge operator()(BitPattern player, BitPattern opponent) override {
    return test_evaluator_.Evaluate(player, opponent, depth_);
  }
  NVisited GetNVisited() const override { return test_evaluator_.GetNVisited(); }
 private:
  DepthValue depth_;
  PatternEvaluator depth_one_evaluator_;
  TestEvaluator test_evaluator_;
};

class EvaluateThor {
 public:
  EvaluateThor() :
      boards_(load_test_set()),
      sum_error_squared_(0),
      num_boards_(0),
      n_visited_(0),
      elapsed_time_() {}

  void Run(Evaluator* evaluator, int min_error_print, int print_every) {
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
      if ((error > min_error_print || error < -min_error_print)
          && (double) rand() / RAND_MAX < 0.001) {
        PatternEvaluator eval;
        eval.Setup(board.GetPlayer(), board.GetOpponent());
        std::cout << board.GetBoard().ToString() << "\n";
        double result = eval.EvaluateBase<true>() / 8.0;
        std::cout << result << " " << board.GetEval() << "\n\n";
      }
      sum_error_squared_ += error * error;
      num_boards_++;
      n_visited_ += evaluator->GetNVisited();
      if (++i % print_every == 0) {
        Print();
      }
    }
  }

  NVisited GetNVisited() const { return n_visited_; }

  void Print() {
    double time = elapsed_time_.Get();
    std::cout << "After " << num_boards_ << ":\n";
    std::cout << "  Error: " << sqrt(sum_error_squared_ / num_boards_) << "\n";
    std::cout << "  Positions: " << n_visited_ << "\n";
    std::cout << "  Positions / sec: " << (double) n_visited_ / time << "\n";
    std::cout << "  Total time: " << time << "\n";
  }
 private:
  double sum_error_squared_;
  int num_boards_;
  std::vector<EvaluatedBoard> boards_;
  NVisited n_visited_;
  ElapsedTime elapsed_time_;
};

int main() {
  EvaluateDepth0 eval_depth_0;
  EvaluateInDepth eval_in_depth(4);
  EvaluateThor evaluate_thor;
  evaluate_thor.Run(&eval_in_depth, 10000, 1000);
  evaluate_thor.Print();
  return 0;
}
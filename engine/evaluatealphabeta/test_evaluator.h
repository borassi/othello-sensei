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

#ifndef EVALUATE_IN_DEPTH_TEST_EVALUATOR_H
#define EVALUATE_IN_DEPTH_TEST_EVALUATOR_H

#include <math.h>
#include <memory>
#include "../board/get_moves.h"
#include "../evaluatedepthone/evaluator_depth_one_base.h"
#include "../evaluatedepthone/test_evaluator_depth_one.h"

class TestEvaluator {
 public:
  TestEvaluator(EvaluatorFactory evaluator_depth_one_factory) :
      n_visited_(0),
      depth_one_evaluator_(evaluator_depth_one_factory()) {}

  NVisited GetNVisited() const { return n_visited_; }

  EvalLarge Evaluate(BitPattern player, BitPattern opponent, int depth) {
    n_visited_ = 0;
    depth_one_evaluator_->Setup(player, opponent);
    return EvaluateInternal(player, opponent, depth);
  }
 private:

  EvalLarge EvaluateInternal(BitPattern player, BitPattern opponent, int depth);

  NVisited n_visited_;
  std::unique_ptr<EvaluatorDepthOneBase> depth_one_evaluator_;
};

#endif  // EVALUATE_IN_DEPTH_TEST_EVALUATOR_H
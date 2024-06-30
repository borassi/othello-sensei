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

#ifndef EVALUATE_DEPTH_ONE_TEST_EVALUATOR_DEPTH_ONE_H
#define EVALUATE_DEPTH_ONE_TEST_EVALUATOR_DEPTH_ONE_H

#include <utility>
#include "evaluator_depth_one_base.h"
#include "../board/board.h"

/**
 * Test-only evaluator that returns the disk difference plus 2.
 */

class TestEvaluatorDepthOne : public EvaluatorDepthOneBase {
 private:
  BitPattern player_;
  BitPattern opponent_;

 public:
  static EvaluatorFactory Factory() {
    return []() { return std::make_unique<TestEvaluatorDepthOne>(); };
  }
  ~TestEvaluatorDepthOne() {}
  EvalLarge Evaluate() const {
    return (EvalLarge) (8 * (__builtin_popcountll(player_) - __builtin_popcountll(opponent_) + 2));
  }

  void Setup(BitPattern player, BitPattern opponent) {
    player_ = player;
    opponent_ = opponent;
  }

  void Update(BitPattern square, BitPattern flip) {
    assert(__builtin_popcountll(square) == 1);
    player_ = NewPlayer(flip, player_);
    opponent_ = NewOpponent(flip, opponent_);
  }

  void UndoUpdate(BitPattern square, BitPattern flip) {
    assert(__builtin_popcountll(square) == 1);
    player_ = (player_ | flip) & ~square;
    opponent_ = (opponent_ & ~flip);
  }

  void Invert() {
    BitPattern tmp = player_;
    player_ = opponent_;
    opponent_ = tmp;
  }
};

#endif  // EVALUATE_DEPTH_ONE_TEST_EVALUATOR_DEPTH_ONE_H
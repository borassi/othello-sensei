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

#ifndef LOAD_TRAINING_SET_H
#define LOAD_TRAINING_SET_H

#include <iostream>
#include "../board/bitpattern.h"
#include "../board/board.h"

using ::std::istream;

typedef int EvalApprox;

class EvaluatedBoard {
 public:
  EvaluatedBoard() : player_(0), opponent_(0), eval_(0) {}
  EvaluatedBoard(BitPattern player, BitPattern opponent, EvalApprox eval)
      : player_(player), opponent_(opponent), eval_(eval) {}

  Board GetBoard() const {
    return Board(player_, opponent_);
  }

  BitPattern GetPlayer() const {
    return player_;
  }

  BitPattern GetOpponent() const {
    return opponent_;
  }

  int GetEval() const {
    return eval_;
  }

  int Empties() const {
    return (int) __builtin_popcountll(~(player_ | opponent_));
  }

  bool IsValid() const {
    return Empties() >= 1;
  }

  friend istream& operator>>(istream& is, EvaluatedBoard& board);

  bool operator==(const EvaluatedBoard& other) const {
    return GetEval() == other.GetEval() && GetPlayer() == other.GetPlayer() &&
        GetOpponent() == other.GetOpponent();
  }

 private:
  BitPattern player_;
  BitPattern opponent_;
  int eval_;
};

template<>
struct std::hash<EvaluatedBoard>
{
  std::size_t operator()(const EvaluatedBoard& f) const noexcept {
    return std::hash<EvalApprox>{}(f.GetEval()) ^
        HashFull(f.GetPlayer(), f.GetOpponent());
  }
};

std::vector<EvaluatedBoard> load_set(const std::vector<std::string>& files);

std::vector<EvaluatedBoard> load_hard_set(int last_file_number);

std::vector<EvaluatedBoard> load_set(int start_year, int end_year);

std::vector<EvaluatedBoard> load_train_set();

std::vector<EvaluatedBoard> load_test_set();

#endif /* LOAD_TRAINING_SET_H */
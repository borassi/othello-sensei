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
#include <math.h>
#include "../board/get_moves.h"
#include "../evaluatedepthone/evaluator.h"
#include "../utils/load_training_set.h"

class EvaluateThor {
 public:
  EvaluateThor() {
    boards = load_test_set();
  }

  double GetError(EvaluatedBoard board) {
    Evaluator eval;
    eval.Setup(board.GetPlayer(), board.GetOpponent());
    return eval.Evaluate() / 8.0 - (board.GetEval());
  }

  double Run() {
    double sum_error_squared = 0;
    double num_boards = 0;

    for (EvaluatedBoard board : boards) {
      if (board.Empties() < 1 || board.Empties() > 60) {
        continue;
      }
      double error = GetError(board);
      if ((error > 12 || error < -12) && (double) rand() / RAND_MAX < 0.001) {
        Evaluator eval;
        eval.Setup(board.GetPlayer(), board.GetOpponent());
        std::cout << board.GetBoard().ToString() << "\n";
        double result = eval.EvaluateBase<true>() / 8.0;
        std::cout << result << " " << board.GetEval() << "\n\n";
      }
      sum_error_squared += error * error;
      num_boards++;
    }
    return sqrt(sum_error_squared / num_boards);
  }

 private:
  std::vector<EvaluatedBoard> boards;
};

int main() {
  std::cout << EvaluateThor().Run() << "\n";
  return 0;
}
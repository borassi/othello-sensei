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

#include <iostream>
#include "../board/board.h"
#include "load_training_set.h"

int main(int argc, char** argv) {
  std::vector<EvaluatedBoard> b = load_hard_set(184);
  std::cout << b.size() << "\n";
  for (int i = 0; i < 120; ++i) {
    std::cout << b[i].GetBoard() << (int) b[i].GetEval() << "\n\n";
  }
}

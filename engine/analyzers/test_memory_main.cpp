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

#include "endgame_ffo.h"

#include <array>
#include <iomanip>
#include <iostream>
#include "../hashmap/hash_map.h"
#include "../book/book.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../evaluatealphabeta/evaluator_last_moves.h"
#include "../thor/thor.h"
#include "../utils/misc.h"
#include "../utils/parse_flags.h"

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  bool include_thor = parse_flags.GetBoolFlagOrDefault("thor", true);
  bool include_evaluator = parse_flags.GetIntFlagOrDefault("evaluator", true);
  bool include_book = parse_flags.GetIntFlagOrDefault("book", true);
  std::unique_ptr<Thor<>> thor = nullptr;
  std::unique_ptr<EvalType> evals = nullptr;
  std::unique_ptr<HashMap<kBitHashMap>> hash_map = nullptr;
  std::unique_ptr<TreeNodeSupplier> tree_node_supplier = nullptr;
  std::unique_ptr<EvaluatorDerivative> evaluator = nullptr;
  std::unique_ptr<Book<>> book = nullptr;

  if (include_thor) {
    thor = std::make_unique<Thor<>>("assets/archive", "/tmp/saved_games.txt");
  }
  if (include_evaluator) {
    evals = std::make_unique<EvalType>(EvalType(LoadEvals()));
    hash_map = std::make_unique<HashMap<kBitHashMap>>();
    tree_node_supplier = std::make_unique<TreeNodeSupplier>();
    evaluator = std::make_unique<EvaluatorDerivative>(tree_node_supplier.get(), hash_map.get(), PatternEvaluator::Factory(evals->data()));
  }
  if (book) {
    book = std::make_unique<Book<>>("assets/book");
  }

  while (true) {
    if (thor) {
      std::cout << "Thor has " << thor->GetGamesFromAllSources(Sequence("e6")).num_games << "\n";
    }
    if (evaluator) {
      Board b;
      evaluator->Evaluate(b.Player(), b.Opponent(), -63, 63, 10000, 1200, 12, false);
      std::cout
          << "Evaluation of start "
          << evaluator->GetFirstPosition()->GetEval()
          << "\n";
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
}

/*
 * Copyright 2023 Google LLC
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

#include <filesystem>
#include <limits>

#include "book.h"
#include "../hashmap/hash_map.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/misc.h"
#include "../utils/parse_flags.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string start_line = parse_flags.GetFlagOrDefault("start", "");
//  std::string start_line = parse_flags.GetFlagOrDefault("start", "e6f4c3c4d3d6e3c2b3d2c5f5f3f6e1d1e2f1g4g3g5h5f2h4c7g6e7a4a3a2");
  std::string filepath = parse_flags.GetFlagOrDefault("folder", kBookFilepath);
  NVisited n_descendants_children = 200 * 1000 * 1000UL; //parse_flags.GetIntFlagOrDefault("n_descendants_children", 200 * 1000 * 1000UL);
  NVisited n_descendants_solve = 5 * 1000 * 1000 * 1000UL; //parse_flags.GetIntFlagOrDefault("n_descendants_solve",  5 * 1000 * 1000 * 1000UL);
  bool force_first_position = parse_flags.GetBoolFlagOrDefault("force_first_position", false);

  if (!fs::is_directory(filepath) || !fs::exists(filepath)) {
    fs::create_directory(filepath);
  }
  Book book(filepath);
  HashMap<kBitHashMap> hash_map;
  auto evals = LoadEvals();
  TreeNodeSupplier tree_node_supplier;
  std::array<std::unique_ptr<EvaluatorDerivative>, 64> evaluators;
  for (int i = 0; i < evaluators.size(); ++i) {
    evaluators[i] = std::make_unique<EvaluatorDerivative>(
        &tree_node_supplier, &hash_map,
        PatternEvaluator::Factory(evals.data()),
        std::thread::hardware_concurrency(),
        static_cast<u_int8_t>(i));
  }
  if (!book.Get(Board(start_line))) {
    if (!force_first_position) {
      std::cout << "The node '" << start_line << "' is not in the library. Run with --force_first_position, or choose a different start.\n";
      return 1;
    }
    std::unique_ptr<TreeNode> t(new TreeNode());
    t->Reset(Board(start_line), 1, 0, 0, 1, -63, 63);
    t->AddDescendants(1);
    book.Add(*t);
  }

  while (true) {
    ElapsedTime t;
    auto start = book.Get(Board(start_line)).value();
    if (start->IsSolved()) {
      std::cout << "Solved the position!\n";
      break;
    }
    tree_node_supplier.Reset();
    NVisited n_visited = 0;
    Eval eval_goal = start->NextPositionEvalGoal(0, 1, kLessThenMinEval);
    std::cout
        << "Expanding line:        " << start_line << "\n"
        << "Positions:             " << PrettyPrintDouble(book.Size()) << "\n"
        << "Descendants of start:  " << PrettyPrintDouble(start->GetNVisited()) << "\n"
        << "Evaluation of start:   " << std::setprecision(2) << start->GetEval() << "\n"
        << "Missing:               " << PrettyPrintDouble(start->RemainingWork(-63, 63)) << "\n"
        << "Eval goal:             " << (int) eval_goal << "\n";

    auto leaf = LeafToUpdate<BookNode>::BestDescendant(start, 0, kLessThenMinEval).value();
    std::cout
        << "Board:\n" << Indent(leaf.Leaf()->ToBoard().ToString(), "                       ");
    bool solved = false;
    int alpha = leaf.Alpha();
    int beta = leaf.Beta();
    auto node = leaf.Leaf();
    assert(leaf.Alpha() <= leaf.EvalGoal() && leaf.EvalGoal() <= leaf.Beta());
    if (node->RemainingWork(alpha, beta) < n_descendants_solve) {
      std::cout << "Solving with alpha=" << alpha << " beta=" << beta << "\n";
      auto evaluator = evaluators[0].get();
      evaluator->Evaluate(node->Player(), node->Opponent(), alpha, beta, 5 * n_descendants_solve, 240, false);
      auto result = evaluator->GetFirstPosition();
      auto eval = result->GetEval();
      auto lower = result->Lower();
      auto upper = result->Upper();
      node->SetLower(lower);
      node->SetUpper(upper);
      n_visited += result->GetNVisited();
      solved = lower != -64 || upper != 64;
      if (solved) {
        std::cout << "Solved: " << (int) result->Lower() << " <= eval <= " << (int) result->Upper() << "\n";
      } else {
        std::cout << "Did not solve it in time\n";
      }
    } else {
      std::cout << "Too early to solve\n";
    }
    if (!solved) {
      std::cout << "Adding children\n";
      std::vector<TreeNode*> children;
      int i = 0;
      for (auto [child, unused_flip] : GetUniqueNextBoardsWithPass(node->ToBoard())) {
        auto evaluator = evaluators[++i].get();
        evaluator->Evaluate(
            child.Player(), child.Opponent(), -63, 63, n_descendants_children / 100, 300);
        auto remaining_work = std::max((NVisited) 1000, (NVisited) evaluator->GetFirstPosition()->RemainingWork(alpha, beta));
        evaluator->ContinueEvaluate(
            std::min(n_descendants_children, (NVisited) remaining_work / 30), 300);
        children.push_back(evaluator->GetFirstPosition());
        n_visited += evaluator->GetFirstPosition()->GetNVisited();
      }
      node->AddChildrenToBook(children);
      node->UpdateFather();
    }
    leaf.Finalize(n_visited);
    book.Commit();
    double time = t.Get();
    std::cout << "Position:              " << PrettyPrintDouble(n_visited) << "\n";
    std::cout << "Time:                  " << PrettyPrintDouble(time) << " sec\n";
    std::cout << "Positions / sec:       " << PrettyPrintDouble(n_visited / time) << "\n";
    std::cout << "\n";
  }
}

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

#include "book.h"
#include "../hashmap/hash_map.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../utils/parse_flags.h"

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string start_line = parse_flags.GetFlagOrDefault("start", "");
  std::string filepath = parse_flags.GetFlagOrDefault("folder", kBookFilepath);
  int n_descendants = parse_flags.GetIntFlagOrDefault("n_descendants", /*1G*/ 200000000);
  bool force_first_position = parse_flags.GetBoolFlagOrDefault("force_first_position", false);

  Book book(filepath);
  HashMap hash_map;
  auto evals = LoadEvals();
  TreeNodeSupplier tree_node_supplier;
  EvaluatorDerivative evaluator(&tree_node_supplier, &hash_map, PatternEvaluator::Factory(evals.data()), 12);
  auto start = book.Get(Board(start_line));
  if (!start->IsValid()) {
    if (!force_first_position) {
      std::cout << "The node '" << start_line << "' is not in the library. Run with --force_first_position, or choose a different start.\n";
      return 1;
    }
    std::unique_ptr<TreeNode> t(new TreeNode());
    t->Reset(start->ToBoard(), 1, 0, -63, 63);
    t->SetLeaf(0, 1);
    start->Update(*t);
    book.Get(t->ToBoard())->AsLeaf(1).Finalize(1);
  }

  while (true) {
    start = book.Get(Board(start_line));
    ElapsedTime t;
    NVisited n_visited;

    std::cout
        << "Expanding line:        '" << start_line << "'\n"
        << "Positions:             " << PrettyPrintDouble(book.Size()) << "\n"
        << "Descendants of start:  " << PrettyPrintDouble(start->GetNVisited()) << "\n"
        << "Evaluation of start:   " << PrettyPrintDouble(start->GetEval()) << "\n";

    auto leaf = LeafToUpdate<BookNode>::BestDescendant(start, 0).value();
    std::cout
        << "Eval goal:             " << (int) start->NextPositionEvalGoal(0, 1) << "\n"
        << "Board:\n" << leaf.Leaf()->ToBoard();
    tree_node_supplier.Reset();
    evaluator.Evaluate(leaf.Leaf()->Player(), leaf.Leaf()->Opponent(), leaf.Alpha(), leaf.Beta(), n_descendants, 300, false);
    double time = t.Get();
    n_visited = evaluator.GetFirstPosition()->GetNVisited();
    std::cout << "Time:            " << PrettyPrintDouble(time) << " sec\n";
    std::cout << "Positions / sec: " << PrettyPrintDouble(n_visited / time) << "\n\n";
    auto children = evaluator.GetFirstPosition()->GetChildren();
    leaf.Leaf()->AddChildrenToBook(children);
    leaf.Finalize(n_visited);
    book.Commit();
  }
}
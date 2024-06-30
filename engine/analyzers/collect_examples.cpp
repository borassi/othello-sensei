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

#include <chrono>
#include <fstream>
#include <random>
#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../hashmap/hash_map.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../utils/load_training_set.h"
#include "../utils/parse_flags.h"

class Collector {
 public:

  Collector(int to_collect) : to_collect_(to_collect), total_(0), collected_(), seen_() {}

  void AddBoard(const Board& b) {
    if (seen_.find(b) != seen_.end()) {
      return;
    }
    ++total_;

    // If you want to collect n, this will happen the first n times.
    if (total_ <= to_collect_) {
      collected_.push_back(b);
      return;
    }

    // This happens with probability to_collect_ / total_.
    if ((double) rand() / RAND_MAX < (double) to_collect_ / total_) {
      collected_[rand() % collected_.size()] = b;
    }
  }

  const std::vector<Board>& Get() { return collected_; }

 private:
  int to_collect_;
  int total_;
  std::vector<Board> collected_;
  std::unordered_set<Board> seen_;
};

int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
//  int n = parse_flags.GetIntFlagOrDefault("n", 20);
  std::vector<Collector> collectors;
  for (int i = 0; i < 64; ++i) {
    if (i <= 51) {
      collectors.push_back(Collector(0));
//    } else if (i <= 27) {
//      // ~3 hours
//      collectors.push_back(Collector(40));
    } else if (i == 52) {
      // ~1 hour
      collectors.push_back(Collector(4));
    } else {
      // ~1 hour for 30, 2 hours for 31 (time cutoff), ...
      collectors.push_back(Collector(15));
    }
  }

  std::ofstream output;
  HashMap<kBitHashMap> hash_map;
  auto evals = LoadEvals();
  TreeNodeSupplier tree_node_supplier;
  EvaluatorDerivative evaluator(&tree_node_supplier, &hash_map, PatternEvaluator::Factory(evals.data()), 12);
  PatternEvaluator evaluator0(evals.data());
  EvaluatorAlphaBeta evaluator_alpha_beta(&hash_map, PatternEvaluator::Factory(evals.data()));

  for (const auto& board : load_train_set()) {
    collectors[board.Empties()].AddBoard(board.GetBoard());
  }

  std::string filename = "endgame_results.txt";
  int index = 0;
  while (std::ifstream(filename).good()) {
    ++index;
    filename = "endgame_results_" + std::to_string(index) + ".txt";
  }
  output.open(filename, std::ios::app);
  output
      << "player opponent empties real_eval perc_lower solve_probability_lower "
      << "perc_upper solve_probability_upper alpha_beta final_lower "
      << "final_upper final_eval final_visited final_proof final_disproof "
      << "final_weaklower final_weakupper eval0 eval1 eval2 eval3 eval4 "
      << "moves_player moves_player_corner moves_player_x "
      << "moves_player_app moves_player_corner_app moves_player_x_app "
      << "moves_opponent moves_opponent_corner moves_opponent_x "
      << "moves_opponent_app moves_opponent_corner_app moves_opponent_x_app\n";
  output.close();
  for (Collector collector : collectors) {
    const auto& boards = collector.Get();
    if (boards.size() == 0) {
      continue;
    }
    auto t = std::time(nullptr);
    tm time;
    localtime_r(&t, &time);
    std::cout
        << "\nStarting " << collector.Get()[0].NEmpties()
        << "(" << boards.size() << ") at "
        << std::put_time(&time, "%H:%M:%S") << "\n";
    for (const Board& b : collector.Get()) {
      std::stringstream low_depth_evals;
      evaluator0.Setup(b.Player(), b.Opponent());
      low_depth_evals << evaluator0.Evaluate();
      for (int d = 1; d <= 4; ++d) {
        low_depth_evals << " " << evaluator_alpha_beta.Evaluate(b.Player(), b.Opponent(), d);
      }
      for (bool invert : {false, true}) {
        for (bool approx : {false, true}) {
          BitPattern new_player = invert ? b.Opponent() : b.Player();
          BitPattern new_opponent = invert ? b.Player() : b.Opponent();
          BitPattern moves = approx ?
              Neighbors(new_opponent) & ~(new_player | new_opponent)
              : GetMoves(new_player, new_opponent);
          low_depth_evals
              << " " << __builtin_popcountll(moves)
              << " " << __builtin_popcountll(moves & kCornerPattern)
              << " " << __builtin_popcountll(moves & kXPattern);
        }
      }
      tree_node_supplier.Reset();
      evaluator.Evaluate(b.Player(), b.Opponent(), -63, 63, 1000000000000L, 300, 1, false);
      auto first_position_ptr = evaluator.GetFirstPosition();
      assert(first_position_ptr);
      auto first_position = *first_position_ptr;
      double real_eval = first_position.GetEval();
      double perc_lower = first_position.GetPercentileUpper(0.5F);
      double solve_probability_lower = first_position.SolveProbabilityLower(-63);
      double perc_upper = first_position.GetPercentileLower(0.5F);
      double solve_probability_upper = first_position.SolveProbabilityUpper(63);
      std::stringstream result;
      int step = b.NEmpties() > 28 ? 3 : 1;
      for (int i = -63 + (rand() % step) * 2; i <= 63; i += 2 * step) {
        tree_node_supplier.Reset();
        hash_map.Reset();
        evaluator.Evaluate(b.Player(), b.Opponent(), i, i, 1000000000000L, 20, 1, false);
        auto first_position_ptr = evaluator.GetFirstPosition();
        assert(first_position_ptr);
        auto first_position = *first_position_ptr;
        result
            << b.Player() << " " << b.Opponent() << " " << b.NEmpties()
            << " " << real_eval << " " << perc_lower << " " << solve_probability_lower << " "
            << perc_upper << " " << solve_probability_upper << " "
            << i << " " << (int) first_position.Lower()
            << " " << (int) first_position.Upper() << " " << first_position.GetEval()
            << " " << first_position.GetNVisited() << " " << first_position.ProofNumber(i)
            << " " << first_position.DisproofNumber(i) << " " << (int) first_position.WeakLower()
            << " " << (int) first_position.WeakUpper() << " " << low_depth_evals.str() << "\n";
        std::cout << "." << std::flush;
      }
      std::cout << "\n" << std::flush;
      output.open(filename, std::ios::app);
      output << result.str();
      output.close();
    }
    t = std::time(nullptr);
    localtime_r(&t, &time);
    std::cout << "\nFinished at " << std::put_time(&time, "%H:%M:%S") << "\n";
  }
  return 0;
}
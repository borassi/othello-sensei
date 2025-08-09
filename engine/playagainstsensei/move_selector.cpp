/*
 * Copyright 2025 Michele Borassi
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

#include <math.h>
#include "move_selector.h"
#include "../evaluatederivative/tree_node.h"

namespace {

constexpr double kErrorPercentage[] = {
    0.0, 0.0013976365946040002, 0.003801571230374871, 0.003271857745814194, 0.008279673683894071,
    0.008448142395832598, 0.007552751937430454, 0.009881970091850992, 0.008117337597219946, 0.010962527491184877,
    0.008174796896444377, 0.011785666258739507, 0.011155759522086578, 0.013035210812129754, 0.012621659877825714,
    0.014082477740390948, 0.013300534179687138, 0.016098406205464023, 0.0159153368478561, 0.015072612919671647,
    0.017690716977093606, 0.016949190538568137, 0.016673427732177476, 0.01835123305537761, 0.019839462019340278,
    0.019490903403816973, 0.02057108925096853, 0.02083143089724683, 0.01944704940037023, 0.021583527522205877,
    0.022466329982804203, 0.020537626724224736, 0.02414092384423285, 0.0217643074362023, 0.026820380927631608,
    0.024126893706231237, 0.027589297421430747, 0.024389761884498425, 0.028485842029613167, 0.02738794538885042,
    0.028161200889229937, 0.02987614383696257, 0.029009659154296107, 0.02963863278982487, 0.028527184060206438,
    0.028527184060206438, 0.025580160913490895, 0.02408138914173271, 0.022178454195567812, 0.023609865438258222,
    0.01987135607499622, 0.018439944832305814, 0.01613284671173422, 0.013168983432751732, 0.011501810338324083,
    0.008251664810803515, 0.005725644970761622, 0.0038227100245967305, 0.0018018941525632167, 0.0
};

double GetWeight(double error, double lambda) {
  return exp(-lambda * error);
}

double GetTotalWeight(const std::vector<double>& errors, double lambda) {
  double total_weight = 0;
  for (double error: errors) {
    total_weight += GetWeight(error, lambda);
  }
  return total_weight;
}
double GetExpectedError(const std::vector<double>& errors, double lambda) {
  double weight_times_error = 0;
  for (double error : errors) {
    weight_times_error += error * GetWeight(error, lambda);
  }
  return weight_times_error / GetTotalWeight(errors, lambda);
}
}


double GetMoveMultiplier(int move) {
  // We multiply by 2 because SUM(kErrorPercentage) = 1, but we want to set the total error for
  // White or Black.
  return kErrorPercentage[move] * 2;
}

const Node* FindNextMove(const std::vector<const Node*>& children, double expected_error, double max_error) {
  if (children.empty()) {
    return nullptr;
  }
  const Node* best_child = nullptr;
  std::vector<double> errors;
  std::vector<const Node*> child_for_error;
  double best_eval = DBL_MAX;

  for (const auto& child : children) {
    double eval = child->GetEval();
    if (eval < best_eval) {
      best_eval = eval;
      best_child = child;
    }
  }
  for (const auto& child : children) {
    double error = child->GetEval() - best_eval;
    if (error <= std::max(0.01, max_error)) {
      errors.push_back(error);
      child_for_error.push_back(child);
    }
  }
  // Binary search to find a lambda corresponding to the right error.
  double l = 0;    // all moves same probability.
  double u = 100;  // a move with error 0.01 has probability 1/e.
  if (GetExpectedError(errors, l) <= expected_error) {
    // All moves are so good that we can't achieve the expected error. Choose a random move.
    return child_for_error[rand() % child_for_error.size()];
  } else if (GetExpectedError(errors, u) >= expected_error) {
    // The error is so small that we can just pick the best move.
    return best_child;
  }
  while (u - l > 1E-2) {
    assert(GetExpectedError(errors, u) < expected_error && expected_error <= GetExpectedError(errors, l));
    double mid = (l + u) / 2;
    double error_mid = GetExpectedError(errors, mid);
    if (error_mid < expected_error) {
      u = mid;
    } else {
      l = mid;
    }
  }
  double lambda = (l + u) / 2;
  double point = rand() / (double) RAND_MAX * GetTotalWeight(errors, lambda);
  for (int i = 0; i < errors.size(); ++i) {
    point -= GetWeight(errors[i], lambda);
    if (point <= 0) {
      return child_for_error[i];
    }
  }
  return child_for_error[0];
  assert(false);
}

const Node* FindNextMoveTotalError(const std::vector<const Node*>& children, double total_error, double max_error) {
  // Max is needed in case someone sets up a board with <4 disks.
  if (children.size() == 0) {
    return nullptr;
  }
  int move = std::max(0, 59 - children[0]->NEmpties());
  return FindNextMove(children, total_error * GetMoveMultiplier(move), max_error);
}

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
#ifndef OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATION_H
#define OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATION_H

#include <cassert>
#include <cmath>
#include <float.h>
#include <iomanip>
#include "prob_combiners.h"
#include "../board/bitpattern.h"
#include "../estimators/endgame_time.h"
#include "../estimators/win_probability.h"

constexpr int kLogDerivativeMinusInf = -1000000;
constexpr int kLogDerivativeMultiplier = 100;

inline int LeafLogDerivative(double prob) {
  return (int) round(kLogDerivativeMultiplier * 1 * log(prob * (1 - prob)));
}

struct CombineProb {
  Probability combine_prob[kProbStep + 1][kProbStep + 1];
  int log_derivative[kProbStep + 1];
  PN combine_disproof_number[kProofNumberStep + 1][kProofNumberStep + 1];
  PN disproof_to_proof_number[kProofNumberStep + 1][kProbStep + 1];
  int leaf_log_derivative[kProbStep + 1];
  double prob_lower_cubed[kProbStep + 1];

  CombineProb() : combine_prob(), log_derivative(), combine_disproof_number() {
    ProbCombiner combiner(ExponentialTimesPolyLog);
    for (int i = 0; i <= kProofNumberStep; ++i) {
      for (int j = 0; j <= kProofNumberStep; ++j) {
        if (i == kProofNumberStep || j == kProofNumberStep) {
          combine_disproof_number[i][j] = kProofNumberStep;
        } else {
          combine_disproof_number[i][j] = std::max(i, std::max(j, (int)
              ProofNumberToByte(std::min(ByteToProofNumberExplicit(i) + ByteToProofNumberExplicit(j), kMaxProofNumber - 1))));
        }
        assert(combine_disproof_number[i][j] >= i);
        assert(combine_disproof_number[i][j] >= j);
        assert(i == kProofNumberStep || j == kProofNumberStep || combine_disproof_number[i][j] < kProofNumberStep);
      }
      for (int j = 0; j <= kProbStep; ++j) {
        if (j == kProbStep || i == kProofNumberStep) {
          disproof_to_proof_number[i][j] = kProofNumberStep;
        } else if (i == 0) {
          disproof_to_proof_number[i][j] = 0;
        } else {
          double proof_number =  ByteToProofNumberExplicit(i);
          double prob = ByteToProbabilityExplicit(j);
          if (prob > 0.5) {
            proof_number *= pow(0.5 / (1 - prob), 3);
          }
          disproof_to_proof_number[i][j] = ProofNumberToByte(std::max(
              1.0F,
              std::min(kMaxProofNumber, (float) proof_number)));
        }
        assert((i == 0 && j != kProbStep) == (disproof_to_proof_number[i][j] == 0));
        assert(
            (i == kProofNumberStep || j == kProbStep) == (disproof_to_proof_number[i][j] == kProofNumberStep));
      }
    }

    for (int i = 0; i <= kProbStep; ++i) {
      double x1 = ByteToProbabilityExplicit(i);
      prob_lower_cubed[i] = pow(1 - x1, 3);
      leaf_log_derivative[i] = std::max(kLogDerivativeMinusInf, LeafLogDerivative(x1));
      log_derivative[i] = (int) round(std::max(
          (double) kLogDerivativeMinusInf,
          std::min((double) -kLogDerivativeMinusInf,
                   kLogDerivativeMultiplier * log(combiner.derivative(x1)))));
      assert(log_derivative[i] > kLogDerivativeMinusInf);
      for (int j = i; j <= kProbStep; ++j) {
        double x2 = ByteToProbabilityExplicit(j);
        combine_prob[i][j] = ProbabilityToByteExplicit(combiner.inverse(combiner.f(x1) + combiner.f(x2)));
        combine_prob[j][i] = combine_prob[i][j];
        assert(j == i || i == 0 || combine_prob[i][j] >= combine_prob[i][j-1]);
        assert(combine_prob[i][j] <= i);
        assert(combine_prob[i][j] <= j);
      }
    }
    for (int i = 0; i <= kProbStep; ++i) {
      assert(combine_prob[i][0] == 0);
      assert(combine_prob[0][i] == 0);
      assert(combine_prob[i][kProbStep] == i);
      assert(combine_prob[kProbStep][i] == i);
      assert(leaf_log_derivative[i] == leaf_log_derivative[kProbStep - i]);
      assert(i == kProbStep || log_derivative[i] >= log_derivative[i+1]);
    }
  }
};

class Evaluation {
 public:
  static const CombineProb kCombineProb;
  bool operator==(const Evaluation& other) const {
    return
        prob_greater_equal_ == other.prob_greater_equal_
        && proof_number_ == other.proof_number_
        && disproof_number_ == other.disproof_number_
        && max_log_derivative_ == other.max_log_derivative_;
  }
  bool operator!=(const Evaluation& other) const { return !operator==(other); }
  float ProofNumber() const { return ByteToProofNumber(proof_number_); }
  float DisproofNumber() const { return ByteToProofNumber(disproof_number_); }
  int MaxLogDerivative() const { return max_log_derivative_; }

  double ProbGreaterEqual() const {
    return ByteToProbability(prob_greater_equal_);
  }

  double ProbLowerCubed() const {
    return kCombineProb.prob_lower_cubed[prob_greater_equal_];
  }

  bool IsSolved() const {
    Check();
    return (proof_number_ == 0 || disproof_number_ == 0);
  }

  int LogDerivative(const Evaluation& father) const {
    if (prob_greater_equal_ == 0 || prob_greater_equal_ == kProbStep ||
        father.prob_greater_equal_ == 0 || father.prob_greater_equal_ == kProbStep) {
      return kLogDerivativeMinusInf;
    }
    return max_log_derivative_ + kCombineProb.log_derivative[prob_greater_equal_]
           - kCombineProb.log_derivative[kProbStep - father.prob_greater_equal_];
  }

  void Initialize() {
    proof_number_ = kProofNumberStep;
    disproof_number_ = 0;
    prob_greater_equal_ = kProbStep;
    max_log_derivative_ = kLogDerivativeMinusInf;
  }

  void UpdateFatherWithThisChild(const Evaluation& child) {
    child.Check();
    assert(max_log_derivative_ - kCombineProb.log_derivative[prob_greater_equal_] < 0);

    int current_log_derivative;
    prob_greater_equal_ = kCombineProb.combine_prob[prob_greater_equal_][child.prob_greater_equal_];
    current_log_derivative = child.max_log_derivative_ + kCombineProb.log_derivative[child.prob_greater_equal_];
    max_log_derivative_ = std::max(max_log_derivative_, current_log_derivative);
    assert(prob_greater_equal_ <= child.prob_greater_equal_);
    assert(max_log_derivative_ - kCombineProb.log_derivative[prob_greater_equal_] < 0);

    PN cur_proof_number = kCombineProb.disproof_to_proof_number[child.disproof_number_][child.prob_greater_equal_];
    assert((cur_proof_number > 0) || prob_greater_equal_ == 0);
    assert((child.prob_greater_equal_ == kProbStep) ==
               (cur_proof_number == kProofNumberStep));
    proof_number_ = std::min(proof_number_, cur_proof_number);

    auto new_disproof_number = kCombineProb.combine_disproof_number[disproof_number_][child.proof_number_];
    assert((new_disproof_number == kProofNumberStep) ==
           ((disproof_number_ == kProofNumberStep) || child.proof_number_ == kProofNumberStep));
    disproof_number_ = new_disproof_number;
  }

  void Finalize() {
    if (prob_greater_equal_ == kProbStep || prob_greater_equal_ == 0) {
      max_log_derivative_ = kLogDerivativeMinusInf;
    } else {
      max_log_derivative_ -= kCombineProb.log_derivative[prob_greater_equal_];
      assert(max_log_derivative_ < 0);
      assert(max_log_derivative_ > kLogDerivativeMinusInf);
    }
    prob_greater_equal_ = (kProbStep - prob_greater_equal_);
    if (prob_greater_equal_ == kProbStep) {
      disproof_number_ = kProofNumberStep;
    } else {
      assert(disproof_number_ < kProofNumberStep);
    }
    assert((prob_greater_equal_ == 0) == (proof_number_ == kProofNumberStep));
    Check();
  }

  void SetLeaf(
      BitPattern player, BitPattern opponent, EvalLarge goal, EvalLarge eval,
      Square depth, Square n_empties) {
    prob_greater_equal_ = WinProbability(depth, n_empties, goal, eval);
    proof_number_ = prob_greater_equal_ == 0 ? kProofNumberStep : ::ProofNumber(player, opponent, goal, eval);
    if (prob_greater_equal_ > 0) {
      assert(isfinite(ProofNumber()) && ProofNumber() < FLT_MAX);
      assert(ProofNumber() > 0);
    } else {
      assert(ProofNumber() == FLT_MAX);
    }
    disproof_number_ = prob_greater_equal_ == kProbStep ? kProofNumberStep : ::DisproofNumber(player, opponent, goal, eval);
    if (prob_greater_equal_ < kProbStep) {
      assert(isfinite(DisproofNumber()) && DisproofNumber() < FLT_MAX && DisproofNumber() > 0);
    } else {
      assert(DisproofNumber() == FLT_MAX);
    }
    assert(disproof_number_ > 0);
    max_log_derivative_ = kCombineProb.leaf_log_derivative[prob_greater_equal_];
    Check();
  }

  void SetDisproved() {
    prob_greater_equal_ = 0;
    proof_number_ = kProofNumberStep;
    disproof_number_ = 0;
    max_log_derivative_ = kLogDerivativeMinusInf;
    Check();
  }

  void SetProved() {
    prob_greater_equal_ = kProbStep;
    proof_number_ = 0;
    disproof_number_ = kProofNumberStep;
    max_log_derivative_ = kLogDerivativeMinusInf;
    Check();
  }

  void SetProving(const Evaluation& other_eval, int delta) {
    assert(delta > 0);
    assert(delta % 2 == 0);
    prob_greater_equal_ = kProbStep;
    proof_number_ = ProofNumberToByte((float) ConvertProofNumber(other_eval.ProofNumber(), delta));
    disproof_number_ = kProofNumberStep;
    max_log_derivative_ = kLogDerivativeMinusInf;
  }

  void SetDisproving(const Evaluation& other_eval, int delta) {
    prob_greater_equal_ = 0;
    proof_number_ = kProofNumberStep;
    disproof_number_ = ProofNumberToByte((float) ConvertDisproofNumber(other_eval.DisproofNumber(), delta));
    max_log_derivative_ = kLogDerivativeMinusInf;
  }

  void SetProving(PN proof_number) {
    prob_greater_equal_ = kProbStep;
    proof_number_ = proof_number;
    disproof_number_ = kProofNumberStep;
    max_log_derivative_ = kLogDerivativeMinusInf;
  }

  void SetDisproving(PN disproof_number) {
    prob_greater_equal_ = 0;
    proof_number_ = kProofNumberStep;
    disproof_number_ = disproof_number;
    max_log_derivative_ = kLogDerivativeMinusInf;
  }

  PN ProofNumberSmall() const { return proof_number_; }
  PN DisproofNumberSmall() const { return disproof_number_; }
  Probability ProbGreaterEqualSmall() const { return prob_greater_equal_; }
  int MaxLogDerivativeSmall() const { return max_log_derivative_; }

  void Set(Probability prob_greater_equal, PN proof_number,
           PN disproof_number, int max_log_derivative) {
    prob_greater_equal_ = prob_greater_equal;
    proof_number_ = proof_number;
    disproof_number_ = disproof_number;
    max_log_derivative_ = max_log_derivative;
  }

 private:
  Probability prob_greater_equal_;
  PN proof_number_;
  PN disproof_number_;
  int max_log_derivative_;

  void Check() const {
    assert((prob_greater_equal_ == 0) == (proof_number_ == kProofNumberStep));
    assert(proof_number_ != 0 || prob_greater_equal_ == kProbStep);
    assert(disproof_number_ != 0 || prob_greater_equal_ == 0);
    assert((prob_greater_equal_ == 0 || prob_greater_equal_ == kProbStep) ==
           (max_log_derivative_ == kLogDerivativeMinusInf));
    assert(max_log_derivative_ < 0);
  }
};

std::ostream& operator<<(std::ostream& stream, const Evaluation& e);
#endif  // OTHELLOSENSEI_EVALUATEDERIVATIVE_EVALUATION_H
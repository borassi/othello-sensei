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
#include "../estimators/endgame_time.h"
#include "prob_combiners.h"
#include "../board/bitpattern.h"
#include "../estimators/gaussian.h"

typedef uint8_t Probability;
typedef uint8_t PN;
constexpr Probability kProbStep = 255;
constexpr int kLogDerivativeMinusInf = -100000000;
constexpr int kLogDerivativeMultiplier = 100;
constexpr int kProofNumberStep = 255;
constexpr float kBaseLogProofNumber = ConstexprPow(kMaxProofNumber, 1.0 / (kProofNumberStep - 1.99));  // kBaseLogProofNumber ^ (kProofNumberStep-1) = kMaxProofNumber.

PN ProofNumberToByte(float proof_number);

float ByteToProofNumberExplicit(PN byte);
float ByteToProofNumber(PN byte);

double ByteToProbabilityExplicit(Probability byte);
double ByteToProbability(Probability byte);
Probability ProbabilityToByte(double probability);

inline int LeafLogDerivative(double prob) {
  return round(kLogDerivativeMultiplier * 1 * log(prob * (1 - prob)));
}

constexpr float kErrors[][60] = {
    {},
    {2.00, 2.00, 2.00, 2.00, 6.64, 6.87, 7.64, 7.77, 8.18, 8.30, 8.72, 8.73,
     8.98, 8.71, 8.65, 8.35, 8.29, 8.05, 8.20, 7.64, 7.55, 7.05, 6.82, 6.00,
     6.36, 5.61, 5.86, 5.19, 5.76, 5.13, 5.58, 4.91, 5.19, 4.39, 4.89, 4.14,
     4.82, 4.03, 4.38, 3.83, 4.16, 3.52, 3.82, 3.20, 3.28, 2.79, 2.96, 2.41,
     2.84, 2.57, 2.51, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00},
    {2.00, 2.00, 2.00, 2.00, 5.57, 5.86, 6.40, 7.08, 7.23, 7.63, 7.77, 8.06,
     8.03, 8.19, 7.91, 7.81, 7.57, 7.51, 7.41, 7.29, 6.92, 6.56, 6.18, 5.41,
     5.67, 5.14, 5.13, 4.66, 4.98, 4.56, 4.78, 4.37, 4.21, 3.90, 4.03, 3.73,
     3.97, 3.63, 3.54, 3.46, 3.51, 3.15, 3.17, 2.94, 2.67, 2.52, 2.56, 2.21,
     2.10, 2.48, 2.09, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00},
    {2.00, 2.00, 2.00, 2.00, 5.17, 4.96, 5.63, 5.93, 6.68, 6.74, 7.21, 7.25,
     7.47, 7.38, 7.51, 7.21, 7.10, 6.97, 6.89, 6.66, 6.57, 6.03, 5.75, 5.01,
     5.30, 4.71, 5.16, 4.30, 4.73, 4.09, 4.59, 3.88, 4.40, 3.50, 3.97, 3.29,
     4.03, 3.17, 3.81, 3.09, 3.50, 2.74, 3.25, 2.59, 3.00, 2.28, 2.61, 2.05,
     2.25, 2.00, 2.37, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00},
    {2.00, 2.00, 2.00, 2.00, 2.00, 4.72, 4.71, 5.25, 5.53, 6.34, 6.37, 6.77,
     6.71, 6.87, 6.81, 7.03, 6.56, 6.54, 6.37, 6.27, 6.05, 5.93, 5.32, 4.77,
     4.93, 4.39, 4.64, 3.98, 4.05, 3.66, 3.96, 3.53, 3.67, 3.24, 3.15, 2.96,
     3.34, 2.93, 3.06, 2.86, 2.80, 2.57, 2.72, 2.40, 2.50, 2.13, 2.13, 2.00,
     2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00}
};

constexpr int kMaxCDFOffset = /*depth*/ 4 * /*n_empties*/ 64 * /*eval_delta*/ (256*8+1);

constexpr int DataToCDFOffset(Square depth, Square n_empties, EvalLarge eval_delta) {
  return (depth-1) | (n_empties << 2) | ((eval_delta - 2 * kMinEvalLarge) << 8);
}

constexpr std::tuple<Square, Square, EvalLarge> CDFOffsetToDepthEmptiesEval(int cdf_offset) {
  return std::make_tuple((cdf_offset & 3) + 1, (cdf_offset >> 2) & 63, (cdf_offset >> 8) + 2 * kMinEvalLarge);
}

struct CombineProb {
  Probability combine_prob[kProbStep + 1][kProbStep + 1];
  Probability combine_prob_shallow[kProbStep + 1][kProbStep + 1];
  Probability gaussian_cdf[kMaxCDFOffset + 1];
  int log_derivative[kProbStep + 1];
  int log_derivative_shallow[kProbStep + 1];
  PN combine_disproof_number[kProofNumberStep + 1][kProofNumberStep + 1];
  PN disproof_to_proof_number[kProofNumberStep + 1][kProbStep + 1];
  float byte_to_proof_number[kProofNumberStep + 1];
  double byte_to_probability[kProbStep + 1];
  int leaf_log_derivative[kProbStep + 1];

  CombineProb() : combine_prob(), log_derivative(), combine_disproof_number() {
    ProbCombiner combiner(ExpPolyLog<135>);
    ProbCombiner combiner_shallow(ExpPolyLog<175>);
    for (int i = 0; i <= kProofNumberStep; ++i) {
      byte_to_proof_number[i] = ByteToProofNumberExplicit(i);
      for (int j = 0; j <= kProofNumberStep; ++j) {
        if (i == kProofNumberStep || j == kProofNumberStep) {
          combine_disproof_number[i][j] = kProofNumberStep;
        } else {
          combine_disproof_number[i][j] = std::max(i, std::max(j, (int)
              ProofNumberToByte(std::min(ByteToProofNumber(i) + ByteToProofNumber(j), kMaxProofNumber - 1))));
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
          disproof_to_proof_number[i][j] = ProofNumberToByte(std::max(
              1.0,
              std::min((double) kMaxProofNumber, ByteToProofNumber(i) / (1.0 -
                  ByteToProbabilityExplicit(j)))));
        }
        assert((i == 0 && j != kProbStep) == (disproof_to_proof_number[i][j] == 0));
        assert(
            (i == kProofNumberStep || j == kProbStep) == (disproof_to_proof_number[i][j] == kProofNumberStep));
      }
    }

    for (int i = 0; i <= kProbStep; ++i) {
      double x1 = ByteToProbabilityExplicit(i);
      leaf_log_derivative[i] = std::max(kLogDerivativeMinusInf, LeafLogDerivative(x1));
      byte_to_probability[i] = x1;
      log_derivative[i] = round(std::max(
          (double) kLogDerivativeMinusInf,
          std::min((double) -kLogDerivativeMinusInf,
                   kLogDerivativeMultiplier * log(combiner.derivative(x1)))));
      log_derivative_shallow[i] = round(std::max(
          (double) kLogDerivativeMinusInf,
          std::min((double) -kLogDerivativeMinusInf,
                   kLogDerivativeMultiplier * log(combiner_shallow.derivative(x1)))));
      assert(log_derivative[i] > kLogDerivativeMinusInf);
      for (int j = i; j <= kProbStep; ++j) {
        double x2 = ByteToProbabilityExplicit(j);
        combine_prob[i][j] = ProbabilityToByte(combiner.inverse(combiner.f(x1) + combiner.f(x2)));
        combine_prob[j][i] = combine_prob[i][j];
        combine_prob_shallow[i][j] = ProbabilityToByte(
            combiner_shallow.inverse(combiner_shallow.f(x1) + combiner_shallow.f(x2)));
        combine_prob_shallow[j][i] = combine_prob_shallow[i][j];
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
      assert(i == kProbStep || log_derivative_shallow[i] >= log_derivative_shallow[i+1]);
    }

    for (Square depth = 1; depth <= 4; ++depth) {
      for (EvalLarge delta = 2 * kMinEvalLarge; delta <= -2 * kMinEvalLarge; ++delta) {
        for (Square empties = 0; empties < 64; ++empties) {
          double prob = 1 - GaussianCDF(delta, 0, 1 * 8 * std::max(3.0F, kErrors[depth][empties]));
          gaussian_cdf[DataToCDFOffset(depth, empties, delta)] = ProbabilityToByte(prob);
        }
      }
    }
  }
};

class Evaluation {
 public:
  static const CombineProb kCombineProb;
  bool operator==(const Evaluation& other) const = default;
  float ProofNumber() const { return ByteToProofNumber(proof_number_); }
  float DisproofNumber() const { return ByteToProofNumber(disproof_number_); }
  int MaxLogDerivative() const { return max_log_derivative_; }

  float ProbGreaterEqual() const {
    return ByteToProbability(prob_greater_equal_);
  }

  bool IsSolved() const {
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

  template<bool shallow>
  void UpdateFatherWithThisChild(const Evaluation& child) {
    child.Check();
    assert(max_log_derivative_ - kCombineProb.log_derivative[prob_greater_equal_] < 0);

    int current_log_derivative;
    if (shallow) {
      prob_greater_equal_ = kCombineProb.combine_prob_shallow[prob_greater_equal_][child.prob_greater_equal_];
      current_log_derivative = child.max_log_derivative_ + kCombineProb.log_derivative_shallow[child.prob_greater_equal_];
    } else {
      prob_greater_equal_ = kCombineProb.combine_prob[prob_greater_equal_][child.prob_greater_equal_];
      current_log_derivative = child.max_log_derivative_ + kCombineProb.log_derivative[child.prob_greater_equal_];
    }
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

  void Finalize(bool shallow) {
    if (prob_greater_equal_ == kProbStep || prob_greater_equal_ == 0) {
      max_log_derivative_ = kLogDerivativeMinusInf;
    } else {
      if (shallow) {
        max_log_derivative_ -= kCombineProb.log_derivative_shallow[prob_greater_equal_];
      } else {
        max_log_derivative_ -= kCombineProb.log_derivative[prob_greater_equal_];
      }
      assert(max_log_derivative_ < 0);
      assert(max_log_derivative_ > kLogDerivativeMinusInf);
    }
    prob_greater_equal_ = (kProbStep - prob_greater_equal_);
    if (prob_greater_equal_ == kProbStep) {
      disproof_number_ = kProofNumberStep;
    } else {
      assert(disproof_number_ < kProofNumberStep);
    }
    Check();
  }

  void SetLeaf(
      BitPattern player, BitPattern opponent, EvalLarge goal, EvalLarge eval,
      Square depth, Square n_empties) {
    prob_greater_equal_ = kCombineProb.gaussian_cdf[DataToCDFOffset(depth, n_empties, goal - eval)];
    float proof_number = ::ProofNumber(player, opponent, goal, eval);
    assert(isfinite(proof_number) && proof_number > 0);
    float disproof_number = ::DisproofNumber(player, opponent, goal, eval);

    assert(isfinite(disproof_number) && disproof_number > 0);
    proof_number_ = prob_greater_equal_ == 0 ? kProofNumberStep :
        ProofNumberToByte(proof_number);
    disproof_number_ = prob_greater_equal_ == kProbStep ? kProofNumberStep :
        ProofNumberToByte(disproof_number);
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

  double RemainingWork() const {
    Check();
    double prob_greater_equal = ProbGreaterEqual();
    if (prob_greater_equal < 0.01) {
      return DisproofNumber();
    } else if (prob_greater_equal > 0.99) {
      return ProofNumber();
    }
    return std::max(ProofNumber(), DisproofNumber());
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
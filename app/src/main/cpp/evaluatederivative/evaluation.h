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

#include <cassert>
#include <cmath>
#include <float.h>
#include "prob_combiners.h"
#include "../board/bitpattern.h"
#include "../utils/misc.h"

typedef uint8_t Probability;
typedef uint8_t PN;
constexpr Probability kProbStep = 255;
constexpr int kLogDerivativeMinusInf = -100000000;
constexpr int kNoLogDerivative = -kLogDerivativeMinusInf;
constexpr int kLogDerivativeMultiplier = 10000;
constexpr float kMaxProofNumber = 1E25;
constexpr int kProofNumberStep = 255;
constexpr float kBaseLogProofNumber = ConstexprPow(kMaxProofNumber, 1.0 / (kProofNumberStep - 1.99));  // kBaseLogProofNumber ^ (kProofNumberStep-1) = kMaxProofNumber.

PN ProofNumberToByte(float proof_number) {
  assert(proof_number == 0 || proof_number >= 1);
  if (proof_number <= 1E-8) {
    return 0;
  } else if (proof_number > kMaxProofNumber) {
    return kProofNumberStep;
  }
  float rescaled_proof_number = log(proof_number) / log(kBaseLogProofNumber) + 1;
  assert(rescaled_proof_number >= 0.5 && rescaled_proof_number <= kProofNumberStep - 0.5);
  return (PN) round(rescaled_proof_number);
}

float ByteToProofNumber(PN byte) {
  if (byte == 0) {
    return 0;
  } else if (byte == kProofNumberStep) {
    return INFINITY;
  }
  return pow(kBaseLogProofNumber, byte - 1);
}

inline int LeafLogDerivative(float prob) {
  return round(kLogDerivativeMultiplier * 1 * log(prob * (1 - prob)));
}

struct CombineProb {
  Probability combine_prob[kProbStep + 1][kProbStep + 1];
  int log_derivative[kProbStep + 1];
  PN combine_disproof_number[kProofNumberStep + 1][kProofNumberStep + 1];
  PN disproof_to_proof_number[kProofNumberStep + 1][kProbStep + 1];

  CombineProb() : combine_prob(), log_derivative(), combine_disproof_number() {
    ProbCombiner combiner(ExpPolyLog<170>);
    for (int i = 0; i <= kProofNumberStep; ++i) {
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
        disproof_to_proof_number[i][j] = ProofNumberToByte(std::max(
        1.0F,
        std::min(kMaxProofNumber, ByteToProofNumber(i) / std::max(0.0001F, (1.0F - j / (float) kProofNumberStep)))));
        assert(i == 0 || disproof_to_proof_number[i][j] > 0);
      }
    }
    for (int i = 0; i <= kProbStep; ++i) {
      double x1 = i / (double) kProbStep;
      if (x1 < 1E-15 || x1 > 1 - 1E-15) {
        log_derivative[i] = kLogDerivativeMinusInf;
      } else {
        log_derivative[i] = round(
            kLogDerivativeMultiplier
            * log(combiner.f(x1) / combiner.derivative(x1)));
        assert(log_derivative[i] > kLogDerivativeMinusInf);
      }
      for (int j = i; j <= kProbStep; ++j) {
        double x2 = j / (double) kProbStep;
        combine_prob[i][j] = (Probability) (combiner.inverse(combiner.f(x1) * combiner.f(x2)) * kProbStep + 0.5);
        combine_prob[j][i] = combine_prob[i][j];
        assert(j == i || i == 0 || combine_prob[i][j] >= combine_prob[i][j-1]);
        assert(combine_prob[i][j] <= i);
        assert(combine_prob[i][j] <= j);
      }
    }
  }
};

const CombineProb kCombineProb;

class Evaluation {
 public:
  Evaluation() { Reset(); }
  void Reset() {
    max_log_derivative_ = kNoLogDerivative;
  }
  float ProofNumber() const { return ByteToProofNumber(proof_number_); }
  float DisproofNumber() const { return ByteToProofNumber(disproof_number_); }
  int MaxLogDerivative() const { return max_log_derivative_; }
  bool IsValid() const { return max_log_derivative_ != kNoLogDerivative; }

  float ProbGreaterEqual() const {
    return IsValid() ? ProbGreaterEqualUnsafe() : -1;
  }

  bool IsSolved() const {
    return IsValid() && (proof_number_ == 0 || disproof_number_ == 0);
  }

  int LogDerivative(const Evaluation& father) const {
    if (!IsValid()) {
      return 0;
    }
    if (prob_greater_equal_ == 0 || prob_greater_equal_ == kProbStep ||
        father.prob_greater_equal_ == 0 || father.prob_greater_equal_ == kProbStep) {
      return kLogDerivativeMinusInf;
    }
    return max_log_derivative_ - kCombineProb.log_derivative[prob_greater_equal_]
           + kCombineProb.log_derivative[kProbStep - father.prob_greater_equal_];
  }

  void Initialize() {
    proof_number_ = kProofNumberStep;
    disproof_number_ = 0;
    prob_greater_equal_ = kProbStep;
    max_log_derivative_ = kLogDerivativeMinusInf;
  }

  void UpdateFatherWithThisChild(const Evaluation& child) {
    assert(IsValid());
    assert(child.IsValid());
    assert(max_log_derivative_ + kCombineProb.log_derivative[prob_greater_equal_] < 0);
    assert(child.max_log_derivative_ < 0);
    float child_prob_greater_equal = child.ProbGreaterEqualUnsafe();
    if (child.IsSolved()) {
      assert(child_prob_greater_equal == 0 || child_prob_greater_equal == 1);
      if (child_prob_greater_equal == 0) {
        prob_greater_equal_ = 0;
        proof_number_ = 0;
        disproof_number_ = kProofNumberStep;
        max_log_derivative_ = kLogDerivativeMinusInf;
      }
      // If child_prob_greater_equal == 1 do nothing, it's as if the child did
      // not exist.
      return;
    }
    assert(child.proof_number_ != 0 && child.proof_number_ != kProofNumberStep);
    prob_greater_equal_ = kCombineProb.combine_prob[prob_greater_equal_][child.prob_greater_equal_];
    assert(prob_greater_equal_ <= child.prob_greater_equal_);

    PN cur_proof_number = kCombineProb.disproof_to_proof_number[child.disproof_number_][child.prob_greater_equal_];
    assert(cur_proof_number > 0);
    assert(cur_proof_number < kProofNumberStep);
    if (cur_proof_number < proof_number_) {
      proof_number_ = cur_proof_number;
    }
    disproof_number_ = kCombineProb.combine_disproof_number[disproof_number_][child.proof_number_];
    if (child_prob_greater_equal > 0 && child_prob_greater_equal < 1) {
      assert(child.max_log_derivative_ < 0);
      int current_log_derivative =
          child.max_log_derivative_ - kCombineProb.log_derivative[child.prob_greater_equal_];
      if (current_log_derivative > max_log_derivative_) {
        max_log_derivative_ = current_log_derivative;
      }
      assert(max_log_derivative_ + kCombineProb.log_derivative[prob_greater_equal_] < 0);
    }
  }

  void Finalize() {
    if (prob_greater_equal_ == kProbStep || prob_greater_equal_ == 0) {
      max_log_derivative_ = kLogDerivativeMinusInf;
    } else {
      max_log_derivative_ = max_log_derivative_ + kCombineProb.log_derivative[prob_greater_equal_];
      assert(max_log_derivative_ < 0);
      assert(max_log_derivative_ > kLogDerivativeMinusInf);
    }
    prob_greater_equal_ = (kProbStep - prob_greater_equal_);
  }

  void SetLeaf(float prob_greater_equal, float proof_number,
               float disproof_number, float log_derivative_add = 0) {
    prob_greater_equal_ = RoundProb(prob_greater_equal);
    proof_number_ = ProofNumberToByte(proof_number);
    disproof_number_ = ProofNumberToByte(disproof_number);
    assert((proof_number_ == 0) == (disproof_number_ == kProofNumberStep));
    assert((disproof_number_ == 0) == (proof_number_ == kProofNumberStep));
    assert(proof_number_ != 0 || prob_greater_equal_ == kProbStep);
    assert(disproof_number_ != 0 || prob_greater_equal_ == 0);
    if (prob_greater_equal_ == 0 || prob_greater_equal_ == kProbStep) {
      max_log_derivative_ = kLogDerivativeMinusInf;
    } else {
      max_log_derivative_ = LeafLogDerivative(ProbGreaterEqualUnsafe()) + log_derivative_add;
    }
    assert(max_log_derivative_ < 0);
  }

  void SetDisproved() {
    SetLeaf(0, INFINITY, 0);
  }

  void SetProved() {
    SetLeaf(1, 0, INFINITY);
  }

  double GetValue(const Evaluation& father) const {
    float prob = father.ProbGreaterEqual();
    if (IsSolved()) {
      return -DBL_MAX;
    }
    if (prob == 0) {
      assert(prob_greater_equal_ == kProbStep);
      return 0.00000001 * ProofNumber();
         //* (1 + 5000 / n_visited)
//         - 1000.0 * kLogDerivativeMinusInf;
//             - child.getStoredBoard().getNThreadsWorking() * 1.0E15;
    } else if (prob == 1) {
      return -kCombineProb.disproof_to_proof_number[disproof_number_][prob_greater_equal_];
    } else {
      return LogDerivative(father);// - (1 / std::min(prob, 0.5F));
//             * avoidNextPosFailCoeff.get()
//             * child.getStoredBoard().getNThreadsWorking();
    }
  }

  float RemainingWork() const {
    float prob_greater_equal = ProbGreaterEqual();
    return ProofNumber() * prob_greater_equal + DisproofNumber() * (1 - prob_greater_equal);
  }

 private:
  Probability prob_greater_equal_;
  PN proof_number_;
  PN disproof_number_;
  int max_log_derivative_;

  static Probability RoundProb(float prob) {
    return (Probability) round(prob * kProbStep);
  }

  float ProbGreaterEqualUnsafe() const {
    return ((float) prob_greater_equal_) / kProbStep;
  }
};

//
// private
//  double getValue(Evaluation father, boolean proof) {
//    synchronized(StoredBoard.this) {
//      float prob = father.getProb();
//      Evaluation child = getEvaluation(-father.evalGoal);
//      if (isSolved()) {
//        return Double.NEGATIVE_INFINITY;
//      }
//      if (prob == 0) {
//        return child.proofNumber
//               - child.getStoredBoard().getNThreadsWorking() * 1.0E15;
//      } else if (prob == 1 || (proof && prob > 0.5)) {
//        return -Math.max(1, child.disproofNumber
//                            / Math.max(0.0001F, 1 - child.getProb()));
//      } else {
//        return child.logDerivative(father) -
//               (1 / Math.min(prob, 0.5)) * avoidNextPosFailCoeff.get()
//               * child.getStoredBoard().getNThreadsWorking();
//      }
//    }
//  }
//
//
// private
//  StoredBoardBestDescendant bestDescendant(int alpha, int beta, boolean proof,
//                                           ArrayList <Evaluation> ancestors) {
//    Evaluation bestChild;
//    synchronized(StoredBoard.this) {
//      increaseNThreadsWorking();
//      ancestors.add(this);
//      if (isSolved() || (isLeaf() && getNThreadsWorking() > 1)) {
//        bestChild = null;
//      } else {
//        if (isLeaf()) {
//          avoidNextPosFailCoeff.addAndGet(-1);
//          Stats.addToNSuccessNextPosition(1);
//          return new StoredBoardBestDescendant(
//              this,
//              ancestors,
//              alpha,
//              beta
//          );
//        }
//        alpha = Math.max(alpha, Math.min(
//            getPercentileLower(Constants.PROB_FOR_ENDGAME_ALPHA_BETA) + 100,
//            evalGoal));
//        beta = Math.min(beta, Math.max(
//            getPercentileUpper(Constants.PROB_FOR_ENDGAME_ALPHA_BETA) - 100,
//            evalGoal));
//
//        bestChild = bestChild(proof);
//      }
//    }
//    if (bestChild == null) {
//      avoidNextPosFailCoeff.addAndGet(10);
//      for (Evaluation eval : ancestors) {
//        eval.getStoredBoard().decreaseNThreadsWorking();
//      }
//      Stats.addToNFailNextPosition(1);
//      return null;
//    }
//    return bestChild.bestDescendant(-beta, -alpha, proof, ancestors);
//  }
//
//  StoredBoard getStoredBoard() {
//    return StoredBoard.this;
//  }
//
//
//
// private
//  boolean isChildLogDerivativeOK() {
//    if (isLeaf()) {
//      return true;
//    }
//    int expectedLogDerivative = LOG_DERIVATIVE_MINUS_INF;
//    String childLogDerivatives = "";
//    for (StoredBoard child : children) {
//      synchronized(child)
//      {
//        Evaluation childEval = child.getEvaluation(-evalGoal);
//        if (childEval.getProb() > 0 && childEval.getProb() < 1) {
//          assert
//          childEval.logDerivative(this) <= child.maxLogDerivative(-evalGoal);
//          childLogDerivatives += "  " + childEval.logDerivative(this) + "\n";
//          expectedLogDerivative =
//              Math.max(expectedLogDerivative, childEval.logDerivative(this));
//        }
//      }
//    }
//    if (maxLogDerivative != expectedLogDerivative) {
//      throw new AssertionError(
//          "Wrong maxLogDerivative. Expected " + expectedLogDerivative + ", got "
//          + maxLogDerivative + ".\n" + childLogDerivatives);
//    }
//    return true;
//  }
//
//};
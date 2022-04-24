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
#include "prob_combiners.h"
#include "../board/bitpattern.h"
#include "../utils/misc.h"

typedef uint8_t Probability;
constexpr Probability kProbStep = 255;
constexpr int kLogDerivativeMinusInf = -1000000000;
constexpr int kNoLogDerivative = kLogDerivativeMinusInf - 1;
constexpr int kLogDerivativeMultiplier = 10000;

struct CombineProb {
  Probability combine_prob[kProbStep + 1][kProbStep + 1];
  int log_derivative[kProbStep + 1];

  CombineProb() : combine_prob(), log_derivative() {
    ProbCombiner combiner(ExpPolyLog<170>);
    for (int i = 0; i <= kProbStep; ++i) {
      double x1 = i / (double) kProbStep;
      if (x1 < 1E-15 || x1 > 1 - 1E-15) {
        log_derivative[i] = kLogDerivativeMinusInf;
      } else {
        log_derivative[i] = (int) (
            -kLogDerivativeMultiplier
            * ConstexprLog(combiner.derivative(x1) / combiner.f(x1)) + 0.5);
        assert(log_derivative[i] > kLogDerivativeMinusInf);
      }
      for (int j = i; j <= kProbStep; ++j) {
        double x2 = j / (double) kProbStep;
        combine_prob[i][j] = (Probability) (combiner.inverse(combiner.f(x1) * combiner.f(x2)) * kProbStep + 0.5);
        combine_prob[j][i] = combine_prob[i][j];
        assert(j == i || i == 0 || combine_prob[i][j] >= combine_prob[i][j-1]);
      }
    }
  }
};

const CombineProb kCombineProb;

class Evaluation {
 public:
  Evaluation() : max_log_derivative_(0), n_visited_(0) {}
  void Reset() {
    max_log_derivative_ = 0;
    n_visited_ = 0;
  }
  float ProofNumber() const { return proof_number_; }
  float DisproofNumber() const { return disproof_number_; }
  int MaxLogDerivative() const { return max_log_derivative_; }
  bool IsValid() const { return max_log_derivative_ < 0; }
  NVisited GetNVisited() const { return n_visited_; }

  float ProbGreaterEqual() const {
    assert(IsValid());
    return ProbGreaterEqualUnsafe();
  }

  bool IsSolved() const {
    assert(IsValid());
    return proof_number_ == 0 || disproof_number_ == 0;
  }

  int LogDerivative(const Evaluation& father) const {
    assert(IsValid());
    if (prob_greater_equal_ == 0 || prob_greater_equal_ == kProbStep ||
        father.prob_greater_equal_ == 0 || father.prob_greater_equal_ == kProbStep) {
      return kLogDerivativeMinusInf;
    }
    return max_log_derivative_ - kCombineProb.log_derivative[prob_greater_equal_]
           + kCombineProb.log_derivative[kProbStep - father.prob_greater_equal_];
  }

  NVisited Descendants() const {
    assert(IsValid());
    return n_visited_;
  }

  void AddDescendants(NVisited new_descendants) {
    assert(IsValid());
    n_visited_ += new_descendants;
  }

  void UpdateFatherWithThisChild(const Evaluation& child) {
    assert(IsValid());
    assert(child.IsValid());
    if (child.IsSolved()) {
      if (child.ProbGreaterEqual() > 0.5) {
        return;
      } else {
        prob_greater_equal_ = 0;
        proof_number_ = 0;
        disproof_number_ = -INFINITY;
        max_log_derivative_ = kLogDerivativeMinusInf;
      }
    }
    prob_greater_equal_ = kCombineProb.combine_prob[prob_greater_equal_][child.prob_greater_equal_];

    float cur_proof_number = std::max(1.0F, child.disproof_number_ / std::max(0.0001F, 1.0F - child.ProbGreaterEqual()));
    if (cur_proof_number < proof_number_) {
//      assert(Utils.arrayContains(getChildren(), child.getStoredBoard()));
      proof_number_ = cur_proof_number;
    }

    assert(child.proof_number_ > 0);
    disproof_number_ += child.proof_number_;
    if (child.ProbGreaterEqual() > 0 && child.ProbGreaterEqual() < 1) {
      int current_log_derivative =
          child.max_log_derivative_ - kCombineProb.log_derivative[child.prob_greater_equal_];
      if (current_log_derivative > max_log_derivative_) {
//        assert(Utils.arrayContains(getChildren(), child.getStoredBoard()));
        max_log_derivative_ = current_log_derivative;
      }
    }
  }

  void SetLeaf(float prob_greater_equal, float proof_number,
               float disproof_number) {
    SetLeaf(prob_greater_equal, proof_number, disproof_number, 0);
  }
  void SetLeaf(float prob_greater_equal, float proof_number,
               float disproof_number, float log_derivative_add) {
    prob_greater_equal_ = RoundProb(prob_greater_equal);
    proof_number_ = proof_number;
    disproof_number_ = disproof_number;
    if (prob_greater_equal_ == 0 || prob_greater_equal_ == kProbStep) {
      max_log_derivative_ = kLogDerivativeMinusInf;
    } else {
      max_log_derivative_ = round(
          kLogDerivativeMultiplier * kLeafMultiplier
          * (log(ProbGreaterEqualUnsafe() * (1 - ProbGreaterEqualUnsafe()))
             + log_derivative_add));
    }
    assert(max_log_derivative_ < 0);
  }

  void SetDisproved() {
    SetLeaf(0, INFINITY, 0);
  }

  void SetProved() {
    SetLeaf(1, 0, INFINITY);
  }

 private:

  static Probability RoundProb(float prob) {
    return (Probability) round(prob * kProbStep);
  }

  float ProbGreaterEqualUnsafe() const {
    return ((float) prob_greater_equal_) / kProbStep;
  }

  Probability prob_greater_equal_;
  float proof_number_;
  float disproof_number_;
  int max_log_derivative_;
  NVisited n_visited_ = 0;
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
// public
//  Evaluation bestChild(boolean proof) {
//    assert !isLeaf();
//    assert isLowerUpperOK();
//    Evaluation bestChild = null;
//    double bestChildValue = Double.NEGATIVE_INFINITY;
//    for (StoredBoard childBoard : getChildren()) {
//      synchronized(childBoard)
//      {
//        Evaluation child = childBoard.getEvaluation(-evalGoal);
//        double childValue = child.getValue(this, proof);
//        if (childValue > bestChildValue) {
//          bestChild = child;
//          bestChildValue = childValue;
//        }
//      }
//    }
//    assert isNThreadsWorkingOK();
//    if (bestChild == null) {
//      return null;
//    }
//    if (bestChild.getStoredBoard().isLeaf()
//        && bestChild.getStoredBoard().getNThreadsWorking() > 0) {
//      return null;
//    }
//    return bestChild;
//  }
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
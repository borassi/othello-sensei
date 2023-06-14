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

#include <limits.h>
#include <math.h>
#include "win_probability.h"
#include "../board/bitpattern.h"
#include "../board/get_moves.h"
#include "../utils/misc.h"

#ifndef OTHELLOSENSEI_ENDGAME_TIME_H
#define OTHELLOSENSEI_ENDGAME_TIME_H

typedef uint8_t PN;
constexpr float kMaxProofNumber = 1E25;
constexpr int kProofNumberStep = 255;
constexpr float kBaseLogProofNumber = ConstexprPow(kMaxProofNumber, 1.0 / (kProofNumberStep - 1.99));
constexpr int kMaxProofNumberOffset = /*depth*/ 16 * /*n_empties*/ 64 * /*eval_delta*/ (256+1);

constexpr PN ProofNumberToByte(float proof_number) {
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

constexpr float ByteToProofNumberExplicit(PN byte) {
  if (byte == 0) {
    return 0;
  } else if (byte == kProofNumberStep) {
    return INFINITY;
  }
  return pow(kBaseLogProofNumber, byte - 1);
}


constexpr int DataToProofNumberOffset(Square empties, Square moves, int eval_delta) {
  assert(moves <= 15);
  assert(empties <= 63);
  assert(eval_delta <= 128);
  assert(eval_delta >= -128);
  return empties | (moves << 6) | (eval_delta - 2 * kMinEval) << 10;
}

constexpr std::tuple<Square, Square, int> ProofNumberOffsetToEmptiesMovesEval(int cdf_offset) {
  return std::make_tuple((cdf_offset & 63), (cdf_offset >> 6) & 15, (cdf_offset >> 10) + 2 * kMinEval);
}

inline double Bound(double value) {
  return std::max(1.0, std::min(kMaxProofNumber * 0.99, value));
}

inline double ConvertProofNumber(double old, Eval old_goal, Eval new_goal) {
  assert(new_goal < old_goal);
  return Bound(exp(log(old) + 0.07 * (new_goal - old_goal)));
}

inline double ConvertDisproofNumber(double old, int old_goal, int new_goal) {
  assert(new_goal > old_goal);
  return Bound(exp(log(old) - 0.07 * (new_goal - old_goal)));
}

inline double LogProofNumber(int n_empties, Square moves_opponent, int error) {
switch (n_empties) {
  case 0:
    return +2.3823 -0.0049 * moves_opponent -0.0069 * error;
  case 1:
    return +2.6645 -0.0049 * moves_opponent -0.0069 * error;
  case 2:
    return +2.9467 -0.0049 * moves_opponent -0.0069 * error;
  case 3:
    return +3.2290 -0.0049 * moves_opponent -0.0069 * error;
  case 4:
    return +3.4752 +0.0019 * moves_opponent -0.0067 * error;
  case 5:
    return +3.8135 +0.0052 * moves_opponent -0.0083 * error;
  case 6:
    return +4.1489 +0.0257 * moves_opponent -0.0101 * error;
  case 7:
    return +4.5471 +0.0315 * moves_opponent -0.0121 * error;
  case 8:
    return +4.9379 +0.0384 * moves_opponent -0.0139 * error;
  case 9:
    return +5.3321 +0.0647 * moves_opponent -0.0176 * error;
  case 10:
    return +5.7920 +0.0703 * moves_opponent -0.0214 * error;
  case 11:
    return +6.2460 +0.0834 * moves_opponent -0.0253 * error;
  case 12:
    return +6.7418 +0.0994 * moves_opponent -0.0294 * error;
  case 13:
    return +7.2755 +0.1125 * moves_opponent -0.0345 * error;
  case 14:
    return +7.7354 +0.1332 * moves_opponent -0.0379 * error;
  case 15:
    return +8.1444 +0.1637 * moves_opponent -0.0420 * error;
  case 16:
    return +8.6897 +0.1802 * moves_opponent -0.0471 * error;
  case 17:
    return +9.2969 +0.1900 * moves_opponent -0.0522 * error;
  case 18:
    return +10.0266 +0.1858 * moves_opponent -0.0570 * error;
  case 19:
    return +10.6556 +0.1936 * moves_opponent -0.0604 * error;
  case 20:
    return +11.3347 +0.1924 * moves_opponent -0.0625 * error;
  case 21:
    return +11.9484 +0.2040 * moves_opponent -0.0664 * error;
  case 22:
    return +12.5270 +0.2224 * moves_opponent -0.0698 * error;
  case 23:
    return +13.1520 +0.2373 * moves_opponent -0.0745 * error;
  case 24:
    return +13.8372 +0.2411 * moves_opponent -0.0763 * error;
  case 25:
    return +14.5052 +0.2408 * moves_opponent -0.0763 * error;
  case 26:
    return +15.1244 +0.2425 * moves_opponent -0.0756 * error;
  case 27:
    return +15.5652 +0.2653 * moves_opponent -0.0765 * error;
  case 28:
    return +16.0781 +0.2727 * moves_opponent -0.0764 * error;
  case 29:
    return +16.5090 +0.2955 * moves_opponent -0.0784 * error;
  case 30:
    return +17.1433 +0.2902 * moves_opponent -0.0806 * error;
  case 31:
    return +17.6227 +0.3109 * moves_opponent -0.0825 * error;
  case 32:
    return +17.3572 +0.4034 * moves_opponent -0.0837 * error;
  case 33:
    return +17.9530 +0.3978 * moves_opponent -0.0809 * error;
  case 34:
    return +18.8028 +0.3810 * moves_opponent -0.0821 * error;
  case 35:
    return +19.3981 +0.3939 * moves_opponent -0.0799 * error;
  case 36:
    return +21.3964 +0.2864 * moves_opponent -0.0840 * error;
  case 37:
    return +22.5100 +0.3092 * moves_opponent -0.0825 * error;
  case 38:
    return +24.1326 +0.1616 * moves_opponent -0.0824 * error;
  case 39:
    return +24.7671 +0.1616 * moves_opponent -0.0824 * error;
  case 40:
    return +25.4017 +0.1616 * moves_opponent -0.0824 * error;
  case 41:
    return +26.0363 +0.1616 * moves_opponent -0.0824 * error;
  case 42:
    return +26.6709 +0.1616 * moves_opponent -0.0824 * error;
  case 43:
    return +27.3055 +0.1616 * moves_opponent -0.0824 * error;
  case 44:
    return +27.9400 +0.1616 * moves_opponent -0.0824 * error;
  case 45:
    return +28.5746 +0.1616 * moves_opponent -0.0824 * error;
  case 46:
    return +29.2092 +0.1616 * moves_opponent -0.0824 * error;
  case 47:
    return +29.8438 +0.1616 * moves_opponent -0.0824 * error;
  case 48:
    return +30.4783 +0.1616 * moves_opponent -0.0824 * error;
  case 49:
    return +31.1129 +0.1616 * moves_opponent -0.0824 * error;
  case 50:
    return +31.7475 +0.1616 * moves_opponent -0.0824 * error;
  case 51:
    return +32.3821 +0.1616 * moves_opponent -0.0824 * error;
  case 52:
    return +33.0166 +0.1616 * moves_opponent -0.0824 * error;
  case 53:
    return +33.6512 +0.1616 * moves_opponent -0.0824 * error;
  case 54:
    return +34.2858 +0.1616 * moves_opponent -0.0824 * error;
  case 55:
    return +34.9204 +0.1616 * moves_opponent -0.0824 * error;
  case 56:
    return +35.5549 +0.1616 * moves_opponent -0.0824 * error;
  case 57:
    return +36.1895 +0.1616 * moves_opponent -0.0824 * error;
  case 58:
    return +36.8241 +0.1616 * moves_opponent -0.0824 * error;
  case 59:
    return +37.4587 +0.1616 * moves_opponent -0.0824 * error;
  case 60:
    return +38.0932 +0.1616 * moves_opponent -0.0824 * error;
  default:
    assert(false);
    return 1;
}
}

inline double LogDisproofNumber(int n_empties, Square moves_player, int error) {
switch (n_empties) {
  case 0:
    return +2.5647 +0.3849 * moves_player +0.0063 * error;
  case 1:
    return +2.6427 +0.3849 * moves_player +0.0063 * error;
  case 2:
    return +2.7207 +0.3849 * moves_player +0.0063 * error;
  case 3:
    return +2.7987 +0.3849 * moves_player +0.0063 * error;
  case 4:
    return +3.0259 +0.3559 * moves_player +0.0089 * error;
  case 5:
    return +3.2196 +0.3445 * moves_player +0.0111 * error;
  case 6:
    return +3.5497 +0.3195 * moves_player +0.0137 * error;
  case 7:
    return +3.8825 +0.3106 * moves_player +0.0160 * error;
  case 8:
    return +4.2705 +0.3033 * moves_player +0.0189 * error;
  case 9:
    return +4.7928 +0.2886 * moves_player +0.0243 * error;
  case 10:
    return +5.3074 +0.2795 * moves_player +0.0297 * error;
  case 11:
    return +5.7678 +0.2878 * moves_player +0.0351 * error;
  case 12:
    return +6.2748 +0.2993 * moves_player +0.0407 * error;
  case 13:
    return +6.8043 +0.3054 * moves_player +0.0456 * error;
  case 14:
    return +7.3693 +0.3106 * moves_player +0.0513 * error;
  case 15:
    return +7.9107 +0.3224 * moves_player +0.0575 * error;
  case 16:
    return +8.4207 +0.3410 * moves_player +0.0639 * error;
  case 17:
    return +8.8964 +0.3661 * moves_player +0.0689 * error;
  case 18:
    return +9.4206 +0.3756 * moves_player +0.0723 * error;
  case 19:
    return +10.0301 +0.3825 * moves_player +0.0754 * error;
  case 20:
    return +10.6434 +0.3907 * moves_player +0.0798 * error;
  case 21:
    return +11.1585 +0.4021 * moves_player +0.0807 * error;
  case 22:
    return +11.6420 +0.4162 * moves_player +0.0815 * error;
  case 23:
    return +12.3193 +0.4172 * moves_player +0.0834 * error;
  case 24:
    return +12.9593 +0.4149 * moves_player +0.0840 * error;
  case 25:
    return +13.5496 +0.4171 * moves_player +0.0836 * error;
  case 26:
    return +14.2067 +0.4112 * moves_player +0.0834 * error;
  case 27:
    return +14.7368 +0.4132 * moves_player +0.0823 * error;
  case 28:
    return +15.3281 +0.4165 * moves_player +0.0819 * error;
  case 29:
    return +15.6478 +0.4418 * moves_player +0.0815 * error;
  case 30:
    return +16.4196 +0.4197 * moves_player +0.0815 * error;
  case 31:
    return +17.5832 +0.3736 * moves_player +0.0867 * error;
  case 32:
    return +16.8901 +0.4616 * moves_player +0.0767 * error;
  case 33:
    return +17.3387 +0.4674 * moves_player +0.0752 * error;
  case 34:
    return +18.0499 +0.4549 * moves_player +0.0753 * error;
  case 35:
    return +18.1173 +0.4807 * moves_player +0.0751 * error;
  case 36:
    return +18.5480 +0.4755 * moves_player +0.0772 * error;
  case 37:
    return +18.6004 +0.4705 * moves_player +0.0753 * error;
  case 38:
    return +20.8487 +0.3855 * moves_player +0.0753 * error;
  case 39:
    return +21.3969 +0.3855 * moves_player +0.0753 * error;
  case 40:
    return +21.9451 +0.3855 * moves_player +0.0753 * error;
  case 41:
    return +22.4934 +0.3855 * moves_player +0.0753 * error;
  case 42:
    return +23.0416 +0.3855 * moves_player +0.0753 * error;
  case 43:
    return +23.5898 +0.3855 * moves_player +0.0753 * error;
  case 44:
    return +24.1381 +0.3855 * moves_player +0.0753 * error;
  case 45:
    return +24.6863 +0.3855 * moves_player +0.0753 * error;
  case 46:
    return +25.2345 +0.3855 * moves_player +0.0753 * error;
  case 47:
    return +25.7827 +0.3855 * moves_player +0.0753 * error;
  case 48:
    return +26.3310 +0.3855 * moves_player +0.0753 * error;
  case 49:
    return +26.8792 +0.3855 * moves_player +0.0753 * error;
  case 50:
    return +27.4274 +0.3855 * moves_player +0.0753 * error;
  case 51:
    return +27.9756 +0.3855 * moves_player +0.0753 * error;
  case 52:
    return +28.5239 +0.3855 * moves_player +0.0753 * error;
  case 53:
    return +29.0721 +0.3855 * moves_player +0.0753 * error;
  case 54:
    return +29.6203 +0.3855 * moves_player +0.0753 * error;
  case 55:
    return +30.1685 +0.3855 * moves_player +0.0753 * error;
  case 56:
    return +30.7168 +0.3855 * moves_player +0.0753 * error;
  case 57:
    return +31.2650 +0.3855 * moves_player +0.0753 * error;
  case 58:
    return +31.8132 +0.3855 * moves_player +0.0753 * error;
  case 59:
    return +32.3614 +0.3855 * moves_player +0.0753 * error;
  case 60:
    return +32.9097 +0.3855 * moves_player +0.0753 * error;
  default:
    assert(false);
    return 1;
}
}

struct ProofDisproofNumberData {
  PN proof_number[kMaxProofNumberOffset + 1];
  PN disproof_number[kMaxProofNumberOffset + 1];
  int disproof_number_over_prob[kMaxProofNumberOffset + 1];
  double byte_to_proof_number[kProofNumberStep + 1];

  ProofDisproofNumberData() : proof_number(), disproof_number(), byte_to_proof_number() {
    for (Square empties = 1; empties < 60; ++empties) {
      for (Square moves = 0; moves <= 15; ++moves) {
        for (EvalLarge delta = -128; delta <= 128; ++delta) {
          int offset = DataToProofNumberOffset(empties, moves, delta);
          assert(offset >= 0);
          assert(offset <= kMaxProofNumberOffset);
          proof_number[offset] =
              ProofNumberToByte(Bound(exp(LogProofNumber(empties, moves, delta))));
          disproof_number[offset] =
              ProofNumberToByte(Bound(exp(LogDisproofNumber(empties, moves, delta))));
          disproof_number_over_prob[offset] =
              (int) round(
                  std::min(
                      (double) INT_MAX - 2,
                      Bound(exp(LogDisproofNumber(empties, moves, delta))) /
                          ProbabilityExplicit(1, empties, delta)));
          assert(proof_number[offset] >= 1);
          assert(proof_number[offset] <= kProofNumberStep - 1);
          assert(disproof_number[offset] >= 1);
          assert(disproof_number[offset] <= kProofNumberStep - 1);
        }
      }
    }
    for (int i = 0; i <= kProofNumberStep; ++i) {
      byte_to_proof_number[i] = ByteToProofNumberExplicit(i);
    }
  }
};

const ProofDisproofNumberData kProofDisproofNumberData;

constexpr float ByteToProofNumber(PN byte) {
  return kProofDisproofNumberData.byte_to_proof_number[byte];
}

inline PN DisproofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return kProofDisproofNumberData.disproof_number[DataToProofNumberOffset(
      __builtin_popcountll(~(player | opponent)),
      std::min(15, GetNMoves(player, opponent)),
      (approx_eval - lower) >> 3
  )];
}

inline PN ProofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return kProofDisproofNumberData.proof_number[DataToProofNumberOffset(
      __builtin_popcountll(~(player | opponent)),
      std::min(15, GetNMoves(opponent, player)),
      (approx_eval - lower) >> 3
  )];
}

inline float DisproofNumberOverProb(
    BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return kProofDisproofNumberData.disproof_number_over_prob[DataToProofNumberOffset(
      __builtin_popcountll(~(player | opponent)),
      std::min(15, GetNMoves(player, opponent)),
      (approx_eval - lower) >> 3
  )];
}
#endif  // OTHELLOSENSEI_ENDGAME_TIME_H
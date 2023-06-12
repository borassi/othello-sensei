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
    case 1:
      return 0;
    case 2:
      return 0.69314718056; /*ln(2)*/
    case 3:
      return 1.79175946923; /*ln(3*2)*/
    case 4:
      return 3.17805383035; /*ln(4*3*2)*/
    case 5:
      return +3.9006 -0.0721 * moves_opponent -0.0057 * error;
    case 6:
      return +4.2667 -0.0299 * moves_opponent -0.0076 * error;
    case 7:
      return +4.3829 -0.0032 * moves_opponent -0.0075 * error;
    case 8:
      return +4.7421 +0.0309 * moves_opponent -0.0088 * error;
    case 9:
      return +5.5805 -0.0370 * moves_opponent -0.0150 * error;
    case 10:
      return +6.0840 +0.0094 * moves_opponent -0.0224 * error;
    case 11:
      return +6.6991 +0.0362 * moves_opponent -0.0328 * error;
    case 12:
      return +6.6929 +0.0884 * moves_opponent -0.0258 * error;
    case 13:
      return +7.2202 +0.1581 * moves_opponent -0.0403 * error;
    case 14:
      return +8.1257 +0.1112 * moves_opponent -0.0470 * error;
    case 15:
      return +8.9624 +0.1124 * moves_opponent -0.0596 * error;
    case 16:
      return +8.3900 +0.2078 * moves_opponent -0.0478 * error;
    case 17:
      return +9.3032 +0.2437 * moves_opponent -0.0675 * error;
    case 18:
      return +10.0704 +0.2507 * moves_opponent -0.0743 * error;
    case 19:
      return +10.6314 +0.1971 * moves_opponent -0.0653 * error;
    case 20:
      return +11.0976 +0.2351 * moves_opponent -0.0638 * error;
    case 21:
      return +12.0104 +0.2209 * moves_opponent -0.0770 * error;
    case 22:
      return +12.2194 +0.2467 * moves_opponent -0.0672 * error;
    case 23:
      return +12.4221 +0.2798 * moves_opponent -0.0657 * error;
    case 24:
      return +12.9089 +0.3175 * moves_opponent -0.0684 * error;
    case 25:
      return +13.7242 +0.3027 * moves_opponent -0.0715 * error;
    case 26:
      return +13.3424 +0.3869 * moves_opponent -0.0678 * error;
    case 27:
      return +14.3268 +0.3242 * moves_opponent -0.0656 * error;
    case 28:
      return +13.4507 +0.4558 * moves_opponent -0.0574 * error;
    case 29:
      return +16.5436 +0.2591 * moves_opponent -0.0623 * error;
    case 30:
      return +16.3636 +0.3260 * moves_opponent -0.0576 * error;
    case 31:
      return +17.2346 +0.2994 * moves_opponent -0.0616 * error;
    case 32:
      return +15.1237 +0.5037 * moves_opponent -0.0589 * error;
    case 33:
      return +18.0885 +0.3284 * moves_opponent -0.0596 * error;
    case 34:
      return +17.9011 +0.3910 * moves_opponent -0.0578 * error;
    case 35:
      return +18.7904 +0.3712 * moves_opponent -0.0659 * error;
    case 36:
      return +20.8997 +0.2061 * moves_opponent -0.0533 * error;
    case 37:
      return +21.2439 +0.2679 * moves_opponent -0.0637 * error;
    case 38:
      return +16.2205 +0.6015 * moves_opponent -0.0230 * error;
    case 39:
      return +20.2654 +0.3554 * moves_opponent -0.0362 * error;
    case 40:
      return +20.9171 +0.3336 * moves_opponent -0.0430 * error;
    case 41:
      return +22.5117 +0.2216 * moves_opponent -0.0399 * error;
    case 42:
      return +23.5641 +0.1357 * moves_opponent -0.0200 * error;
    case 43:
      return +24.1953 +0.0963 * moves_opponent -0.0187 * error;
    case 44:
      return +24.4849 +0.0483 * moves_opponent -0.0118 * error;
    case 45:
      return +22.6243 +0.2248 * moves_opponent -0.0158 * error;
    case 46:
      return +25.2968 -0.0183 * moves_opponent -0.0117 * error;
    case 47:
      return +25.2717 -0.0144 * moves_opponent -0.0116 * error;
    case 48:
      return +24.6883 +0.0403 * moves_opponent -0.0123 * error;
    case 49:
      return +25.2025 +0.0403 * moves_opponent -0.0123 * error;
    case 50:
      return +25.7166 +0.0403 * moves_opponent -0.0123 * error;
    case 51:
      return +26.2307 +0.0403 * moves_opponent -0.0123 * error;
    case 52:
      return +26.7448 +0.0403 * moves_opponent -0.0123 * error;
    case 53:
      return +27.2589 +0.0403 * moves_opponent -0.0123 * error;
    case 54:
      return +27.7730 +0.0403 * moves_opponent -0.0123 * error;
    case 55:
      return +28.2872 +0.0403 * moves_opponent -0.0123 * error;
    case 56:
      return +28.8013 +0.0403 * moves_opponent -0.0123 * error;
    case 57:
      return +29.3154 +0.0403 * moves_opponent -0.0123 * error;
    case 58:
      return +29.8295 +0.0403 * moves_opponent -0.0123 * error;
    case 59:
      return +30.3436 +0.0403 * moves_opponent -0.0123 * error;
    default:
      assert(false);
      return 1;
  }
}

inline double LogDisproofNumber(int n_empties, Square moves_player, int error) {
  switch (n_empties) {
    case 1:
      return 0;
    case 2:
      return 0.69314718056; /*ln(2)*/
    case 3:
      return 1.79175946923; /*ln(3*2)*/
    case 4:
      return 3.17805383035; /*ln(4*3*2)*/
    case 5:
      return +2.6487 +0.4631 * moves_player +0.0004 * error;
    case 6:
      return +3.2865 +0.3527 * moves_player +0.0130 * error;
    case 7:
      return +3.6414 +0.2925 * moves_player +0.0119 * error;
    case 8:
      return +4.2059 +0.2752 * moves_player +0.0178 * error;
    case 9:
      return +4.5260 +0.3005 * moves_player +0.0238 * error;
    case 10:
      return +4.9128 +0.3028 * moves_player +0.0274 * error;
    case 11:
      return +5.8246 +0.2820 * moves_player +0.0367 * error;
    case 12:
      return +6.4117 +0.2783 * moves_player +0.0454 * error;
    case 13:
      return +7.0188 +0.2877 * moves_player +0.0539 * error;
    case 14:
      return +7.6066 +0.2966 * moves_player +0.0586 * error;
    case 15:
      return +7.3853 +0.3711 * moves_player +0.0555 * error;
    case 16:
      return +8.2774 +0.3425 * moves_player +0.0611 * error;
    case 17:
      return +9.1560 +0.3551 * moves_player +0.0806 * error;
    case 18:
      return +10.9860 +0.2312 * moves_player +0.0845 * error;
    case 19:
      return +11.0712 +0.2700 * moves_player +0.0830 * error;
    case 20:
      return +10.0164 +0.4357 * moves_player +0.0795 * error;
    case 21:
      return +12.2923 +0.2840 * moves_player +0.0853 * error;
    case 22:
      return +11.2717 +0.4465 * moves_player +0.0811 * error;
    case 23:
      return +12.6521 +0.3762 * moves_player +0.0824 * error;
    case 24:
      return +12.9556 +0.4043 * moves_player +0.0866 * error;
    case 25:
      return +14.1359 +0.3698 * moves_player +0.0863 * error;
    case 26:
      return +13.6867 +0.4271 * moves_player +0.0742 * error;
    case 27:
      return +14.8523 +0.3689 * moves_player +0.0667 * error;
    case 28:
      return +14.7807 +0.4103 * moves_player +0.0604 * error;
    case 29:
      return +13.4046 +0.5485 * moves_player +0.0572 * error;
    case 30:
      return +16.8306 +0.3594 * moves_player +0.0583 * error;
    case 31:
      return +15.6065 +0.4406 * moves_player +0.0477 * error;
    case 32:
      return +19.8453 +0.2348 * moves_player +0.0610 * error;
    case 33:
      return +17.0428 +0.4922 * moves_player +0.0545 * error;
    case 34:
      return +19.0283 +0.3669 * moves_player +0.0632 * error;
    case 35:
      return +17.0419 +0.5606 * moves_player +0.0548 * error;
    case 36:
      return +21.4776 +0.2469 * moves_player +0.0629 * error;
    case 37:
      return +20.2735 +0.3842 * moves_player +0.0538 * error;
    case 38:
      return +20.4844 +0.3797 * moves_player +0.0413 * error;
    case 39:
      return +21.0481 +0.3619 * moves_player +0.0532 * error;
    case 40:
      return +21.3959 +0.3300 * moves_player +0.0253 * error;
    case 41:
      return +21.8329 +0.3346 * moves_player +0.0261 * error;
    case 42:
      return +22.7822 +0.2554 * moves_player +0.0186 * error;
    case 43:
      return +24.2546 +0.1090 * moves_player +0.0028 * error;
    case 44:
      return +23.8442 +0.1390 * moves_player +0.0032 * error;
    case 45:
      return +22.8040 +0.2311 * moves_player -0.0030 * error;
    case 46:
      return +24.7534 +0.0592 * moves_player -0.0048 * error;
    case 47:
      return +25.2828 +0.0210 * moves_player -0.0044 * error;
    case 48:
      return +24.5828 +0.0785 * moves_player -0.0064 * error;
    case 49:
      return +25.0947 +0.0785 * moves_player -0.0064 * error;
    case 50:
      return +25.6066 +0.0785 * moves_player -0.0064 * error;
    case 51:
      return +26.1185 +0.0785 * moves_player -0.0064 * error;
    case 52:
      return +26.6304 +0.0785 * moves_player -0.0064 * error;
    case 53:
      return +27.1424 +0.0785 * moves_player -0.0064 * error;
    case 54:
      return +27.6543 +0.0785 * moves_player -0.0064 * error;
    case 55:
      return +28.1662 +0.0785 * moves_player -0.0064 * error;
    case 56:
      return +28.6781 +0.0785 * moves_player -0.0064 * error;
    case 57:
      return +29.1900 +0.0785 * moves_player -0.0064 * error;
    case 58:
      return +29.7020 +0.0785 * moves_player -0.0064 * error;
    case 59:
      return +30.2139 +0.0785 * moves_player -0.0064 * error;
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
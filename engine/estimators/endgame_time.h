/*
 * Copyright 2024 Michele Borassi
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
constexpr float kMaxProofNumber = 1E25F;
constexpr int kProofNumberStep = 255;
constexpr float kBaseLogProofNumber = (float) ConstexprPow(kMaxProofNumber, 1.0 / (kProofNumberStep - 1.99));
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
    return FLT_MAX;
  }
  return (float) pow(kBaseLogProofNumber, byte - 1);
}


constexpr int DataToProofNumberOffset(Square empties, Square moves, int eval_delta) {
  assert(moves <= 15);
  assert(empties <= 63);
  assert(empties >= 0);
  assert(eval_delta <= 128);
  assert(eval_delta >= -128);
  return (int) empties | ((int) moves << 6) | (eval_delta - 2 * kMinEval) << 10;
}

constexpr std::tuple<Square, Square, int> ProofNumberOffsetToEmptiesMovesEval(int cdf_offset) {
  return std::make_tuple((cdf_offset & 63), (cdf_offset >> 6) & 15, (cdf_offset >> 10) + 2 * kMinEval);
}

inline float Bound(double value) {
  return (float) std::max(1.0, std::min(kMaxProofNumber * 0.99, value));
}

inline double ConvertProofNumber(double old, int delta) {
  assert(delta > 0);
  return Bound(exp(log(old) - 0.07 * delta));
}

inline double ConvertDisproofNumber(double old, int delta) {
  assert(delta > 0);
  return Bound(exp(log(old) - 0.07 * delta));
}

inline double LogProofNumber(int n_empties, Square moves_opponent, int error) {
  double log_moves_opponent = log(std::max((int) moves_opponent, 1) + 2);
  error = std::max(-70*8, std::min(70*8, error));
  switch (n_empties) {
    case 0:
      return +2.3775 -0.0135 * log_moves_opponent -0.0077 * error;
    case 1:
      return +2.6704 -0.0135 * log_moves_opponent -0.0077 * error;
    case 2:
      return +2.9633 -0.0135 * log_moves_opponent -0.0077 * error;
    case 3:
      return +3.2562 -0.0135 * log_moves_opponent -0.0077 * error;
    case 4:
      return +3.4484 +0.0531 * log_moves_opponent -0.0077 * error;
    case 5:
      return +3.7903 +0.0611 * log_moves_opponent -0.0095 * error;
    case 6:
      return +4.0437 +0.1433 * log_moves_opponent -0.0111 * error;
    case 7:
      return +4.3923 +0.1815 * log_moves_opponent -0.0134 * error;
    case 8:
      return +4.7118 +0.2290 * log_moves_opponent -0.0151 * error;
    case 9:
      return +5.0509 +0.3189 * log_moves_opponent -0.0195 * error;
    case 10:
      return +5.4526 +0.3808 * log_moves_opponent -0.0243 * error;
    case 11:
      return +5.8066 +0.4850 * log_moves_opponent -0.0291 * error;
    case 12:
      return +6.1568 +0.6330 * log_moves_opponent -0.0343 * error;
    case 13:
      return +6.5719 +0.7582 * log_moves_opponent -0.0407 * error;
    case 14:
      return +6.8365 +0.9411 * log_moves_opponent -0.0453 * error;
    case 15:
      return +6.9268 +1.1992 * log_moves_opponent -0.0497 * error;
    case 16:
      return +7.2731 +1.3660 * log_moves_opponent -0.0556 * error;
    case 17:
      return +7.7136 +1.4870 * log_moves_opponent -0.0612 * error;
    case 18:
      return +8.3667 +1.5015 * log_moves_opponent -0.0652 * error;
    case 19:
      return +8.9113 +1.5736 * log_moves_opponent -0.0688 * error;
    case 20:
      return +9.4660 +1.6287 * log_moves_opponent -0.0711 * error;
    case 21:
      return +9.8823 +1.7680 * log_moves_opponent -0.0755 * error;
    case 22:
      return +9.9799 +2.0293 * log_moves_opponent -0.0778 * error;
    case 23:
      return +10.1760 +2.2368 * log_moves_opponent -0.0801 * error;
    case 24:
      return +10.5026 +2.3894 * log_moves_opponent -0.0810 * error;
    case 25:
      return +11.0438 +2.4437 * log_moves_opponent -0.0812 * error;
    case 26:
      return +11.5552 +2.5013 * log_moves_opponent -0.0809 * error;
    case 27:
      return +11.2036 +2.9100 * log_moves_opponent -0.0817 * error;
    case 28:
      return +11.5537 +3.0210 * log_moves_opponent -0.0820 * error;
    case 29:
      return +11.3580 +3.3440 * log_moves_opponent -0.0838 * error;
    case 30:
      return +11.9478 +3.3756 * log_moves_opponent -0.0858 * error;
    case 31:
      return +11.1340 +3.9379 * log_moves_opponent -0.0869 * error;
    case 32:
      return +11.2932 +4.1384 * log_moves_opponent -0.0903 * error;
    case 33:
      return +12.2237 +4.0371 * log_moves_opponent -0.0922 * error;
    case 34:
      return +12.4435 +4.1864 * log_moves_opponent -0.0930 * error;
    case 35:
      return +12.8772 +4.2497 * log_moves_opponent -0.0935 * error;
    case 36:
      return +14.2472 +3.9304 * log_moves_opponent -0.0946 * error;
    case 37:
      return +14.7386 +3.9651 * log_moves_opponent -0.0950 * error;
    case 38:
      return +14.8980 +4.1735 * log_moves_opponent -0.0970 * error;
    case 39:
      return +14.0817 +4.7402 * log_moves_opponent -0.0984 * error;
    case 40:
      return +14.3064 +4.8891 * log_moves_opponent -0.0986 * error;
    case 41:
      return +14.6816 +5.0126 * log_moves_opponent -0.0987 * error;
    case 42:
      return +15.8863 +4.7816 * log_moves_opponent -0.0986 * error;
    case 43:
      return +17.3965 +4.4429 * log_moves_opponent -0.0992 * error;
    case 44:
      return +18.3813 +4.3203 * log_moves_opponent -0.0999 * error;
    case 45:
      return +20.5447 +3.7845 * log_moves_opponent -0.1017 * error;
    case 46:
      return +22.0146 +3.4876 * log_moves_opponent -0.1032 * error;
    case 47:
      return +23.6573 +3.1384 * log_moves_opponent -0.1040 * error;
    case 48:
      return +25.6805 +2.6265 * log_moves_opponent -0.1055 * error;
    case 49:
      return +27.6521 +2.1471 * log_moves_opponent -0.1058 * error;
    case 50:
      return +28.3654 +2.1083 * log_moves_opponent -0.1074 * error;
    case 51:
      return +30.2369 +1.6629 * log_moves_opponent -0.1073 * error;
    case 52:
      return +31.2131 +1.5556 * log_moves_opponent -0.1095 * error;
    case 53:
      return +32.9067 +1.1731 * log_moves_opponent -0.1091 * error;
    case 54:
      return +33.4042 +1.2356 * log_moves_opponent -0.1087 * error;
    case 55:
      return +34.3972 +1.1098 * log_moves_opponent -0.1088 * error;
    case 56:
      return +35.4394 +0.9344 * log_moves_opponent -0.1093 * error;
    case 57:
      return +36.4362 +0.7680 * log_moves_opponent -0.1091 * error;
    case 58:
      return +36.4893 +1.1315 * log_moves_opponent -0.1097 * error;
    case 59:
      return +37.9377 +0.5617 * log_moves_opponent -0.1093 * error;
    case 60:
      return +39.9141 -0.1785 * log_moves_opponent -0.1098 * error;
    case 61:
      return +38.2563 +0.6285 * log_moves_opponent -0.1082 * error;
    case 62:
      return +42.0388 +0.5295 * log_moves_opponent -0.1078 * error;
    case 63:
      return +41.8089 +0.0181 * log_moves_opponent -0.1080 * error;
    default:
      assert(false);
      return 1;
  }
}

inline double LogDisproofNumber(int n_empties, Square moves_player, int error) {
  double log_moves_player = log(std::max((int) moves_player, 1) + 2);
  error = std::max(-70*8, std::min(70*8, error));
  switch (n_empties) {
    case 0:
      return +0.7470 +1.8772 * log_moves_player +0.0068 * error;
    case 1:
      return +0.8341 +1.8772 * log_moves_player +0.0068 * error;
    case 2:
      return +0.9211 +1.8772 * log_moves_player +0.0068 * error;
    case 3:
      return +1.0081 +1.8772 * log_moves_player +0.0068 * error;
    case 4:
      return +1.2574 +1.8207 * log_moves_player +0.0093 * error;
    case 5:
      return +1.4380 +1.8150 * log_moves_player +0.0117 * error;
    case 6:
      return +1.7417 +1.7745 * log_moves_player +0.0141 * error;
    case 7:
      return +2.0862 +1.7405 * log_moves_player +0.0165 * error;
    case 8:
      return +2.3991 +1.7622 * log_moves_player +0.0196 * error;
    case 9:
      return +2.9038 +1.7680 * log_moves_player +0.0265 * error;
    case 10:
      return +3.4093 +1.7722 * log_moves_player +0.0328 * error;
    case 11:
      return +3.7167 +1.9012 * log_moves_player +0.0390 * error;
    case 12:
      return +3.9560 +2.0881 * log_moves_player +0.0453 * error;
    case 13:
      return +4.2470 +2.2542 * log_moves_player +0.0515 * error;
    case 14:
      return +4.5899 +2.4023 * log_moves_player +0.0584 * error;
    case 15:
      return +4.8119 +2.6004 * log_moves_player +0.0653 * error;
    case 16:
      return +4.8123 +2.8932 * log_moves_player +0.0708 * error;
    case 17:
      return +4.7712 +3.2071 * log_moves_player +0.0753 * error;
    case 18:
      return +5.0249 +3.3671 * log_moves_player +0.0789 * error;
    case 19:
      return +5.4563 +3.4749 * log_moves_player +0.0819 * error;
    case 20:
      return +5.3436 +3.7827 * log_moves_player +0.0837 * error;
    case 21:
      return +5.7800 +3.8638 * log_moves_player +0.0846 * error;
    case 22:
      return +6.1249 +3.9909 * log_moves_player +0.0855 * error;
    case 23:
      return +6.5926 +4.0829 * log_moves_player +0.0871 * error;
    case 24:
      return +6.9811 +4.1664 * log_moves_player +0.0867 * error;
    case 25:
      return +7.5065 +4.2070 * log_moves_player +0.0861 * error;
    case 26:
      return +7.9698 +4.2483 * log_moves_player +0.0849 * error;
    case 27:
      return +8.4343 +4.2815 * log_moves_player +0.0839 * error;
    case 28:
      return +8.7454 +4.3870 * log_moves_player +0.0839 * error;
    case 29:
      return +8.7884 +4.6196 * log_moves_player +0.0845 * error;
    case 30:
      return +9.2611 +4.6733 * log_moves_player +0.0852 * error;
    case 31:
      return +10.1316 +4.5851 * log_moves_player +0.0868 * error;
    case 32:
      return +9.0824 +5.1902 * log_moves_player +0.0846 * error;
    case 33:
      return +9.1862 +5.4088 * log_moves_player +0.0862 * error;
    case 34:
      return +9.8346 +5.3909 * log_moves_player +0.0876 * error;
    case 35:
      return +9.1160 +5.9012 * log_moves_player +0.0881 * error;
    case 36:
      return +10.0605 +5.8064 * log_moves_player +0.0896 * error;
    case 37:
      return +10.5922 +5.8341 * log_moves_player +0.0901 * error;
    case 38:
      return +10.2118 +6.2229 * log_moves_player +0.0919 * error;
    case 39:
      return +10.5953 +6.3295 * log_moves_player +0.0931 * error;
    case 40:
      return +11.0811 +6.4034 * log_moves_player +0.0940 * error;
    case 41:
      return +12.6840 +6.0612 * log_moves_player +0.0946 * error;
    case 42:
      return +14.5836 +5.5750 * log_moves_player +0.0949 * error;
    case 43:
      return +15.6792 +5.4182 * log_moves_player +0.0955 * error;
    case 44:
      return +18.2796 +4.7097 * log_moves_player +0.0979 * error;
    case 45:
      return +20.0477 +4.2865 * log_moves_player +0.0987 * error;
    case 46:
      return +21.2313 +4.1294 * log_moves_player +0.0999 * error;
    case 47:
      return +24.3182 +3.2275 * log_moves_player +0.0999 * error;
    case 48:
      return +25.9864 +2.8570 * log_moves_player +0.1005 * error;
    case 49:
      return +27.5568 +2.5071 * log_moves_player +0.1023 * error;
    case 50:
      return +28.2594 +2.5344 * log_moves_player +0.1035 * error;
    case 51:
      return +28.8723 +2.5801 * log_moves_player +0.1053 * error;
    case 52:
      return +29.8889 +2.4971 * log_moves_player +0.1056 * error;
    case 53:
      return +31.5168 +2.1207 * log_moves_player +0.1064 * error;
    case 54:
      return +32.2733 +2.1401 * log_moves_player +0.1069 * error;
    case 55:
      return +33.0243 +2.1285 * log_moves_player +0.1074 * error;
    case 56:
      return +33.4561 +2.2713 * log_moves_player +0.1076 * error;
    case 57:
      return +34.1675 +2.3004 * log_moves_player +0.1081 * error;
    case 58:
      return +34.9810 +2.2548 * log_moves_player +0.1080 * error;
    case 59:
      return +35.4962 +2.4597 * log_moves_player +0.1090 * error;
    case 60:
      return +36.8959 +1.8881 * log_moves_player +0.1089 * error;
    case 61:
      return +38.4175 +1.4472 * log_moves_player +0.1099 * error;
    case 62:
      return +36.1571 +2.9271 * log_moves_player +0.1099 * error;
    case 63:
      return +41.8230 +0.0181 * log_moves_player +0.1107 * error;
    default:
      assert(false);
      return 1;
  }
}

struct ProofDisproofNumberData {
  PN proof_number[kMaxProofNumberOffset + 1];
  PN disproof_number[kMaxProofNumberOffset + 1];
  int disproof_number_over_prob[kMaxProofNumberOffset + 1];
  float byte_to_proof_number[kProofNumberStep + 1];

  ProofDisproofNumberData() : proof_number(), disproof_number(), byte_to_proof_number() {
    for (int empties = 0; empties <= 63; ++empties) {
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

inline float ByteToProofNumber(PN byte) {
  return kProofDisproofNumberData.byte_to_proof_number[byte];
}

inline PN DisproofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return kProofDisproofNumberData.disproof_number[DataToProofNumberOffset(
      (Square) __builtin_popcountll(~(player | opponent)),
      std::min(15, GetNMoves(player, opponent)),
      (approx_eval - lower) >> 3
  )];
}

inline PN ProofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return kProofDisproofNumberData.proof_number[DataToProofNumberOffset(
      (Square) __builtin_popcountll(~(player | opponent)),
      std::min(15, GetNMoves(opponent, player)),
      (approx_eval - lower) >> 3
  )];
}

inline int DisproofNumberOverProb(
    BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return kProofDisproofNumberData.disproof_number_over_prob[DataToProofNumberOffset(
      (Square) __builtin_popcountll(~(player | opponent)),
      std::min(15, GetNMoves(player, opponent)),
      (approx_eval - lower) >> 3
  )];
}
#endif  // OTHELLOSENSEI_ENDGAME_TIME_H
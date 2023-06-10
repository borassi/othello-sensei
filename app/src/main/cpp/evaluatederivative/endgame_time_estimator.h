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

#include <math.h>
#include "../board/bitpattern.h"
#include "../board/get_moves.h"

#ifndef OTHELLOSENSEI_ENDGAME_TIME_ESTIMATOR_H
#define OTHELLOSENSEI_ENDGAME_TIME_ESTIMATOR_H

constexpr double kErrorWeight = 0.1;
constexpr float kMaxProofNumber = 1E25;



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

inline double LogProofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  BitPattern empties = ~(player | opponent);
  int n_empties = __builtin_popcountll(empties);
  EvalLarge error = approx_eval - lower;
  switch (n_empties) {
  case 5:
    return +2.7732 +0.0602 * GetNMoves(opponent, player) +0.2851 * GetNMoves(player, opponent) -0.0080 * error / 8;
  case 6:
    return +2.5441 +0.1501 * GetNMoves(opponent, player) +0.3602 * GetNMoves(player, opponent) -0.0096 * error / 8;
  case 7:
    return +2.7848 +0.1439 * GetNMoves(opponent, player) +0.3046 * GetNMoves(player, opponent) -0.0089 * error / 8;
  case 8:
    return +3.3361 +0.1446 * GetNMoves(opponent, player) +0.2248 * GetNMoves(player, opponent) -0.0110 * error / 8;
  case 9:
    return +4.3329 +0.0666 * GetNMoves(opponent, player) +0.1746 * GetNMoves(player, opponent) -0.0157 * error / 8;
  case 10:
    return +3.9689 +0.1997 * GetNMoves(opponent, player) +0.2584 * GetNMoves(player, opponent) -0.0248 * error / 8;
  case 11:
    return +5.0026 +0.1339 * GetNMoves(opponent, player) +0.2097 * GetNMoves(player, opponent) -0.0344 * error / 8;
  case 12:
    return +5.6690 +0.1550 * GetNMoves(opponent, player) +0.1035 * GetNMoves(player, opponent) -0.0264 * error / 8;
  case 13:
    return +5.4306 +0.2542 * GetNMoves(opponent, player) +0.1857 * GetNMoves(player, opponent) -0.0413 * error / 8;
  case 14:
    return +7.4029 +0.1484 * GetNMoves(opponent, player) +0.0714 * GetNMoves(player, opponent) -0.0474 * error / 8;
  case 15:
    return +7.3226 +0.2202 * GetNMoves(opponent, player) +0.1349 * GetNMoves(player, opponent) -0.0607 * error / 8;
  case 16:
    return +7.9160 +0.2276 * GetNMoves(opponent, player) +0.0468 * GetNMoves(player, opponent) -0.0480 * error / 8;
  case 17:
    return +9.4770 +0.2385 * GetNMoves(opponent, player) -0.0182 * GetNMoves(player, opponent) -0.0674 * error / 8;
  case 18:
    return +10.2402 +0.2458 * GetNMoves(opponent, player) -0.0156 * GetNMoves(player, opponent) -0.0741 * error / 8;
  case 19:
    return +9.8372 +0.2298 * GetNMoves(opponent, player) +0.0663 * GetNMoves(player, opponent) -0.0661 * error / 8;
  case 20:
    return +10.6212 +0.2493 * GetNMoves(opponent, player) +0.0407 * GetNMoves(player, opponent) -0.0640 * error / 8;
  case 21:
    return +12.1072 +0.2160 * GetNMoves(opponent, player) -0.0069 * GetNMoves(player, opponent) -0.0770 * error / 8;
  case 22:
    return +11.7269 +0.2661 * GetNMoves(opponent, player) +0.0362 * GetNMoves(player, opponent) -0.0674 * error / 8;
  case 23:
    return +13.5002 +0.2543 * GetNMoves(opponent, player) -0.0825 * GetNMoves(player, opponent) -0.0644 * error / 8;
  case 24:
    return +13.8058 +0.2944 * GetNMoves(opponent, player) -0.0727 * GetNMoves(player, opponent) -0.0673 * error / 8;
  case 25:
    return +15.0216 +0.2667 * GetNMoves(opponent, player) -0.0976 * GetNMoves(player, opponent) -0.0708 * error / 8;
  case 26:
    return +14.4929 +0.3614 * GetNMoves(opponent, player) -0.0842 * GetNMoves(player, opponent) -0.0670 * error / 8;
  case 27:
    return +15.8368 +0.2876 * GetNMoves(opponent, player) -0.1098 * GetNMoves(player, opponent) -0.0654 * error / 8;
  case 28:
    return +13.6469 +0.4414 * GetNMoves(opponent, player) -0.0089 * GetNMoves(player, opponent) -0.0572 * error / 8;
  case 29:
    return +16.2992 +0.2582 * GetNMoves(opponent, player) +0.0218 * GetNMoves(player, opponent) -0.0624 * error / 8;
  case 30:
    return +15.5976 +0.3042 * GetNMoves(opponent, player) +0.0848 * GetNMoves(player, opponent) -0.0581 * error / 8;
  case 31:
    return +17.9854 +0.2917 * GetNMoves(opponent, player) -0.0564 * GetNMoves(player, opponent) -0.0606 * error / 8;
  case 32:
    return +13.1845 +0.5378 * GetNMoves(opponent, player) +0.1222 * GetNMoves(player, opponent) -0.0591 * error / 8;
  case 33:
    return +16.3814 +0.3654 * GetNMoves(opponent, player) +0.1215 * GetNMoves(player, opponent) -0.0602 * error / 8;
  case 34:
    return +18.7239 +0.3497 * GetNMoves(opponent, player) -0.0330 * GetNMoves(player, opponent) -0.0576 * error / 8;
  case 35:
    return +20.2311 +0.3157 * GetNMoves(opponent, player) -0.0808 * GetNMoves(player, opponent) -0.0653 * error / 8;
  case 36:
    return +19.5560 +0.1927 * GetNMoves(opponent, player) +0.1145 * GetNMoves(player, opponent) -0.0534 * error / 8;
  case 37:
    return +21.7995 +0.2178 * GetNMoves(opponent, player) -0.0041 * GetNMoves(player, opponent) -0.0642 * error / 8;
  case 38:
    return +17.0210 +0.6595 * GetNMoves(opponent, player) -0.1109 * GetNMoves(player, opponent) -0.0226 * error / 8;
  case 39:
    return +23.5217 +0.2420 * GetNMoves(opponent, player) -0.1619 * GetNMoves(player, opponent) -0.0340 * error / 8;
  case 40:
    return +22.7273 +0.2812 * GetNMoves(opponent, player) -0.1006 * GetNMoves(player, opponent) -0.0418 * error / 8;
  case 41:
    return +21.6721 +0.2179 * GetNMoves(opponent, player) +0.0758 * GetNMoves(player, opponent) -0.0397 * error / 8;
  case 42:
    return +23.8956 +0.1272 * GetNMoves(opponent, player) -0.0202 * GetNMoves(player, opponent) -0.0197 * error / 8;
  case 43:
    return +25.7905 +0.0694 * GetNMoves(opponent, player) -0.1084 * GetNMoves(player, opponent) -0.0186 * error / 8;
  case 44:
    return +24.8834 +0.0379 * GetNMoves(opponent, player) -0.0241 * GetNMoves(player, opponent) -0.0117 * error / 8;
  case 45:
    return +23.2936 +0.2420 * GetNMoves(opponent, player) -0.0837 * GetNMoves(player, opponent) -0.0154 * error / 8;
  case 46:
    return +25.5664 -0.0134 * GetNMoves(opponent, player) -0.0309 * GetNMoves(player, opponent) -0.0114 * error / 8;
  case 47:
    return +25.8121 -0.0117 * GetNMoves(opponent, player) -0.0514 * GetNMoves(player, opponent) -0.0115 * error / 8;
  case 48:
    return +25.4308 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 49:
    return +25.9603 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 50:
    return +26.4899 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 51:
    return +27.0195 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 52:
    return +27.5491 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 53:
    return +28.0787 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 54:
    return +28.6082 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 55:
    return +29.1378 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 56:
    return +29.6674 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 57:
    return +30.1970 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 58:
    return +30.7265 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  case 59:
    return +31.2561 +0.0321 * GetNMoves(opponent, player) -0.0558 * GetNMoves(player, opponent) -0.0123 * error / 8;
  default:
    assert(false);
    return 1;
}
}

inline double ProofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return Bound(exp(LogProofNumber(player, opponent, lower, approx_eval)));
}

inline double LogDisproofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  BitPattern empties = ~(player | opponent);
  int n_empties = __builtin_popcountll(empties);
  EvalLarge error = approx_eval - lower;
  switch (n_empties) {
    case 5:
      return +3.0396 +0.0813 * GetNMoves(opponent, player) +0.3701 * GetNMoves(player, opponent) +0.0142 * error / 8;
    case 6:
      return +3.2391 +0.0749 * GetNMoves(opponent, player) +0.3692 * GetNMoves(player, opponent) +0.0168 * error / 8;
    case 7:
      return +3.5195 +0.0680 * GetNMoves(opponent, player) +0.3633 * GetNMoves(player, opponent) +0.0195 * error / 8;
    case 8:
      return +3.7935 +0.0706 * GetNMoves(opponent, player) +0.3671 * GetNMoves(player, opponent) +0.0226 * error / 8;
    case 9:
      return +4.1662 +0.0664 * GetNMoves(opponent, player) +0.3628 * GetNMoves(player, opponent) +0.0257 * error / 8;
    case 10:
      return +4.5668 +0.0803 * GetNMoves(opponent, player) +0.3577 * GetNMoves(player, opponent) +0.0321 * error / 8;
    case 11:
      return +5.0562 +0.0748 * GetNMoves(opponent, player) +0.3520 * GetNMoves(player, opponent) +0.0368 * error / 8;
    case 12:
      return +5.6552 +0.0647 * GetNMoves(opponent, player) +0.3439 * GetNMoves(player, opponent) +0.0422 * error / 8;
    case 13:
      return +6.2899 +0.0575 * GetNMoves(opponent, player) +0.3396 * GetNMoves(player, opponent) +0.0491 * error / 8;
    case 14:
      return +6.9639 +0.0519 * GetNMoves(opponent, player) +0.3315 * GetNMoves(player, opponent) +0.0548 * error / 8;
    case 15:
      return +7.5526 +0.0551 * GetNMoves(opponent, player) +0.3359 * GetNMoves(player, opponent) +0.0617 * error / 8;
    case 16:
      return +7.9873 +0.0538 * GetNMoves(opponent, player) +0.3573 * GetNMoves(player, opponent) +0.0669 * error / 8;
    case 17:
      return +8.9373 +0.0238 * GetNMoves(opponent, player) +0.3424 * GetNMoves(player, opponent) +0.0710 * error / 8;
    case 18:
      return +9.6694 +0.0023 * GetNMoves(opponent, player) +0.3460 * GetNMoves(player, opponent) +0.0733 * error / 8;
    case 19:
      return +10.3353 -0.0096 * GetNMoves(opponent, player) +0.3524 * GetNMoves(player, opponent) +0.0755 * error / 8;
    case 20:
      return +11.1757 -0.0266 * GetNMoves(opponent, player) +0.3487 * GetNMoves(player, opponent) +0.0791 * error / 8;
    case 21:
      return +12.0230 -0.0404 * GetNMoves(opponent, player) +0.3453 * GetNMoves(player, opponent) +0.0822 * error / 8;
    case 22:
      return +12.1221 -0.0185 * GetNMoves(opponent, player) +0.3753 * GetNMoves(player, opponent) +0.0804 * error / 8;
    case 23:
      return +12.7062 -0.0235 * GetNMoves(opponent, player) +0.3780 * GetNMoves(player, opponent) +0.0789 * error / 8;
    case 24:
      return +13.2587 -0.0311 * GetNMoves(opponent, player) +0.3906 * GetNMoves(player, opponent) +0.0782 * error / 8;
    case 25:
      return +13.9265 -0.0310 * GetNMoves(opponent, player) +0.3877 * GetNMoves(player, opponent) +0.0779 * error / 8;
    case 26:
      return +14.3955 -0.0266 * GetNMoves(opponent, player) +0.3986 * GetNMoves(player, opponent) +0.0771 * error / 8;
    case 27:
      return +15.0048 -0.0228 * GetNMoves(opponent, player) +0.3964 * GetNMoves(player, opponent) +0.0764 * error / 8;
    case 28:
      return +15.6646 -0.0229 * GetNMoves(opponent, player) +0.3971 * GetNMoves(player, opponent) +0.0755 * error / 8;
    case 29:
      return +15.9333 -0.0022 * GetNMoves(opponent, player) +0.4058 * GetNMoves(player, opponent) +0.0737 * error / 8;
    case 30:
      return +16.1738 +0.0120 * GetNMoves(opponent, player) +0.4169 * GetNMoves(player, opponent) +0.0711 * error / 8;
    case 31:
      return +17.2787 -0.0297 * GetNMoves(opponent, player) +0.3883 * GetNMoves(player, opponent) +0.0631 * error / 8;
    case 32:
      return +18.1678 -0.0719 * GetNMoves(opponent, player) +0.3875 * GetNMoves(player, opponent) +0.0587 * error / 8;
    case 33:
      return +19.0195 -0.0934 * GetNMoves(opponent, player) +0.3835 * GetNMoves(player, opponent) +0.0579 * error / 8;
    case 34:
      return +19.7383 -0.0680 * GetNMoves(opponent, player) +0.3505 * GetNMoves(player, opponent) +0.0574 * error / 8;
    case 35:
      return +20.3546 -0.0801 * GetNMoves(opponent, player) +0.3525 * GetNMoves(player, opponent) +0.0569 * error / 8;
    case 36:
      return +20.9937 -0.0680 * GetNMoves(opponent, player) +0.3297 * GetNMoves(player, opponent) +0.0536 * error / 8;
    case 37:
      return +21.3823 -0.0751 * GetNMoves(opponent, player) +0.3320 * GetNMoves(player, opponent) +0.0488 * error / 8;
    case 38:
      return +21.7518 -0.0573 * GetNMoves(opponent, player) +0.3107 * GetNMoves(player, opponent) +0.0438 * error / 8;
    case 39:
      return +22.0374 -0.0458 * GetNMoves(opponent, player) +0.3004 * GetNMoves(player, opponent) +0.0383 * error / 8;
    case 40:
      return +22.8615 -0.0496 * GetNMoves(opponent, player) +0.2617 * GetNMoves(player, opponent) +0.0336 * error / 8;
    case 41:
      return +22.7758 -0.0431 * GetNMoves(opponent, player) +0.2805 * GetNMoves(player, opponent) +0.0266 * error / 8;
    case 42:
      return +23.4292 -0.0542 * GetNMoves(opponent, player) +0.2547 * GetNMoves(player, opponent) +0.0222 * error / 8;
    case 43:
      return +23.7090 -0.0577 * GetNMoves(opponent, player) +0.2451 * GetNMoves(player, opponent) +0.0199 * error / 8;
    case 44:
      return +24.0587 -0.0644 * GetNMoves(opponent, player) +0.2335 * GetNMoves(player, opponent) +0.0181 * error / 8;
    case 45:
      return +24.1514 -0.0612 * GetNMoves(opponent, player) +0.2226 * GetNMoves(player, opponent) +0.0133 * error / 8;
    case 46:
      return +24.0176 -0.0102 * GetNMoves(opponent, player) +0.1812 * GetNMoves(player, opponent) +0.0098 * error / 8;
    case 47:
      return +24.6878 -0.0158 * GetNMoves(opponent, player) +0.1073 * GetNMoves(player, opponent) +0.0013 * error / 8;
    case 48:
      return +24.6706 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 49:
      return +24.7978 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 50:
      return +24.9250 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 51:
      return +25.0523 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 52:
      return +25.1795 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 53:
      return +25.3067 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 54:
      return +25.4339 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 55:
      return +25.5611 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 56:
      return +25.6883 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 57:
      return +25.8155 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 58:
      return +25.9427 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    case 59:
      return +26.0700 -0.0103 * GetNMoves(opponent, player) +0.1162 * GetNMoves(player, opponent) +0.0006 * error / 8;
    default:
      assert(false);
      return 1;
  }
}

inline double DisproofNumber(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge approx_eval) {
  return Bound(exp(LogDisproofNumber(player, opponent, lower, approx_eval)));
}
#endif  // OTHELLOSENSEI_ENDGAME_TIME_ESTIMATOR_H
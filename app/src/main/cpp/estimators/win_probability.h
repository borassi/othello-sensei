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

#ifndef ESTIMATORS_GAUSSIAN_H
#define ESTIMATORS_GAUSSIAN_H

#include <math.h>
#include <tuple>
#include "../board/bitpattern.h"
#include "../utils/functions.h"

typedef uint8_t Probability;
constexpr Probability kProbStep = 255;

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

constexpr double BaseRescaleProb(double x) {
  if (x == 0) {
    return 0;
  }
  return pow(-log(0.08 * x), -2);
}

constexpr double RescaleProb(double x) {
  return (BaseRescaleProb(x) - BaseRescaleProb(1-x)) / (BaseRescaleProb(1) - BaseRescaleProb(0)) / 2 + 0.5;
}

constexpr double InverseRescaleProb(double y) {
  return Inverse(RescaleProb, y, 0, 1);
}

constexpr Probability ProbabilityToByteExplicit(double probability) {
  assert(probability >= 0);
  assert(probability <= 1);
  return round(RescaleProb(probability) * kProbStep);
}

constexpr double ByteToProbabilityExplicit(Probability byte) {
  assert(byte >= 0);
  assert(byte <= kProbStep);
  return InverseRescaleProb(byte / (double) kProbStep);
}

inline double GaussianCDF(double value) {
  // TODO: Make constexpr by changing erfc with gcem::erfc.
  return 0.5 * erfc(-value * 0.70710678118  /* sqrt(2) */);
}

inline double GaussianCDF(double x, double mean, double stddev) {
  return GaussianCDF((x - mean) / stddev);
}

constexpr double ProbabilityExplicit(Square depth, Square empties, EvalLarge delta) {
  return 1 - GaussianCDF(delta, 0, 8 * std::max(3.0F, kErrors[depth][empties]));
}

struct WinProbabilityData {
  Probability win_probability[kMaxCDFOffset + 1];
  double byte_to_probability[kProbStep + 1];

  WinProbabilityData() : win_probability(), byte_to_probability() {
    for (Square depth = 1; depth <= 4; ++depth) {
      for (Square empties = 0; empties < 64; ++empties) {
        for (EvalLarge delta = 2 * kMinEvalLarge; delta <= -2 * kMinEvalLarge; ++delta) {
          double prob = ProbabilityExplicit(depth, empties, delta);
          win_probability[DataToCDFOffset(depth, empties, delta)] = ProbabilityToByteExplicit(prob);
        }
      }
    }
    for (int i = 0; i <= kProbStep; ++i) {
      byte_to_probability[i] = ByteToProbabilityExplicit(i);
    }
  }
};

const WinProbabilityData kWinProbability;

constexpr double ByteToProbability(Probability byte) {
  return kWinProbability.byte_to_probability[byte];
}

constexpr Probability WinProbability(Square depth, Square n_empties, EvalLarge goal, EvalLarge eval) {
  return kWinProbability.win_probability[DataToCDFOffset(depth, n_empties, goal - eval)];
}

#endif  // ESTIMATORS_GAUSSIAN_H
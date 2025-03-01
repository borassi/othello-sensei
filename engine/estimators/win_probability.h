/*
 * Copyright 2023-2024 Michele Borassi
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

#include <float.h>
#include <math.h>
#include <tuple>
#include "../board/bitpattern.h"
#include "../utils/functions.h"

typedef uint8_t Probability;
constexpr Probability kProbStep = 255;

constexpr float kErrors[][60] = {
    {},
    {2.00F, 2.00F, 2.00F, 2.00F, 6.64F, 6.87F, 7.64F, 7.77F, 8.18F, 8.30F, 8.72F, 8.73F,
     8.98F, 8.71F, 8.65F, 8.35F, 8.29F, 8.05F, 8.20F, 7.64F, 7.55F, 7.05F, 6.82F, 6.00F,
     6.36F, 5.61F, 5.86F, 5.19F, 5.76F, 5.13F, 5.58F, 4.91F, 5.19F, 4.39F, 4.89F, 4.14F,
     4.82F, 4.03F, 4.38F, 3.83F, 4.16F, 3.52F, 3.82F, 3.20F, 3.28F, 2.79F, 2.96F, 2.41F,
     2.84F, 2.57F, 2.51F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F},
    {2.00F, 2.00F, 2.00F, 2.00F, 5.57F, 5.86F, 6.40F, 7.08F, 7.23F, 7.63F, 7.77F, 8.06F,
     8.03F, 8.19F, 7.91F, 7.81F, 7.57F, 7.51F, 7.41F, 7.29F, 6.92F, 6.56F, 6.18F, 5.41F,
     5.67F, 5.14F, 5.13F, 4.66F, 4.98F, 4.56F, 4.78F, 4.37F, 4.21F, 3.90F, 4.03F, 3.73F,
     3.97F, 3.63F, 3.54F, 3.46F, 3.51F, 3.15F, 3.17F, 2.94F, 2.67F, 2.52F, 2.56F, 2.21F,
     2.10F, 2.48F, 2.09F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F},
    {2.00F, 2.00F, 2.00F, 2.00F, 5.17F, 4.96F, 5.63F, 5.93F, 6.68F, 6.74F, 7.21F, 7.25F,
     7.47F, 7.38F, 7.51F, 7.21F, 7.10F, 6.97F, 6.89F, 6.66F, 6.57F, 6.03F, 5.75F, 5.01F,
     5.30F, 4.71F, 5.16F, 4.30F, 4.73F, 4.09F, 4.59F, 3.88F, 4.40F, 3.50F, 3.97F, 3.29F,
     4.03F, 3.17F, 3.81F, 3.09F, 3.50F, 2.74F, 3.25F, 2.59F, 3.00F, 2.28F, 2.61F, 2.05F,
     2.25F, 2.00F, 2.37F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F},
    {2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 4.72F, 4.71F, 5.25F, 5.53F, 6.34F, 6.37F, 6.77F,
     6.71F, 6.87F, 6.81F, 7.03F, 6.56F, 6.54F, 6.37F, 6.27F, 6.05F, 5.93F, 5.32F, 4.77F,
     4.93F, 4.39F, 4.64F, 3.98F, 4.05F, 3.66F, 3.96F, 3.53F, 3.67F, 3.24F, 3.15F, 2.96F,
     3.34F, 2.93F, 3.06F, 2.86F, 2.80F, 2.57F, 2.72F, 2.40F, 2.50F, 2.13F, 2.13F, 2.00F,
     2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F, 2.00F}
};

constexpr int kMaxCDFOffset = /*depth*/ 4 * /*n_empties*/ 64 * /*eval_delta*/ (256*8+1);

constexpr int DataToCDFOffset(Square depth, Square n_empties, EvalLarge eval_delta) {
  assert(depth >= 1 && depth <= 4);
  assert(n_empties >= 0 && n_empties <= 63);
  assert(eval_delta >= 2 * kMinEvalLarge && eval_delta <= -2 * kMinEvalLarge);
  return (depth-1) | (n_empties << 2) | ((eval_delta - 2 * kMinEvalLarge) << 8);
}

constexpr std::tuple<Square, Square, EvalLarge> CDFOffsetToDepthEmptiesEval(int cdf_offset) {
  return std::make_tuple((cdf_offset & 3) + 1, (cdf_offset >> 2) & 63, (cdf_offset >> 8) + 2 * kMinEvalLarge);
}

inline double BaseRescaleProb(double x) {
  if (x < DBL_MIN) {
    return 0;
  }
  return pow(-log(x) + 10, -3.5);
}

inline double RescaleProb(double x) {
  if (x <= 1E-14) {
    return 0;
  } else if (x >= 1-1E-14) {
    return 1;
  }
  return (BaseRescaleProb(x) - BaseRescaleProb(1-x)) / (BaseRescaleProb(1) - BaseRescaleProb(0)) / 2 + 0.5;
}

constexpr double InverseRescaleProb(double y) {
  if (y <= 1E-14) {
    return 0;
  } else if (y >= 1-1E-14) {
    return 1;
  }
  return Inverse(RescaleProb, y, 0, 1);
}

inline Probability ProbabilityToByteExplicit(double probability) {
  assert(probability >= 0);
  assert(probability <= 1);
  return (Probability) round(RescaleProb(probability) * kProbStep);
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

inline double ProbabilityExplicit(Square depth, Square empties, EvalLarge delta) {
  float error;
  if (empties < 60) {
    error = std::max(3.0F, kErrors[depth][empties]);
  } else {
    error = 3.0F;
  }
  return 1 - GaussianCDF(delta, 0, 8 * error);
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

inline double ByteToProbability(Probability byte) {
  return kWinProbability.byte_to_probability[byte];
}

inline Probability WinProbability(Square depth, Square n_empties, EvalLarge goal, EvalLarge eval) {
  return kWinProbability.win_probability[DataToCDFOffset(depth, n_empties, goal - eval)];
}

#endif  // ESTIMATORS_GAUSSIAN_H
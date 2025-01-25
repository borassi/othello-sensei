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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "types.h"

#ifdef _MSC_VER
#include <immintrin.h>
// From https://stackoverflow.com/a/78599923.
#define _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR

#ifdef __POPCNT__
#define __builtin_popcountll _mm_popcnt_u64
#define __builtin_popcount __popcnt
#else
inline int __builtin_popcountll(uint64_t i) {
  i = i - ((i >> 1) & 0x5555555555555555ULL);
  i = (i & 0x3333333333333333ULL) + ((i >> 2) & 0x3333333333333333ULL);
  i = (i + (i >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return (i * 0x0101010101010101ULL) >> 56;
}
inline int __builtin_popcount(uint32_t i){
    i = (i & 0x55555555) + ((i & 0xAAAAAAAA) >> 1);
    i = (i & 0x33333333) + ((i & 0xCCCCCCCC) >> 2);
    return (i & 0x0F0F0F0F) + ((i & 0xF0F0F0F0) >> 4);
}
#endif

#define __builtin_ctzll _tzcnt_u64
#define __builtin_clzll _lzcnt_u64
#define __builtin_ctz _tzcnt_u32
#define __builtin_clz _lzcnt_u32
#define __builtin_expect(x, y) x
#define forceinline(function_def) __forceinline function_def
#define localtime_r(x, y) localtime_s(y, x)
#define strncpy(x, y, z) strcpy_s(x, z, y)
#else
#define forceinline(function_def) inline function_def __attribute__((always_inline))
#endif

constexpr int CountTrailingZerosConstexpr(BitPattern x) {
#ifdef _MSC_VER
  // See https://stackoverflow.com/a/72088344.
  uint64_t y = x ^ (x - 1);
  constexpr uint64_t debruijn = 0x03f79d71b4cb0a89;
  uint8_t z = (debruijn * y) >> 58;
  constexpr int lookup[] = {
       0, 47,  1, 56, 48, 27,  2, 60,
      57, 49, 41, 37, 28, 16,  3, 61,
      54, 58, 35, 52, 50, 42, 21, 44,
      38, 32, 29, 23, 17, 11,  4, 62,
      46, 55, 26, 59, 40, 36, 15, 53,
      34, 51, 20, 43, 31, 22, 10, 45,
      25, 39, 14, 33, 19, 30,  9, 24,
      13, 18,  8, 12,  7,  6,  5, 63
  };
  return lookup[z];
#else
  return __builtin_ctzll(x);
#endif
}

#ifdef ANDROID
constexpr int kBitHashMap = 23;
#elif defined(IOS)
constexpr int kBitHashMap = 23;
#else
constexpr int kBitHashMap = 23;
#endif

constexpr int kHashMapSize = 1 << kBitHashMap;
constexpr int kDerivativeEvaluatorSize = kHashMapSize / 2;

constexpr int kWeightDepthOne = 2;
constexpr int kWeightDepthZero = 1;

constexpr int kMinEmptiesForHashMap = 10;
constexpr int kMinDepthForHashMap = 3;

constexpr float kMultStddev = 1.03F;
constexpr float kLeafMultiplier = 0.8F;

constexpr int kMaxParallelTasks = 1;
constexpr float kProbForProof = 0.03F;

constexpr float kZeroPercForWeak = 0.05F;
constexpr float kMinProbEvalGoal = 0.03F;
constexpr float kProbReduceWeakEval = 0.001F;
constexpr float kProbForEndgameAlphaBeta = 0.05F;
constexpr float kProbForEarlyMidgame = 0.25F;


#endif /* CONSTANTS_H */
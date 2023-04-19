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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef ANDROID
constexpr int kBitHashMap = 23;
#else
constexpr int kBitHashMap = 24;
#endif

constexpr int kHashMapSize = 1 << kBitHashMap;
constexpr int kDerivativeEvaluatorSize = kHashMapSize / 2;

constexpr int kWeightDepthOne = 2;
constexpr int kWeightDepthZero = 1;

constexpr int kMinEmptiesForHashMap = 10;
constexpr int kMinDepthForHashMap = 3;

constexpr float kMultStddev = 1.03;
constexpr float kLeafMultiplier = 0.8F;

constexpr int kMaxParallelTasks = 1;
constexpr float kProbForProof = 0.03F;

constexpr float kZeroPercForWeak = 0.05F;
constexpr float kMinProbEvalGoal = 0.03F;
constexpr float kProbReduceWeakEval = 0.001F;
constexpr float kProbForEndgameAlphaBeta = 0.05F;
constexpr float kProbForEarlyMidgame = 0.25F;


#endif /* CONSTANTS_H */
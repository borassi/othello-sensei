/*
 * Copyright 2021 Google LLC
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

#ifndef EVALUATOR_LAST_MOVES_H
#define EVALUATOR_LAST_MOVES_H

#include <jni.h>
#include "get_flip.h"

constexpr Eval kMinEval = -66;


Eval EvalOneEmpty(Move x, BitPattern player, BitPattern opponent);
Eval EvalTwoEmpties(
    const Move x1, const Move x2, const BitPattern player,
    const BitPattern opponent, const Eval lower, const Eval upper,
    int* const n_visited);
Eval EvalThreeEmpties(
    const Move x1, const Move x2, const Move x3,
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper,
    int* const n_visited);
Eval EvalFourEmpties(
    const Move x1, const Move x2, const Move x3, const Move x4,
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper, const bool swap,
    const BitPattern last_flip, const BitPattern stable, 
    int* const n_visited);
Eval EvalFiveEmpties(
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper,
    const BitPattern last_flip, const BitPattern stable,
    int* const n_visited);

Eval Evaluate(
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper, int* const n_visited);

#ifdef __cplusplus
extern "C" {
#endif
  
JNIEXPORT jobject JNICALL Java_evaluateposition_EvaluatorLastMoves_evaluateCPPInternal(
    JNIEnv* env, jclass java_class, jlong player, jlong opponent, jint lower,
    jint upper);

#ifdef __cplusplus
}
#endif

#endif /* EVALUATOR_LAST_MOVES_H */


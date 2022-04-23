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

#include <jni.h>
#include "../hashmap/hash_map.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../evaluatedepthone/pattern_evaluator.h"

class JNIWrapper {
 public:
  JNIWrapper() :
      evals_(LoadEvals()),
      hash_map_(),
      evaluator_alpha_beta_(&hash_map_, PatternEvaluator::Factory(evals_.data())),
      evaluator_derivative_(&evaluator_alpha_beta_) {}

  jobject EvalEndgame(JNIEnv* env, BitPattern player, BitPattern opponent, jint lower, jint upper) {
    jclass EvalWithVisited = env->FindClass("evaluateposition/EvalWithVisited");
    EvalLarge eval = evaluator_alpha_beta_.Evaluate(player, opponent, 64, EvalJavaToEvalLarge(lower), EvalJavaToEvalLarge(upper));
    NVisited n_visited = evaluator_alpha_beta_.GetNVisited();
    return env->NewObject(
        EvalWithVisited,
        env->GetMethodID(EvalWithVisited, "<init>", "(IJ)V"),
        static_cast<jint>(EvalLargeToEvalJava(eval)),
        static_cast<jlong>(n_visited));

  }
  jint EvalDerivative(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge upper, NVisited max_n_visited, double max_time) {
    return evaluator_derivative_.Evaluate(player, opponent, lower, upper, max_n_visited, max_time);
  }
  jlong GetNVisitedDerivative() {
    return static_cast<jlong>(evaluator_derivative_.GetNVisited());
  }

 private:
  static EvalLarge EvalJavaToEvalLarge(jint eval) {
    int tmp = ((eval + 100000) / 200) * 2 - 1000;
    return 8 * (eval - (tmp * 100) > 0 ? tmp + 1 : tmp);
  }
  static jint EvalLargeToEvalJava(EvalLarge eval) {
    return eval * 100 / 8;
  }
  EvalType evals_;
  HashMap hash_map_;
  EvaluatorAlphaBeta evaluator_alpha_beta_;
  EvaluatorDerivative evaluator_derivative_;
};

JNIWrapper kJNIWrapper;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jobject JNICALL Java_jni_JNI_evaluateCPPInternal(
    JNIEnv* env, jclass, jlong player, jlong opponent, jint lower,
    jint upper) {
  return kJNIWrapper.EvalEndgame(env, player, opponent, lower, upper);
}

#ifdef __cplusplus
}
#endif
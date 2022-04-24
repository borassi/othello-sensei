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
  static jobject TreeNodeToJava(const TreeNode* const n, JNIEnv* env) {
    jclass TreeNodeCPP = env->FindClass("jni/TreeNodeCPP");
    return env->NewObject(
        TreeNodeCPP,
        env->GetMethodID(TreeNodeCPP, "<init>", "(J)V"),
        (jlong) n);
  }

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
  void EvalDerivative(BitPattern player, BitPattern opponent, EvalLarge lower, EvalLarge upper, NVisited max_n_visited, double max_time) {
    evaluator_derivative_.Evaluate(player, opponent, lower, upper, max_n_visited, max_time);
  }

  jobject GetFirstPosition(JNIEnv* env) {
    return TreeNodeToJava(&evaluator_derivative_.GetFirstPosition(), env);
  }

  jobject Get(JNIEnv* env, BitPattern player, BitPattern opponent) {
    return TreeNodeToJava(&evaluator_derivative_.Get(player, opponent), env);
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
  TreeNode t;
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

JNIEXPORT void JNICALL Java_jni_JNI_evaluate(
    JNIEnv* env, jobject obj, jlong player, jlong opponent, jint lower,
    jint upper, jlong maxNVisited, jint maxTimeMillis) {
  kJNIWrapper.EvalDerivative(player, opponent, lower / 100, upper / 100, maxNVisited, maxTimeMillis);
}

JNIEXPORT void JNICALL Java_jni_JNI_empty(JNIEnv* env, jclass) {

}

JNIEXPORT void JNICALL Java_jni_JNI_stop(JNIEnv* env, jclass) {

}

JNIEXPORT jlong JNICALL Java_jni_JNI_getNVisited(JNIEnv* env, jclass) {
  return 1;
}

JNIEXPORT jobject JNICALL Java_jni_JNI_getStatus(JNIEnv* env, jclass) {
  return 0;
}

JNIEXPORT jobject JNICALL Java_jni_JNI_getFirstPosition(JNIEnv* env, jclass) {
  return kJNIWrapper.GetFirstPosition(env);
}

JNIEXPORT jobject JNICALL Java_jni_JNI_get(JNIEnv* env, jclass, jlong player, jlong opponent) {
  return kJNIWrapper.Get(env, static_cast<BitPattern>(player), static_cast<BitPattern>(opponent));
}

TreeNode* TreeNodeFromJava(JNIEnv* env, jobject tree_node_java) {
  jclass cls = env->GetObjectClass(tree_node_java);
  jmethodID mid = env->GetMethodID(cls, "getNodeAddress", "()J");
  return (TreeNode*) env->CallLongMethod(tree_node_java, mid);\
}

JNIEXPORT jlong JNICALL Java_jni_TreeNodeCPP_getDescendants(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  return TreeNodeFromJava(env, tree_node_java)->GetNVisited(eval_goal / 100);
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getEval(JNIEnv* env, jobject tree_node_java) {
  return TreeNodeFromJava(env, tree_node_java)->GetEval() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getLower(JNIEnv* env, jobject tree_node_java) {
  return TreeNodeFromJava(env, tree_node_java)->Lower() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getUpper(JNIEnv* env, jobject tree_node_java) {
  return TreeNodeFromJava(env, tree_node_java)->Upper() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getWeakLower(JNIEnv* env, jobject tree_node_java) {
  return TreeNodeFromJava(env, tree_node_java)->WeakLower() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getWeakUpper(JNIEnv* env, jobject tree_node_java) {
  return TreeNodeFromJava(env, tree_node_java)->WeakUpper() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getPercentileLower(JNIEnv* env, jobject tree_node_java, jfloat p) {
  return TreeNodeFromJava(env, tree_node_java)->GetPercentileLower(p) * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getPercentileUpper(JNIEnv* env, jobject tree_node_java, jfloat p) {
  return TreeNodeFromJava(env, tree_node_java)->GetPercentileUpper(p) * 100;
}

JNIEXPORT jfloat JNICALL Java_jni_TreeNodeCPP_proofNumber(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  return TreeNodeFromJava(env, tree_node_java)->ProofNumber(eval_goal / 100);
}

JNIEXPORT jfloat JNICALL Java_jni_TreeNodeCPP_disproofNumber(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  return TreeNodeFromJava(env, tree_node_java)->DisproofNumber(eval_goal / 100);
}

JNIEXPORT jfloat JNICALL Java_jni_TreeNodeCPP_getProb(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  return TreeNodeFromJava(env, tree_node_java)->ProbGreaterEqual(eval_goal / 100);
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_maxLogDerivative(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  return TreeNodeFromJava(env, tree_node_java)->MaxLogDerivative(eval_goal / 100);
}

#ifdef __cplusplus
}
#endif
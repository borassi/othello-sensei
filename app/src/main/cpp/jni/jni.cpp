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

#include <cmath>
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

  jobject EvalAlphaBeta(JNIEnv* env, BitPattern player, BitPattern opponent, jint depth, jint lower, jint upper) {
    jclass EvalWithVisited = env->FindClass("evaluateposition/EvalWithVisited");
    EvalLarge eval = evaluator_alpha_beta_.Evaluate(player, opponent, depth, EvalJavaToEvalLarge(lower), EvalJavaToEvalLarge(upper));
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
    return TreeNodeToJava(evaluator_derivative_.GetFirstPosition(), env);
  }

  jobject Get(JNIEnv* env, BitPattern player, BitPattern opponent) {
    return TreeNodeToJava(evaluator_derivative_.Get(player, opponent), env);
  }

  void Empty() { Stop(); hash_map_.Reset(); }
  void Stop() { evaluator_derivative_.Stop(); }
  jobject GetStatus(JNIEnv* env) {
    Status status = evaluator_derivative_.GetStatus();
    jclass JavaStatus = env->FindClass("evaluateposition/Status");
    jfieldID status_id;
    switch (status) {
      case SOLVED:
        status_id = env->GetStaticFieldID(JavaStatus, "SOLVED", "Levaluateposition/Status;");
        break;
      case NONE:
        status_id = env->GetStaticFieldID(JavaStatus, "NONE", "Levaluateposition/Status;");
        break;
      case STOPPED_TIME:
        status_id = env->GetStaticFieldID(JavaStatus, "STOPPED_TIME", "Levaluateposition/Status;");
        break;
      case STOPPED_POSITIONS:
        status_id = env->GetStaticFieldID(JavaStatus, "STOPPED_POSITIONS", "Levaluateposition/Status;");
        break;
      case KILLING:
        status_id = env->GetStaticFieldID(JavaStatus, "KILLING", "Levaluateposition/Status;");
        break;
      case KILLED:
        status_id = env->GetStaticFieldID(JavaStatus, "KILLED", "Levaluateposition/Status;");
        break;
      case RUNNING:
        status_id = env->GetStaticFieldID(JavaStatus, "RUNNING", "Levaluateposition/Status;");
        break;
      case FAILED:
        status_id = env->GetStaticFieldID(JavaStatus, "FAILED", "Levaluateposition/Status;");
        break;
    }
    return env->GetStaticObjectField(JavaStatus, status_id);
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
    JNIEnv* env, jclass, jlong player, jlong opponent, jint depth, jint lower,
    jint upper) {
  return kJNIWrapper.EvalAlphaBeta(env, player, opponent, depth, lower, upper);
}

JNIEXPORT void JNICALL Java_jni_JNI_evaluate(
    JNIEnv* env, jobject obj, jlong player, jlong opponent, jint lower,
    jint upper, jlong maxNVisited, jint maxTimeMillis) {
  kJNIWrapper.EvalDerivative(player, opponent, lower / 100, upper / 100, maxNVisited, maxTimeMillis / 1000.0);
}

JNIEXPORT void JNICALL Java_jni_JNI_empty(JNIEnv* env, jobject) {
  kJNIWrapper.Empty();
}

JNIEXPORT void JNICALL Java_jni_JNI_resetHashMap(JNIEnv* env, jclass) {
  kJNIWrapper.Empty();
}

JNIEXPORT void JNICALL Java_jni_JNI_stop(JNIEnv* env, jobject) {
  kJNIWrapper.Stop();
}

JNIEXPORT jobject JNICALL Java_jni_JNI_getStatus(JNIEnv* env, jobject thiz) {
  return kJNIWrapper.GetStatus(env);
}

JNIEXPORT jobject JNICALL Java_jni_JNI_getFirstPosition(JNIEnv* env, jobject) {
  return kJNIWrapper.GetFirstPosition(env);
}

JNIEXPORT jobject JNICALL Java_jni_JNI_get(JNIEnv* env, jobject, jlong player, jlong opponent) {
  return kJNIWrapper.Get(env, static_cast<BitPattern>(player), static_cast<BitPattern>(opponent));
}

TreeNode* TreeNodeFromJava(JNIEnv* env, jobject tree_node_java) {
  jclass cls = env->GetObjectClass(tree_node_java);
  jmethodID mid = env->GetMethodID(cls, "getNodeAddress", "()J");
  return (TreeNode*) env->CallLongMethod(tree_node_java, mid);\
}

JNIEXPORT jlong JNICALL Java_jni_TreeNodeCPP_getDescendants(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? 0 : static_cast<long>(node->GetNVisited());
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getEval(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? 0 : lround(node->GetEval() * 100);
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getLower(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? -6600 : node->Lower() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getUpper(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? 6600 : node->Upper() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getWeakLower(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? -6600 : node->WeakLower() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getWeakUpper(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? 6600 : node->WeakUpper() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getPercentileLower(JNIEnv* env, jobject tree_node_java, jfloat p) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? -6600 : node->GetPercentileLower(p) * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getPercentileUpper(JNIEnv* env, jobject tree_node_java, jfloat p) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? 6600 : node->GetPercentileUpper(p) * 100;
}

JNIEXPORT jfloat JNICALL Java_jni_TreeNodeCPP_proofNumber(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? NAN : node->ProofNumber(eval_goal / 100);
}

JNIEXPORT jfloat JNICALL Java_jni_TreeNodeCPP_disproofNumber(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? NAN : node->DisproofNumber(eval_goal / 100);
}

JNIEXPORT jfloat JNICALL Java_jni_TreeNodeCPP_getProb(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? NAN : node->ProbGreaterEqual(eval_goal / 100);
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_maxLogDerivative(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? kLogDerivativeMinusInf : node->MaxLogDerivative(eval_goal / 100);
}

JNIEXPORT jlong JNICALL Java_jni_TreeNodeCPP_getPlayer(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return static_cast<long long>(node->Player());
}

JNIEXPORT jlong JNICALL Java_jni_TreeNodeCPP_getOpponent(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return static_cast<long long>(node->Opponent());
}

#ifdef __cplusplus
}
#endif
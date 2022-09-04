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
#include <jni.h>

constexpr int kNumEvaluators = 60;

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
      tree_node_supplier_(),
      last_player_(0),
      last_opponent_(0),
      last_gap_(-1),
      num_active_evaluators_(0) {
    for (int i = 0; i < kNumEvaluators; ++i) {
      evaluator_derivative_[i] = std::make_unique<EvaluatorDerivative>(
          &tree_node_supplier_, &hash_map_,
          PatternEvaluator::Factory(evals_.data()), 1,
          static_cast<u_int8_t>(i));
    }
  }

  EvaluatorDerivative* BestEvaluator(float gap) {
    double best = -DBL_MAX;
    EvaluatorDerivative* best_evaluator = nullptr;
    for (int i = 0; i < num_active_evaluators_; ++i) {
      EvaluatorDerivative* evaluator = evaluator_derivative_[i].get();
      double value = evaluator->Progress(gap);
      if (value > best) {
        best = value;
        best_evaluator = evaluator;
      }
    }
    return best_evaluator;
  }

  bool Finished(NVisited max_n_visited) {
    NVisited visited = 0;
    bool all_solved = true;
    for (int i = 0; i < this->num_active_evaluators_; ++i) {
      EvaluatorDerivative* evaluator = evaluator_derivative_[i].get();
      visited += evaluator->GetFirstPosition()->GetNVisited();
      switch (evaluator->GetStatus()) {
        case NONE:
        case RUNNING:
          assert(false);
        case FAILED:
        case KILLING:
        case KILLED:
          return true;
        case SOLVED:
          break;
        case STOPPED_TIME:
        case STOPPED_POSITIONS:
          all_solved = false;
      }
    }
    return visited > max_n_visited || all_solved;
  }

  void EvalDerivative(
      BitPattern player, BitPattern opponent, Eval lower, Eval upper, NVisited max_n_visited, double max_time, float gap) {
    bool reset = player != last_player_ || opponent != last_opponent_ || ((gap == 0) != (last_gap_ == 0));
    if (reset) {
      tree_node_supplier_.Reset();
      if (gap <= 0) {
        num_active_evaluators_ = 1;
        evaluator_derivative_[0]->Evaluate(player, opponent, lower, upper, max_n_visited, max_time);
      } else {
        const auto flips = GetAllMovesWithPass(player, opponent);
        num_active_evaluators_ = static_cast<int>(flips.size());
        for (int i = 0; i < num_active_evaluators_; ++i) {
          auto flip = flips[i];
          evaluator_derivative_[i]->Evaluate(
              NewPlayer(flip, opponent),
              NewOpponent(flip, player), lower, upper, max_n_visited,
              max_time / num_active_evaluators_);
        }
      }
    } else {
      for (int step = 0; step < 5 && !Finished(max_n_visited); ++step) {
        BestEvaluator(gap)->ContinueEvaluate(max_n_visited, max_time / 5);
      }
    }
    last_player_ = player;
    last_opponent_ = opponent;
    last_gap_ = gap;
  }

  jobject GetFirstPosition(JNIEnv* env) {
    return TreeNodeToJava(evaluator_derivative_[0]->GetFirstPosition(), env);
  }

  jobject Get(JNIEnv* env, BitPattern player, BitPattern opponent) {
    TreeNode* node = tree_node_supplier_.Get(player, opponent);
    if (node != nullptr) {
      return TreeNodeToJava(node, env);
    }
    for (int i = 0; i < kNumEvaluators; ++i) {
      TreeNode* first_position = evaluator_derivative_[i]->GetFirstPosition();
      if (first_position == nullptr) {
        continue;
      }
      if (first_position->Player() == player && first_position->Opponent() == opponent) {
        return TreeNodeToJava(first_position, env);
      }
      for (TreeNode* const child : first_position->GetChildren()) {
        if (child->Player() == player && child->Opponent() == opponent) {
          return TreeNodeToJava(child, env);
        }
      }
    }
    for (BitPattern flip : GetAllMovesWithPass(player, opponent)) {
      TreeNode* child = tree_node_supplier_.Get(NewPlayer(flip, opponent),
                                                NewOpponent(flip, player));
      if (child == nullptr) {
        continue;
      }
      for (TreeNode* father : child->Fathers()) {
        if (father->Player() == player && father->Opponent() == opponent) {
          return TreeNodeToJava(father, env);
        }
      }
    }
    return TreeNodeToJava(node, env);
  }

  void Empty() {
    Stop();
    hash_map_.Reset();
    tree_node_supplier_.Reset();
  }

  void Stop() {
    for (int i = 0; i < kNumEvaluators; ++i) {
      evaluator_derivative_[i]->Stop();
    }
  }

  jobject GetStatus(JNIEnv* env) {
    Status status = evaluator_derivative_[0]->GetStatus();
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
  int num_active_evaluators_;
  EvalType evals_;
  HashMap hash_map_;
  TreeNode t;
  std::array<std::unique_ptr<EvaluatorDerivative>, kNumEvaluators> evaluator_derivative_;
  TreeNodeSupplier tree_node_supplier_;
  BitPattern last_player_;
  BitPattern last_opponent_;
  float last_gap_;
};

JNIWrapper kJNIWrapper;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_jni_JNI_evaluate(
    JNIEnv* env, jobject obj, jlong player, jlong opponent, jint lower,
    jint upper, jlong maxNVisited, jint maxTimeMillis, jfloat gap) {
  kJNIWrapper.EvalDerivative(player, opponent, static_cast<Eval>(lower / 100), static_cast<Eval>(upper / 100), maxNVisited, maxTimeMillis / 1000.0, gap);
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
  return (TreeNode*) env->CallLongMethod(tree_node_java, mid);
}

bool IsEvalGoalInvalid(TreeNode* node, int eval_goal) {
  return node == nullptr || node->WeakLower() > eval_goal / 100 || node->WeakUpper() < eval_goal / 100;
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
  return IsEvalGoalInvalid(node, eval_goal) ? NAN : node->ProofNumber(static_cast<Eval>(eval_goal / 100));
}

JNIEXPORT jfloat JNICALL Java_jni_TreeNodeCPP_disproofNumber(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return IsEvalGoalInvalid(node, eval_goal) ? NAN : node->DisproofNumber(static_cast<Eval>(eval_goal / 100));
}

JNIEXPORT jfloat JNICALL Java_jni_TreeNodeCPP_getProb(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return IsEvalGoalInvalid(node, eval_goal) ? NAN : node->ProbGreaterEqual(static_cast<Eval>(eval_goal / 100));
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_maxLogDerivative(JNIEnv* env, jobject tree_node_java, jint eval_goal) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return IsEvalGoalInvalid(node, eval_goal) ? kLogDerivativeMinusInf : node->MaxLogDerivative(static_cast<Eval>(eval_goal / 100));
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_childLogDerivative(JNIEnv* env, jobject father_node_java, jobject child_node_java, jint eval_goal) {
  auto father = TreeNodeFromJava(env, father_node_java);
  auto child = TreeNodeFromJava(env, child_node_java);
  return IsEvalGoalInvalid(father, eval_goal) || IsEvalGoalInvalid(child, -eval_goal) ?
      kLogDerivativeMinusInf : father->ChildLogDerivative(child, static_cast<Eval>(eval_goal / 100));
}

JNIEXPORT jlong JNICALL Java_jni_TreeNodeCPP_getPlayer(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? 0 : static_cast<long long>(node->Player());
}

JNIEXPORT jlong JNICALL Java_jni_TreeNodeCPP_getOpponent(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node == nullptr ? 0 : static_cast<long long>(node->Opponent());
}

JNIEXPORT jboolean JNICALL Java_jni_JNI_finished(JNIEnv* env, jobject thiz, jlong max_nvisited) {
  return kJNIWrapper.Finished(max_nvisited);
}

#ifdef __cplusplus
}
#endif
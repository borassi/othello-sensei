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
#include <thread>

#include "../analyzers/endgame_ffo.h"
#include "../book/book.h"
#include "../hashmap/hash_map.h"
#include "../evaluatederivative/evaluator_derivative.h"
#include "../evaluatealphabeta/evaluator_alpha_beta.h"
#include "../evaluatedepthone/pattern_evaluator.h"
#include "../utils/assets.h"

constexpr int kNumEvaluators = 60;

jobject BoardToJava(JNIEnv* env, const Board& board) {
    jclass BoardJava = env->FindClass("board/Board");
    return env->NewObject(
        BoardJava,
        env->GetMethodID(BoardJava, "<init>", "(JJ)V"),
        (jlong) board.Player(), (jlong) board.Opponent());
}

Board BoardToCPP(JNIEnv* env, jobject board) {
  jclass BoardJava = env->FindClass("board/Board");
  auto get_player = env->GetMethodID(BoardJava, "getPlayer", "()J");
  auto get_opponent = env->GetMethodID(BoardJava, "getOpponent", "()J");
  return {
      static_cast<BitPattern>(env->CallLongMethod(board, get_player)),
      static_cast<BitPattern>(env->CallLongMethod(board, get_opponent))
  };
}

class JNIWrapper {
 public:
  static jobject TreeNodeToJava(const std::optional<BookTreeNode>& n, JNIEnv* env) {
    if (!n) {
      return NULL;
    }
    BookTreeNode* node = new BookTreeNode(*n);
    jclass TreeNodeCPP = env->FindClass("jni/TreeNodeCPP");
    return env->NewObject(
        TreeNodeCPP,
        env->GetMethodID(TreeNodeCPP, "<init>", "(J)V"),
        (jlong) node);
  }

  JNIWrapper() :
      evals_(LoadEvals()),
      hash_map_(),
      tree_node_supplier_(),
      last_boards_(),
      last_gap_(-1),
      stopping_(false),
      reset_(true),
      book_(kBookFilepath) {
    for (int i = 0; i < kNumEvaluators; ++i) {
      evaluator_derivative_[i] = std::make_unique<EvaluatorDerivative>(
          &tree_node_supplier_, &hash_map_,
          PatternEvaluator::Factory(evals_.data()),
          std::thread::hardware_concurrency(),
          static_cast<u_int8_t>(i));
    }
  }

  EvaluatorDerivative* BestEvaluator(float gap) {
    double best = -DBL_MAX;
    EvaluatorDerivative* best_evaluator = nullptr;
    for (int i = 0; i < last_boards_.size(); ++i) {
      EvaluatorDerivative* evaluator = evaluator_derivative_[i].get();
      double value = evaluator->Progress(gap);
      if (evaluator->GetStatus() != STOPPED_TIME) {
        value += 1E12;
      }
      if (value > best) {
        best = value;
        best_evaluator = evaluator;
      }
    }
    return best_evaluator;
  }

  bool Finished(NVisited max_n_visited) {
    std::ostringstream stringStream;
    NVisited visited = 0;
    bool all_solved = true;
    for (int i = 0; i < last_boards_.size(); ++i) {
      EvaluatorDerivative* evaluator = evaluator_derivative_[i].get();
      visited += evaluator->GetFirstPosition()->GetNVisited();
      switch (evaluator->GetStatus()) {
        case NONE:
        case RUNNING:
        case FAILED:
          assert(false);
        case KILLING:
        case KILLED:
        case STOPPED_TREE_POSITIONS:
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

  void Evaluate(
      std::vector<Board> boards, Eval lower, Eval upper, NVisited max_n_visited,
      double max_time, float gap) {
    stopping_ = false;
    bool reset = reset_ || ((gap == 0) != (last_gap_ == 0)) || boards !=
        last_boards_;

    last_boards_ = boards;
    last_gap_ = gap;
    if (reset) {
      reset_ = false;
      tree_node_supplier_.Reset();
      for (int i = 0; i < boards.size(); ++i) {
        Board b = boards[i];
        evaluator_derivative_[i]->Evaluate(
            b.Player(), b.Opponent(), lower, upper, max_n_visited,
            max_time / boards.size());
      }
    } else {
      for (int step = 0; step < boards.size() && !stopping_ && !Finished(max_n_visited); ++step) {
        BestEvaluator(gap)->ContinueEvaluate(max_n_visited, max_time / boards.size());
      }
    }
  }

  jobject GetFirstPosition(JNIEnv* env) {
    TreeNode* node = evaluator_derivative_[0]->GetFirstPosition();
    if (node == nullptr) {
      return NULL;
    }
    std::cout << "Firstpos\n" << std::flush;
    return TreeNodeToJava(*node, env);
  }

  jobject GetFromBook(JNIEnv* env, BitPattern player, BitPattern opponent) {
    std::cout << "Book\n" << std::flush;
    return TreeNodeToJava(book_.Get(player, opponent), env);
  }

  jobject Get(JNIEnv* env, BitPattern player, BitPattern opponent) {
    TreeNode* node = nullptr;
    for (int i = 0; i < last_boards_.size(); ++i) {
      const auto& evaluator = evaluator_derivative_[i];
      TreeNode* first_position = evaluator->GetFirstPosition();
      if (first_position != nullptr &&
          first_position->Player() == player &&
          first_position->Opponent() == opponent) {
        node = first_position;
        break;
      }
      if (!node) {
        node = tree_node_supplier_.Get(player, opponent, evaluator->Index());
      }
    }
    if (node == nullptr) {
      return NULL;
    }
    return TreeNodeToJava(BookTreeNode(*node), env);
  }

  void Empty() {
    Stop();
    hash_map_.Reset();
    tree_node_supplier_.Reset();
    reset_ = true;
  }

  void Stop() {
    stopping_ = true;
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
      case STOPPED_TREE_POSITIONS:
        status_id = env->GetStaticFieldID(JavaStatus, "STOPPED_TREE_POSITIONS", "Levaluateposition/Status;");
        break;
    }
    return env->GetStaticObjectField(JavaStatus, status_id);
  }

  void AddToBook() {
    for (int i = 0; i < last_boards_.size(); ++i) {
      book_.Put(*evaluator_derivative_[i]->GetFirstPosition());
    }
  }

 private:
  Book book_;
  EvalType evals_;
  HashMap hash_map_;
  std::array<std::unique_ptr<EvaluatorDerivative>, kNumEvaluators> evaluator_derivative_;
  TreeNodeSupplier tree_node_supplier_;
  std::vector<Board> last_boards_;
  float last_gap_;
  bool stopping_;
  bool reset_;
};

#ifdef __cplusplus
extern "C" {
#endif

JNIWrapper* JNIFromJava(JNIEnv* env, jobject obj) {
  jclass cls = env->GetObjectClass(obj);
  jmethodID mid = env->GetMethodID(cls, "getPointer", "()J");
  return (JNIWrapper*) env->CallLongMethod(obj, mid);
}

JNIEXPORT void JNICALL Java_jni_JNI_create(JNIEnv* env, jobject obj, jobject
fileAccessor) {
  jclass cls = env->FindClass("jni/JNI");
  jmethodID mid = env->GetMethodID(cls, "setup", "(J)V");
#if ANDROID
  jclass file_accessor = env->FindClass("ui_mobile/FileAccessorMobile");
  jmethodID get_asset = env->GetMethodID(file_accessor,
                                         "getAssetManager",
                                         "()Landroid/content/res/AssetManager;");
  AndroidAsset::Setup(env, env->CallObjectMethod(
      fileAccessor, get_asset));
#endif
  env->CallVoidMethod(obj, mid, (jlong) new JNIWrapper());
}

BookTreeNode* TreeNodeFromJava(JNIEnv* env, jobject tree_node_java) {
  jclass cls = env->GetObjectClass(tree_node_java);
  jmethodID mid = env->GetMethodID(cls, "getNodeAddress", "()J");
  return (BookTreeNode*) env->CallLongMethod(tree_node_java, mid);
}

JNIEXPORT void JNICALL Java_jni_JNI_finalize(JNIEnv* env, jobject obj) {
  delete JNIFromJava(env, obj);
}

JNIEXPORT void JNICALL Java_jni_JNI_evaluate(
    JNIEnv* env, jobject obj, jobject boards, jint lower,
    jint upper, jlong maxNVisited, jint maxTimeMillis, jfloat gap) {
  jclass arraylist = env->FindClass("java/util/ArrayList");
  jmethodID sizeId = env->GetMethodID(arraylist, "size", "()I");
  jmethodID getId = env->GetMethodID(arraylist, "get", "(I)Ljava/lang/Object;");
  jclass boardClass = env->FindClass("board/Board");
  jmethodID playerId = env->GetMethodID(boardClass, "getPlayer", "()J");
  jmethodID opponentId = env->GetMethodID(boardClass, "getOpponent", "()J");
  int size = env->CallIntMethod(boards, sizeId);
  std::vector<Board> boards_vector;
  for (int i = 0; i < size; ++i) {
    jobject board = env->CallObjectMethod(boards, getId, i);
    BitPattern player = (BitPattern) env->CallLongMethod(board, playerId);
    BitPattern opponent = (BitPattern) env->CallLongMethod(board, opponentId);
    boards_vector.push_back(Board(player, opponent));
  }
  JNIFromJava(env, obj)->Evaluate(
      boards_vector, static_cast<Eval>(lower / 100),
      static_cast<Eval>(upper / 100), maxNVisited,
      maxTimeMillis / 1000.0, gap);
}

JNIEXPORT void JNICALL Java_jni_JNI_empty(JNIEnv* env, jobject obj) {
  JNIFromJava(env, obj)->Empty();
}

JNIEXPORT void JNICALL Java_jni_JNI_stop(JNIEnv* env, jobject obj) {
  JNIFromJava(env, obj)->Stop();
}

JNIEXPORT void JNICALL Java_jni_JNI_addToBook(JNIEnv* env, jobject obj) {
  JNIFromJava(env, obj)->AddToBook();
}

JNIEXPORT jobject JNICALL Java_jni_JNI_getStatus(JNIEnv* env, jobject obj) {
  return JNIFromJava(env, obj)->GetStatus(env);
}

JNIEXPORT jobject JNICALL Java_jni_JNI_getFirstPosition(JNIEnv* env, jobject obj) {
  return JNIFromJava(env, obj)->GetFirstPosition(env);
}

JNIEXPORT jobject JNICALL Java_jni_JNI_get(
    JNIEnv* env, jobject obj, jlong player, jlong opponent) {
  return JNIFromJava(env, obj)->Get(env, static_cast<BitPattern>(player), static_cast<BitPattern>(opponent));
}

JNIEXPORT jobject JNICALL Java_jni_JNI_getFromBook(
    JNIEnv* env, jobject obj, jlong player, jlong opponent) {
  return JNIFromJava(env, obj)->GetFromBook(
      env, static_cast<BitPattern>(player),
      static_cast<BitPattern>(opponent));
}

JNIEXPORT jboolean JNICALL Java_jni_JNI_finished(JNIEnv* env, jobject obj, jlong max_nvisited) {
  return JNIFromJava(env, obj)->Finished(max_nvisited);
}

bool IsEvalGoalInvalid(BookTreeNode* node, int eval_goal) {
  return node->WeakLower() > eval_goal / 100 || node->WeakUpper() < eval_goal / 100;
}

JNIEXPORT void JNICALL Java_jni_TreeNodeCPP_close(JNIEnv *env, jobject thiz) {
  auto node = TreeNodeFromJava(env, thiz);
  delete(node);
}

JNIEXPORT jlong JNICALL Java_jni_TreeNodeCPP_getDescendants(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return static_cast<long>(node->GetNVisited());
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getEval(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return lround(node->GetEval() * 100);
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getLower(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node->Lower() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getUpper(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node->Upper() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getWeakLower(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node->WeakLower() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getWeakUpper(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node->WeakUpper() * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getPercentileLower(JNIEnv* env, jobject tree_node_java, jfloat p) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node->GetPercentileLower(p) * 100;
}

JNIEXPORT jint JNICALL Java_jni_TreeNodeCPP_getPercentileUpper(JNIEnv* env, jobject tree_node_java, jfloat p) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return node->GetPercentileUpper(p) * 100;
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
  return static_cast<long long>(node->Player());
}

JNIEXPORT jlong JNICALL Java_jni_TreeNodeCPP_getOpponent(JNIEnv* env, jobject tree_node_java) {
  auto node = TreeNodeFromJava(env, tree_node_java);
  return static_cast<long long>(node->Opponent());
}

JNIEXPORT jobject JNICALL
Java_jni_JNI_getEndgameBoard(JNIEnv* env, jclass clazz, jint i) {
  return BoardToJava(env, GetIthBoard(i));
}

JNIEXPORT jobject JNICALL
Java_jni_JNI_move(JNIEnv *env, jclass clazz, jobject board, jint move) {
  Board next = BoardToCPP(env, board);
  if ((1ULL << move) & (next.Player() | next.Opponent())) {
    return board;
  }
  BitPattern flip = GetFlip(move, next.Player(), next.Opponent());
  if (flip == 0) {
    return board;
  }
  next.PlayMove(flip);
  return BoardToJava(env, next);
}

JNIEXPORT jboolean JNICALL
Java_jni_JNI_haveToPass(JNIEnv *env, jclass clazz, jobject board) {
  Board b = BoardToCPP(env, board);
  return HaveToPass(b.Player(), b.Opponent());
}

JNIEXPORT jobject JNICALL
Java_jni_JNI_descendants(JNIEnv *env, jclass clazz, jobject board) {
  Board b = BoardToCPP(env, board);
  auto java_util_ArrayList = static_cast<jclass>(env->NewGlobalRef
      (env->FindClass("java/util/ArrayList")));
  jmethodID java_util_ArrayList_ = env->GetMethodID(java_util_ArrayList, "<init>", "(I)V");
  jmethodID java_util_ArrayList_add = env->GetMethodID(java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
  auto flips = GetAllMoves(b.Player(), b.Opponent());
  jobject result = env->NewObject(java_util_ArrayList, java_util_ArrayList_,
                                  (jint) flips.size());

  for (BitPattern flip : flips) {
    Board next = b;
    next.PlayMove(flip);
    env->CallBooleanMethod(result, java_util_ArrayList_add,
                           BoardToJava(env, next));
  }
  return result;
}
#ifdef __cplusplus
};
#endif
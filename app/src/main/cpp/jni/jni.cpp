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
#include <jni.h>

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

  static jobject EvaluationToJava(const Evaluation& evaluation, JNIEnv* env) {
    const jclass EvaluationJava = env->FindClass("jni/Evaluation");
    const jmethodID initEvaluationJava = env->GetMethodID(EvaluationJava, "<init>", "(DIFF)V");
    return env->NewObject(
        EvaluationJava,
        initEvaluationJava,
        evaluation.ProbGreaterEqual(),
        evaluation.MaxLogDerivative(),
        evaluation.ProofNumber(),
        evaluation.DisproofNumber()
        );
  }

  static jobject NodeToJava(std::optional<Node> n, JNIEnv* env) {
    if (!n) {
      return NULL;
    }
    const Node& node = *n;
    const jclass NodeJava = env->FindClass("jni/Node");
    const jmethodID initNodeJava = env->GetMethodID(NodeJava, "<init>", "(JJJFFIIIIIILjava/util/ArrayList;)V");

    const jclass ArrayList = env->FindClass("java/util/ArrayList");
    const jmethodID initArrayList = env->GetMethodID(ArrayList, "<init>", "()V");
    const jmethodID addArrayList = env->GetMethodID(ArrayList, "add", "(Ljava/lang/Object;)Z");

    jobject evaluations = env->NewObject(ArrayList, initArrayList);
    assert((node.WeakLower() - kMinEval) % 2 == 1);
    for (int i = node.WeakLower(); i <= node.WeakUpper(); i += 2) {
      env->CallBooleanMethod(evaluations, addArrayList, EvaluationToJava(node.GetEvaluation(i), env));
    }
    return env->NewObject(
        NodeJava,
        initNodeJava,
        (jlong) node.Player(),
        (jlong) node.Opponent(),
        (jlong) node.GetNVisited(),
        node.GetEval(),
        node.LeafEval(),
        node.Lower(),
        node.Upper(),
        node.WeakLower(),
        node.WeakUpper(),
        node.GetPercentileLower(kProbIncreaseWeakEval),
        node.GetPercentileUpper(kProbIncreaseWeakEval),
        evaluations
    );
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
//          1,
          std::thread::hardware_concurrency(),
          static_cast<u_int8_t>(i));
    }
    if (!book_.Get(Board())) {
      std::shared_ptr<TreeNode> t(new TreeNode());
      t->Reset(Board().Player(), Board().Opponent(), 0, 0, 0, 2, -63, 63);
      book_.Add(*t);
      book_.Commit();
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
  NVisited TotalVisited() const {
    NVisited visited = 0;
    for (int i = 0; i < last_boards_.size(); ++i) {
      visited += evaluator_derivative_[i]->GetFirstPosition().GetNVisited();
    }
    return visited;
  }

  bool Finished(NVisited max_n_visited) const {
    std::ostringstream stringStream;
    NVisited visited = 0;
    bool all_solved = true;
    if (stopping_) {
      return true;
    }
    for (int i = 0; i < last_boards_.size(); ++i) {
      EvaluatorDerivative* evaluator = evaluator_derivative_[i].get();
      visited += evaluator->GetFirstPosition().GetNVisited();
      switch (evaluator->GetStatus()) {
        case NONE:
        case RUNNING:
        case FAILED:
          assert(false);
        case STOPPED_TREE_POSITIONS:
          return true;
        case SOLVED:
          break;
        case KILLING:
        case KILLED:
        case STOPPED_TIME:
        case STOPPED_POSITIONS:
          all_solved = false;
      }
    }
    return visited > 0.8 * max_n_visited || all_solved;
  }

  void Evaluate(
      std::vector<Board> boards, Eval lower, Eval upper, NVisited max_n_visited,
      double max_time, float gap, bool approx) {
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
            b.Player(), b.Opponent(), lower, upper, max_n_visited / boards.size(),
            max_time / boards.size(), approx);
      }
    } else {
      NVisited visited = TotalVisited();
      int increments = 10;
      for (int step = 0; step < increments && !Finished(max_n_visited); ++step) {
        auto evaluator = BestEvaluator(gap);
        evaluator->ContinueEvaluate(
            (max_n_visited - visited) / increments,
            max_time / increments);
      }
    }
  }

  jobject GetFirstPosition(JNIEnv* env) {
    return NodeToJava(evaluator_derivative_[0]->GetFirstPosition(), env);
  }

  jobject GetFromBook(JNIEnv* env, BitPattern player, BitPattern opponent) {
    return NodeToJava(book_.Get(player, opponent), env);
  }

  jobject Get(JNIEnv* env, BitPattern player, BitPattern opponent) {
    std::optional<Node> node = std::nullopt;
    Board unique = Board(player, opponent).Unique();
    for (int i = 0; i < last_boards_.size(); ++i) {
      const auto& evaluator = evaluator_derivative_[i];
      Node first_position = evaluator->GetFirstPosition();
      if (first_position.ToBoard().Unique() == unique) {
        return NodeToJava(first_position, env);
      }
      for (Board b : unique.AllTranspositions()) {
        auto node = tree_node_supplier_.Get(b.Player(), b.Opponent(), evaluator->Index());
        if (node) {
          return NodeToJava(node, env);
        }
      }
    }
    return NULL;
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

  void AddToBook(const Board& father, const std::vector<Board>& parents) {
    auto father_node = book_.Get(father);
    if (!father_node || !father_node.value().IsLeaf()) {
      return;
    }
    std::vector<Node> children(last_boards_.size());
    NVisited n_visited = 0;
    for (int i = 0; i < last_boards_.size(); ++i) {
      Node child = evaluator_derivative_[i]->GetFirstPosition();
      children.push_back(child);
      n_visited += child.GetNVisited();
    }
    std::vector<Book<>::BookNode*> parents_node;

    for (auto parent : parents) {
      parents_node.push_back(book_.Mutable(parent).value());
    }
    book_.AddChildren(father, children);
    LeafToUpdate<Book<>::BookNode>::Leaf(parents_node).Finalize(n_visited);
    book_.Commit();
  }

 private:
  Book<> book_;
  EvalType evals_;
  HashMap<kBitHashMap> hash_map_;
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
  AndroidAsset::Setup(env, env->CallObjectMethod(fileAccessor, get_asset));
#endif
  env->CallVoidMethod(obj, mid, (jlong) new JNIWrapper());
}

TreeNode* TreeNodeFromJava(JNIEnv* env, jobject tree_node_java) {
  jclass cls = env->GetObjectClass(tree_node_java);
  jmethodID mid = env->GetMethodID(cls, "getNodeAddress", "()J");
  return (TreeNode*) env->CallLongMethod(tree_node_java, mid);
}

JNIEXPORT void JNICALL Java_jni_JNI_finalize(JNIEnv* env, jobject obj) {
  delete JNIFromJava(env, obj);
}

JNIEXPORT void JNICALL Java_jni_JNI_evaluate(
    JNIEnv* env, jobject obj, jobject boards, jint lower,
    jint upper, jlong maxNVisited, jint maxTimeMillis, jfloat gap, jboolean approx) {
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
      maxTimeMillis / 1000.0, gap, approx);
}

JNIEXPORT void JNICALL Java_jni_JNI_empty(JNIEnv* env, jobject obj) {
  JNIFromJava(env, obj)->Empty();
}

JNIEXPORT void JNICALL Java_jni_JNI_stop(JNIEnv* env, jobject obj) {
  JNIFromJava(env, obj)->Stop();
}

JNIEXPORT void JNICALL Java_jni_JNI_addToBook(
    JNIEnv* env, jobject obj, jobject board, jobject parents) {
  std::vector<Board> parents_cpp;

  jclass arraylist = env->FindClass("java/util/ArrayList");
  jmethodID sizeId = env->GetMethodID(arraylist, "size", "()I");
  jmethodID getId = env->GetMethodID(arraylist, "get", "(I)Ljava/lang/Object;");
  int size = env->CallIntMethod(parents, sizeId);
  parents_cpp.reserve(size);
  for (int i = 0; i < size; ++i) {
    jobject parent = env->CallObjectMethod(parents, getId, i);
    parents_cpp.push_back(BoardToCPP(env, parent));
  }
  JNIFromJava(env, obj)->AddToBook(BoardToCPP(env, board), parents_cpp);
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
Java_jni_JNI_children(JNIEnv *env, jclass clazz, jobject board) {
  Board b = BoardToCPP(env, board);
  auto java_util_ArrayList = static_cast<jclass>(env->NewGlobalRef
      (env->FindClass("java/util/ArrayList")));
  jmethodID java_util_ArrayList_ = env->GetMethodID(java_util_ArrayList, "<init>", "(I)V");
  jmethodID java_util_ArrayList_add = env->GetMethodID(java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
  std::vector<Board> children = GetNextBoardsWithPass(b);
  jobject result = env->NewObject(java_util_ArrayList, java_util_ArrayList_,
                                  (jint) children.size());

  for (Board child : children) {
    env->CallBooleanMethod(result, java_util_ArrayList_add,
                           BoardToJava(env, child));
  }
  return result;
}

JNIEXPORT jobject JNICALL
Java_jni_JNI_uniqueChildren(JNIEnv *env, jclass clazz, jobject board) {
  Board b = BoardToCPP(env, board);
  auto java_util_ArrayList = static_cast<jclass>(env->NewGlobalRef
      (env->FindClass("java/util/ArrayList")));
  jmethodID java_util_ArrayList_ = env->GetMethodID(java_util_ArrayList, "<init>", "(I)V");
  jmethodID java_util_ArrayList_add = env->GetMethodID(java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
  auto children = GetUniqueNextBoardsWithPass(b);
  jobject result = env->NewObject(java_util_ArrayList, java_util_ArrayList_,
                                  (jint) children.size());

  for (auto child : children) {
    env->CallBooleanMethod(result, java_util_ArrayList_add,
                           BoardToJava(env, child.first));
  }
  return result;
}

JNIEXPORT jstring JNICALL
Java_jni_JNI_prettyPrintDouble(JNIEnv *env, jclass clazz, jdouble d) {
  return env->NewStringUTF(PrettyPrintDouble(d).c_str());
}

#ifdef __cplusplus
};
#endif
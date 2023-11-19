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
#include "../thor/thor.h"
#include "../utils/files.h"

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

JNIEXPORT jbyteArray JNICALL Java_Test_returnArray(JNIEnv *env, jobject This)
{
        jbyte a[] = {1,2,3,4,5,6};
        jbyteArray ret = env->NewByteArray(6);
        env->SetByteArrayRegion (ret, 0, 6, a);
        return ret;
}

jobject ThorGameToJava(JNIEnv* env, const Game& game) {
    jclass ThorGameJava = env->FindClass("jni/ThorGame");
    jbyteArray moves = env->NewByteArray(60);
    env->SetByteArrayRegion (moves, 0, 60, (jbyte*) game.MovesByte().data());
    return env->NewObject(
        ThorGameJava,
        env->GetMethodID(ThorGameJava, "<init>",
                         "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[BSS)V"),
        env->NewStringUTF(game.Black().c_str()),
        env->NewStringUTF(game.White().c_str()),
        env->NewStringUTF(game.Tournament().c_str()),
        moves,
        game.Year(),
        game.Score());
}

template<class T, class U>
jobject VectorToArrayList(JNIEnv* env, const std::vector<U>& v, T(*convert)(JNIEnv* env, const U&)) {
  auto java_util_ArrayList = static_cast<jclass>(env->NewGlobalRef
      (env->FindClass("java/util/ArrayList")));
  jmethodID java_util_ArrayList_ = env->GetMethodID(java_util_ArrayList, "<init>", "()V");
  jmethodID java_util_ArrayList_add = env->GetMethodID(java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
  jobject result = env->NewObject(java_util_ArrayList, java_util_ArrayList_);
  for (const U& element : v) {
    env->CallBooleanMethod(result, java_util_ArrayList_add, convert(env, element));
  }
  return result;
}

template<class T, class U>
std::unordered_set<U> ListToSet(JNIEnv* env, jobject l, U(*convert)(JNIEnv* env, const T&)) {
  jclass java_util_List = env->FindClass("java/util/List");
  // retrieve the size and the get method
  jmethodID java_util_List_size = env->GetMethodID(java_util_List, "size", "()I");
  jmethodID java_util_List_get = env->GetMethodID(java_util_List, "get", "(I)Ljava/lang/Object;");
  std::unordered_set<U> result;
  int size = env->CallIntMethod(l, java_util_List_size);
  result.reserve(size);
  for (int i = 0; i < size; ++i) {
    result.insert(convert(env, (T) env->CallObjectMethod(l, java_util_List_get, i)));
  }
  return result;
}

class JNIWrapper {
 public:

  JNIWrapper(
      const std::string& evals_filepath,
      const std::string& book_filepath,
      const std::string& thor_filepath) :
      evals_(LoadEvals(evals_filepath)),
      hash_map_(),
      tree_node_supplier_(),
      last_boards_(),
      last_gap_(-1),
      stopping_(false),
      reset_(true),
      book_(book_filepath),
      thor_(thor_filepath) {
    for (int i = 0; i < kNumEvaluators; ++i) {
      evaluator_derivative_[i] = std::make_unique<EvaluatorDerivative>(
          &tree_node_supplier_, &hash_map_,
          PatternEvaluator::Factory(evals_.data()),
          static_cast<u_int8_t>(i));
    }
    if (!book_.Get(Board())) {
      std::shared_ptr<TreeNode> t(new TreeNode());
      t->Reset(Board().Player(), Board().Opponent(), 0, 0, 0, 2, -63, 63);
      book_.Add(*t);
      book_.Commit();
    }
  }

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

  static jobject NodeToJava(Node* n, JNIEnv* env) {
    if (!n) {
      return NULL;
    }
    const Node& node = *n;
    const jclass NodeJava = env->FindClass("jni/Node");
    const jmethodID initNodeJava = env->GetMethodID(NodeJava, "<init>", "(JJJDFIIIIIILjava/util/ArrayList;)V");

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
      auto first_position = evaluator_derivative_[i]->GetFirstPosition();
      if (first_position) {
        visited += first_position->GetNVisited();
      }
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
      auto first_position = evaluator_derivative_[i]->GetFirstPosition();
      if (first_position) {
        visited += first_position->GetNVisited();
      }
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
      double max_time, float gap, int n_threads, bool approx) {
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
            max_time / boards.size(), n_threads, approx);
      }
    } else {
      NVisited visited = TotalVisited();
      int increments = 10;
      for (int step = 0; step < increments && !Finished(max_n_visited); ++step) {
        auto evaluator = BestEvaluator(gap);
        evaluator->ContinueEvaluate(
            (max_n_visited - visited) / increments,
            max_time / increments, n_threads);
      }
    }
  }

  jobject GetFirstPosition(JNIEnv* env) {
    auto board = evaluator_derivative_[0]->GetFirstPosition();
    return NodeToJava(board.get(), env);
  }

  jobject GetFromBook(JNIEnv* env, BitPattern player, BitPattern opponent) {
    auto board = book_.Get(player, opponent);
    return NodeToJava(board.get(), env);
  }

  jobject Get(JNIEnv* env, BitPattern player, BitPattern opponent) {
    Board unique = Board(player, opponent).Unique();
    for (int i = 0; i < last_boards_.size(); ++i) {
      const auto& evaluator = evaluator_derivative_[i];
      auto first_position = evaluator->GetFirstPosition();
      if (first_position && first_position->ToBoard().Unique() == unique) {
        return NodeToJava(first_position.get(), env);
      }
      for (Board b : unique.AllTranspositions()) {
        auto node = tree_node_supplier_.Get(b.Player(), b.Opponent(), evaluator->Index());
        if (node) {
          return NodeToJava(node.get(), env);
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
    if (!father_node || !father_node->IsLeaf()) {
      return;
    }
    std::vector<Node> children;
    children.reserve(last_boards_.size());
    NVisited n_visited = 0;
    for (int i = 0; i < last_boards_.size(); ++i) {
      auto child = evaluator_derivative_[i]->GetFirstPosition();
      assert(child);
      children.push_back(*child);
      n_visited += child->GetNVisited();
    }
    std::vector<Book<>::BookNode*> parents_node;

    for (auto parent : parents) {
      auto parent_in_book = book_.Mutable(parent);
      assert(parent_in_book);
      parents_node.push_back(parent_in_book);
    }
    auto leaf = LeafToUpdate<Book<>::BookNode>::Leaf(parents_node);
    book_.AddChildren(father, children);
    leaf.Finalize(n_visited);
    book_.Commit();
  }

  std::vector<std::string> GetThorPlayers() {
    return thor_.Players();
  }

  std::vector<std::string> GetThorTournaments() {
    return thor_.Tournaments();
  }

  void LookupThorPositions(
      std::unordered_set<std::string> black,
      std::unordered_set<std::string> white,
      std::unordered_set<std::string> tournaments) {
    thor_.LookupPositions(black, white, tournaments);
  }

  std::vector<Game> GetThorGames(const Board& b) {
    return thor_.GetGames(b);
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
  Thor thor_;
};

#ifdef __cplusplus
extern "C" {
#endif

JNIWrapper* JNIFromJava(JNIEnv* env, jobject obj) {
  jclass cls = env->GetObjectClass(obj);
  jmethodID mid = env->GetMethodID(cls, "getPointer", "()J");
  return (JNIWrapper*) env->CallLongMethod(obj, mid);
}

JNIEXPORT void JNICALL Java_jni_JNI_create(JNIEnv* env, jobject obj, jstring evalFile, jstring bookFolder, jstring thorFolder) {
  jclass jni = env->FindClass("jni/JNI");
  jmethodID setup = env->GetMethodID(jni, "setup", "(J)V");
  jboolean isCopy;
  const char* eval_file = (env)->GetStringUTFChars(evalFile, &isCopy);
  const char* book_folder = (env)->GetStringUTFChars(bookFolder, &isCopy);
  const char* thor_folder = (env)->GetStringUTFChars(thorFolder, &isCopy);
  env->CallVoidMethod(obj, setup, (jlong) new JNIWrapper(eval_file, book_folder, thor_folder));
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
    jint upper, jlong maxNVisited, jint maxTimeMillis, jfloat gap, jint n_threads, jboolean approx) {
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
      maxTimeMillis / 1000.0, gap, n_threads, approx);
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
Java_jni_JNI_prettyPrintDouble(JNIEnv* env, jclass clazz, jdouble d) {
  return env->NewStringUTF(PrettyPrintDouble(d).c_str());
}

JNIEXPORT jobject JNICALL
Java_jni_JNI_getThorPlayers(JNIEnv* env, jobject thiz) {
  return VectorToArrayList<jstring, std::string>(
      env,
      JNIFromJava(env, thiz)->GetThorPlayers(),
      [](JNIEnv* env, const std::string& s) {
        return env->NewStringUTF(s.c_str());
      });
}

JNIEXPORT jobject JNICALL
Java_jni_JNI_getThorTournaments(JNIEnv *env, jobject thiz) {
    return VectorToArrayList<jstring, std::string>(
      env,
      JNIFromJava(env, thiz)->GetThorTournaments(),
      [](JNIEnv* env, const std::string& s) {
        return env->NewStringUTF(s.c_str());
      });
}

JNIEXPORT jobject JNICALL
Java_jni_JNI_getThorGames(JNIEnv *env, jobject thiz, jobject b) {
  return VectorToArrayList<jobject, Game>(
      env,
      JNIFromJava(env, thiz)->GetThorGames(BoardToCPP(env, b)),
      &ThorGameToJava);
}

JNIEXPORT void JNICALL
Java_jni_JNI_lookupThorPositions(
    JNIEnv *env,
    jobject thiz,
    jobject black,
    jobject white,
    jobject tournaments) {
  auto conversion = [](JNIEnv *env, const jstring& s) {
    auto c_string = env->GetStringUTFChars(s, NULL);
    std::string result(c_string);
    env->ReleaseStringUTFChars(s, c_string);
    return result;
  };
  JNIFromJava(env, thiz)->LookupThorPositions(
      ListToSet<jstring, std::string>(env, black, conversion),
      ListToSet<jstring, std::string>(env, white, conversion),
      ListToSet<jstring, std::string>(env, tournaments, conversion));
}

#ifdef __cplusplus
}
#endif
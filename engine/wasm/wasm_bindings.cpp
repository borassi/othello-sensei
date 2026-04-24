/*
 * Copyright 2026 Michele Borassi
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
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "../main/ui.h" // Header for all the functions we'll use
#include <iostream>
#include <thread>    // Required for hardware_concurrency
#include <algorithm> // Required for std::max

using namespace emscripten;

// We need static storage for JS callbacks because C function pointers
// don't carry "context" (like JS 'this' or closures).
static val js_set_board = val::undefined();
static val js_update_annotations = val::undefined();

// These C-style functions match the typedefs in your header
// and forward the calls to the stored JavaScript functions.
void WasmSetBoard(struct BoardUpdate b) {
  // We pass the struct fields as arguments to the main thread.
  // player/opponent are BitPatterns (uint64_t), which JS receives as BigInt.
  MAIN_THREAD_EM_ASM({
                         if (typeof window.onSetBoard === 'function') {
                           window.onSetBoard({
                                                 player: $0,
                                                 opponent: $1,
                                                 black_turn: !!$2,
                                                 last_move: $3,
                                                 handle_game_over: !!$4,
                                                 start_annotations: $5 // Pointer address
                                             });
                         }
                     }, b.player, b.opponent, b.black_turn, b.last_move, b.handle_game_over, b.start_annotations);
}

// Helper function to map the struct to a JS object
emscripten::val AnnotationToJS(const struct Annotations* ann) {
  if (!ann) return emscripten::val::null();

  emscripten::val obj = emscripten::val::object();

  // Extract only the valid moves into a JS array
  emscripten::val moves_arr = emscripten::val::array();
  for (int i = 0; i < ann->num_moves; ++i) {
    moves_arr.set(i, ann->moves[i]);
  }

  obj.set("moves", moves_arr);
  obj.set("num_moves", ann->num_moves);
  obj.set("black_turn", ann->black_turn);
  obj.set("depth", ann->depth);
  obj.set("depth_no_pass", ann->depth_no_pass);
  obj.set("valid", ann->valid);
  obj.set("eval", ann->eval);
  obj.set("leaf_eval", ann->leaf_eval);
  obj.set("eval_best_line", ann->eval_best_line);
  obj.set("median_eval", ann->median_eval);
  obj.set("median_eval_best_line", ann->median_eval_best_line);
  obj.set("provenance", static_cast<int>(ann->provenance)); // Cast enum to int
  obj.set("derived", ann->derived);
  obj.set("prob_lower_eval", ann->prob_lower_eval);
  obj.set("prob_upper_eval", ann->prob_upper_eval);
  obj.set("proof_number_lower", ann->proof_number_lower);
  obj.set("disproof_number_upper", ann->disproof_number_upper);
  obj.set("lower", ann->lower);
  obj.set("upper", ann->upper);
  obj.set("weak_lower", ann->weak_lower);
  obj.set("weak_upper", ann->weak_upper);
  obj.set("seconds", ann->seconds);
  obj.set("descendants_evaluating_this", ann->descendants_evaluating_this);
  obj.set("descendants", ann->descendants);
  obj.set("descendants_book", ann->descendants_book);
  obj.set("missing", ann->missing);
  obj.set("num_thor_games", ann->num_thor_games);
  obj.set("thor_winning_percentage", ann->thor_winning_percentage);
  obj.set("during_analysis", ann->during_analysis);
  obj.set("modified", ann->modified);

  return obj;
}

// Return the evaluation of the current node as a Javascript objects.
emscripten::val GetCurrentEvaluation(uintptr_t ptr, int thread_id) {
  struct Annotations* ann = GetCurrentAnnotations(reinterpret_cast<void*>(ptr), thread_id);
  if (ann == nullptr || !ann->valid) {
    return emscripten::val::null();
  }
  return AnnotationToJS(ann);
}

// Return the children evaluations as Javascript objects.
emscripten::val GetChildrenEvaluations(uintptr_t ptr, int thread_id) {
  emscripten::val result = emscripten::val::array();

  struct Annotations* ann = GetCurrentAnnotations(reinterpret_cast<void*>(ptr), thread_id);
  if (ann == nullptr || !ann->valid) {
    return result;
  }

  int index = 0;
  for (struct Annotations* child = ann->first_child; child != nullptr; child = child->next_sibling) {
    result.set(index++, AnnotationToJS(child));
  }
  return result;
}

void WasmUpdateAnnotations(unsigned int thread_id, bool finished, int move) {
  MAIN_THREAD_EM_ASM({
                         if (typeof window.onUpdateAnnotations === 'function') {
                           window.onUpdateAnnotations($0, !!$1, $2);
                         }
                     }, thread_id, finished, move);
}

void WasmUpdateTimers(double t1, double t2) {
  MAIN_THREAD_EM_ASM({
                         if (typeof window.onUpdateTimers === 'function') {
                           window.onUpdateTimers($0, $1);
                         }
                     }, t1, t2);}

void WasmSendMessage(const char* const m) {}

// Helper for standard functions: void* -> uintptr_t
template<auto Func> struct wrap;
template<typename R, typename... Args, R(*func)(void*, Args...)>
struct wrap<func> {
  static R call(uintptr_t ptr, Args... args) {
    return func(reinterpret_cast<void*>(ptr), args...);
  }
};

// Helper for functions taking a struct pointer (like SetEvaluateParams)
template<auto Func> struct wrap_st;
template<typename R, typename S, typename... Args, R(*func)(void*, S*, Args...)>
struct wrap_st<func> {
  static R call(uintptr_t ptr, S s, Args... args) {
    return func(reinterpret_cast<void*>(ptr), &s, args...);
  }
};

EMSCRIPTEN_BINDINGS(othello_module) {
    // Register the complex structs so JS can read their properties
    value_object<BoardUpdate>("BoardUpdate")
        .field("player", &BoardUpdate::player)
        .field("opponent", &BoardUpdate::opponent)
        .field("black_turn", &BoardUpdate::black_turn)
        .field("last_move", &BoardUpdate::last_move);

    enum_<SenseiAction>("SenseiAction")
        .value("InvalidAction", SENSEI_INVALID_ACTION)
        .value("Inactive",      SENSEI_INACTIVE)
        .value("Evaluates",     SENSEI_EVALUATES)
        .value("PlaysBlack",    SENSEI_PLAYS_BLACK)
        .value("PlaysWhite",    SENSEI_PLAYS_WHITE)
        .value("PlaysBoth",     SENSEI_PLAYS_BOTH);

    value_object<ThorParams>("ThorParams")
        .field("max_games", &ThorParams::max_games)
        .field("start_year",  &ThorParams::start_year)
        .field("end_year",  &ThorParams::end_year);

    value_object<EvaluateParams>("EvaluateParams")
        .field("lower",                      &EvaluateParams::lower)
        .field("upper",                      &EvaluateParams::upper)
        .field("max_positions",              &EvaluateParams::max_positions)
        .field("max_time_first_eval",        &EvaluateParams::max_time_first_eval)
        .field("max_time_next_evals",        &EvaluateParams::max_time_next_evals)
        .field("max_time_analysis",          &EvaluateParams::max_time_analysis)
        .field("max_time_play",              &EvaluateParams::max_time_play)
        .field("error_play",                 &EvaluateParams::error_play)
        .field("max_error_move_play",        &EvaluateParams::max_error_move_play)
        .field("n_threads",                  &EvaluateParams::n_threads)
        .field("delta",                      &EvaluateParams::delta)
        .field("approx",                     &EvaluateParams::approx)
        .field("use_book",                   &EvaluateParams::use_book)
        .field("reevaluate_during_analysis", &EvaluateParams::reevaluate_during_analysis)
        .field("thor_filters",               &EvaluateParams::thor_filters)
        .field("sensei_action",              &EvaluateParams::sensei_action);

    // Bind the initialization logic
    function("mainInit", optional_override([](
      std::string evals, std::string book, std::string thor,
      std::string saved, std::string xot_s, std::string xot_l
    ) {

      void* main = MainInit(
          (char*)evals.c_str(), (char*)book.c_str(), (char*)thor.c_str(),
          (char*)saved.c_str(), (char*)xot_s.c_str(), (char*)xot_l.c_str(),
          WasmSetBoard, WasmUpdateAnnotations, WasmUpdateTimers, WasmSendMessage
      );

      return reinterpret_cast<uintptr_t>(main);
    }), allow_raw_pointers());
    // --- 3. Core Engine Actions ---
    function("newGame",             &wrap<NewGame>::call);
    function("playMove",            &wrap<PlayMove>::call);
    function("undo",                &wrap<Undo>::call);
    function("redo",                &wrap<Redo>::call);
    function("toLastImportantNode", &wrap<ToLastImportantNode>::call);
    function("evaluate",            &wrap<Evaluate>::call);
    function("analyze",             &wrap<Analyze>::call);
    function("stop",                &wrap<Stop>::call);
    function("resetAnalyzedGame",   &wrap<ResetAnalyzedGame>::call);

    // --- 4. Board State & XOT ---
    function("setBlackSquare", &wrap<SetBlackSquare>::call);
    function("setWhiteSquare", &wrap<SetWhiteSquare>::call);
    function("setEmptySquare", &wrap<SetEmptySquare>::call);
    function("invertTurn",     &wrap<InvertTurn>::call);
    function("randomXOT",      &wrap<RandomXOT>::call);
    function("isXot",          &wrap<IsXot>::call);
    function("setXOTState",    &wrap<SetXOTState>::call);
    function("getXOTState",    &wrap<GetXOTState>::call);

    // --- 5. Sequence & Navigation ---
    function("setCurrentMove", &wrap<SetCurrentMove>::call);

    // String functions still need manual lambdas to handle std::string <-> char*
    function("setSequence", optional_override([](uintptr_t ptr, std::string seq) {
      return SetSequence(reinterpret_cast<void*>(ptr), (char*)seq.c_str());
    }));
    function("pasteBoard", optional_override([](uintptr_t ptr, std::string seq) {
      return PasteBoard(reinterpret_cast<void*>(ptr), (char*)seq.c_str());
    }));
    function("getSequence", optional_override([](uintptr_t ptr) {
      char* res = GetSequence(reinterpret_cast<void*>(ptr));
      return res ? std::string(res) : std::string("");
    }));

    // --- 6. Thor & File IO ---
    function("open", optional_override([](uintptr_t ptr, std::string path) {
      Open(reinterpret_cast<void*>(ptr), (char*)path.c_str());
    }));

    function("setFileSources", optional_override([](uintptr_t ptr, val folders) {
      auto folderStrings = vecFromJSArray<std::string>(folders);
      std::vector<char*> cStrings;
      for (auto& s : folderStrings) cStrings.push_back((char*)s.c_str());
      SetFileSources(reinterpret_cast<void*>(ptr), cStrings.size(), cStrings.data());
    }));

    // --- 7. Settings ---
    function("setCountingTime",      &wrap<SetCountingTime>::call);
    function("setEvaluateParams",    &wrap_st<SetEvaluateParams>::call);

    // --- 8. Metadata & Annotations ---
    function("getCurrentEvaluation", optional_override([](uintptr_t ptr, int thread_id) {
      return GetCurrentEvaluation(ptr, thread_id);
    }));
    function("getChildrenEvaluations", optional_override([](uintptr_t ptr, int thread_id) {
      return GetChildrenEvaluations(ptr, thread_id);
    }));
    function("getCurrentAnnotations", &wrap<GetCurrentAnnotations>::call, allow_raw_pointers());
    function("getStartAnnotations",   &wrap<GetStartAnnotations>::call,   allow_raw_pointers());
    function("getGameToSave",         &wrap<GetGameToSave>::call,         allow_raw_pointers());
    function("mutableGameMetadata",   &wrap<MutableGameMetadata>::call,   allow_raw_pointers());
    function("getThorMetadata",       &wrap<GetThorMetadata>::call,       allow_raw_pointers());
}
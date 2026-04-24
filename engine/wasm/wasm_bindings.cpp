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

// Define your default parameters globally or statically
static constexpr EvaluateParams kDefaultEvaluateParams = {
    -63,
    63,
    10000000,
    0.1,
    1.0,
    1.0,
    1.0,
    20.0,
    6.0,
    1,
    6,
    false,
    true,
    false,
    {
      100,
      1900,
      3000
    },
    SenseiAction::SENSEI_EVALUATES
};

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
// Updated function to return the current annotation and its children
emscripten::val GetEvaluations(uintptr_t ptr, int thread_id) {
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

void WasmUpdateTimers(double t1, double t2) {}

void WasmSendMessage(const char* const m) {}

EMSCRIPTEN_BINDINGS(othello_module) {
    // Register the complex structs so JS can read their properties
    value_object<BoardUpdate>("BoardUpdate")
        .field("player", &BoardUpdate::player)
        .field("opponent", &BoardUpdate::opponent)
        .field("black_turn", &BoardUpdate::black_turn)
        .field("last_move", &BoardUpdate::last_move);

    // Bind the initialization logic
    function("mainInit", optional_override([](
      std::string evals, std::string book, std::string thor,
      std::string saved, std::string xot_s, std::string xot_l,
      val set_board, val update_annotations
    ) {
      // Store JS functions globally so the C-callbacks can find them
      js_set_board = set_board;
      js_update_annotations = update_annotations;

      void* main = MainInit(
          (char*)evals.c_str(), (char*)book.c_str(), (char*)thor.c_str(),
          (char*)saved.c_str(), (char*)xot_s.c_str(), (char*)xot_l.c_str(),
          WasmSetBoard, WasmUpdateAnnotations, WasmUpdateTimers, WasmSendMessage
      );

      EvaluateParams params = kDefaultEvaluateParams;
      params.n_threads = static_cast<int>(std::thread::hardware_concurrency());
      std::cout << "Detected threads: " << params.n_threads << std::endl << std::flush;
      SetEvaluateParams(main, &params);

      return reinterpret_cast<uintptr_t>(main);
    }), allow_raw_pointers());

    // Wrap the existing C functions to bypass the "void*" Embind limitation
    function("playMove", optional_override([](uintptr_t ptr, int square, bool automatic) -> bool {
      return PlayMove(reinterpret_cast<void*>(ptr), square, automatic);
    }));

    function("undo", optional_override([](uintptr_t ptr) {
      Undo(reinterpret_cast<void*>(ptr));
    }));

    function("evaluate", optional_override([](uintptr_t ptr) {
      Evaluate(reinterpret_cast<void*>(ptr));
    }));

    function("stop", optional_override([](uintptr_t ptr) {
      Stop(reinterpret_cast<void*>(ptr));
    }));

    function("mainDelete", optional_override([](uintptr_t ptr) {
      MainDelete(reinterpret_cast<void*>(ptr));
    }));

    function("getEvaluations", optional_override([](uintptr_t ptr, int thread_id) {
      return GetEvaluations(ptr, thread_id);
    }));

    function("newGame", optional_override([](uintptr_t ptr) {
      NewGame(reinterpret_cast<void*>(ptr));
    }));

    function("toLastImportantNode", optional_override([](uintptr_t ptr) {
      ToLastImportantNode(reinterpret_cast<void*>(ptr));
    }));

    function("redo", optional_override([](uintptr_t ptr) {
      Redo(reinterpret_cast<void*>(ptr));
    }));
}
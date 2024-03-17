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
#include "stdbool.h"

#include "../utils/types.h"

#ifndef OTHELLO_SENSEI_BINDINGS_H
#define OTHELLO_SENSEI_BINDINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
   #define EXPORT __declspec(dllexport)
#else
   #define EXPORT __attribute__((visibility("default"))) __attribute__((used))
#endif

struct ThorSourceMetadata {
  const char* name;
  const char** players;
  int num_players;
  const char** tournaments;
  int num_tournaments;
  int* selected_blacks;
  int* selected_whites;
  int* selected_tournaments;
  bool active;
};

struct ThorMetadata {
  struct ThorSourceMetadata** sources;
  int num_sources;
};

struct BoardUpdate {
  BitPattern player;
  BitPattern opponent;
  bool black_turn;
};

struct ThorGame {
  const char* black;
  const char* white;
  const char* tournament;
  Square moves[60];
  int moves_played;
  int score;
  int year;
};

enum AnnotationsProvenance {
  EVALUATE,
  BOOK,
  CHILD_EVALUATE,
  CHILD_BOOK,
  GAME_OVER,
};

struct Annotations {
  // Filled when defining the annotation.
  Square move;
  bool black_turn;
  int depth;
  struct Annotations* father;
  struct Annotations* first_child;
  struct Annotations* next_sibling;
  struct Annotations* next_state_played;
  struct Annotations* next_state_in_analysis;
  bool valid;
  // Filled when evaluating this position.
  double eval;
  double leaf_eval;
  int median_eval;
  enum AnnotationsProvenance provenance;
  bool derived;  // If true, ignore it for time and descendants.
  double seconds;
  double prob_lower_eval;
  double prob_upper_eval;
  double proof_number_lower;
  double disproof_number_upper;
  Eval lower;
  Eval upper;
  Eval weak_lower;
  Eval weak_upper;
  NVisited descendants;
  NVisited descendants_no_book;
  double missing;
  bool finished;
  bool analyzed;
  // Filled when reading Thor.
  unsigned int num_thor_games;
  struct ThorGame* example_thor_games;
  unsigned int num_example_thor_games;
  double thor_winning_percentage;
};

struct ThorParams {
  int max_games;
  int start_year;
  int end_year;
};

struct EvaluateParams {
  int lower;
  int upper;
  NVisited max_positions;
  double max_time_first_eval;
  double max_time_next_evals;
  double max_time_analysis;
  int n_threads;
  double delta;
  bool approx;
  bool use_book;
  struct ThorParams thor_filters;
};

typedef void (*SetBoard)(struct BoardUpdate);
typedef void (*UpdateAnnotations)(int, bool);

#ifdef __cplusplus
}
#endif
#endif // OTHELLO_SENSEI_BINDINGS_H

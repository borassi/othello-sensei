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

#include "hash_map.h"


std::ostream& operator<<(std::ostream& stream, const HashMapEntry& e) {
  stream << "board: " << e.player << " " << e.opponent
         << "  eval: [" << (int) e.lower << " " << (int) e.upper << "] @ "
         << e.depth << "  best_moves: " << e.best_move << ", "
         << e.second_best_move;
  return stream;
}
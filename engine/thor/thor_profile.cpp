/*
 * Copyright 2024 Michele Borassi
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

#include <string>

#include "thor.h"
#include "../utils/parse_flags.h"

// Usage:
// $ cmake -S engine -B build -DANDROID=FALSE -DCMAKE_BUILD_TYPE=RelWithDebInfo \
// -DLIBRARY_OUTPUT_DIRECTORY=build && \
// cmake --build build --parallel=12 --target=thor_profile && \
// ./build/thor/thor_profile --path=assets/archive
int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string path = parse_flags.GetFlag("path");
  ElapsedTime t;
  Thor thor(path);
  std::cout << "Thor loaded in " << t.Get() << " sec.\n";
  Sequence sequence("");
  int tmp = 0;
  for (int i = 0; i < 1000; ++i) {
    tmp ^= thor.GetGames("Thor", sequence, 100).num_games;
    tmp ^= thor.GetGames("PlayOK", sequence, 100).num_games;
    tmp ^= thor.GetGames("OthelloQuest", sequence, 100).num_games;
  }
  std::cout << "Fetch time: " << t.Get() / 1000 << "\n";
  std::cout << "To make sure that the compiler doesn't optimize: " << tmp << "\n";
}
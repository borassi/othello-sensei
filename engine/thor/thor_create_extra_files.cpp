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
// cmake --build build --parallel=12 --target=thor_create_extra_files && \
// ./build/thor/thor_create_extra_files --path=assets/archive
int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string path = parse_flags.GetFlag("path");
  bool rebuild_canonicalizer = parse_flags.GetBoolFlagOrDefault("rebuild_canonicalizer", true);
  bool rebuild_games_order = parse_flags.GetBoolFlagOrDefault("rebuild_games_order", true);
  bool rebuild_games_small_hash = parse_flags.GetBoolFlagOrDefault("rebuild_games_small_hash", true);
  Thor thor(path, rebuild_canonicalizer, rebuild_games_order, rebuild_games_small_hash);
  thor.SaveAll();
}
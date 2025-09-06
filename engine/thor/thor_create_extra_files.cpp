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

#include "fix_xot_source.h"
#include "thor.h"
#include "../utils/parse_flags.h"
#include "../xot/xot.h"

// Usage:
// $ cmake -S engine -B build -DANDROID=FALSE -DCMAKE_BUILD_TYPE=RelWithDebInfo \
// -DLIBRARY_OUTPUT_DIRECTORY=build && \
// cmake --build build --parallel=12 --target=thor_create_extra_files && \
// ./build/thor/thor_create_extra_files --path=assets/archive
int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string input_path = parse_flags.GetFlag("input_path");
  std::string output_path = parse_flags.GetFlagOrDefault("output_path", "");
  std::string xot_path = parse_flags.GetFlagOrDefault("xot_path", "");
  bool rebuild_canonicalizer = parse_flags.GetBoolFlagOrDefault("rebuild_canonicalizer", true);
  bool rebuild_games_order = parse_flags.GetBoolFlagOrDefault("rebuild_games_order", true);
  bool rebuild_games_small_hash = parse_flags.GetBoolFlagOrDefault("rebuild_games_small_hash", true);

  if (output_path != "") {
    fs::create_directories(fs::path(output_path).remove_filename());
  }

  if (xot_path.empty()) {
    if (output_path != "") {
      std::cout << "No XOT path, copying...\n";
      fs::copy(input_path, output_path, fs::copy_options::recursive);
    } else {
      std::cout << "No output path, adding files to the input path...\n";
      output_path = input_path;
    }
  } else {
    if (output_path.empty()) {
      return 1;
    }
    std::cout << "XOT path, fixing...\n";
    XOT xot(LoadTextFile(xot_path));
    for (const auto& entry : GetAllFiles(input_path, /*include_files=*/false, /*include_directories=*/true)) {
      std::cout << "  Starting " << entry << "\n";
      auto result = FixXOTSource(xot, entry, (fs::path(output_path) / fs::path(entry).filename()).string());
      std::cout << "    " << result << "\n";
    }
  }

  Thor<GameGetterInMemory> thor(output_path, "/tmp/saved_files.txt", rebuild_canonicalizer, rebuild_games_order, rebuild_games_small_hash);
  thor.SaveAll();
}
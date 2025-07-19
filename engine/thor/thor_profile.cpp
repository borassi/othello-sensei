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

template<class GameGetter>
void ProfileThor(const std::string& path) {
  int N = 100;
  std::vector<Sequence> sequences;
  for (int i = 0; i < N; ++i) {
    sequences.push_back(Sequence::RandomSequence(rand() % 10));
  }

  ElapsedTime load_time;
  Thor<GameGetterOnDisk> thor(path, path + "/saved_files.txt");
  std::cout << "Thor loaded in " << load_time.Get() << " sec.\n";
  int tmp = 0;
  ElapsedTime fetch_time;
  for (int i = 0; i < N; ++i) {
    for (auto& source : {"Thor", "PlayOK", "OthelloQuest"}) {
      GamesList result = thor.GetGames(source, sequences[i], 100);
      tmp ^= result.examples.size() ^ result.next_moves.size() ^ result.num_games;
    }
  }
  std::cout << "Fetch time: " << fetch_time.Get() / N << "\n";
  std::cout << "To make sure that the compiler doesn't optimize: " << tmp << "\n";
}

// Usage:
// $ cmake -S engine -B build -DANDROID=FALSE -DCMAKE_BUILD_TYPE=RelWithDebInfo \
// -DLIBRARY_OUTPUT_DIRECTORY=build && \
// cmake --build build --parallel=12 --target=thor_profile && \
// ./build/thor/thor_profile --path=assets/archive
int main(int argc, char* argv[]) {
  ParseFlags parse_flags(argc, argv);
  std::string path = parse_flags.GetFlag("path");
  std::cout << "\nPROFILE ON DISK\n";
  ProfileThor<GameGetterOnDisk>(path);
  std::cout << "\nPROFILE IN MEMORY\n";
  ProfileThor<GameGetterInMemory>(path);
}
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
 
#ifndef OTHELLO_SENSEI_THOR_H
#define OTHELLO_SENSEI_THOR_H

#include <future>
#include <string>

#include "source.h"
#include "../savegame/saved_game_list.h"
#include "../utils/files.h"

template<class GameGetter = GameGetterOnDisk>
class Thor {
 public:
  static constexpr std::string_view kSavedGamesListFilename = "saved_games_folders.txt";
  static constexpr int kMaxNumFolders = 20;

  Thor(
      const std::string& folder,
      bool rebuild_canonicalizer = false,
      bool rebuild_games_order = false,
      bool rebuild_games_small_hash = false)
      : folder_(folder), sources_() {
    std::vector<std::future<std::pair<std::string, std::unique_ptr<GenericSource>>>> sources_futures;

    for (const auto& entry : GetAllFiles(folder, /*include_files=*/true, /*include_directories=*/true)) {
      if (fs::is_directory(entry)) {
        sources_futures.push_back(
            std::move(std::async(std::launch::async, &Thor::LoadSource, this, entry, rebuild_games_order,
                       rebuild_games_small_hash)));
      } else if (fs::path(entry).filename() == kSavedGamesListFilename) {
        for (const std::string& saved_game_folder : Split(LoadTextFile(entry), '\n')) {
          sources_futures.push_back(
              std::move(std::async(
                  std::launch::async, &Thor::LoadSavedGameList, this, saved_game_folder)));
        }
      }
    }
    std::unique_ptr<std::future<void>> load_canonicalizer_future = nullptr;
    if (!rebuild_canonicalizer && FileExists(CanonicalizerPath())) {
      load_canonicalizer_future = std::make_unique<std::future<void>>(
          std::async(std::launch::async, &Thor::LoadCanonicalizer, this));
    }

    for (auto& future : sources_futures) {
      auto tmp = std::move(future);
      auto pair = tmp.get();
      if (pair.second != nullptr) {
        sources_.insert(std::move(pair));
      }
    }
    if (load_canonicalizer_future != nullptr) {
      load_canonicalizer_future.get();
    } else {
      ComputeCanonicalizer();
    }
  }

  bool AddFileSource(const std::string& folder) {
    auto value = LoadSavedGameList(folder);
    if (value.second == nullptr) {
      return false;
    }
    std::fstream saved_games_list(folder_ + "/" + std::string(kSavedGamesListFilename));
    saved_games_list << folder << "\n";
    sources_.insert(std::move(value));
    return true;
  }

  std::pair<std::string, std::unique_ptr<GenericSource>> LoadSavedGameList(
      const std::string& path) {
    if (!fs::exists(path) || !fs::is_directory(path)) {
      return {"", nullptr};
    }
    std::unique_ptr<GenericSource> result(new SavedGameList(path));
    if (result->NumGames() == 0) {
      return {"", nullptr};
    }
    std::string name = fs::path(path).filename().string().substr(0, 15);
    int i = 1;
    while (Contains(sources_, name)) {
      name += "_" + std::to_string(i++);
    }
    return {name, std::move(result)};
  }

  std::pair<std::string, std::unique_ptr<GenericSource>> LoadSource(
      const std::string& source, bool rebuild_games_order, bool rebuild_games_small_hash) {
    std::unique_ptr<GenericSource> result(new Source<GameGetter>(source, rebuild_games_order, rebuild_games_small_hash));
    if (result->NumGames() == 0) {
      return {"", nullptr};
    }
    return {fs::path(source).filename(), std::move(result)};
  }

  std::vector<std::string> Sources() const {
    std::vector<std::string> result;
    result.reserve(sources_.size());
    for (const auto& [name, _] : sources_) {
      result.push_back(name);
    }
    return result;
  }

  std::unordered_map<std::string, const std::vector<std::string>&> Players() const {
    std::unordered_map<std::string, const std::vector<std::string>&> result;
    for (const auto& [name, source] : sources_) {
      result.insert({name, source->Players()});
    }
    return result;
  }

  std::unordered_map<std::string, const std::vector<std::string>&> Tournaments() const {
    std::unordered_map<std::string, const std::vector<std::string>&> result;
    for (const auto& [name, source] : sources_) {
      result.insert({name, source->Tournaments()});
    }
    return result;
  }

  std::vector<Game> GetAllGames() const {
    std::vector<Game> result;
    for (const auto& [_, source] : sources_) {
      std::vector<Game> games = source->AllGames();
      result.insert(result.end(), games.begin(), games.end());
    }
    return result;
  }

  template<bool transpositions = true>
  GamesList GetGamesFromAllSources(
      const Sequence& sequence,
      int max_games = 1,
      std::vector<std::string> blacks = {},
      std::vector<std::string> whites = {},
      std::vector<std::string> tournaments = {},
      short start_year = SHRT_MIN,
      short end_year = SHRT_MAX) const {
    GamesList games {.max_games = max_games};
    for (const auto& [source_name, _] : sources_) {
      GamesList new_games = GetGames<transpositions>(
          source_name, sequence, max_games, blacks, whites, tournaments, start_year, end_year);
      games.Merge(new_games);
    }
    return games;
  }

  template<bool transpositions = true>
  GamesList GetGames(
      const std::string& source,
      const Sequence& sequence,
      int max_games = 1,
      const std::vector<std::string>& blacks = {},
      const std::vector<std::string>& whites = {},
      const std::vector<std::string>& tournaments = {},
      short start_year = SHRT_MIN,
      short end_year = SHRT_MAX) const {
    assert(max_games > 0);  // Otherwise the Rotate() does not know how to rotate.
    const Sequence& canonical = sequence.ToCanonicalGame();
    GamesList games;
    games.max_games = max_games;

    std::vector<Sequence> lookup_sequences =
        transpositions ? canonicalizer_.AllEquivalentSequences(canonical)
        : std::vector<Sequence>({canonical});
    for (const Sequence& lookup_sequence : lookup_sequences) {
      GamesList new_games = sources_.at(source)->GetGames(
          lookup_sequence, max_games, blacks, whites, tournaments, start_year, end_year);
      new_games.Rotate(sequence);
      games.Merge(new_games);
    }
    return games;
  }

  std::string CanonicalizerPath() {
    return folder_ + "/sequence_canonicalizer.bin";
  }

  void SaveCanonicalizer() {
    std::ofstream file(CanonicalizerPath(), std::ios::binary);
    std::vector<char> serialized = canonicalizer_.Serialize();
    file.write((char*) serialized.data(),
               static_cast<int>(serialized.size()) * sizeof(Square));
  }

  void SaveAll() {
    SaveCanonicalizer();
    for (const auto& [_, source] : sources_) {
      source->Save();
    }
  }

 private:
  std::string folder_;
  std::unordered_map<std::string, std::unique_ptr<GenericSource>> sources_;
  SequenceCanonicalizer canonicalizer_;

  void LoadCanonicalizer() {
    canonicalizer_.Load(ReadFile<char>(CanonicalizerPath()));
  }

  void ComputeCanonicalizer() {
    ElapsedTime t;
    std::vector<Sequence> all_sequences;

    for (const auto& [_, source] : sources_) {
      for (const Game& game : source->AllGames()) {
        all_sequences.push_back(game.Moves());
      }
    }
    canonicalizer_.AddAll(all_sequences);
  }

};

#endif //OTHELLO_SENSEI_THOR_H

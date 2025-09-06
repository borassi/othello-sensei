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
  typedef std::vector<std::future<std::pair<std::string, std::unique_ptr<GenericSource>>>> FutureVector;

  Thor(
      const std::string& folder,
      const std::string& saved_files_path,
      bool rebuild_canonicalizer = false,
      bool rebuild_games_order = false,
      bool rebuild_games_small_hash = false)
      : folder_(folder), saved_files_path_(saved_files_path), sources_() {
    FutureVector sources_futures;
    for (const auto& entry : GetAllFiles(folder, /*include_files=*/true, /*include_directories=*/true)) {
      if (fs::is_directory(entry)) {
        sources_futures.push_back(
            std::move(std::async(std::launch::async, &Thor::LoadSource, this, entry, rebuild_games_order,
                       rebuild_games_small_hash)));
      }
    }
    FutureVector saved_games_futures = LoadAllSavedGameLists();
    std::unique_ptr<std::future<void>> load_canonicalizer_future = nullptr;
    if (!rebuild_canonicalizer && FileExists(CanonicalizerPath())) {
      load_canonicalizer_future = std::make_unique<std::future<void>>(
          std::async(std::launch::async, &Thor::LoadCanonicalizer, this));
    }
    ParseGameListFutures(sources_futures);
    ParseGameListFutures(saved_games_futures);

    if (load_canonicalizer_future != nullptr) {
      try {
        (void) load_canonicalizer_future.get();  // Silence a warning.
      } catch (const std::exception& e) {
        std::cerr << "Caught exception when loading canonicalizer: " << e.what() << std::endl;
        throw; // Re-raises the original exception
      }
    } else {
      ComputeCanonicalizer();
    }
  }

  bool ReloadSource(const std::string& file) {
    std::string file_canonical = fs::weakly_canonical(file).string();
    bool reloaded = false;
    for (auto& [source_name, source] : sources_) {
      if (source->GetType() != SOURCE_TYPE_SAVED_GAMES) {
        continue;
      }
      std::string source_canonical = fs::weakly_canonical(source->GetFolder()).string();
      if (source_canonical.back() != fs::path::preferred_separator) {
        source_canonical += fs::path::preferred_separator;
      }
      if (file_canonical.rfind(source_canonical, 0) != 0) {
        continue;
      }
      source = std::unique_ptr<GenericSource>(new SavedGameList(source->GetFolder()));
      reloaded = true;
    }
    return reloaded;
  }

  void ParseGameListFutures(FutureVector& futures) {
    for (auto& future : futures) {
      auto pair = std::move(future).get();
      int i = 1;
      std::string name = pair.first;
      while (Contains(sources_, name)) {
        name = pair.first + "_" + std::to_string(i++);
      }
      pair.first = name;
      sources_.insert(std::move(pair));
      sources_order_.push_back(name);
    }
  }

  void SetFileSources(const std::vector<std::string>& sources) {
    std::ofstream saved_games_list(saved_files_path_);
    for (const std::string& source : sources) {
      saved_games_list << source << "\n";
    }
    saved_games_list.close();
    std::unordered_map<std::string, std::unique_ptr<GenericSource>> new_sources;
    std::vector<std::string> new_sources_order;
    for (const std::string& source_name : sources_order_) {
      auto& source = sources_[source_name];
      if (source->GetType() != SOURCE_TYPE_SAVED_GAMES) {
        new_sources.insert({source_name, std::move(source)});
        new_sources_order.push_back(source_name);
      }
    }
    sources_ = std::move(new_sources);
    sources_order_ = std::move(new_sources_order);
    FutureVector futures = LoadAllSavedGameLists();
    ParseGameListFutures(futures);
  }

  FutureVector LoadAllSavedGameLists() {
    FutureVector futures;
    for (const std::string& saved_game_folder : Split(LoadTextFile(saved_files_path_), '\n')) {
      if (saved_game_folder.empty()) {
        continue;
      }
      futures.push_back(
          std::move(std::async(
              std::launch::async, &Thor::LoadSavedGameList, this, saved_game_folder)));
    }
    return futures;
  }

  std::pair<std::string, std::unique_ptr<GenericSource>> LoadSavedGameList(
      const std::string& path) {
    std::unique_ptr<GenericSource> result(new SavedGameList(path));
    std::string name = fs::path(path).filename().string().substr(0, 15);
    return {name, std::move(result)};
  }

  std::pair<std::string, std::unique_ptr<GenericSource>> LoadSource(
      const std::string& source, bool rebuild_games_order, bool rebuild_games_small_hash) {
    std::unique_ptr<GenericSource> result(new Source<GameGetter>(source, rebuild_games_order, rebuild_games_small_hash));
    return {fs::path(source).filename().string(), std::move(result)};
  }

  std::vector<std::string> Sources() const {
    return sources_order_;
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
    GamesList games;
    games.num_games = 0;
    games.max_games = max_games;
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
    auto source_element = sources_.find(source);
    if (source_element == sources_.end()) {
      return games;
    }

    std::vector<Sequence> lookup_sequences =
        transpositions ? canonicalizer_.AllEquivalentSequences(canonical)
        : std::vector<Sequence>({canonical});
    for (const Sequence& lookup_sequence : lookup_sequences) {
      GamesList new_games = source_element->second->GetGames(
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
               (int) (serialized.size() * sizeof(Square)));
  }

  void SaveAll() {
    SaveCanonicalizer();
    for (const auto& [_, source] : sources_) {
      source->Save();
    }
  }

  const GenericSource& GetSource(const std::string& name) const {
    return *sources_.at(name);
  }

 private:
  std::string folder_;
  std::string saved_files_path_;
  std::vector<std::string> sources_order_;
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

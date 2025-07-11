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
#include "../utils/files.h"

template<class GameGetter = GameGetterOnDisk>
class Thor {
 public:
  Thor(const std::string& folder, bool rebuild_canonicalizer = false, bool rebuild_games_order = false, bool rebuild_games_small_hash = false)
      : folder_(folder), sources_() {
    std::vector<std::future<std::pair<std::string, std::unique_ptr<Source<GameGetter>>>>> sources_futures;

    for (const auto& entry : GetAllFiles(folder, /*include_files=*/false, /*include_directories=*/true)) {
      sources_futures.push_back(std::async(std::launch::async, &Thor::LoadSource, this, entry, rebuild_games_order, rebuild_games_small_hash));
    }
    std::unique_ptr<std::future<void>> load_canonicalizer_future = nullptr;
    if (!rebuild_canonicalizer && FileExists(CanonicalizerPath())) {
      load_canonicalizer_future = std::make_unique<std::future<void>>(
          std::async(std::launch::async, &Thor::LoadCanonicalizer, this));
    }

    for (auto& future : sources_futures) {
      sources_.insert(std::move(future.get()));
    }
    if (load_canonicalizer_future != nullptr) {
      load_canonicalizer_future.get();
    } else {
      ComputeCanonicalizer();
    }
  }

  std::pair<std::string, std::unique_ptr<Source<GameGetter>>> LoadSource(
      const std::string& source, bool rebuild_games_order, bool rebuild_games_small_hash) {
    return {Filename(source), std::make_unique<Source<GameGetter>>(source, rebuild_games_order, rebuild_games_small_hash)};
  }

  std::vector<std::string> Sources() const {
    std::vector<std::string> result;
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
    GamesList games;
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
      std::vector<std::string> blacks = {},
      std::vector<std::string> whites = {},
      std::vector<std::string> tournaments = {},
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
    file.write((char*) serialized.data(), serialized.size() * sizeof(Square));
  }

  void SaveAll() {
    SaveCanonicalizer();
    for (const auto& [_, source] : sources_) {
      source->SaveSortedGames();
      source->SaveGamesSmallHash();
    }
  }

 private:
  std::string folder_;
  std::unordered_map<std::string, std::unique_ptr<Source<GameGetter>>> sources_;
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

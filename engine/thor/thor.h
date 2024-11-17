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

#include <string>

#include "source.h"
#include "../utils/files.h"

class Thor {
 public:
  Thor(const std::string& folder, bool rebuild_canonicalizer = false, bool rebuild_games_order = false)
      : folder_(folder), sources_() {
    for (const auto& entry : GetAllFiles(folder, /*include_files=*/false, /*include_directories=*/true)) {
      sources_.insert({Filename(entry), std::make_unique<Source>(entry, rebuild_games_order)});
    }
    if (!rebuild_canonicalizer && FileExists(CanonicalizerPath())) {
      LoadCanonicalizer();
    } else {
      ComputeCanonicalizer();
    }
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

  GamesList GetGames(
      const std::string& source,
      const Sequence& sequence,
      int max_games = INT_MAX,
      std::vector<std::string> blacks = {},
      std::vector<std::string> whites = {},
      std::vector<std::string> tournaments = {},
      short start_year = SHRT_MIN,
      short end_year = SHRT_MAX) {
    const Sequence& canonical = sequence.ToCanonicalGame();
    GamesList games;
    games.max_games = max_games;

    for (const Sequence& equivalent : canonicalizer_.AllEquivalentSequences(canonical)) {
      GamesList new_games = sources_[source]->GetGames(equivalent, max_games, blacks, whites, tournaments, start_year, end_year);
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
    }
  }

 private:
  std::string folder_;
  std::unordered_map<std::string, std::unique_ptr<Source>> sources_;
  SequenceCanonicalizer canonicalizer_;

  void LoadCanonicalizer() {
    std::ifstream file(CanonicalizerPath(), std::ios::binary);
    canonicalizer_.Load(ReadFile<char>(CanonicalizerPath()));
  }

  virtual void ComputeCanonicalizer() {
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

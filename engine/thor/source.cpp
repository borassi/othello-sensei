/*
 * Copyright 2024-2025 Michele Borassi
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

#include "source.h"

template<class GameGetter>
GamesList Source<GameGetter>::GetGames(
    const Sequence& sequence,
    int max_games,
    const std::vector<std::string>& blacks,
    const std::vector<std::string>& whites,
    const std::vector<std::string>& tournaments,
    short start_year,
    short end_year) const {
  assert(max_games > 0);
  start_year = std::max(start_year, game_getter_.MinYear());
  end_year = std::min(end_year, game_getter_.MaxYear());
  std::vector<GamesInterval> intervals;
  bool must_iterate_over_games = true;

  GamesInterval games_interval;
  if (!blacks.empty()) {
    for (const std::string& black : blacks) {
      games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
          game_index_by_black_, {black, sequence}, CmpByBlack<GameGetter>(game_getter_));
      intervals.push_back(games_interval);
    }
  } else if (!whites.empty()) {
    for (const std::string& white : whites) {
      games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
          game_index_by_white_, {white, sequence}, CmpByWhite<GameGetter>(game_getter_));
      intervals.push_back(games_interval);
    }
  } else if (!tournaments.empty()) {
    for (const std::string& tournament : tournaments) {
      games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
          game_index_by_tournament_, {tournament, sequence}, CmpByTournament<GameGetter>(game_getter_));
      intervals.push_back(games_interval);
    }
  } else {
    for (int year = end_year; year >= start_year; --year) {
      games_interval = GetGameIntervals<std::pair<int, Sequence>>(
          game_index_by_year_, {year, sequence}, CmpByYear<GameGetter>(game_getter_));
      intervals.push_back(games_interval);
    }
    must_iterate_over_games = false;
  }
  GamesList result;
  result.max_games = max_games;
  int sequence_size = sequence.Size();
  if (must_iterate_over_games) {
    for (const GamesInterval interval : intervals) {
      for (auto it = interval.start; it != interval.end; ++it) {
        Game game = game_getter_.GetGame(*it);
        if ((blacks.empty() || Contains(blacks, game.Black())) &&
            (whites.empty() || Contains(whites, game.White())) &&
            (tournaments.empty() || Contains(tournaments, game.Tournament())) &&
            game.Year() >= start_year && game.Year() <= end_year) {
          result.next_moves[game.Move(sequence_size)]++;
          result.examples.push_back(game);
        }
      }
    }
    result.num_games = (int) result.examples.size();
  } else {
    for (const GamesInterval& interval : intervals) {
      int games_in_interval = (int) (interval.end - interval.start);
      result.num_games += games_in_interval;
      FillNextMoves(interval, sequence_size, result.next_moves);

      int games_to_add = (int) (max_games - result.examples.size());
      if (games_to_add <= 0 || games_in_interval == 0) {
        continue;
      }
      double include_probability = std::min(1.0, games_to_add / (double) games_in_interval);
      int hash_index = std::min((int) games_with_small_hash_.size() - 1, IncludeProbabilityToHashIndex(1-include_probability));
      assert(hash_index < (int) games_with_small_hash_.size());
      for (; hash_index >= 0; --hash_index) {
        auto& games_with_small_hash = games_with_small_hash_[hash_index];
        auto start = std::lower_bound(games_with_small_hash.begin(), games_with_small_hash.end(), interval.start - game_index_by_year_.begin());
        auto end = std::lower_bound(games_with_small_hash.begin(), games_with_small_hash.end(), interval.end - game_index_by_year_.begin());
        if (end - start < games_to_add) {
          continue;
        }
        for (auto it = start; it < end; ++it) {
          result.examples.push_back(game_getter_.GetGame(game_index_by_year_[*it]));
        }
        break;
      }
      if (hash_index < 0) {
        for (auto it = interval.start; it < interval.end; ++it) {
          result.examples.push_back(game_getter_.GetGame(*it));
        }
      }
    }
  }
  SortAndCutExamples(max_games, result.examples);
  return result;
}

template class Source<GameGetterOnDisk>;
template class Source<GameGetterInMemory>;
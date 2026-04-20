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
    short end_year,
    std::vector<bool> xot_values,
    std::vector<bool> bot_values) const {
  assert(!xot_values.empty());
  assert(!bot_values.empty());
  assert(xot_values.size() == std::set<bool>(xot_values.begin(), xot_values.end()).size());
  assert(bot_values.size() == std::set<bool>(bot_values.begin(), bot_values.end()).size());
  assert(max_games > 0);
  start_year = std::max(start_year, game_getter_.MinYear());
  end_year = std::min(end_year, game_getter_.MaxYear());
  std::vector<GamesIntervalSequenceSorted> intervals;
  bool must_iterate_over_games = true;

  GamesIntervalSequenceSorted games_interval;
  if (!blacks.empty()) {
    for (const std::string& black : blacks) {
      games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
          game_index_by_black_, {black, sequence}, CmpByBlack<GameGetter>(game_getter_), 0);
      intervals.push_back(games_interval);
    }
  } else if (!whites.empty()) {
    for (const std::string& white : whites) {
      games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
          game_index_by_white_, {white, sequence}, CmpByWhite<GameGetter>(game_getter_), 0);
      intervals.push_back(games_interval);
    }
  } else if (!tournaments.empty()) {
    for (const std::string& tournament : tournaments) {
      games_interval = GetGameIntervals<std::pair<std::string, Sequence>>(
          game_index_by_tournament_, {tournament, sequence}, CmpByTournament<GameGetter>(game_getter_), 0);
      intervals.push_back(games_interval);
    }
  } else {
    for (short year = end_year; year >= start_year; --year) {
      for (bool xot : xot_values) {
        for (bool bot : bot_values) {
          XotBotYear xot_bot_year {.xot = xot, .bot = bot, .year = year};
          games_interval = GetGameIntervals<std::pair<XotBotYear, Sequence>>(
              game_index_by_xot_bot_year_, {xot_bot_year, sequence}, CmpByXotBotYear<GameGetter>(game_getter_), year);
          intervals.push_back(games_interval);
        }
      }
    }
    must_iterate_over_games = false;
  }
  GamesList result;
  result.max_games = max_games;
  int sequence_size = sequence.Size();
  if (must_iterate_over_games) {
    for (const GamesIntervalSequenceSorted interval : intervals) {
      for (auto it = interval.start; it != interval.end; ++it) {
        Game game = game_getter_.GetGame(*it);
        if (!game.PassesFilters(blacks, whites, tournaments, start_year, end_year, xot_values, bot_values)) {
          continue;
        }
        result.next_moves[game.Move(sequence_size)]++;
        result.examples.push_back(game);
      }
    }
    result.num_games = (int) result.examples.size();
  } else {
    int games_higher_year = 0;
    int current_year = INT_MAX;
    for (const GamesIntervalSequenceSorted& interval : intervals) {
      int games_in_interval = (int) (interval.end - interval.start);
      if (games_in_interval == 0) {
        continue;
      }

      result.num_games += games_in_interval;
      FillNextMoves(interval, sequence_size, result.next_moves);

      // We can have multiple intervals within a year.
      assert(interval.year <= current_year);
      if (interval.year < current_year) {
        current_year = interval.year;
        games_higher_year = result.examples.size();
      }
      int games_to_add = (int) (max_games - games_higher_year);
      if (games_to_add <= 0) {
        continue;
      }
      double include_probability = std::min(1.0, games_to_add / (double) games_in_interval);
      int hash_index = std::min((int) games_with_small_hash_.size() - 1, IncludeProbabilityToHashIndex(1-include_probability));
      assert(hash_index < (int) games_with_small_hash_.size());
      for (; hash_index >= 0; --hash_index) {
        auto& games_with_small_hash = games_with_small_hash_[hash_index];
        auto start = std::lower_bound(games_with_small_hash.begin(), games_with_small_hash.end(), interval.start - game_index_by_xot_bot_year_.begin());
        auto end = std::lower_bound(games_with_small_hash.begin(), games_with_small_hash.end(), interval.end - game_index_by_xot_bot_year_.begin());
        if (end - start < games_to_add) {
          continue;
        }
        for (auto it = start; it < end; ++it) {
          result.examples.push_back(game_getter_.GetGame(game_index_by_xot_bot_year_[*it]));
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
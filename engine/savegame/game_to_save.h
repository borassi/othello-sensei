/*
 * Copyright 2025 Michele Borassi
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


#ifndef OTHELLO_SENSEI_SAVE_GAME_GAME_TO_SAVE_H
#define OTHELLO_SENSEI_SAVE_GAME_GAME_TO_SAVE_H

#include <fstream>
#include <regex>
#include <sstream>

#include "../board/get_moves.h"
#include "../board/sequence.h"
#include "../thor/game.h"
#include "../utils/misc.h"

class GameToSave {

 public:
  GameToSave(
      const Sequence& moves,
      std::string black,
      std::string white,
      std::string tournament,
      std::string notes,
      short year,
      Eval black_disks,
      std::string round) :
      moves_(moves),
      round_(std::move(round)),
      black_(std::move(black)),
      white_(std::move(white)),
      tournament_(std::move(tournament)),
      notes_(std::move(notes)),
      year_(year),
      black_disks_(black_disks) {}

  const std::string& Round() const { return round_; }
  const Sequence& Moves() const { return moves_; }
  const std::string& Black() const { return black_; }
  const std::string& White() const { return white_; }
  const std::string& Tournament() const { return tournament_; }
  int Year() const { return year_; }
  const std::string& Notes() const { return notes_; }
  int BlackDisks() const { return black_disks_; }

  static GameToSave FromString(const std::string& game);

  std::string ToString() const {
    std::stringstream result;
    result
        << moves_ << "\n"
        << "Round: " << round_ << "\n"
        << "Black: " << black_ << "\n"
        << "White: " << white_ << "\n"
        << "Tournament: " << tournament_ << "\n"
        << "Year: " << year_ << "\n"
        << "Black disks: " << std::to_string(black_disks_) << "\n"
        << "White disks: " << std::to_string(64 - black_disks_) << "\n"
        << "Notes: " << notes_ << "\n";
    return result.str();
  }

  bool operator==(const GameToSave& other) const {
    if (round_ != other.round_ ||
        black_ != other.black_ ||
        white_ != other.white_ ||
        tournament_ != other.tournament_ ||
        notes_ != other.notes_ ||
        black_disks_ != other.black_disks_) {
      return false;
    }
    return moves_ == other.moves_;
  }

  Game ToGame(bool canonicalize, const std::string* black = nullptr, const std::string* white = nullptr, const std::string* tournament = nullptr) const {
    auto final_black = black == nullptr ? &black_ : black;
    auto final_white = white == nullptr ? &white_ : white;
    auto final_tournament = tournament == nullptr ? &tournament_ : tournament;
    assert(*final_black == black_);
    assert(*final_white == white_);
    assert(*final_tournament == tournament_);
    return {moves_, final_black, final_white, final_tournament, year_, black_disks_, 0, canonicalize};
  }

  static GameToSave FromGame(const Game& game) {
    return {game.Moves(), game.Black(), game.White(), game.Tournament(), "", game.Year(), game.Score(), ""};
  }

 private:
  Sequence moves_;
  std::string round_;
  std::string black_;
  std::string white_;
  std::string tournament_;
  std::string notes_;
  short year_;
  Eval black_disks_;
};

std::ostream& operator<<(std::ostream& stream, const GameToSave& s);

#endif // OTHELLO_SENSEI_SAVE_GAME_GAME_TO_SAVE_H
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
      const std::string& black,
      const std::string& white,
      const std::string& tournament,
      const std::string& notes,
      short year,
      Eval black_disks) :
      moves_(moves),
      black_(black),
      white_(white),
      tournament_(tournament),
      year_(year),
      notes_(notes),
      black_disks_(black_disks) {}

  const Sequence& Moves() { return moves_; }
  const std::string& Black() { return black_; }
  const std::string& White() { return white_; }
  const std::string& Tournament() { return tournament_; }
  int Year() { return year_; }
  const std::string& Notes() { return notes_; }
  int BlackDisks() { return black_disks_; }

  static GameToSave FromString(const std::string& game);

  std::string ToString() const {
    std::stringstream result;
    result
        << moves_ << "\n"
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
    if (black_ != other.black_ ||
        white_ != other.white_ ||
        tournament_ != other.tournament_ ||
        notes_ != other.notes_ ||
        black_disks_ != other.black_disks_) {
      return false;
    }
    return moves_ == other.moves_;
  }

  Game ToGame() const {
    return Game(moves_, &black_, &white_, &tournament_, year_, black_disks_, 0);
  }

 private:
  Sequence moves_;
  std::string black_;
  std::string white_;
  std::string tournament_;
  std::string notes_;
  short year_;
  Eval black_disks_;
};

std::ostream& operator<<(std::ostream& stream, const GameToSave& s);

#endif // OTHELLO_SENSEI_SAVE_GAME_GAME_TO_SAVE_H
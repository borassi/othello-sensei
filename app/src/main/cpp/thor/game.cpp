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
 
 #include "game.h"

std::string Game::Moves() const {
  std::string result = "";
  for (auto move : moves_) {
    if (move == kNoSquare) {
      break;
    }
    result += SquareToMove(move);
  }
  return result;
}

std::string Game::ToString() const {
  std::stringstream result;
  result << *black_ << " " << (int) score_ << " - " << (64-score_) << " " << *white_ << " (" << *tournament_ + "): " << Moves();
  return result.str();
}
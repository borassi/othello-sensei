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

#ifndef OTHELLOSENSEI_APP_SRC_MAIN_CPP_ANALYZERS_ENDGAME_FFO_H_
#define OTHELLOSENSEI_APP_SRC_MAIN_CPP_ANALYZERS_ENDGAME_FFO_H_

#include "../board/board.h"

constexpr char const* kPositions[] = {
      "XXXXXXXOOOOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO-- X",
      "--XXXXX--OOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO-- X",
      "-XXXXXX---XOOOO--XOXXOOX-OOOOOOOOOOOXXOOOOOXXOOX--XXOO----XXXXX- X",
      "----OX----OOXX---OOOXX-XOOXXOOOOOXXOXXOOOXXXOOOOOXXXXOXO--OOOOOX X",
      "-XXXXXX-X-XXXOO-XOXXXOOXXXOXOOOX-OXOOXXX--OOOXXX--OOXX----XOXXO- X",
      "-OOOOO----OXXO-XXXOXOXX-XXOXOXXOXXOOXOOOXXXXOO-OX-XOOO---XXXXX-- X",
      "--OXXX--OOOXXX--OOOXOXO-OOXOOOX-OOXXXXXXXOOXXOX--OOOOX---XXXXXX- X",
      "--OXXO--XOXXXX--XOOOXXXXXOOXXXXXXOOOOXXX-XXXXXXX--XXOOO----XXOO- X",
      "---X-X--X-XXXX--XXXXOXXXXXXOOOOOXXOXXXO-XOXXXXO-XOOXXX--XOOXXO-- O",
      "--XOXX--O-OOXXXX-OOOXXXX-XOXXXOXXXOXOOOXOXXOXOXX--OXOO----OOOO-- O",
      "-XXXX-----OXXX--XOXOXOXXOXOXXOXXOXXOXOOOXXXOXOOX--OXXO---OOOOO-- O",
      "---O-XOX----XXOX---XXOOXO-XXOXOXXXXOOXOX-XOOXXXXXOOOXX-XOOOOOOO- O",
      "--O--O--X-OOOOX-XXOOOXOOXXOXOXOOXXOXXOOOXXXXOOOO--OXXX---XXXXX-- O",
      "--XXXXX--OOOXX---OOOXXXX-OXOXOXXOXXXOXXX--XOXOXX--OXOOO--OOOOO-- X",
      "--XXXXX---OOOX---XOOXXXX-OOOOOOOOOOXXXOOOOOXXOOX--XXOO----XXXXX- X",
      "----O------OOX---OOOXX-XOOOXOOOOOXXOXXOOOXXXOOOOOXXXOOXO--OOOOOX X",
      "-XXXXXX-X-XXXOO-XOXXXOOXXOOXXXOX-OOOXXXX--OOXXXX---OOO----XOX-O- X",
      "-OOOOO----OXXO-XXXOOOXX-XXOXOXXOXXOOXOOOXXXXOO-OX-XOO----XXXX--- X",
      "-XXX------OOOX--XOOOOOXXOXOXOOXXOXXOOOOOXXXOXOOX--OXXO---OOOOO-- X",
      "--OXXO--XOXXXX--XOOOOXXXXOOOXXXXX-OOOXXX--OOOOXX--XXOOO----XXOO- X",
      "XXXOXXXXOXXXXXXXOOXXXXXXOOOXXXXXOOOXXOO-OOOOO---OOOOOOO-OOOOOOO- X",
      "OOOOOOOOXOOXXX--XXOOXOO-XOXOOO--XOOOOX--XOOXOO--XOOOOO--XXXX---- O",
      "--OOOO--X-OOOOO-XXOOXOXXXOXOXXXXXXXOXXXX-XXOXOXX--OXXX-X----X--- O",
      "--O-------OOX---OOOXXXO-OOOOXOXXXXXOOXOXXXXXXOOXX-XXXXOX--XXXX-- X", // 24
      "--O--O-----OOOX--X-XOXOO--XXXOOOXXXXOOOOXXXOXXOOXXXXXX--XOXX-O-- O",
      "----X------XXXO--OOOXXXXXOOOOXXO-XXOOXXOOOXOXXXXOOOXX---X-XXXX-- O",
      "-OOOOO----OXXO---OOOOXXO-OOOXOXX-OOXOOXX-XOXXOXX--O-XXXX--O----O X",
      "--XO-O----OOOO--OOXOXXO-OOOOXXOOOOOXXOX-OXOXXXXX--XXXX----X-O-X- X",
      "--O-------OOO--X-XOOOOXXXXXXOXOX-XXOXOOXXXOXOOXX-OOOOO-X---OOO-- X", // 29
      "-OXXXX----OXXO--XXOOXOOOXXXOOXOOXXOOXOOOXXXXOO-XX-XXO----------- X",
      "-XXX----X-XOO---XXOXOO--XOXOXO--XOOXOXXXXOOXXOX---OOOOO--XXXXX-- X",
      "-OOOOO----OOOO--OXXOOO---XXXOO--XXXXXXO-XXXOOO-OX-OOOO---OOOOO-- X",
      "--XX----O-XXOX--OOXOO---OXOXOOO-OOXXOOOXOOXXXOOX--XXXXOX--X--X-X X",
      "-XXXXXXX--XOOO----OXOOXX-OOXXOXX-OOOOOXX-X-XOOXX---O-X-X--OOOO-- X",
      "-------------O-O-OOOOOOOOOOOOXOOOXXOOOXO-XXXOXOO--XXXOXO--OXXXXO X",
      "--XXX-----XXXX-OOOXXOOOOOOOOOOXO-OOXXXXO-OOOXXXO---XOXX---X----- O",
      "---X-O----XXXO-XXXXXXXXXXOOXXOOXXOXOOOXXXXOOOO-XX--OOOO--------- O", // 37
      "--OOOO--O-OOOO--OXXXOOO-OXXOXO--OOXXOXX-OOXXXX--O-XXX-----XX-O-- X",
      "--OOOO----OOOO---XOXXOOXOOXOOOOX-OOOOOXXXOOXXXXX--X-X----------- X",
      "O-OOOO--XOXXOX--XOOOXXX-XOOOXX--XOOXOX--XOXXX---X-XX------------ O", // 40
      "O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X",
      "-OOOOO----OOOOX--OOOOOO-XXXXXOO--XXOOX--OOXOXX----OXXO---OOO--O- X",
      "--OOO-------XX-OOOOOOXOO-OOOOXOOX-OOOXXO---OOXOO---OOOXO--OOOO-- X", // 43
      "--XXXXX---XXXX---OOOXX---OOXXXX--OOXXXO-OOOOXOO----XOX----XXXXX- O",
      "--O-X-O---O-XO-O-OOXXXOOOOOOXXXOOOOOXX--XXOOXO----XXXX-----XXX-- O",
      "---XXXX-X-XXXO--XXOXOO--XXXOXO--XXOXXO---OXXXOO-O-OOOO------OO-- X", // 46
      "---XXX----OOOX----OOOXX--OOOOXXX--OOOOXX--OXOXXX--XXOO---XXXX-O- X",
      "-OOOOO----OOOO---OOOOX--XXXXXX---OXOOX--OOOXOX----OOXX----XXXX-- O",
      "-----X--X-XXX---XXXXOO--XOXOOXX-XOOXXX--XOOXX-----OOOX---XXXXXX- O",
      "--OX-O----XXOO--OOOOOXX-OOOOOX--OOOXOXX-OOOOXX-----OOX----X-O--- X", // 50
      "----X-----XXX----OOOXOOO-OOOXOOO-OXOXOXO-OOXXOOO--OOXO----O--O-- X",
      "----O-X------X-----XXXO-OXXXXXOO-XXOOXOOXXOXXXOO--OOOO-O----OO-- O",
      "---X-------OX--X--XOOXXXXXXOXXXXXXXOOXXXXXXOOOXX--XO---X-------- O",
      "----OO-----OOO---XXXXOOO--XXOOXO-XXXXXOO--OOOXOO--X-OX-O-----X-- X",
      "--OOO---XXOO----XXXXOOOOXXXXOX--XXXOXX--XXOOO------OOO-----O---- X", // 55
      "--------X-X------XXXXOOOOOXOXX--OOOXXXX-OOXXXX--O-OOOX-----OO--- O",
      "--XXXXX---XXXX---OOOXX---OOXOX---OXXXXX-OOOOOXO----OXX---------- O",
      "-------------------XXOOO--XXXOOO--XXOXOO-OOOXXXO--OXOO-O-OOOOO-- X",
      "--XOOO----OOO----OOOXOO--OOOOXO--OXOXXX-OOXXXX----X-XX---------- X",
      "-----------------------O--OOOOO---OOOOOXOOOOXXXX--XXOOXX--XX-O-X X", // 60
      "---XXXX---XXXO-OXXOXOO-OOOOOXOOO-OOOOXO----OOOX----------------- X", // 61
      "-----------X------XXXO----OXXOXX---OOXXX--OOOXXX---OOX-X--XXXXX- O", // 62
      "----X-----OOX----XXXXX---OXXOX---OOXXOX---OOOOO---XOOX-------X-- X", // 63
      "-XXXXX----OOOX--XXXOO-X---XOOO---XXOOO--XX-OOO------------------ O", // 64
      "---------------------------OX------XO--------------------------- X", // 65
      "-------------X----XXXX----XXOX----XOOX----XXXX------------------ X", // 66
      "-------------X----XXXX----XXOOO---XOOX----XXXX------------------ X", // 67
      "---------------------------OOO-----OOOO----OOOOX---OOXXX--XXXXXX X",
      "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXOOO X",
};

Board GetIthBoard(int i) {
  std::string input = kPositions[i-1];
  auto board = input.substr(0, input.find(' '));
  auto color = input.substr(input.find(' ') + 1, input.size());
  return {board.c_str(), color == "X"};
}

#endif //OTHELLOSENSEI_APP_SRC_MAIN_CPP_ANALYZERS_ENDGAME_FFO_H_

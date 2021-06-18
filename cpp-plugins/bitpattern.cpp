/*
 * Copyright 2021 Google LLC
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

#include <algorithm>
#include <immintrin.h>
#include <iostream>
#include <regex>
#include <string>
#include "bitpattern.h"

int GetEvaluationGameOver(BitPattern player, BitPattern opponent) {
  int playerDisks = __builtin_popcountll(player);
  int opponentDisks = __builtin_popcountll(opponent);
  int diff = playerDisks - opponentDisks;
  return (diff + (int) ((diff > 0) - (diff < 0)) * (64 - playerDisks - opponentDisks));
}

std::string PatternToString(BitPattern pattern) {
  std::string result = "";
  for (int i = 63; i >= 0; --i) {
    BitPattern mask = (1L << i);
    if ((pattern & mask) != 0) {
      result += 'X';
    } else {
      result += '-';
    }
    if (i % 8 == 0) {
      result += '\n';
    }
  }
  return result;
}

BitPattern Neighbors(BitPattern b) {
  return (((b << 1) | (b << 9) | (b >> 7)) & ~kLastColumnPattern)
          | (((b >> 1) | (b >> 9) | (b << 7)) & ~kFirstColumnPattern)
          | (b >> 8) | (b << 8);
}

BitPattern UniqueSet(BitPattern b) {
  return __builtin_popcountll(b) == 1 ? b : 0;
}

BitPattern FirstLastSet(BitPattern b) {
  return (1ULL << __builtin_ctzll(b)) | (1ULL << (63 - __builtin_clzll(b)));
}

BitPattern UniqueInEdges(BitPattern empties) {
  return UniqueSet(empties & kPatternLastRow)
          | UniqueSet(empties & kFirstRowPattern)
          | UniqueSet(empties & kFirstColumnPattern)
          | UniqueSet(empties & kLastColumnPattern);
}

BitPattern FirstLastInEdges(BitPattern empties) {
  return (FirstLastSet(empties & kPatternLastRow)
          | FirstLastSet(empties & kFirstRowPattern)
          | FirstLastSet(empties & kFirstColumnPattern)
          | FirstLastSet(empties & kLastColumnPattern)) & ~kCornerPattern;
}
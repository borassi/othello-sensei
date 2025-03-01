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

#include <algorithm>
#include <cassert>
#include <bitset>
#include <string>

#include "bitpattern.h"
#include "get_flip.h"


void PrintOutflank() {
  std::string result = "constexpr uint8_t OUTFLANK[] = {";
  
  for (int move = 0; move < 8; ++move) {
    for (int opponent = 0; opponent < 256; ++opponent) {
      if (opponent % 32 == 0) {
        if (move != 0 || opponent != 0) {
          result += ",";
        }
        result += "\n    ";
      } else {
        result += ", ";
      }
      if (((1L << move) & opponent) != 0) {
        result += "0";
        continue;
      }
      uint8_t outflank = 0;
      int disk = move - 1;
      if (((1L << disk) & opponent) != 0) {
        for (disk = move - 2; disk >= 0; --disk) {
          if (((1L << disk) & opponent) == 0) {
            outflank |= (1L << disk);
            break;
          }
        }
      }
      disk = move + 1;
      if (((1L << disk) & opponent) != 0) {
        for (disk = move + 2; disk <= 7; ++disk) {
          if (((1L << disk) & opponent) == 0) {
            outflank |= (1L << disk);
            break;
          }
        }
      }
      result += std::to_string(outflank);
    }
  }
  result += "};\n";
  std::cout << result;
}

void PrintFlip() {
  std::string result = "constexpr uint8_t FLIP[] = {";
  
  for (Square move = 0; move < 8; ++move) {
    for (unsigned int outflank = 0; outflank < 256; ++outflank) {
      if (outflank % 32 == 0) {
        if (move != 0 || outflank != 0) {
          result += ",";
        }
        result += "\n    ";
        if (outflank % 256 == 0) {
          result += "// Move " + std::to_string(move) + "\n    ";
        }
      } else {
        result += ", ";
      }
      if (((1L << move) & outflank) != 0 || outflank == 0 || __builtin_popcount(outflank) > 2) {
        result += "0";
        continue;
      }
      int l;  // First flipped
      int r;  // Last flipped
      if (__builtin_popcount(outflank) == 1) {  // Only one outflank.
        int x = __builtin_ctz(outflank);
        if (x > move + 1) {
          l = x - 1;
          r = move;
        } else if (x < move - 1) {
          l = move;
          r = x + 1;
        } else {
          result += "0";
          continue;          
        }
      } else {  // Two outflanks.
        l = 31 - __builtin_clz(outflank) - 1;
        r = __builtin_ctz(outflank) + 1;
        if (l <= move || r >= move) {
          result += "0";
          continue;
        }
      }
      result += std::to_string((1 << (l+1)) - (1 << r));
    }
  }
  result += "};\n";
  std::cout << result;
}

void PrintMoveMetadata() {
  std::string result = "constexpr MoveMetadata MOVE_METADATA[] = {";
  
  for (Square move = 0; move < 64; ++move) {
    if (move % 4 == 0) {
      if (move != 0) {
        result += ",";
      }
      result += "\n    ";
    } else {
      result += ", ";
    }
    result += "MoveMetadata(" + std::to_string(move) + ")";
  }
  result += "};\n";
  std::cout << result;
}
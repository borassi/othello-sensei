/*
 * Copyright 2024 Michele Borassi
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

#include "../board/bitpattern.h"
#include "../board/board.h"
#if __BMI2__
#include <immintrin.h>
#endif
#include <tuple>

constexpr int kNumPatterns = 38;

typedef void (*FlipFunction)(short patterns[kNumPatterns]);
typedef bool (*FlipAllFunction)(short patterns[kNumPatterns]);
typedef void (*PlayMoveFunction)(short patterns[kNumPatterns]);

constexpr short kPowersOf3[] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561};
constexpr int kNumCompressedFlipValues = 42;

constexpr int8_t Row(Square square) { return square / 8; }
constexpr int8_t Column(Square square) { return square % 8; }
constexpr int8_t Diag9(Square square) { return Row(square) - Column(square); }
constexpr int8_t Diag7(Square square) { return Row(square) - (7 - Column(square)); }

constexpr void DoNothing() {}

constexpr int GetNSquare(short pattern, int i) {
  assert(i >= 0 && i < 8);
  return (pattern % kPowersOf3[i+1]) / kPowersOf3[i] - 1;
}

constexpr u_int8_t ContiguousPattern(int start, int end) {
  assert(start >= 0 && start <= 31);
  assert(end >= 0 && end <= 31);
  if (start == end+1) {
    return 0;
  }
  assert(start <= end);
  return (1 << (end+1)) - (1 << start);
}

template<Square square, bool new_disk>
void Flip(short patterns[kNumPatterns]) {
  assert(square < 64);
  int8_t row = Row(square);
  int8_t column = Column(square);
  int8_t diag9 = Diag9(square);
  int8_t diag7 = Diag7(square);

  short multiplier = (new_disk ? 1 : 2);
  patterns[row] += multiplier * kPowersOf3[column];
  patterns[8 + column] += multiplier * kPowersOf3[row];
  if (diag9 >= -5 && diag9 <= 5) {
    patterns[21 + diag9] += multiplier * kPowersOf3[row - std::max((int8_t) 0, diag9)];
  }
  if (diag7 >= -5 && diag7 <= 5) {
    patterns[32 + diag7] += multiplier * kPowersOf3[row - std::max((int8_t) 0, diag7)];
  }
}

template<int i>
constexpr void InitFlipFunction(FlipFunction* flip_function) {
  flip_function[i-1] = &Flip<(i-1) % 64, (i-1) / 64>;
  InitFlipFunction<i-1>(flip_function);
}

template <>
constexpr void InitFlipFunction<0>(FlipFunction* flip_function) {}

constexpr int NumFlipOffset(Square move, short pattern) {
  assert(move < 8);
  assert(pattern < kPowersOf3[8]);
  return move + pattern * 8;
}

constexpr bool IsContiguous(u_int8_t pattern) {
  if (pattern == 0) {
    return true;
  }
  pattern = pattern >> __builtin_ctz(pattern);
  return __builtin_popcount(pattern + 1) == 1;
}

constexpr u_int8_t CompressFlip(u_int8_t flip) {
  assert((flip & 1) == 0);
  assert((flip & 128) == 0);
  if (flip == 0) {
    return 0;
  }
  if (IsContiguous(flip)) {
    int start = __builtin_ctz(flip);
    int end = 31 - __builtin_clz(flip);
    assert (end >= start);
    switch(start) {
      case 1:
        return end;      // 1-6
      case 2:
        return 5 + end;  // 7-11
      case 3:
        return 9 + end;  // 12-15
      case 4:
        return 12 + end; // 16-18
      case 5:
        return 14 + end; // 19-20
      case 6:
        return 21;
      default:
        assert(false);
    }
  } else {
    int start = __builtin_ctz(flip);
    int end = 31 - __builtin_clz(flip);
    int mid = __builtin_ctz((~flip) >> start) + start;
    assert(end - start >= 2);
    assert(end - mid >= 1);
    assert(mid - start >= 1);
    switch(end - start) {
      case 5:
        return 20 + mid;   // 22-25
      case 4:
        if (start == 1) {
          assert(mid >= 2 && mid <= 4);
          return 24 + mid; // 26-28
        } else {
          assert(start == 2);
          assert(mid >= 3 && mid <= 5);
          return 26 + mid; // 29-31
        }
      case 3:
        if (start == 1) {
          assert(mid >= 2 && mid <= 3);
          return 30 + mid; // 32-33
        } else if (start == 2) {
          assert(mid >= 3 && mid <= 4);
          return 31 + mid; // 34-35
        } else {
          assert(start == 3);
          assert(mid >= 4 && mid <= 5);
          return 32 + mid; // 36-37
        }
      case 2:
        assert(start >= 1 && start <= 4);
        assert(mid == start + 1);
        assert(end == start + 2);
        return 37 + start; // 38-41
      default:
        assert(false);
    }
  }
  return 255;
}

constexpr u_int8_t DecompressFlip(u_int8_t flip) {
  assert(flip < kNumCompressedFlipValues);
  if (flip >= 38) {
    return ContiguousPattern(flip-37, flip-35) & ~(1 << (flip-36));
  } else if (flip >= 36) {
    return ContiguousPattern(3, 6) & ~(1 << (flip-32));
  } else if (flip >= 34) {
    return ContiguousPattern(2, 5) & ~(1 << (flip-31));
  } else if (flip >= 32) {
    return ContiguousPattern(1, 4) & ~(1 << (flip-30));
  } else if (flip >= 29) {
    return ContiguousPattern(2, 6) & ~(1 << (flip-26));
  } else if (flip >= 26) {
    return ContiguousPattern(1, 5) & ~(1 << (flip-24));
  } else if (flip >= 22) {
    return ContiguousPattern(1, 6) & ~(1 << (flip-20));
  } else if (flip == 21) {
    return ContiguousPattern(6, 6);
  } else if (flip >= 19) {
    return ContiguousPattern(5, flip - 14);
  } else if (flip >= 16) {
    return ContiguousPattern(4, flip - 12);
  } else if (flip >= 12) {
    return ContiguousPattern(3, flip - 9);
  } else if (flip >= 7) {
    return ContiguousPattern(2, flip - 5);
  } else if (flip >= 1) {
    return ContiguousPattern(1, flip);
  } else {
    return 0;
  }
}

constexpr int Direction(u_int8_t pattern_index) {
  if (pattern_index < 8) {
    return 1;
  } else if (pattern_index < 16) {
    return 8;
  } else if (pattern_index < 27) {
    return 9;
  } else {
    return 7;
  }
}

constexpr u_int8_t GetFlip(Square position_in_pattern, short pattern) {
  assert (position_in_pattern < 8);
  assert (pattern >= 0 && pattern < kPowersOf3[8]);
  if (GetNSquare(pattern, position_in_pattern) != 0) {
    return 0;
  }
  u_int8_t flip = 0;
  for (int i = position_in_pattern+1; i <= 7; ++i) {
    int square = GetNSquare(pattern, i);
    if (square == 1) {
      flip |= ContiguousPattern(position_in_pattern+1, i-1);
      break;
    } else if (square != -1) {
      break;
    }
  }
  for (int i = position_in_pattern-1; i >= 0; --i) {
    int square = GetNSquare(pattern, i);
    if (square == 1) {
      flip |= ContiguousPattern(i+1, position_in_pattern-1);
      break;
    } else if (square != -1) {
      break;
    }
  }
  assert((flip & 1) == 0);
  assert((flip & 128) == 0);
  return CompressFlip(flip);
}

template<u_int8_t pattern_index, u_int8_t compressed_flip>
constexpr bool FlipAll(short patterns[kNumPatterns]);

template<Square move>
constexpr void PlayMove(short patterns[kNumPatterns]);

constexpr int FlipAllOffset(u_int8_t pattern_index, u_int8_t compressed_flip) {
  return pattern_index + compressed_flip * kNumPatterns;
}

template<int start, int end>
constexpr void InitFlipAllFunctions(FlipAllFunction* function) {
  assert(end > start);
  if (start == end - 1) {
    function[start] = &FlipAll<start % kNumPatterns, start / kNumPatterns>;
  } else {
    constexpr int mid = (start + end) / 2;
    InitFlipAllFunctions<start, mid>(function);
    InitFlipAllFunctions<mid, end>(function);
  }
}

template<int start, int end>
constexpr void InitPlayMoveFunctions(PlayMoveFunction* function) {
  assert(end > start);
  if (start == end - 1) {
    function[start] = &PlayMove<start>;
  } else {
    constexpr int mid = (start + end) / 2;
    InitPlayMoveFunctions<start, mid>(function);
    InitPlayMoveFunctions<mid, end>(function);
  }
}

struct Constants {
  BitPattern patterns[kNumPatterns];
  u_int8_t pattern_to_square[kNumPatterns][8];
  u_int8_t num_flips[8 * kPowersOf3[8]];
  FlipFunction flip_function[128];
  FlipAllFunction flip_all_function[kNumCompressedFlipValues * kNumPatterns];
  PlayMoveFunction play_move_function[64];


  constexpr Constants() : patterns(), num_flips(), flip_function(), flip_all_function(), pattern_to_square(), play_move_function() {
    for (int i = 0; i < 8; ++i) {
      patterns[i] = kLastRowPattern << (i * 8);
    }
    for (int i = 0; i < 8; ++i) {
      patterns[i + 8] = kLastColumnPattern << i;
    }
    for (int i = -5; i <= 5; ++i) {
      patterns[i + 21] = i > 0 ? (kMainDiag9Pattern << (8 * i)) : (kMainDiag9Pattern >> -(8 * i));
    }
    for (int i = -5; i <= 5; ++i) {
      patterns[i + 32] = i > 0 ? (kMainDiag7Pattern << (8 * i)) : (kMainDiag7Pattern >> -(8 * i));
    }
    for (int i = 0; i <= 7; ++i) {
      for (int state = 0; state < kPowersOf3[8]; ++state) {
        num_flips[NumFlipOffset(i, state)] = GetFlip(i, state);
      }
    }
    for (int i = 0; i < kNumPatterns; ++i) {
      BitPattern pattern = patterns[i];
      for (int j = 0; j < 8; ++j) {
        if (pattern == 0) {
          pattern_to_square[i][j] = 255;
        } else {
          pattern_to_square[i][j] = __builtin_ctzll(pattern);
          pattern = pattern & ~(1ULL << pattern_to_square[i][j]);
        }
      }
    }
    InitFlipFunction<128>(flip_function);
    InitFlipAllFunctions<0, kNumPatterns * kNumCompressedFlipValues>(flip_all_function);
    InitPlayMoveFunctions<0, 64>(play_move_function);
  }
};

constexpr Constants kConstants;

template<u_int8_t pattern_index, u_int8_t compressed_flip>
constexpr bool FlipAll(short patterns[kNumPatterns]) {
  constexpr u_int8_t flip = DecompressFlip(compressed_flip);
  if (flip & 2) {
    Flip<kConstants.pattern_to_square[pattern_index][1], false>(patterns);
  }
  if (flip & 4) {
    Flip<kConstants.pattern_to_square[pattern_index][2], false>(patterns);
  }
  if (flip & 8) {
    Flip<kConstants.pattern_to_square[pattern_index][3], false>(patterns);
  }
  if (flip & 16) {
    Flip<kConstants.pattern_to_square[pattern_index][4], false>(patterns);
  }
  if (flip & 32) {
    Flip<kConstants.pattern_to_square[pattern_index][5], false>(patterns);
  }
  if (flip & 64) {
    Flip<kConstants.pattern_to_square[pattern_index][6], false>(patterns);
  }
  return compressed_flip != 0;
}

short GetSquare(short patterns[kNumPatterns], int i) {
  assert(i >= 0 && i < 64);
  BitPattern square = 1ULL << i;
  int result = -2;
  int num_seen = 0;
  int num_seen1 = 0;
  for (int j = 0; j < kNumPatterns; ++j) {
    BitPattern pattern = kConstants.patterns[j];
    if ((pattern & square) != 0) {
      int pattern_counter = 0;
      ++num_seen;
      for (int k = 0; k < 64; ++k) {
        if (((1ULL << k) & pattern) == 0) {
          continue;
        }
        if (i == k) {
          ++num_seen1;
          assert(result == -2 || result == GetNSquare(patterns[j], pattern_counter));
          result = GetNSquare(patterns[j], pattern_counter);
          break;
        }
        ++pattern_counter;
      }
    }
  }
  assert(num_seen == num_seen1);
  assert(num_seen == 3 || num_seen == 4);
  assert((num_seen == 3) ==
      (i == 0 || i == 1 || i == 6 || i == 7 || i == 8 || i == 15
       || i == 48 || i == 55 || i == 56 || i == 57 || i == 62 || i == 63));
  assert(result != -2);
  return result;
}

short LastRowToPattern(BitPattern player, BitPattern opponent) {
  assert((player & opponent) == 0);
  assert(player < 256);
  assert(opponent < 256);
  short result = 0;

  for (int i = 0; i < 8; ++i) {
    if (player & (1 << i)) {
      result += 2 * kPowersOf3[i];
    } else if (!(opponent & (1 << i))) {
      result += kPowersOf3[i];
    }
  }
  assert(result < kPowersOf3[8]);
  return result;
}

void PatternsInit(short patterns[kNumPatterns], BitPattern player, BitPattern opponent) {
  for (int i = 0; i < kNumPatterns; ++i) {
    BitPattern pattern = kConstants.patterns[i];
    BitPattern player_pattern = _pext_u64(player, pattern);
    BitPattern opponent_pattern = _pext_u64(opponent, pattern);
    patterns[i] = LastRowToPattern(player_pattern, opponent_pattern);
  }
}

void CheckPatternsOk(short patterns[kNumPatterns]) {
  for (int i = 0; i < 64; ++i) {
    GetSquare(patterns, i);
  }
}

Board ToBoard(short patterns[kNumPatterns]) {
  BitPattern player = 0;
  BitPattern opponent = 0;
  for (int i = 0; i < 64; ++i) {
    switch (GetSquare(patterns,i)) {
      case -1:
        opponent |= 1ULL << i;
        break;
      case 1:
        player |= 1ULL << i;
        break;
      case 0:
        break;
      default:
        assert(false);
    }
  }
  return Board(player, opponent);
}

template<Square square>
constexpr void PlayMove(short patterns[kNumPatterns]) {
  assert(square < 64);
  constexpr int8_t row = Row(square);
  constexpr int8_t column = Column(square);
  bool flipped = false;

  int compressed_flip = kConstants.num_flips[NumFlipOffset(column, patterns[row])];
  flipped |= kConstants.flip_all_function[FlipAllOffset(row, compressed_flip)](patterns);

  compressed_flip = kConstants.num_flips[NumFlipOffset(row, patterns[8 + column])];
  flipped |= kConstants.flip_all_function[FlipAllOffset(8 + column, compressed_flip)](patterns);

  constexpr int8_t diag9 = Diag9(square);
  if (diag9 >= -5 && diag9 <= 5) {
    compressed_flip = kConstants.num_flips[NumFlipOffset(row - std::max(diag9, (int8_t) 0), patterns[21 + diag9])];
    flipped |= kConstants.flip_all_function[FlipAllOffset(21 + diag9, compressed_flip)](patterns);
  }

  constexpr int8_t diag7 = Diag7(square);
  if (diag7 >= -5 && diag7 <= 5) {
    compressed_flip = kConstants.num_flips[NumFlipOffset(row - std::max(diag7, (int8_t) 0), patterns[32 + diag7])];
    flipped |= kConstants.flip_all_function[FlipAllOffset(32 + diag7, compressed_flip)](patterns);
  }
  if (flipped) {
    Flip<square, true>(patterns);
  }
}

void Invert(short patterns[kNumPatterns]) {
  for (int i = 0; i < kNumPatterns; ++i) {
    patterns[i] = 6560 - patterns[i];
  }
}

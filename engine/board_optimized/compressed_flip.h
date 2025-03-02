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
#ifndef SENSEI_BOARD_OPTIMIZED_COMPRESSED_FLIP_H
#define SENSEI_BOARD_OPTIMIZED_COMPRESSED_FLIP_H

#include "board_optimized.h"

typedef uint8_t LastRowPattern;
typedef uint8_t CompressedFlipForEval;

constexpr int kNumCompressedFlipValues = 42;

constexpr LastRowPattern ContiguousPattern(int start, int end) {
  assert(start >= 0 && start <= 31);
  assert(end >= 0 && end <= 31);
  if (start == end+1) {
    return 0;
  }
  assert(start <= end);
  return (1 << (end+1)) - (1 << start);
}

constexpr bool IsContiguous(LastRowPattern pattern) {
  if (pattern == 0) {
    return true;
  }
  pattern = pattern >> __builtin_ctz(pattern);
  return __builtin_popcount(pattern + 1) == 1;
}

constexpr bool IsValidFlip(LastRowPattern flip) {
  if (flip & 129) {
    return false;
  }
  for (int i = 1; i < 7; ++i) {
    if (IsContiguous((1 << i) | flip)) {
      return true;
    }
  }
  return false;
}

constexpr CompressedFlipForEval CompressFlip(LastRowPattern flip) {
  assert(IsValidFlip(flip));
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

constexpr LastRowPattern DecompressFlip(CompressedFlipForEval flip) {
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

constexpr CompressedFlipForEval GetFlip(int turn, Square position_in_pattern, SingleRowPattern pattern) {
  assert (position_in_pattern < 8);
  assert (pattern >= 0 && pattern < kPowersOf3[8]);
  if (GetFromPattern(pattern, position_in_pattern) != 0) {
    assert(IsValidFlip(0));
    return 0;
  }
  LastRowPattern flip = 0;
  for (int i = position_in_pattern+1; i <= 7; ++i) {
    int square = GetFromPattern(pattern, i);
    if (square == turn) {
      flip |= ContiguousPattern(position_in_pattern+1, i-1);
      break;
    } else if (square != -turn) {
      break;
    }
  }
  for (int i = position_in_pattern-1; i >= 0; --i) {
    int square = GetFromPattern(pattern, i);
    if (square == turn) {
      flip |= ContiguousPattern(i+1, position_in_pattern-1);
      break;
    } else if (square != -turn) {
      break;
    }
  }
  assert(IsValidFlip(flip));
  return CompressFlip(flip);
}

constexpr int TurnToOffset(int turn) {
  return (turn + 1) >> 1;
}

struct SquareCompressedPatternToFlip {
  CompressedFlipForEval value_8[2][8][6561];
  CompressedFlipForEval value_7[2][7][2187];
  CompressedFlipForEval value_6[2][6][729];
  CompressedFlipForEval value_5[2][5][243];
  CompressedFlipForEval value_4[2][4][81];
  CompressedFlipForEval value_3[2][3][27];

  constexpr SquareCompressedPatternToFlip() : value_8(), value_7(), value_6(), value_5(), value_4(), value_3() {
    for (int turn : {1, -1}) {
      for (int move = 0; move < 8; ++move) {
        for (int pattern = 0; pattern < 6561; ++pattern) {
          value_8[TurnToOffset(turn)][move][pattern] = GetFlip(turn, move, pattern);
          assert(value_8[TurnToOffset(turn)][move][pattern] < kNumCompressedFlipValues);
        }
      }
    }
    for (int value = 3; value < 8; ++value) {
      int value_to_sum = 0;
      for (int i = value; i < 8; ++i) {
        value_to_sum += kPowersOf3[i];
      }
      for (int turn : {1, -1}) {
        for (int move = 0; move < value; ++move) {
          for (int pattern = 0; pattern < kPowersOf3[value]; ++pattern) {
            switch(value) {
              case 3:
                value_3[TurnToOffset(turn)][move][pattern] = value_8[TurnToOffset(turn)][move][pattern + value_to_sum];
                assert(value_3[TurnToOffset(turn)][move][pattern] < kNumCompressedFlipValues);
                break;
              case 4:
                value_4[TurnToOffset(turn)][move][pattern] = value_8[TurnToOffset(turn)][move][pattern + value_to_sum];
                assert(value_4[TurnToOffset(turn)][move][pattern] < kNumCompressedFlipValues);
                break;
              case 5:
                value_5[TurnToOffset(turn)][move][pattern] = value_8[TurnToOffset(turn)][move][pattern + value_to_sum];
                assert(value_5[TurnToOffset(turn)][move][pattern] < kNumCompressedFlipValues);
                break;
              case 6:
                value_6[TurnToOffset(turn)][move][pattern] = value_8[TurnToOffset(turn)][move][pattern + value_to_sum];
                assert(value_6[TurnToOffset(turn)][move][pattern] < kNumCompressedFlipValues);
                break;
              case 7:
                value_7[TurnToOffset(turn)][move][pattern] = value_8[TurnToOffset(turn)][move][pattern + value_to_sum];
                assert(value_7[TurnToOffset(turn)][move][pattern] < kNumCompressedFlipValues);
                break;
            }
          }
        }
      }
    }
  }
};

constexpr SquareCompressedPatternToFlip kSquareCompressedPatternToFlip;

#endif  // SENSEI_BOARD_OPTIMIZED_COMPRESSED_FLIP_H
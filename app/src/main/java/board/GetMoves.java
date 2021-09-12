// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
package board;

import bitpattern.BitPattern;
//import java.lang.management.ManagementFactory;
//import java.lang.management.ThreadMXBean;

/**
 *
 * @author michele
 */
public class GetMoves {
  
  public static abstract class Flipper {
    public abstract long run(long player, long opponent);
  }
  
  private static final long CORNERS = BitPattern.parsePattern(
      "X------X" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "--------" +
      "X------X");
  
  private static final long MASK = BitPattern.parsePattern(
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-" +
      "-XXXXXX-");
  
  private static long getDisksRightOfCells1(long disks, long cells, int dir) {
    return disks & (cells >>> dir);
  }

  private static long getDisksRightOfCells2(long disks, long cells, int dir) {
    long result = disks & (cells >>> dir);
    return disks & (result | (result >>> dir));
  }

  private static long getDisksRightOfCells3(long disks, long cells, int dir) {
    long result = disks & (cells >>> dir);
    result |= disks & (result >>> dir);
    return result | (disks & (result >>> dir));
  }

  private static long getDisksRightOfCells4(long disks, long cells, int dir) {
    long result = disks & (cells >>> dir);
    result |= disks & (result >>> dir);
    result |= disks & (result >>> dir);
    return result | (disks & (result >>> dir));
  }

  private static long getDisksRightOfCells5(long disks, long cells, int dir) {
    long result = disks & (cells >>> dir);
    result |= disks & (result >>> dir);
    result |= disks & (result >>> dir);
    result |= disks & (result >>> dir);
    return result | (disks & (result >>> dir));
  }
  
  private static long getDisksLeftOfCells6(long disks, long cells, int dir) {
    int dir2 = dir * 2;
    
    long result = disks & (cells << dir);
    result |= disks & (result << dir);
    disks = disks & (disks << dir);
    result |= disks & (result << dir2);
    result |= disks & (result << dir2);
    return result;
  }
  
  private static long getDisksRightOfCells6(long disks, long cells, int dir) {
    int dir2 = dir * 2;
    
    long result = disks & (cells >>> dir);
    result |= disks & (result >>> dir);
    disks = disks & (disks >>> dir);
    result |= disks & (result >>> dir2);
    result |= disks & (result >>> dir2);
    return result;
  }
  
  public static long getMoves(long player, long opponent) {
    long opponentFlippable = opponent & MASK;
    long empties = ~(player | opponent);
    return empties & (
        (getDisksLeftOfCells6(opponentFlippable, player, 1) << 1) |
        (getDisksLeftOfCells6(opponent, player, 8) << 8) |
        (getDisksLeftOfCells6(opponentFlippable, player, 7) << 7) |
        (getDisksLeftOfCells6(opponentFlippable, player, 9) << 9) |
        (getDisksRightOfCells6(opponentFlippable, player, 1) >>> 1) |
        (getDisksRightOfCells6(opponent, player, 8) >>> 8) |
        (getDisksRightOfCells6(opponentFlippable, player, 7) >>> 7) |
        (getDisksRightOfCells6(opponentFlippable, player, 9) >>> 9));
  }
  
  public static int getNMoves(long player, long opponent) {
    return Long.bitCount(getMoves(player, opponent));
  }
  
  public static int getWeightedNMoves(long player, long opponent) {
    long moves = getMoves(player, opponent);
    return Long.bitCount(moves) + Long.bitCount(moves & CORNERS);
  }  
  
  private static class Flipper0 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000000302L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000000000FFL & ((0x0000000000000002L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = 0x8040201008040201L & ((0x0000000000000200L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = 0x0101010101010101L & ((0x0000000000000100L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000001L;
    }
  }

  private static class Flipper1 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000000604L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000000000FFL & ((0x0000000000000004L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFF7FBFDFEFF7FBFDL;
      curFlip = 0x0080402010080402L & ((0x0000000000000400L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = 0x0202020202020202L & ((0x0000000000000200L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000002L;
          }
  }

  private static class Flipper2 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000000E0AL) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000000000FFL & ((0x0000000000000008L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFF7FBFDFEFF7FBL;
      curFlip = 0x0000804020100804L & ((0x0000000000000800L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = 0x0404040404040404L & ((0x0000000000000400L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFFFFFEFDFBL;
      curFlip = 0x0000000000010204L & ((0x0000000000000200L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000000000FFL) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000000004L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000004L;
          }
  }

  private static class Flipper3 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000001C14L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000000000FFL & ((0x0000000000000010L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFF7FBFDFEFF7L;
      curFlip = 0x0000008040201008L & ((0x0000000000001000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = 0x0808080808080808L & ((0x0000000000000800L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFFFEFDFBF7L;
      curFlip = 0x0000000001020408L & ((0x0000000000000400L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000000000FFL) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000000000008L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000008L;
          }
  }

  private static class Flipper4 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000003828L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000000000FFL & ((0x0000000000000020L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFF7FBFDFEFL;
      curFlip = 0x0000000080402010L & ((0x0000000000002000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = 0x1010101010101010L & ((0x0000000000001000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFEFDFBF7EFL;
      curFlip = 0x0000000102040810L & ((0x0000000000000800L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000000000FFL) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000000000010L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000010L;
          }
  }

  private static class Flipper5 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000007050L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000000000FFL & ((0x0000000000000040L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFFFF7FBFDFL;
      curFlip = 0x0000000000804020L & ((0x0000000000004000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = 0x2020202020202020L & ((0x0000000000002000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFEFDFBF7EFDFL;
      curFlip = 0x0000010204081020L & ((0x0000000000001000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000000000FFL) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000000000000020L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000020L;
          }
  }

  private static class Flipper6 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000006020L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = 0x4040404040404040L & ((0x0000000000004000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFEFDFBF7EFDFBFL;
      curFlip = 0x0001020408102040L & ((0x0000000000002000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000000000FFL) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0000000000000040L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000040L;
          }
  }

  private static class Flipper7 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x000000000000C040L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = 0x8080808080808080L & ((0x0000000000008000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = 0x0102040810204080L & ((0x0000000000004000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000000000FFL) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0000000000000080L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000080L;
          }
  }

  private static class Flipper8 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000030200L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000000000FF00L & ((0x0000000000000200L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xBFDFEFF7FBFDFEFFL;
      curFlip = 0x4020100804020100L & ((0x0000000000020000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = 0x0101010101010101L & ((0x0000000000010000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000100L;
          }
  }

  private static class Flipper9 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000060400L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000000000FF00L & ((0x0000000000000400L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = 0x8040201008040201L & ((0x0000000000040000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = 0x0202020202020202L & ((0x0000000000020000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000200L;
          }
  }

  private static class Flipper10 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x00000000000E0A00L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000000000FF00L & ((0x0000000000000800L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFF7FBFDFEFF7FBFDL;
      curFlip = 0x0080402010080402L & ((0x0000000000080000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = 0x0404040404040404L & ((0x0000000000040000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFFFEFDFBF7L;
      curFlip = 0x0000000001020408L & ((0x0000000000020000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000000000FF00L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000000400L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000400L;
          }
  }

  private static class Flipper11 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x00000000001C1400L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000000000FF00L & ((0x0000000000001000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFF7FBFDFEFF7FBL;
      curFlip = 0x0000804020100804L & ((0x0000000000100000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = 0x0808080808080808L & ((0x0000000000080000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFEFDFBF7EFL;
      curFlip = 0x0000000102040810L & ((0x0000000000040000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000000000FF00L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000000000800L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000000800L;
          }
  }

  private static class Flipper12 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000382800L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000000000FF00L & ((0x0000000000002000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFF7FBFDFEFF7L;
      curFlip = 0x0000008040201008L & ((0x0000000000200000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = 0x1010101010101010L & ((0x0000000000100000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFEFDFBF7EFDFL;
      curFlip = 0x0000010204081020L & ((0x0000000000080000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000000000FF00L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000000001000L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000001000L;
          }
  }

  private static class Flipper13 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000705000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000000000FF00L & ((0x0000000000004000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFF7FBFDFEFL;
      curFlip = 0x0000000080402010L & ((0x0000000000400000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = 0x2020202020202020L & ((0x0000000000200000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFEFDFBF7EFDFBFL;
      curFlip = 0x0001020408102040L & ((0x0000000000100000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000000000FF00L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000000000002000L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000002000L;
          }
  }

  private static class Flipper14 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000602000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = 0x4040404040404040L & ((0x0000000000400000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = 0x0102040810204080L & ((0x0000000000200000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000000000FF00L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0000000000004000L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000004000L;
          }
  }

  private static class Flipper15 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000000C04000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = 0x8080808080808080L & ((0x0000000000800000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFBF7EFDFBF7FFFL;
      curFlip = 0x0204081020408000L & ((0x0000000000400000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000000000FF00L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0000000000008000L, 1);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000008000L;
          }
  }

  private static class Flipper16 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000003020300L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000000000FF0000L & ((0x0000000000020000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFEFF7FBFDFEFFFFL;
      curFlip = 0x2010080402010000L & ((0x0000000002000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = 0x0101010101010101L & ((0x0000000001000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFFFFFEFDFBL;
      curFlip = opponent & ((((player & 0x0000000000010204L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000010000L, 7);
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = opponent & ((((player & 0x0101010101010101L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000010000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000010000L;
          }
  }

  private static class Flipper17 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000006040600L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000000000FF0000L & ((0x0000000000040000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xBFDFEFF7FBFDFEFFL;
      curFlip = 0x4020100804020100L & ((0x0000000004000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = 0x0202020202020202L & ((0x0000000002000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFFFEFDFBF7L;
      curFlip = opponent & ((((player & 0x0000000001020408L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000020000L, 7);
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = opponent & ((((player & 0x0202020202020202L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000020000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000020000L;
          }
  }

  private static class Flipper18 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x000000000E0A0E00L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000000000FF0000L & ((0x0000000000080000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = 0x8040201008040201L & ((0x0000000008000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = 0x0404040404040404L & ((0x0000000004000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFEFDFBF7EFL;
      curFlip = 0x0000000102040810L & ((0x0000000002000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000000000FF0000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000040000L, 1);
      
      mask = opponent | 0xFFFFFFFEFDFBF7EFL;
      curFlip = opponent & ((((player & 0x0000000102040810L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000040000L, 7);
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = opponent & ((((player & 0x0404040404040404L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000040000L, 8);
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = opponent & ((((player & 0x8040201008040201L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000040000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000040000L;
          }
  }

  private static class Flipper19 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x000000001C141C00L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000000000FF0000L & ((0x0000000000100000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFF7FBFDFEFF7FBFDL;
      curFlip = 0x0080402010080402L & ((0x0000000010000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = 0x0808080808080808L & ((0x0000000008000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFEFDFBF7EFDFL;
      curFlip = 0x0000010204081020L & ((0x0000000004000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000000000FF0000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000000080000L, 1);
      
      mask = opponent | 0xFFFFFEFDFBF7EFDFL;
      curFlip = opponent & ((((player & 0x0000010204081020L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000080000L, 7);
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = opponent & ((((player & 0x0808080808080808L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000080000L, 8);
      
      mask = opponent | 0xFF7FBFDFEFF7FBFDL;
      curFlip = opponent & ((((player & 0x0080402010080402L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000080000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000080000L;
          }
  }

  private static class Flipper20 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000038283800L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000000000FF0000L & ((0x0000000000200000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFF7FBFDFEFF7FBL;
      curFlip = 0x0000804020100804L & ((0x0000000020000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = 0x1010101010101010L & ((0x0000000010000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFEFDFBF7EFDFBFL;
      curFlip = 0x0001020408102040L & ((0x0000000008000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000000000FF0000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000000100000L, 1);
      
      mask = opponent | 0xFFFEFDFBF7EFDFBFL;
      curFlip = opponent & ((((player & 0x0001020408102040L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000100000L, 7);
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = opponent & ((((player & 0x1010101010101010L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000100000L, 8);
      
      mask = opponent | 0xFFFF7FBFDFEFF7FBL;
      curFlip = opponent & ((((player & 0x0000804020100804L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000100000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000100000L;
          }
  }

  private static class Flipper21 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000070507000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000000000FF0000L & ((0x0000000000400000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFF7FBFDFEFF7L;
      curFlip = 0x0000008040201008L & ((0x0000000040000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = 0x2020202020202020L & ((0x0000000020000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = 0x0102040810204080L & ((0x0000000010000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000000000FF0000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000000000200000L, 1);
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = opponent & ((((player & 0x0102040810204080L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000200000L, 7);
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = opponent & ((((player & 0x2020202020202020L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000200000L, 8);
      
      mask = opponent | 0xFFFFFF7FBFDFEFF7L;
      curFlip = opponent & ((((player & 0x0000008040201008L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000200000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000200000L;
          }
  }

  private static class Flipper22 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000060206000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = 0x4040404040404040L & ((0x0000000040000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFBF7EFDFBF7FFFL;
      curFlip = 0x0204081020408000L & ((0x0000000020000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000000000FF0000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0000000000400000L, 1);
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = opponent & ((((player & 0x4040404040404040L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000400000L, 8);
      
      mask = opponent | 0xFFFFFFFF7FBFDFEFL;
      curFlip = opponent & ((((player & 0x0000000080402010L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000400000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000400000L;
          }
  }

  private static class Flipper23 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x00000000C040C000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = 0x8080808080808080L & ((0x0000000080000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBF7EFDFBF7FFFFFL;
      curFlip = 0x0408102040800000L & ((0x0000000040000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000000000FF0000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0000000000800000L, 1);
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = opponent & ((((player & 0x8080808080808080L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000800000L, 8);
      
      mask = opponent | 0xFFFFFFFFFF7FBFDFL;
      curFlip = opponent & ((((player & 0x0000000000804020L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000000800000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000000800000L;
          }
  }

  private static class Flipper24 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000302030000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000FF000000L & ((0x0000000002000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFF7FBFDFEFFFFFFL;
      curFlip = 0x1008040201000000L & ((0x0000000200000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = 0x0101010101010101L & ((0x0000000100000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFFFEFDFBF7L;
      curFlip = opponent & ((((player & 0x0000000001020408L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000001000000L, 7);
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = opponent & ((((player & 0x0101010101010101L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000001000000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000001000000L;
          }
  }

  private static class Flipper25 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000604060000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000FF000000L & ((0x0000000004000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFEFF7FBFDFEFFFFL;
      curFlip = 0x2010080402010000L & ((0x0000000400000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = 0x0202020202020202L & ((0x0000000200000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFEFDFBF7EFL;
      curFlip = opponent & ((((player & 0x0000000102040810L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000002000000L, 7);
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = opponent & ((((player & 0x0202020202020202L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000002000000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000002000000L;
          }
  }

  private static class Flipper26 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000000E0A0E0000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000FF000000L & ((0x0000000008000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xBFDFEFF7FBFDFEFFL;
      curFlip = 0x4020100804020100L & ((0x0000000800000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = 0x0404040404040404L & ((0x0000000400000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFEFDFBF7EFDFL;
      curFlip = 0x0000010204081020L & ((0x0000000200000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000FF000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000004000000L, 1);
      
      mask = opponent | 0xFFFFFEFDFBF7EFDFL;
      curFlip = opponent & ((((player & 0x0000010204081020L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000004000000L, 7);
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = opponent & ((((player & 0x0404040404040404L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000004000000L, 8);
      
      mask = opponent | 0xBFDFEFF7FBFDFEFFL;
      curFlip = opponent & ((((player & 0x4020100804020100L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000004000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000004000000L;
          }
  }

  private static class Flipper27 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000001C141C0000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000FF000000L & ((0x0000000010000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = 0x8040201008040201L & ((0x0000001000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = 0x0808080808080808L & ((0x0000000800000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFEFDFBF7EFDFBFL;
      curFlip = 0x0001020408102040L & ((0x0000000400000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000FF000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000008000000L, 1);
      
      mask = opponent | 0xFFFEFDFBF7EFDFBFL;
      curFlip = opponent & ((((player & 0x0001020408102040L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000008000000L, 7);
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = opponent & ((((player & 0x0808080808080808L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000008000000L, 8);
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = opponent & ((((player & 0x8040201008040201L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000008000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000008000000L;
          }
  }

  private static class Flipper28 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000003828380000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000FF000000L & ((0x0000000020000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFF7FBFDFEFF7FBFDL;
      curFlip = 0x0080402010080402L & ((0x0000002000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = 0x1010101010101010L & ((0x0000001000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = 0x0102040810204080L & ((0x0000000800000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000FF000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000010000000L, 1);
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = opponent & ((((player & 0x0102040810204080L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000010000000L, 7);
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = opponent & ((((player & 0x1010101010101010L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000010000000L, 8);
      
      mask = opponent | 0xFF7FBFDFEFF7FBFDL;
      curFlip = opponent & ((((player & 0x0080402010080402L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000010000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000010000000L;
          }
  }

  private static class Flipper29 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000007050700000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00000000FF000000L & ((0x0000000040000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFF7FBFDFEFF7FBL;
      curFlip = 0x0000804020100804L & ((0x0000004000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = 0x2020202020202020L & ((0x0000002000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFBF7EFDFBF7FFFL;
      curFlip = 0x0204081020408000L & ((0x0000001000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000FF000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000000020000000L, 1);
      
      mask = opponent | 0xFDFBF7EFDFBF7FFFL;
      curFlip = opponent & ((((player & 0x0204081020408000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000020000000L, 7);
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = opponent & ((((player & 0x2020202020202020L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000020000000L, 8);
      
      mask = opponent | 0xFFFF7FBFDFEFF7FBL;
      curFlip = opponent & ((((player & 0x0000804020100804L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000020000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000020000000L;
          }
  }

  private static class Flipper30 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000006020600000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = 0x4040404040404040L & ((0x0000004000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBF7EFDFBF7FFFFFL;
      curFlip = 0x0408102040800000L & ((0x0000002000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000FF000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0000000040000000L, 1);
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = opponent & ((((player & 0x4040404040404040L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000040000000L, 8);
      
      mask = opponent | 0xFFFFFF7FBFDFEFF7L;
      curFlip = opponent & ((((player & 0x0000008040201008L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000040000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000040000000L;
          }
  }

  private static class Flipper31 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x000000C040C00000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = 0x8080808080808080L & ((0x0000008000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7EFDFBF7FFFFFFFL;
      curFlip = 0x0810204080000000L & ((0x0000004000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00000000FF000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0000000080000000L, 1);
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = opponent & ((((player & 0x8080808080808080L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000080000000L, 8);
      
      mask = opponent | 0xFFFFFFFF7FBFDFEFL;
      curFlip = opponent & ((((player & 0x0000000080402010L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000080000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000080000000L;
          }
  }

  private static class Flipper32 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000030203000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000FF00000000L & ((0x0000000200000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7FBFDFEFFFFFFFFL;
      curFlip = 0x0804020100000000L & ((0x0000020000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = 0x0101010101010101L & ((0x0000010000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFFFEFDFBF7EFL;
      curFlip = opponent & ((((player & 0x0000000102040810L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000100000000L, 7);
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = opponent & ((((player & 0x0101010101010101L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000100000000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000100000000L;
          }
  }

  private static class Flipper33 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000060406000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000FF00000000L & ((0x0000000400000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFF7FBFDFEFFFFFFL;
      curFlip = 0x1008040201000000L & ((0x0000040000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = 0x0202020202020202L & ((0x0000020000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFEFDFBF7EFDFL;
      curFlip = opponent & ((((player & 0x0000010204081020L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000200000000L, 7);
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = opponent & ((((player & 0x0202020202020202L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000200000000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000200000000L;
          }
  }

  private static class Flipper34 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x00000E0A0E000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000FF00000000L & ((0x0000000800000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFEFF7FBFDFEFFFFL;
      curFlip = 0x2010080402010000L & ((0x0000080000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = 0x0404040404040404L & ((0x0000040000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFEFDFBF7EFDFBFL;
      curFlip = 0x0001020408102040L & ((0x0000020000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000FF00000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000400000000L, 1);
      
      mask = opponent | 0xFFFEFDFBF7EFDFBFL;
      curFlip = opponent & ((((player & 0x0001020408102040L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000400000000L, 7);
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = opponent & ((((player & 0x0404040404040404L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000400000000L, 8);
      
      mask = opponent | 0xDFEFF7FBFDFEFFFFL;
      curFlip = opponent & ((((player & 0x2010080402010000L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000000400000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000400000000L;
          }
  }

  private static class Flipper35 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x00001C141C000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000FF00000000L & ((0x0000001000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xBFDFEFF7FBFDFEFFL;
      curFlip = 0x4020100804020100L & ((0x0000100000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = 0x0808080808080808L & ((0x0000080000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = 0x0102040810204080L & ((0x0000040000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000FF00000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000800000000L, 1);
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = opponent & ((((player & 0x0102040810204080L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000800000000L, 7);
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = opponent & ((((player & 0x0808080808080808L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000000800000000L, 8);
      
      mask = opponent | 0xBFDFEFF7FBFDFEFFL;
      curFlip = opponent & ((((player & 0x4020100804020100L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000000800000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000000800000000L;
          }
  }

  private static class Flipper36 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000382838000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000FF00000000L & ((0x0000002000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = 0x8040201008040201L & ((0x0000200000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = 0x1010101010101010L & ((0x0000100000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFBF7EFDFBF7FFFL;
      curFlip = 0x0204081020408000L & ((0x0000080000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000FF00000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000001000000000L, 1);
      
      mask = opponent | 0xFDFBF7EFDFBF7FFFL;
      curFlip = opponent & ((((player & 0x0204081020408000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000001000000000L, 7);
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = opponent & ((((player & 0x1010101010101010L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000001000000000L, 8);
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = opponent & ((((player & 0x8040201008040201L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000001000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000001000000000L;
          }
  }

  private static class Flipper37 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000705070000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x000000FF00000000L & ((0x0000004000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFF7FBFDFEFF7FBFDL;
      curFlip = 0x0080402010080402L & ((0x0000400000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = 0x2020202020202020L & ((0x0000200000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBF7EFDFBF7FFFFFL;
      curFlip = 0x0408102040800000L & ((0x0000100000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000FF00000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000002000000000L, 1);
      
      mask = opponent | 0xFBF7EFDFBF7FFFFFL;
      curFlip = opponent & ((((player & 0x0408102040800000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000002000000000L, 7);
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = opponent & ((((player & 0x2020202020202020L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000002000000000L, 8);
      
      mask = opponent | 0xFF7FBFDFEFF7FBFDL;
      curFlip = opponent & ((((player & 0x0080402010080402L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000002000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000002000000000L;
          }
  }

  private static class Flipper38 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000602060000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = 0x4040404040404040L & ((0x0000400000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7EFDFBF7FFFFFFFL;
      curFlip = 0x0810204080000000L & ((0x0000200000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000FF00000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0000004000000000L, 1);
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = opponent & ((((player & 0x4040404040404040L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000004000000000L, 8);
      
      mask = opponent | 0xFFFF7FBFDFEFF7FBL;
      curFlip = opponent & ((((player & 0x0000804020100804L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000004000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000004000000000L;
          }
  }

  private static class Flipper39 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0000C040C0000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = 0x8080808080808080L & ((0x0000800000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFDFBF7FFFFFFFFFL;
      curFlip = 0x1020408000000000L & ((0x0000400000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x000000FF00000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0000008000000000L, 1);
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = opponent & ((((player & 0x8080808080808080L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000008000000000L, 8);
      
      mask = opponent | 0xFFFFFF7FBFDFEFF7L;
      curFlip = opponent & ((((player & 0x0000008040201008L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000008000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000008000000000L;
          }
  }

  private static class Flipper40 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0003020300000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000FF0000000000L & ((0x0000020000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBFDFEFFFFFFFFFFL;
      curFlip = 0x0402010000000000L & ((0x0002000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = 0x0101010101010101L & ((0x0001000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFFFEFDFBF7EFDFL;
      curFlip = opponent & ((((player & 0x0000010204081020L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000010000000000L, 7);
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = opponent & ((((player & 0x0101010101010101L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000010000000000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000010000000000L;
          }
  }

  private static class Flipper41 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0006040600000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000FF0000000000L & ((0x0000040000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7FBFDFEFFFFFFFFL;
      curFlip = 0x0804020100000000L & ((0x0004000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = 0x0202020202020202L & ((0x0002000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFEFDFBF7EFDFBFL;
      curFlip = opponent & ((((player & 0x0001020408102040L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000020000000000L, 7);
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = opponent & ((((player & 0x0202020202020202L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000020000000000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000020000000000L;
          }
  }

  private static class Flipper42 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x000E0A0E00000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000FF0000000000L & ((0x0000080000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFF7FBFDFEFFFFFFL;
      curFlip = 0x1008040201000000L & ((0x0008000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = 0x0404040404040404L & ((0x0004000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = 0x0102040810204080L & ((0x0002000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000FF0000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000040000000000L, 1);
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = opponent & ((((player & 0x0102040810204080L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000040000000000L, 7);
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = opponent & ((((player & 0x0404040404040404L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000040000000000L, 8);
      
      mask = opponent | 0xEFF7FBFDFEFFFFFFL;
      curFlip = opponent & ((((player & 0x1008040201000000L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000040000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000040000000000L;
          }
  }

  private static class Flipper43 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x001C141C00000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000FF0000000000L & ((0x0000100000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFEFF7FBFDFEFFFFL;
      curFlip = 0x2010080402010000L & ((0x0010000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = 0x0808080808080808L & ((0x0008000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFBF7EFDFBF7FFFL;
      curFlip = 0x0204081020408000L & ((0x0004000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000FF0000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000080000000000L, 1);
      
      mask = opponent | 0xFDFBF7EFDFBF7FFFL;
      curFlip = opponent & ((((player & 0x0204081020408000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000080000000000L, 7);
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = opponent & ((((player & 0x0808080808080808L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000080000000000L, 8);
      
      mask = opponent | 0xDFEFF7FBFDFEFFFFL;
      curFlip = opponent & ((((player & 0x2010080402010000L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000080000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000080000000000L;
          }
  }

  private static class Flipper44 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0038283800000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000FF0000000000L & ((0x0000200000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xBFDFEFF7FBFDFEFFL;
      curFlip = 0x4020100804020100L & ((0x0020000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = 0x1010101010101010L & ((0x0010000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFBF7EFDFBF7FFFFFL;
      curFlip = 0x0408102040800000L & ((0x0008000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000FF0000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000100000000000L, 1);
      
      mask = opponent | 0xFBF7EFDFBF7FFFFFL;
      curFlip = opponent & ((((player & 0x0408102040800000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0000100000000000L, 7);
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = opponent & ((((player & 0x1010101010101010L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000100000000000L, 8);
      
      mask = opponent | 0xBFDFEFF7FBFDFEFFL;
      curFlip = opponent & ((((player & 0x4020100804020100L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0000100000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000100000000000L;
          }
  }

  private static class Flipper45 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0070507000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x0000FF0000000000L & ((0x0000400000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = 0x8040201008040201L & ((0x0040000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = 0x2020202020202020L & ((0x0020000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xF7EFDFBF7FFFFFFFL;
      curFlip = 0x0810204080000000L & ((0x0010000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000FF0000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000200000000000L, 1);
      
      mask = opponent | 0xF7EFDFBF7FFFFFFFL;
      curFlip = opponent & ((((player & 0x0810204080000000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0000200000000000L, 7);
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = opponent & ((((player & 0x2020202020202020L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000200000000000L, 8);
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = opponent & ((((player & 0x8040201008040201L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000200000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000200000000000L;
          }
  }

  private static class Flipper46 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0060206000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = 0x4040404040404040L & ((0x0040000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xEFDFBF7FFFFFFFFFL;
      curFlip = 0x1020408000000000L & ((0x0020000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000FF0000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0000400000000000L, 1);
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = opponent & ((((player & 0x4040404040404040L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000400000000000L, 8);
      
      mask = opponent | 0xFF7FBFDFEFF7FBFDL;
      curFlip = opponent & ((((player & 0x0080402010080402L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000400000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000400000000000L;
          }
  }

  private static class Flipper47 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x00C040C000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = 0x8080808080808080L & ((0x0080000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xDFBF7FFFFFFFFFFFL;
      curFlip = 0x2040800000000000L & ((0x0040000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x0000FF0000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0000800000000000L, 1);
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = opponent & ((((player & 0x8080808080808080L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000800000000000L, 8);
      
      mask = opponent | 0xFFFF7FBFDFEFF7FBL;
      curFlip = opponent & ((((player & 0x0000804020100804L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0000800000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0000800000000000L;
          }
  }

  private static class Flipper48 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0002030000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00FF000000000000L & ((0x0002000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFFFEFDFBF7EFDFBFL;
      curFlip = opponent & ((((player & 0x0001020408102040L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0001000000000000L, 7);
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = opponent & ((((player & 0x0101010101010101L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0001000000000000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0001000000000000L;
          }
  }

  private static class Flipper49 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0004060000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00FF000000000000L & ((0x0004000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = opponent & ((((player & 0x0102040810204080L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0002000000000000L, 7);
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = opponent & ((((player & 0x0202020202020202L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0002000000000000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0002000000000000L;
          }
  }

  private static class Flipper50 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x000A0E0000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00FF000000000000L & ((0x0008000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00FF000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0004000000000000L, 1);
      
      mask = opponent | 0xFDFBF7EFDFBF7FFFL;
      curFlip = opponent & ((((player & 0x0204081020408000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0004000000000000L, 7);
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = opponent & ((((player & 0x0404040404040404L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0004000000000000L, 8);
      
      mask = opponent | 0xF7FBFDFEFFFFFFFFL;
      curFlip = opponent & ((((player & 0x0804020100000000L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0004000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0004000000000000L;
          }
  }

  private static class Flipper51 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x00141C0000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00FF000000000000L & ((0x0010000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00FF000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0008000000000000L, 1);
      
      mask = opponent | 0xFBF7EFDFBF7FFFFFL;
      curFlip = opponent & ((((player & 0x0408102040800000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0008000000000000L, 7);
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = opponent & ((((player & 0x0808080808080808L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0008000000000000L, 8);
      
      mask = opponent | 0xEFF7FBFDFEFFFFFFL;
      curFlip = opponent & ((((player & 0x1008040201000000L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0008000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0008000000000000L;
          }
  }

  private static class Flipper52 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0028380000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00FF000000000000L & ((0x0020000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00FF000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0010000000000000L, 1);
      
      mask = opponent | 0xF7EFDFBF7FFFFFFFL;
      curFlip = opponent & ((((player & 0x0810204080000000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0010000000000000L, 7);
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = opponent & ((((player & 0x1010101010101010L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0010000000000000L, 8);
      
      mask = opponent | 0xDFEFF7FBFDFEFFFFL;
      curFlip = opponent & ((((player & 0x2010080402010000L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0010000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0010000000000000L;
          }
  }

  private static class Flipper53 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0050700000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0x00FF000000000000L & ((0x0040000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00FF000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0020000000000000L, 1);
      
      mask = opponent | 0xEFDFBF7FFFFFFFFFL;
      curFlip = opponent & ((((player & 0x1020408000000000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0020000000000000L, 7);
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = opponent & ((((player & 0x2020202020202020L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0020000000000000L, 8);
      
      mask = opponent | 0xBFDFEFF7FBFDFEFFL;
      curFlip = opponent & ((((player & 0x4020100804020100L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0020000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0020000000000000L;
          }
  }

  private static class Flipper54 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0020600000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00FF000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0040000000000000L, 1);
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = opponent & ((((player & 0x4040404040404040L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0040000000000000L, 8);
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = opponent & ((((player & 0x8040201008040201L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0040000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0040000000000000L;
          }
  }

  private static class Flipper55 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0040C00000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0x00FF000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0080000000000000L, 1);
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = opponent & ((((player & 0x8080808080808080L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0080000000000000L, 8);
      
      mask = opponent | 0xFF7FBFDFEFF7FBFDL;
      curFlip = opponent & ((((player & 0x0080402010080402L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0080000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0080000000000000L;
          }
  }

  private static class Flipper56 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0203000000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0xFF00000000000000L & ((0x0200000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFEFDFBF7EFDFBF7FL;
      curFlip = opponent & ((((player & 0x0102040810204080L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0100000000000000L, 7);
      
      mask = opponent | 0xFEFEFEFEFEFEFEFEL;
      curFlip = opponent & ((((player & 0x0101010101010101L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0100000000000000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0100000000000000L;
          }
  }

  private static class Flipper57 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0406000000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0xFF00000000000000L & ((0x0400000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent | 0xFDFBF7EFDFBF7FFFL;
      curFlip = opponent & ((((player & 0x0204081020408000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x0200000000000000L, 7);
      
      mask = opponent | 0xFDFDFDFDFDFDFDFDL;
      curFlip = opponent & ((((player & 0x0202020202020202L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0200000000000000L, 8);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0200000000000000L;
          }
  }

  private static class Flipper58 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x0A0E000000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0xFF00000000000000L & ((0x0800000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0xFF00000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0400000000000000L, 1);
      
      mask = opponent | 0xFBF7EFDFBF7FFFFFL;
      curFlip = opponent & ((((player & 0x0408102040800000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x0400000000000000L, 7);
      
      mask = opponent | 0xFBFBFBFBFBFBFBFBL;
      curFlip = opponent & ((((player & 0x0404040404040404L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0400000000000000L, 8);
      
      mask = opponent | 0xFBFDFEFFFFFFFFFFL;
      curFlip = opponent & ((((player & 0x0402010000000000L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x0400000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0400000000000000L;
          }
  }

  private static class Flipper59 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x141C000000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0xFF00000000000000L & ((0x1000000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0xFF00000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0800000000000000L, 1);
      
      mask = opponent | 0xF7EFDFBF7FFFFFFFL;
      curFlip = opponent & ((((player & 0x0810204080000000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x0800000000000000L, 7);
      
      mask = opponent | 0xF7F7F7F7F7F7F7F7L;
      curFlip = opponent & ((((player & 0x0808080808080808L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x0800000000000000L, 8);
      
      mask = opponent | 0xF7FBFDFEFFFFFFFFL;
      curFlip = opponent & ((((player & 0x0804020100000000L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x0800000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x0800000000000000L;
          }
  }

  private static class Flipper60 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x2838000000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0xFF00000000000000L & ((0x2000000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0xFF00000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x1000000000000000L, 1);
      
      mask = opponent | 0xEFDFBF7FFFFFFFFFL;
      curFlip = opponent & ((((player & 0x1020408000000000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells2(curFlip, 0x1000000000000000L, 7);
      
      mask = opponent | 0xEFEFEFEFEFEFEFEFL;
      curFlip = opponent & ((((player & 0x1010101010101010L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x1000000000000000L, 8);
      
      mask = opponent | 0xEFF7FBFDFEFFFFFFL;
      curFlip = opponent & ((((player & 0x1008040201000000L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells3(curFlip, 0x1000000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x1000000000000000L;
          }
  }

  private static class Flipper61 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x5070000000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = 0xFF00000000000000L & ((0x4000000000000000L + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0xFF00000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x2000000000000000L, 1);
      
      mask = opponent | 0xDFBF7FFFFFFFFFFFL;
      curFlip = opponent & ((((player & 0x2040800000000000L) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells1(curFlip, 0x2000000000000000L, 7);
      
      mask = opponent | 0xDFDFDFDFDFDFDFDFL;
      curFlip = opponent & ((((player & 0x2020202020202020L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x2000000000000000L, 8);
      
      mask = opponent | 0xDFEFF7FBFDFEFFFFL;
      curFlip = opponent & ((((player & 0x2010080402010000L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells4(curFlip, 0x2000000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x2000000000000000L;
          }
  }

  private static class Flipper62 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x2060000000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0xFF00000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x4000000000000000L, 1);
      
      mask = opponent | 0xBFBFBFBFBFBFBFBFL;
      curFlip = opponent & ((((player & 0x4040404040404040L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x4000000000000000L, 8);
      
      mask = opponent | 0xBFDFEFF7FBFDFEFFL;
      curFlip = opponent & ((((player & 0x4020100804020100L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells5(curFlip, 0x4000000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x4000000000000000L;
          }
  }

  private static class Flipper63 extends Flipper {
    @Override
    public long run(long player, long opponent) {
      if ((opponent & 0x40C0000000000000L) == 0) {
        return 0;
      }
      long mask;
      long curFlip;
      long flip = 0;
      
      mask = opponent;
      curFlip = opponent & ((((player & 0xFF00000000000000L) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x8000000000000000L, 1);
      
      mask = opponent | 0x7F7F7F7F7F7F7F7FL;
      curFlip = opponent & ((((player & 0x8080808080808080L) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x8000000000000000L, 8);
      
      mask = opponent | 0x7FBFDFEFF7FBFDFEL;
      curFlip = opponent & ((((player & 0x8040201008040201L) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, 0x8000000000000000L, 9);
      flip = flip & opponent;
      return flip == 0 ? 0 : flip | 0x8000000000000000L;
          }
  }

  private static Flipper[] flipper = new Flipper[] {
    new Flipper0(),
    new Flipper1(),
    new Flipper2(),
    new Flipper3(),
    new Flipper4(),
    new Flipper5(),
    new Flipper6(),
    new Flipper7(),
    new Flipper8(),
    new Flipper9(),
    new Flipper10(),
    new Flipper11(),
    new Flipper12(),
    new Flipper13(),
    new Flipper14(),
    new Flipper15(),
    new Flipper16(),
    new Flipper17(),
    new Flipper18(),
    new Flipper19(),
    new Flipper20(),
    new Flipper21(),
    new Flipper22(),
    new Flipper23(),
    new Flipper24(),
    new Flipper25(),
    new Flipper26(),
    new Flipper27(),
    new Flipper28(),
    new Flipper29(),
    new Flipper30(),
    new Flipper31(),
    new Flipper32(),
    new Flipper33(),
    new Flipper34(),
    new Flipper35(),
    new Flipper36(),
    new Flipper37(),
    new Flipper38(),
    new Flipper39(),
    new Flipper40(),
    new Flipper41(),
    new Flipper42(),
    new Flipper43(),
    new Flipper44(),
    new Flipper45(),
    new Flipper46(),
    new Flipper47(),
    new Flipper48(),
    new Flipper49(),
    new Flipper50(),
    new Flipper51(),
    new Flipper52(),
    new Flipper53(),
    new Flipper54(),
    new Flipper55(),
    new Flipper56(),
    new Flipper57(),
    new Flipper58(),
    new Flipper59(),
    new Flipper60(),
    new Flipper61(),
    new Flipper62(),
    new Flipper63(),
  };
  
  public static long getFlip(int move, long player, long opponent) {
//    if (Math.random() < 0.0001) {
//      System.out.println(move + " " + player + " " + opponent);
//    }
    return flipper[move].run(player, opponent);
  }
  
  public static long getFlipGeneric(int move, long player, long opponent) {
    long mask;
    long curFlip;
    long flip = 0;
    int rowNumber = move >>> 3;
    int colNumber = move & 7;
    long row = BitPattern.LAST_ROW_BIT_PATTERN << (rowNumber << 3);  // move / 8
    long column = BitPattern.LAST_COLUMN_BIT_PATTERN << colNumber; // move % 8
    long moveBit = 1L << move;
    long diag7Number = 7 - rowNumber - colNumber;
    long diag7;
    if (diag7Number > 0) {
      diag7 = BitPattern.MAIN_DIAG7_BIT_PATTERN >>> (diag7Number << 3);
    } else {
      diag7 = BitPattern.MAIN_DIAG7_BIT_PATTERN << ((-diag7Number) << 3);
    }
    long diag9Number = rowNumber - colNumber;
    long diag9;
    if (diag9Number > 0) {
      diag9 = BitPattern.MAIN_DIAG9_BIT_PATTERN << (diag9Number << 3);
    } else {
      diag9 = BitPattern.MAIN_DIAG9_BIT_PATTERN >>> ((-diag9Number) << 3);
    }    

    // Left.
    if (colNumber <= 6) {
      mask = opponent;
      curFlip = row & (((moveBit << 1) + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
    }

    // Left up.
    if (colNumber <= 6 && rowNumber <= 6) {
      mask = opponent | ~diag9;
      curFlip = diag9 & (((moveBit << 9) + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
    }

    // Up.
    if (rowNumber <= 6) {
      mask = opponent | ~column;
      curFlip = column & (((moveBit << 8) + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
    }

    // Right up
    if (colNumber >= 2 && rowNumber <= 6) {
      mask = opponent | ~diag7;
      curFlip = diag7 & (((moveBit << 7) + mask) ^ mask);
      flip |= ((curFlip & player) == 0) ? 0 : curFlip;
    }

    // Right.
    if (colNumber >= 2) {
      mask = opponent;
      curFlip = opponent & ((((player & row) << 1) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, moveBit, 1);
    }

    if (colNumber <= 6 && rowNumber >= 2) {
      // Left down.
      mask = opponent | ~diag7;
      curFlip = opponent & ((((player & diag7) << 7) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, moveBit, 7);
    }

    if (rowNumber >= 2) {
      // Down.
      mask = opponent | ~column;
      curFlip = opponent & ((((player & column) << 8) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, moveBit, 8);
    }

    if (colNumber >= 2 && rowNumber >= 2) {
      // Right down.
      mask = opponent | ~diag9;
      curFlip = opponent & ((((player & diag9) << 9) + mask) ^ mask);
      flip |= getDisksRightOfCells6(curFlip, moveBit, 9);
    }

    flip = flip & opponent;
//    if (flip == 0) {
//      nZeros++;
//    }
//    nTot++;
//    System.out.println(nZeros / nTot);
    return flip == 0 ? 0 : flip | moveBit;
  }
  
  private static int nZeros = 0;
  private static double nTot = 0;
  

  public static String getFlipCodeInterface() {
    String result = "private static Flipper[] flipper = new Flipper[] {\n";
    for (int i = 0; i < 64; ++i) {
      result += String.format("  new Flipper%d(),\n", i);
    }
    result += "};\n";
    return indent(result, 2) + "\n  public static long getFlip(int move, long player, long opponent) {\n    return flipper[move].run(player, opponent);\n  }";
  }

  public static String getFlipCode() {
    String result = "switch (move) {\n";
    for (int i = 0; i < 64; ++i) {
      result += String.format("  case %d:\n    return getFlip%d(player, opponent);\n", i, i);
    }
    result += "  default:\n    assert(false);\n}\nreturn 0;";
    return "  public static long getFlip(int move, long player, long opponent) {\n" + indent(result, 4) + "\n  }";
  }
  
  public static int maxNFlips(int n, int dir) {
    int dirHoriz = (dir + 16) % 8;
    dirHoriz = dirHoriz == 7 ? -1 : dirHoriz;
    for (int i = 0; ; ++i) {
      if (n + i * dir < 0 || n + i * dir > 63 || n % 8 + i * dirHoriz < 0 || n % 8 + i * dirHoriz > 7) {
        return i - 2;
      }
    }
  }
  
  public static String getFlipNCode(int n) {
    String result = "long mask;\nlong curFlip;\nlong flip = 0;\n";
    long firstCaseInEachDir = 0;

    for (int dir : new int[] {1, 9, 8, 7, -1, -7, -8, -9}) {
      int maxNFlips = maxNFlips(n, dir);
      if (maxNFlips <= 0) {
        continue;
      }
      long line = 0;
      switch (Math.abs(dir)) {
        case 1:
          line = BitPattern.getRow(n);
          break;
        case 7:
          line = BitPattern.getDiag7(n);
          break;
        case 8:          
          line = BitPattern.getColumn(n);
          break;
        case 9:
          line = BitPattern.getDiag9(n);
          break;
      }
      long firstCaseInDir = 1L << (n + dir);
      firstCaseInEachDir |= firstCaseInDir;
      result += "\n";
//      result += String.format("if ((opponent & 0x%016XL) != 0) {\n", firstCaseInDir);
      if (Math.abs(dir) == 1) {
        result += "mask = opponent;\n";
      } else {
        result += String.format("mask = opponent | 0x%016XL;\n", ~line);
      }
      if (dir > 0) {
        result += String.format("curFlip = 0x%016XL & ((0x%016XL + mask) ^ mask);\n", line, firstCaseInDir);
        result += "flip |= ((curFlip & player) == 0) ? 0 : curFlip;\n";
      } else {
        result += String.format("curFlip = opponent & ((((player & 0x%016XL) << %d) + mask) ^ mask);\n", line, -dir);
        result += String.format("flip |= getDisksRightOfCells%d(curFlip, 0x%016XL, %d);\n", maxNFlips, 1L << n, -dir);
      }
//      result += "}\n";
    }
    result += "flip = flip & opponent;\n";
    result += String.format("return flip == 0 ? 0 : flip | 0x%016XL;\n", 1L << n);
    result = String.format("if ((opponent & 0x%016XL) == 0) {\n  return 0;\n}\n", firstCaseInEachDir) + result;
    
    return "  private static class Flipper" + n + " extends Flipper {\n" +
           "    @Override\n" +
           "    public long run(long player, long opponent) {\n" +
           indent(result, 6) +
           "    }\n" +
           "  }\n";
//    return "  private static long getFlip" + n + "(long player, long opponent) {\n" + indent(result, 4) + "\n  }";
  }
  
  public static String indent(String str, int n) {
    String indent = new String(new char[n]).replace("\0", " ");
    return indent + str.replace("\n", "\n" + indent);
  }
  
  public static void main(String args[]) {
    for (int i = 0; i < 64; ++i) {
      System.out.println(getFlipNCode(i));
    }
    System.out.println(getFlipCodeInterface());
//      System.out.println(getFlipNCode(63));
    
//    GetFlip flipper = new GetFlip();
    
    long n = 400000000L;
//    ThreadMXBean thread = ManagementFactory.getThreadMXBean();

    long tmp = 0;
    long t = -System.currentTimeMillis();	
//    long cpuT = thread.getCurrentThreadCpuTime();
    
    for (long i = 10000; i > 0; --i) {
      Board b = Board.randomBoard(0.5, 0.5);
      long player = b.getPlayer();
      long opponent = b.getOpponent();
      for (long j = n / 10000; j > 0; --j) {
//        tmp = tmp ^ GetMoves.getMoves(player | (i*j), opponent & ~(i*j));
//        tmp = tmp ^ GetMoves.getFlipGeneric((int) (j % 63), player | (i*j), opponent & ~(i*j));// (int) (i % 11));
        tmp = tmp ^ GetMoves.getFlip((int) (0), player | (i*j), opponent & ~(i*j));// (int) (i % 11));
//      tmp = tmp ^ getFlip(63, player | (2*i), opponent & ~(2*i));// (int) (i % 11));
//      tmp = tmp ^ getFlip(63, player | (4*i), opponent & ~(4*i));// (int) (i % 11));
//      tmp = tmp ^ getFlip(player | i, opponent & ~i, 1);
//      tmp = tmp ^ getFlip(player | i, opponent & ~i, 2);
//      tmp = tmp ^ getFlip(player | i, opponent & ~i, 8);
//      tmp = tmp ^ getFlip(player | i, opponent & ~i, 9);
      }
    }
//    cpuT += thread.getCurrentThreadCpuTime();
    t += System.currentTimeMillis();
    System.out.println(t);
    System.out.println(n * 1000.0 / t);
//    System.out.println(cpuT);
//    System.out.println(n * 1000000000.0 / cpuT);
    System.out.println(BitPattern.patternToString(tmp));
  }
  
//  public static void main(String args[]) {
//    GetFlip flipper = GetFlip.load();
//    
//    Board b = Board.randomBoard();
//    long player = b.getPlayer();
//    long opponent = b.getOpponent();
//    GetMoves getMoves = new GetMoves();
//    
//    long n = 400000000L;
//    long t = -System.currentTimeMillis();	
//    ThreadMXBean thread = ManagementFactory.getThreadMXBean();
//
//    long cpuT = thread.getCurrentThreadCpuTime();
//    long tmp = 0;
//    
//    for (long i = n; i > 0; --i) {
//      tmp = tmp ^ flipper.getFlip(63, player | i, opponent & ~i);
//    }
//    cpuT += thread.getCurrentThreadCpuTime();
//    t += System.currentTimeMillis();
//    System.out.println(t);
//    System.out.println(n * 1000.0 / t);
//    System.out.println(cpuT);
//    System.out.println(n * 1000000000.0 / cpuT);
//    System.out.println(BitPattern.patternToString(tmp));
//  }
}

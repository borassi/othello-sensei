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

import java.io.Serializable;

import bitpattern.BitPattern;
import bitpattern.PositionIJ;
import jni.JNI;

import java.util.ArrayList;

public class Board {

  /**
   * The disks of the current player, as a sequence of bit.
   * If the disk in position (i,j) is of the current player,
   * the bit 8*i+j is set to 1 (i is the row, from 0 to 7
   * and j is the column).
   */
  private long player;
  /**
   * The same as player, for the opponent.
   */
  private long opponent;
  /**
   * A string that represents the initial board
   */
  final static String START_BOARD_STRING = "--------\n"
                                         + "--------\n" 
                                         + "--------\n"
                                         + "---OX---\n"
                                         + "---XO---\n"
                                         + "--------\n"
                                         + "--------\n"
                                         + "--------\n";

  /**
   * @return The bitpattern corresponding to the player.
   */
  public long getPlayer() {
    return player;
  }

  /**
   * @return The bitpattern corresponding to the opponent.
   */
  public long getOpponent() {
    return opponent;
  }
  
  /**
   * @return a deep copy of this board.
   */
  public Board deepCopy() {
    return new Board(player, opponent);
  }

  @Override
  public boolean equals(Object otherBoard) {
    if (!Board.class.isAssignableFrom(otherBoard.getClass())) {
      return false;
    }
    final Board other = (Board) otherBoard;
    return player == other.player && opponent == other.opponent;
  }

  @Override
  public int hashCode() {
    long playerHash = (player ^ (player >>> 32));
    long opponentHash = (opponent ^ (opponent >>> 32));
    return (int) (playerHash ^ (opponentHash << 3)) & Integer.MAX_VALUE;
  }
  
  /**
   * The starting board.
   */
  public Board() {
    this(START_BOARD_STRING, true);
  }
  
  /**
   * Creates a board with the given black and white disks.
   * 
   * @param player
   * @param opponent
   */
  public Board(long player, long opponent) {
    this.player = player;
    this.opponent = opponent;
  }
  
  /**
   * Returns a board after a sequence.
   * COMPUTATIONALLY EXPENSIVE; USE ONLY IN TESTS.
   *
   * @param sequence The sequence (e.g., "e6f4c3c4d3").
   */
  public Board(String sequence) {
    Board b = new Board();
    for (int i = 0; i < sequence.length(); i += 2) {
      b = b.move(sequence.substring(i, i+2));
    }
    this.player = b.player;
    this.opponent = b.opponent;
  }
  
  /**
   * Loads a board from a string. First, removes the newlines, then it iterates
   * over the string. Uses 'X' for a black disk, 'O' for a white disk, anything
   * else for an empty square. Example: 
   * "--------\n" 
   * "--------\n" 
   * "--------\n"
   * "---OX---\n" 
   * "---XO---\n" 
   * "--------\n" 
   * "--------\n" 
   * "--------\n"
   * 
   * @param boardString the board
   * @param blackTurn if True, we assume black has to play
   */
  public Board(String boardString, boolean blackTurn) {
    long black = BitPattern.parsePattern(boardString, 'X');
    long white = BitPattern.parsePattern(boardString, 'O');
    player = blackTurn ? black : white;
    opponent = blackTurn ? white : black;
  }

  public String toString(boolean blackTurn) {
    String boardString = "";
    long black = blackTurn ? player : opponent;
    long white = blackTurn ? opponent : player;

    for (int i = 0; i < 64; i++) {
      if (black >>> 63 == 1) {
        boardString += "X";
      } else if (white >>> 63 == 1) {
        boardString += "O";
      } else {
        boardString += "-";
      }
      black = black << 1;
      white = white << 1;
      if (i % 8 == 7) {
        boardString += "\n";
      }
    }
    return boardString;
  }

  @Override
  public String toString() {
    return toString(true);
  }

  /**
   * Returns the board after a move.
   * @param move a long whose bitwise expansion is '1' on the case where
   * the player played and on each flipped disk.
   * @return the next board.
   */
  public Board move(long move) {
    return new Board(opponent & ~move, player | move);
  }

   /**
   * Returns a board after a move.
   *
   * @param move The move (e.g., "e6").
   */
  public Board move(String move) {
    return JNI.move(this, new PositionIJ(move).toMove());
  }

  public char getCase(int square, boolean blackTurn) {
    if (square < 0 || square > 63) {
      return '?';
    }
    if ((player & 1L << square) != 0) {
      return blackTurn ? 'X' : 'O';
    }
    if ((opponent & 1L << square) != 0) {
      return blackTurn ? 'O' : 'X';
    }
    return '-';
  }
  
  public int getPlayerDisks() {
    return Long.bitCount(player);
  }
  
  public int getOpponentDisks() {
    return Long.bitCount(opponent);
  }
  
  public int getEmptySquares() {
    return Long.bitCount(~(player | opponent));
  }
  
  public int getFullSquares() {
    return Long.bitCount(player | opponent);
  }

}

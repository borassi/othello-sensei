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
import java.util.ArrayList;

public class Board implements Serializable {

  /**
   * Needed to implement Serializable
   */
  private static final long serialVersionUID = 1L;
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
   * A string that represents the board after the first move
   */
  final static String E6 = "--------\n"
                         + "--------\n" 
                         + "--------\n"
                         + "---XO---\n"
                         + "---OO---\n"
                         + "----O---\n"
                         + "--------\n"
                         + "--------\n";
  /**
   * Perpendicolar opening
   */
  final static String E6F4 = "--------\n"
                           + "--------\n" 
                           + "--------\n"
                           + "---OOO--\n"
                           + "---XX---\n"
                           + "----X---\n"
                           + "--------\n"
                           + "--------\n";
  /**
   * Diagonal opening
   */
  final static String E6F6 = "--------\n"
                           + "--------\n" 
                           + "--------\n"
                           + "---OX---\n"
                           + "---XO---\n"
                           + "----XO--\n"
                           + "--------\n"
                           + "--------\n";
  /**
   * Parallel opening
   */
  final static String E6D6 = "--------\n"
                           + "--------\n" 
                           + "--------\n"
                           + "---OX---\n"
                           + "---OX---\n"
                           + "---OX---\n"
                           + "--------\n"
                           + "--------\n";
  /**
   * Board where player has to pass
   */
  final static String PASS = "--------\n"
                           + "--------\n" 
                           + "--------\n"
                           + "--------\n"
                           + "------XX\n"
                           + "------OO\n"
                           + "---XOOOO\n"
                           + "OOOOOOOO\n";
  /**
   * Board where both player and opponent have to pass
   */
  final static String BOTHPASS = "----XXXX\n"
                               + "--------\n" 
                               + "--------\n"
                               + "--------\n"
                               + "--------\n"
                               + "------OO\n"
                               + "---OOOOO\n"
                               + "OOOOOOOO\n";
  

  public void setToOtherBoard(Board newBoard) {
    this.player = newBoard.player;
    this.opponent = newBoard.opponent;
  }

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
    int newPlayer = (int) ((player + (player >> 32)) * 541725397157L);
    int newOpponent = (int) ((opponent + (opponent >> 32)) * 541725397157L);
    //System.out.println((int) (player + (player >> 32)) + " " + (3 * newPlayer + 17 * newOpponent));
    return 3 * newPlayer + 17 * newOpponent;
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
    PossibleMovesFinderImproved pmf = new PossibleMovesFinderImproved();
    Board b = new Board();
    for (int i = 0; i < sequence.length(); i += 2) {
      b = pmf.moveIfPossible(b, sequence.substring(i, i+2));
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
  
  /**
   * The starting board.
   */
  public Board() {
    this(START_BOARD_STRING, true);
  }
  
  public static Board start() {
    return new Board(START_BOARD_STRING, true);
  }
  
  public static Board e6() {
    return new Board(E6, true);
  }
  
  public static Board e6d6() {
    return new Board(E6D6, true);
  }
  
  public static Board e6f4() {
    return new Board(E6F4, true);
  }
  
  
  public static Board e6f6() {
    return new Board(E6F6, true);
  }
  
  
  public static Board pass() {
    return new Board(PASS, true);
  }

  public static Board bothPass() {
    return new Board(BOTHPASS, true);
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
  
  public Board[] allTranspositions() {
    return allTranspositions(this);
  }
  
  public final static Board[] allTranspositions(Board b) {
    Board result[] = new Board[8];
    for (int i = 0; i < 8; i++) {
      result[i] = new Board(0, 0);
    }
    allTranspositions(b.getPlayer(), b.getOpponent(), result);
    return result;
  }
  
  public final static void allTranspositions(Board b, Board result[]) {
    allTranspositions(b.player, b.opponent, result);
  }
  /**
   * @param player the player
   * @param opponent the opponent
   * @param result an array used to store the output
   */
  public final static void allTranspositions(long player, long opponent, Board result[]) {
    int j = 4;
    for (int i = 0; i < 4; i++) {
      result[i].player = player;
      result[i].opponent = opponent;
      player = BitPattern.horizontalMirror(player);
      opponent = BitPattern.horizontalMirror(opponent);
      result[j].player = player;
      result[j++].opponent = opponent;
      player = BitPattern.transpose(player);
      opponent = BitPattern.transpose(opponent);
    }
  }
  
  public Board[] horizontalSymmetries() {
    return new Board[]{this, new Board(
      BitPattern.horizontalMirror(player), BitPattern.horizontalMirror(opponent))};
  }
  
  public Board[] allRotations() {
    return allRotations(this);
  }
  
  public final static Board[] allRotations(Board orig) {
    Board r2 = new Board(0, 0);
    Board r3 = new Board(0, 0);
    Board r4 = new Board(0, 0);
    
    allRotations(orig, r2, r3, r4);
    return new Board[] {orig, r2, r3, r4};
  }  
  /**
   * Computes all rotation of the board.
   */
  public final static void allRotations(
      Board r1, Board r2, Board r3, Board r4) {    
    long player = r1.player;
    long opponent = r1.opponent;
    player = BitPattern.verticalMirror(player);
    opponent = BitPattern.verticalMirror(opponent);
    player = BitPattern.transpose(player);
    opponent = BitPattern.transpose(opponent);
    r2.player = player;
    r2.opponent = opponent;
    player = BitPattern.verticalMirror(player);
    opponent = BitPattern.verticalMirror(opponent);
    player = BitPattern.transpose(player);
    opponent = BitPattern.transpose(opponent);
    r3.player = player;
    r3.opponent = opponent;
    player = BitPattern.verticalMirror(player);
    opponent = BitPattern.verticalMirror(opponent);
    player = BitPattern.transpose(player);
    opponent = BitPattern.transpose(opponent);
    r4.player = player;
    r4.opponent = opponent;
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
   * Returns the board after a move.
   * @param move a long whose bitwise expansion is '1' on the case where
   * the player played and on each flipped disk.
   */
  public void moveInPlace(long move) {
    long oldPlayer = player;
    player = opponent & ~move;
    opponent = oldPlayer | move;
  }
  
  /**
   * Returns a bitpattern having a '1' only where the move is played.
   * @param move a long whose bitwise expansion is '1' on the case where
   * the player played and on each flipped disk.
   * @return the bitpattern.
   */
  public long moveToBit(long move) {
    return move & ~opponent;
  }

  /**
   * Computes the color of the disk at the given coordinates.
   * The coordinates are integers from 0 to 7 for rows and columns.
   * @param coordinates the coordinates
   * @return the color of the disk, 'O' for white, 'X' for black, '-' for empty.
   */
  public char getCase(int row, int column, boolean blackTurn) {
    if (row < 0 || row > 7 || column < 0 || column > 7) {
      return '?';
    }
    if (((player << (row * 8 + column)) & 1L << 63) == 1L << 63) {
      return blackTurn ? 'X' : 'O';
    }
    if (((opponent << (row * 8 + column)) & 1L << 63) == 1L << 63) {
      return blackTurn ? 'O' : 'X';
    }
    return '-';
  }
  
  /**
   * Computes the color of the disk at the given coordinates.
   * @param coordinates the coordinates
   * @return the color of the disk, 'O' for white, 'X' for black, '-' for empty.
   */
  public char getCase(String coordinates, boolean blackTurn) {
    return getCase(coordinates.charAt(0) - 'a', coordinates.charAt(1) - '1', blackTurn);
  }
  

  /**
   * Changes the disk at the given coordinates to the value provided.
   * @param coordinates the coordinates
   * @param value the color of the disk, 'O' for white, 'X' for black, '-' for empty.
   */
  void setCase(String coordinates, char value, boolean blackTurn) {
    int j = coordinates.charAt(0) - 'a';
    int i = coordinates.charAt(1) - '1';
    setCase(i, j, value, blackTurn);
  }
  
  /**
   * Changes the disk at the given coordinates to the value provided.
   * @param i the row (from 0 to 7)
   * @param j the column (from 0 to 7)
   * @param value the color of the disk, 'O' for white, 'X' for black, '-' for empty.
   */
  void setCase(int i, int j, char value, boolean blackTurn) {
    long bitmask = BitPattern.moveToBit(i, j);
    if (value == '-') {
      player = player & ~bitmask;
      opponent = opponent & ~bitmask;
    } else if ((value == 'X' && blackTurn) || (value == 'O' && !blackTurn)) {
      player = player | bitmask;
      opponent = opponent & ~bitmask;
    } else {
      player = player & ~bitmask;
      opponent = opponent | bitmask;      
    }
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
  
  /**
   * Returns a board that is equal to this in the bits that correspond
   * to '1' in the filter, and is 0 elsewhere.
   * @param filter the filter
   * @return the final board
   */
  public Board filter(long filter) {
    return new Board(player & filter, opponent & filter);
  }
  
  public static String randomBoardString(double percBlack, double percWhite) {
    String boardString = "";
    for (int j = 0; j < 64; j++) {
      double rand = Math.random();
      if (rand < percBlack) {
        boardString += "X";
      } else if (rand < percBlack + percWhite) {
        boardString += "O";
      } else {
        boardString += "-";
      }
      if (j % 8 == 7) {
        boardString += "\n";
      }
    }
    return boardString;
  }

  public static Board randomBoard(double percBlack, double percWhite) {
    return new Board(randomBoardString(percBlack, percWhite), Math.random() > 0.5);
  }

  public static Board randomBoard() {
    double percBlack = Math.random();
    double percWhite = Math.random();
    double tot = percBlack + percWhite + Math.random();
    
    
    return new Board(randomBoardString(percBlack / tot, percWhite / tot), Math.random() > 0.5);
  }
  
  
  /*
   * Returns all the possible boards that are empty outside the bitPattern
   * provided.
   * @param bitPattern the bit pattern.
   * @return an ArrayList with all the possible boards.
   */
  public static ArrayList<Board> existingBoardsInBitPattern(long bitPattern) {
    return existingBoardsInBitPattern(bitPattern, new Board(0, 0), 0);
  }
  
  /**
   * Recursive version of existingBoardsInBitPattern(long bitPattern).
   * @param bitPattern the bit pattern.
   * @param startingBoard the cases already filled.
   * @param currentCase the next case to be filled.
   * @return an ArrayList with all the possible boards.
   */
  private static ArrayList<Board> existingBoardsInBitPattern(
          long bitPattern, Board startingBoard, int currentCase) {
    if (currentCase >= 64) {
      ArrayList<Board> b = new ArrayList<>();
      b.add(startingBoard);
      return b;
    }

    if ((bitPattern & (1L << (63 - currentCase))) == 0) {
      return existingBoardsInBitPattern(bitPattern, startingBoard, currentCase + 1);
    }

    ArrayList<Board> result = new ArrayList<>();
    char possibleCaseValues[] = {'X', 'O', '-'};

    for (char caseValue : possibleCaseValues) {
      Board currentBoard = startingBoard.deepCopy();
      currentBoard.setCase(currentCase / 8, currentCase % 8, caseValue, true);
      result.addAll(existingBoardsInBitPattern(bitPattern, currentBoard, currentCase + 1));
    }
    return result;
  }
}

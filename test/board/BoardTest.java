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

import java.util.ArrayList;
import static org.junit.Assert.*;

import java.util.Arrays;
import java.util.HashSet;
import java.util.List;

import org.junit.Test;

public class BoardTest {
  
  public static <T> void assertEqualsIgnoreOrder(T[] list1, T[] list2) {
    HashSet<T> tmp1 = new HashSet<>(Arrays.asList(list1));
    HashSet<T> tmp2 = new HashSet<>(Arrays.asList(list2));
    assertEquals(tmp1, tmp2);
  }

  public static <T> void assertListEqualsIgnoreOrder(List<T> list1, List<T> list2) {
    assertEquals(list1.size(), list2.size());
    HashSet<T> tmp1 = new HashSet<>(list1);
    HashSet<T> tmp2 = new HashSet<>(list2);
    assertEquals(tmp1, tmp2);
  }
  
  public static String randomBoardString() {
    String boardString = "";
    for (int j = 0; j < 64; j++) {
      int rand = (int) (Math.random() * 3);
      if (rand == 0) {
        boardString += "-";
      } else if (rand == 1) {
        boardString += "O";
      } else {
        boardString += "X";
      }
      if (j % 8 == 7) {
        boardString += "\n";
      }
    }
    return boardString;
  }

  public static Board randomBoard() {
    return new Board(randomBoardString(), Math.random() > 0.5);
  }
  
  final static String secondMoveBoardString = "--------\n"
                                            + "--------\n" 
                                            + "--------\n"
                                            + "---OX---\n"
                                            + "---XX---\n"
                                            + "----X---\n"
                                            + "--------\n"
                                            + "--------\n";
  
  @Test
  public void testAllTranspositions() {
    String orig = "XX------\n"
                + "--------\n" 
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "------OO\n";
    String res1 = "-------X\n"
                + "-------X\n" 
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "O-------\n"
                + "O-------\n";
    String res2 = "OO------\n"
                + "--------\n" 
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "------XX\n";
    String res3 = "-------O\n"
                + "-------O\n" 
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "X-------\n"
                + "X-------\n";
    String res4 = "X-------\n"
                + "X-------\n" 
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "-------O\n"
                + "-------O\n";
    String res5 = "------XX\n"
                + "--------\n" 
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "OO------\n";
    String res6 = "O-------\n"
                + "O-------\n" 
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "-------X\n"
                + "-------X\n";
    String res7 = "------OO\n"
                + "--------\n" 
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "--------\n"
                + "XX------\n";
    Board board = new Board(orig, true);
    assertEqualsIgnoreOrder(Board.allTranspositions(board), 
            new Board[]{
                    new Board(orig, true), new Board(res1, true), new Board(res2, true), 
                    new Board(res3, true), new Board(res4, true), new Board(res5, true),
                    new Board(res6, true), new Board(res7, true)});
    Board result[] = new Board[8];
    for (int i = 0; i < 8; i++) {
      result[i] = new Board();
    }
    Board.allTranspositions(board, result);
    assertEqualsIgnoreOrder(result, 
            new Board[]{
                    new Board(orig, true), new Board(res1, true), new Board(res2, true), 
                    new Board(res3, true), new Board(res4, true), new Board(res5, true),
                    new Board(res6, true), new Board(res7, true)});
    assertEqualsIgnoreOrder(Board.allRotations(board), 
            new Board[]{
                    new Board(orig, true), new Board(res1, true), new Board(res2, true), 
                    new Board(res3, true)});
  }

  @Test
  public void boardFromString() {
    assertEquals(new Board("e6"), Board.e6());
    assertEquals(new Board("e6f4"), Board.e6f4());
  }

  @Test
  public void testBoardFromString() {
    String boardString = "--------\n"
                       + "--------\n" 
                       + "--------\n"
                       + "---OX---\n"
                       + "---XO---\n"
                       + "--------\n"
                       + "--------\n"
                       + "--------\n";
    Board board = new Board(boardString, true);
    assertEquals(new Board(
            0b0000000000000000000000000000100000010000000000000000000000000000L,
            0b0000000000000000000000000001000000001000000000000000000000000000L), board);
  }
  
  @Test
  public void testBoardToString() {
    String boardString = Board.START_BOARD_STRING;
    Board board = new Board(
            0b0000000000000000000000000000100000010000000000000000000000000000L,
            0b0000000000000000000000000001000000001000000000000000000000000000L);
    assertEquals(boardString, board.toString());
  }
  
  @Test
  public void testRandomBoardToString() {
    for (int i = 0; i < 100; i++) {
      String boardString = randomBoardString();
      boolean blackTurn = Math.random() > 0.5;
      Board board = new Board(boardString, blackTurn);
      assertEquals(boardString, board.toString(blackTurn));
      assertEquals(board, new Board(boardString, blackTurn));
    }
  }

  @Test
  public void testGetCase() {
    String boardString = "X-------\n"
                       + "--------\n" 
                       + "--------\n"
                       + "---OX---\n"
                       + "---XO---\n"
                       + "--------\n"
                       + "--------\n"
                       + "-------O\n";
    
    for (boolean blackTurn : new boolean[]{true, false}) {
      Board board = new Board(boardString, blackTurn);
  
      assertEquals('X', board.getCase("a1", blackTurn));
      assertEquals('-', board.getCase("h1", blackTurn));
      assertEquals('-', board.getCase("b1", blackTurn));
      assertEquals('O', board.getCase("d4", blackTurn));
      assertEquals('O', board.getCase("h8", blackTurn));
      assertEquals('?', board.getCase("i5", blackTurn));
      assertEquals('?', board.getCase("c9", blackTurn));
    }
  }

  @Test
  public void testSetCase() {
    Board board = new Board(Board.START_BOARD_STRING, true);
    
    board.setCase("e5", 'X', true);
    board.setCase("e6", 'X', true);
    
    assertEquals(board, new Board(secondMoveBoardString, true));
    
    board.setCase("e5", 'O', true);
    board.setCase("e6", '-', true);
    
    assertEquals(board, new Board(Board.START_BOARD_STRING, true));
  }
  
  int countOccurrencesInString(String s, char c) {
    int result = 0;
    for (char i : s.toCharArray()) {
      if (i == c) {
        result++;
      }
    }
    return result;
  }
  
  @Test
  public void testCountDisks() {
    for (int i = 0; i < 10000; i++) {
      Board b = randomBoard();

      int playerDisks = countOccurrencesInString(b.toString(), 'X');
      assertEquals(b.getPlayerDisks(), playerDisks);
      int opponentDisks = countOccurrencesInString(b.toString(), 'O');
      assertEquals(b.getOpponentDisks(), opponentDisks);
      int emptySquares = countOccurrencesInString(b.toString(), '-');
      assertEquals(b.getEmptySquares(), emptySquares);
      int fullSquares = playerDisks + opponentDisks;
      assertEquals(b.getFullSquares(), fullSquares);
    }
  }
  @Test
  public void numberExistingBoardsInBitPatternTest() {
    PossibleMovesFinderImproved finder = new PossibleMovesFinderImproved();
    finder.initBitPatterns();
    
    for (long bitPattern : finder.bitPatterns) {
      ArrayList<Board> boards = Board.existingBoardsInBitPattern(bitPattern);
      assertEquals((int) Math.pow(3., Long.bitCount(bitPattern)), boards.size());
    }
  }

  @Test
  public void existingBoardsInBitPatternTest() {
    PossibleMovesFinderImproved finder = new PossibleMovesFinderImproved();
    finder.initBitPatterns();
    
    for (long bitPattern : finder.bitPatterns) {
      ArrayList<Board> boards = Board.existingBoardsInBitPattern(bitPattern);
      for (int i = 0; i < 1000; i++) {
        Board b = Board.randomBoard();
        assert(boards.contains(b.filter(bitPattern)));
      }
      assertEquals((int) Math.pow(3., Long.bitCount(bitPattern)), boards.size());
    }
  }
}

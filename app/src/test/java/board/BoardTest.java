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

  @Test
  public void boardFromString() {
    assertEquals(new Board("e6"), new Board("e6"));
    assertEquals(new Board("e6f4"), new Board("e6f4"));
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
}

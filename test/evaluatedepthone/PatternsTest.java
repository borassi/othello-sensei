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

package evaluatedepthone;

import static org.junit.Assert.*;

import org.junit.Test;

import board.Board;
import evaluatedepthone.Patterns;

public class PatternsTest {  
  @Test
  public void testGetPattern() {
    String boardString = "XXXXXXXX\n"
                       + "OOOOOOOO\n" 
                       + "XXXXXXXX\n"
                       + "OOOOOOOO\n"
                       + "XXXXXXXX\n"
                       + "OOOOOOOO\n"
                       + "XXXXXXXX\n"
                       + "OOOOOOOO\n";
    Board board = new Board(boardString, true);

    String resultSide = "--------\n"
                      + "--------\n"
                      + "--------\n"
                      + "--------\n"
                      + "--------\n"
                      + "--------\n"
                      + "-X----X-\n"
                      + "OOOOOOOO\n";
    assertEquals(new Board(resultSide, true), 
            Patterns.getPattern(board, Patterns.patternSide, 0));
    
    String resultCorner = "--------\n"
                        + "--------\n" 
                        + "--------\n"
                        + "--------\n"
                        + "--------\n"
                        + "-----OOO\n"
                        + "-----XXX\n"
                        + "-----OOO\n";
    assertEquals(new Board(resultCorner, true),
            Patterns.getPattern(board, Patterns.patternCornerThree, 0));
    
    String resultCenter = "--------\n"
                        + "--------\n" 
                        + "--------\n"
                        + "--------\n"
                        + "--------\n"
                        + "--OOOO--\n"
                        + "--XXXX--\n"
                        + "--------\n";
    assertEquals(new Board(resultCenter, true), 
            Patterns.getPattern(board, Patterns.patternCenter, 0));
  }

  @Test
  public void testGetPatternWithMask() {
    String boardString = "--------\n"
                       + "--------\n" 
                       + "--------\n" 
                       + "--------\n" 
                       + "--------\n" 
                       + "--------\n" 
                       + "--XXXX--\n" 
                       + "-OOOOOO-\n";
    Board board = new Board(boardString, true);

    String boardMaskedString = "--------\n"
                             + "--------\n" 
                             + "--------\n" 
                             + "--------\n" 
                             + "--------\n" 
                             + "--------\n" 
                             + "--XXXX--\n" 
                             + "OOOOOOO-\n";
    Board boardMasked = new Board(boardMaskedString, true);
  
    String result = "--------\n"
                  + "--------\n" 
                  + "--------\n" 
                  + "--------\n" 
                  + "--------\n" 
                  + "--------\n" 
                  + "--XXXX--\n" 
                  + "-OOOOOO-\n";
    assertEquals(new Board(result, true),
            Patterns.getPattern(board, Patterns.patternEdge, Patterns.maskEdge));
    assertEquals(new Board(0, 0), Patterns.getPattern(boardMasked, Patterns.patternEdge, 
            Patterns.maskEdge));
  }
}

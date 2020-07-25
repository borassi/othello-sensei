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
package endgametest;

import board.Board;
import board.PossibleMovesFinderImproved;
import constants.Constants;
import evaluatedepthone.PatternEvaluatorImproved;
import evaluateposition.EvaluatorMCTS;
import evaluateposition.EvaluatorMidgame;
import evaluateposition.HashMap;

public class EndgameTest {
  public final static String[] POSITIONS = {
//          "XXXXXXXOOOOOXXXOXOOOXXOX-OXOXOXXOXXXOXXXXXXOXOXXXXXXOOO--OOOOOX- X", 
          "XXXXXXXOOOOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO-- X", 
          "--XXXXX--OOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO-- X", 
          "-XXXXXX---XOOOO--XOXXOOX-OOOOOOOOOOOXXOOOOOXXOOX--XXOO----XXXXX- X", 
          "----OX----OOXX---OOOXX-XOOXXOOOOOXXOXXOOOXXXOOOOOXXXXOXO--OOOOOX X", 
          "-XXXXXX-X-XXXOO-XOXXXOOXXXOXOOOX-OXOOXXX--OOOXXX--OOXX----XOXXO- X", 
          "-OOOOO----OXXO-XXXOXOXX-XXOXOXXOXXOOXOOOXXXXOO-OX-XOOO---XXXXX-- X", 
          "--OXXX--OOOXXX--OOOXOXO-OOXOOOX-OOXXXXXXXOOXXOX--OOOOX---XXXXXX- X", 
          "--OXXO--XOXXXX--XOOOXXXXXOOXXXXXXOOOOXXX-XXXXXXX--XXOOO----XXOO- X", 
          "---X-X--X-XXXX--XXXXOXXXXXXOOOOOXXOXXXO-XOXXXXO-XOOXXX--XOOXXO-- O",
          "--XOXX--O-OOXXXX-OOOXXXX-XOXXXOXXXOXOOOXOXXOXOXX--OXOO----OOOO-- O",
          "-XXXX-----OXXX--XOXOXOXXOXOXXOXXOXXOXOOOXXXOXOOX--OXXO---OOOOO-- O",
          "---O-XOX----XXOX---XXOOXO-XXOXOXXXXOOXOX-XOOXXXXXOOOXX-XOOOOOOO- O",
          "--O--O--X-OOOOX-XXOOOXOOXXOXOXOOXXOXXOOOXXXXOOOO--OXXX---XXXXX-- O",
          "--XXXXX--OOOXX---OOOXXXX-OXOXOXXOXXXOXXX--XOXOXX--OXOOO--OOOOO-- X", 
          "--XXXXX---OOOX---XOOXXXX-OOOOOOOOOOXXXOOOOOXXOOX--XXOO----XXXXX- X", 
          "----O------OOX---OOOXX-XOOOXOOOOOXXOXXOOOXXXOOOOOXXXOOXO--OOOOOX X", 
          "-XXXXXX-X-XXXOO-XOXXXOOXXOOXXXOX-OOOXXXX--OOXXXX---OOO----XOX-O- X", 
          "-OOOOO----OXXO-XXXOOOXX-XXOXOXXOXXOOXOOOXXXXOO-OX-XOO----XXXX--- X", 
          "-XXX------OOOX--XOOOOOXXOXOXOOXXOXXOOOOOXXXOXOOX--OXXO---OOOOO-- X", 
          "--OXXO--XOXXXX--XOOOOXXXXOOOXXXXX-OOOXXX--OOOOXX--XXOOO----XXOO- X", 
          "XXXOXXXXOXXXXXXXOOXXXXXXOOOXXXXXOOOXXOO-OOOOO---OOOOOOO-OOOOOOO- X",
          "OOOOOOOOXOOXXX--XXOOXOO-XOXOOO--XOOOOX--XOOXOO--XOOOOO--XXXX---- O",
          "--OOOO--X-OOOOO-XXOOXOXXXOXOXXXXXXXOXXXX-XXOXOXX--OXXX-X----X--- O",
          "--O-------OOX---OOOXXXO-OOOOXOXXXXXOOXOXXXXXXOOXX-XXXXOX--XXXX-- X", // 24
          "--O--O-----OOOX--X-XOXOO--XXXOOOXXXXOOOOXXXOXXOOXXXXXX--XOXX-O-- O",
          "----X------XXXO--OOOXXXXXOOOOXXO-XXOOXXOOOXOXXXXOOOXX---X-XXXX-- O",
          "-OOOOO----OXXO---OOOOXXO-OOOXOXX-OOXOOXX-XOXXOXX--O-XXXX--O----O X",
          "--XO-O----OOOO--OOXOXXO-OOOOXXOOOOOXXOX-OXOXXXXX--XXXX----X-O-X- X",
          "--O-------OOO--X-XOOOOXXXXXXOXOX-XXOXOOXXXOXOOXX-OOOOO-X---OOO-- X", // 29
          "-OXXXX----OXXO--XXOOXOOOXXXOOXOOXXOOXOOOXXXXOO-XX-XXO----------- X",
          "-XXX----X-XOO---XXOXOO--XOXOXO--XOOXOXXXXOOXXOX---OOOOO--XXXXX-- X",
          "-OOOOO----OOOO--OXXOOO---XXXOO--XXXXXXO-XXXOOO-OX-OOOO---OOOOO-- X",
          "--XX----O-XXOX--OOXOO---OXOXOOO-OOXXOOOXOOXXXOOX--XXXXOX--X--X-X X",
          "-XXXXXXX--XOOO----OXOOXX-OOXXOXX-OOOOOXX-X-XOOXX---O-X-X--OOOO-- X",
          "-------------O-O-OOOOOOOOOOOOXOOOXXOOOXO-XXXOXOO--XXXOXO--OXXXXO X",
          "--XXX-----XXXX-OOOXXOOOOOOOOOOXO-OOXXXXO-OOOXXXO---XOXX---X----- O",
          "---X-O----XXXO-XXXXXXXXXXOOXXOOXXOXOOOXXXXOOOO-XX--OOOO--------- O", // 37
          "--OOOO--O-OOOO--OXXXOOO-OXXOXO--OOXXOXX-OOXXXX--O-XXX-----XX-O-- X",
          "--OOOO----OOOO---XOXXOOXOOXOOOOX-OOOOOXXXOOXXXXX--X-X----------- X",
          "O-OOOO--XOXXOX--XOOOXXX-XOOOXX--XOOXOX--XOXXX---X-XX------------ O", // 40
          "O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X",
          "-OOOOO----OOOOX--OOOOOO-XXXXXOO--XXOOX--OOXOXX----OXXO---OOO--O- X",
          "--OOO-------XX-OOOOOOXOO-OOOOXOOX-OOOXXO---OOXOO---OOOXO--OOOO-- X", // 43
          "--XXXXX---XXXX---OOOXX---OOXXXX--OOXXXO-OOOOXOO----XOX----XXXXX- O",
          "--O-X-O---O-XO-O-OOXXXOOOOOOXXXOOOOOXX--XXOOXO----XXXX-----XXX-- O",
          "---XXXX-X-XXXO--XXOXOO--XXXOXO--XXOXXO---OXXXOO-O-OOOO------OO-- X", // 46
          "---XXX----OOOX----OOOXX--OOOOXXX--OOOOXX--OXOXXX--XXOO---XXXX-O- X",
          "-OOOOO----OOOO---OOOOX--XXXXXX---OXOOX--OOOXOX----OOXX----XXXX-- O",
          "-----X--X-XXX---XXXXOO--XOXOOXX-XOOXXX--XOOXX-----OOOX---XXXXXX- O",
          "--OX-O----XXOO--OOOOOXX-OOOOOX--OOOXOXX-OOOOXX-----OOX----X-O--- X", // 50
          "----X-----XXX----OOOXOOO-OOOXOOO-OXOXOXO-OOXXOOO--OOXO----O--O-- X",
          "----O-X------X-----XXXO-OXXXXXOO-XXOOXOOXXOXXXOO--OOOO-O----OO-- O",
          "---X-------OX--X--XOOXXXXXXOXXXXXXXOOXXXXXXOOOXX--XO---X-------- O",
          "----OO-----OOO---XXXXOOO--XXOOXO-XXXXXOO--OOOXOO--X-OX-O-----X-- X",
          "--OOO---XXOO----XXXXOOOOXXXXOX--XXXOXX--XXOOO------OOO-----O---- X", // 55
          "--------X-X------XXXXOOOOOXOXX--OOOXXXX-OOXXXX--O-OOOX-----OO--- O",
          "--XXXXX---XXXX---OOOXX---OOXOX---OXXXXX-OOOOOXO----OXX---------- O",
          "-------------------XXOOO--XXXOOO--XXOXOO-OOOXXXO--OXOO-O-OOOOO-- X",
          "--XOOO----OOO----OOOXOO--OOOOXO--OXOXXX-OOXXXX----X-XX---------- X",
          "-----------------------O--OOOOO---OOOOOXOOOOXXXX--XXOOXX--XX-O-X X"};
  EvaluatorMidgame evalMidgame = new EvaluatorMidgame(PatternEvaluatorImproved.load(), new HashMap(2 * Constants.HASH_MAP_SIZE, Constants.HASH_MAP_SIZE));
//  EvaluatorMidgame eval = evalMidgame;
  EvaluatorMCTS eval = new EvaluatorMCTS(Constants.MCTS_SIZE, 2 * Constants.MCTS_SIZE, evalMidgame);

  public static Board readIthBoard(int i) {
    String[] boards = POSITIONS[i - 1].split(" ");
    return new Board(boards[0], boards[1].equals("X"));
  }

  public void run() {
    
    System.out.println(" num empties        t       nVisPos   nVisPos/sec   nStored  n/end  n/mid  eval");
    for (int i = 41; i <= 59; i++) { //POSITIONS.length; i++) {
      Board b = readIthBoard(i);
      System.out.print(String.format("%4d", i));
      System.out.print(String.format("%8d", b.getEmptySquares()));
      evalMidgame.resetNVisitedPositions();
      long t = System.currentTimeMillis();
//      int result = evalMidgame.evaluatePosition(b, b.getEmptySquares(), 1, 199);
      int result = -eval.evaluatePosition(b);//, -6400, 6400);
      t = System.currentTimeMillis() - t;
      
//      eval.resetHashMapVisitedPositions();
//////      eval.evaluatePosition(b, b.getEmptySquares() - 2, -64, 64);
//////      eval.evaluatePosition(b, b.getEmptySquares() - 2, result - 2);
//////      eval.evaluatePosition(b, b.getEmptySquares() - 2, result + 2);
//      eval.resetNVisitedPositions();
//      eval.evaluateAlphaBetaWithHashMap(b, b.getEmptySquares(), -64, 64);
//      eval.evaluateAlphaBetaWithHashMap(b, b.getEmptySquares(), result - 0.01F);
//      eval.evaluateAlphaBetaWithHashMap(b, b.getEmptySquares(), result + 0.01F);

//      System.out.print(String.format("%9.3f", t / 1000.));
//      System.out.print(String.format("%14d", evalMidgame.getNVisited()));
//      System.out.print(String.format("%14.0f", evalMidgame.getNVisited() * 1000. / t));
//      System.out.print(String.format("%10d", 0));
//      System.out.print(String.format("%7.0f", evalMidgame.nVisitedEndgames / (double) evalMidgame.nEndgames));
//      System.out.print(String.format("%7.0f", 0.));
//      System.out.println(String.format("%6d", result / 100));
      System.out.print(String.format("%9.3f", t / 1000.));
      System.out.print(String.format("%14d", eval.getNVisited()));
      System.out.print(String.format("%14.0f", eval.getNVisited() * 1000. / t));
      System.out.print(String.format("%10d", eval.getNStored()));
      System.out.print(String.format("%7.0f", evalMidgame.nVisitedEndgames / (double) evalMidgame.nEndgames));
      System.out.print(String.format("%7.0f", eval.nVisitedEndgames / (double) eval.nEndgames));
      System.out.println(String.format("%6d", result / 100));
      evalMidgame.nEndgames = 0;
      evalMidgame.nVisitedEndgames = 0;
    }
  }

  public static void main(String args[]) {
    new EndgameTest().run();
  }
}

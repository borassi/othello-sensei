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
import evaluateposition.EvaluatorLastMoves;
import evaluateposition.EvaluatorMidgame;
import tmp.OptimumAlphaBeta;

public class EndgameUpperBound {
  final static String[] POSITIONS = {
          "XXXXXXXOOOOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOO---OOOOO-- X", 
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
          "--O-------OOX---OOOXXXO-OOOOXOXXXXXOOXOXXXXXXOOXX-XXXXOX--XXXX-- X",
          "--O--O-----OOOX--X-XOXOO--XXXOOOXXXXOOOOXXXOXXOOXXXXXX--XOXX-O-- O",
          "----X------XXXO--OOOXXXXXOOOOXXO-XXOOXXOOOXOXXXXOOOXX---X-XXXX-- O",
          "-OOOOO----OXXO---OOOOXXO-OOOXOXX-OOXOOXX-XOXXOXX--O-XXXX--O----O X",
          "--XO-O----OOOO--OOXOXXO-OOOOXXOOOOOXXOX-OXOXXXXX--XXXX----X-O-X- X",
          "--O-------OOO--X-XOOOOXXXXXXOXOX-XXOXOOXXXOXOOXX-OOOOO-X---OOO-- X",
          "-OXXXX----OXXO--XXOOXOOOXXXOOXOOXXOOXOOOXXXXOO-XX-XXO----------- X",
          "-XXX----X-XOO---XXOXOO--XOXOXO--XOOXOXXXXOOXXOX---OOOOO--XXXXX-- X",
          "-OOOOO----OOOO--OXXOOO---XXXOO--XXXXXXO-XXXOOO-OX-OOOO---OOOOO-- X",
          "--XX----O-XXOX--OOXOO---OXOXOOO-OOXXOOOXOOXXXOOX--XXXXOX--X--X-X X",
          "-XXXXXXX--XOOO----OXOOXX-OOXXOXX-OOOOOXX-X-XOOXX---O-X-X--OOOO-- X",
          "-------------O-O-OOOOOOOOOOOOXOOOXXOOOXO-XXXOXOO--XXXOXO--OXXXXO X",
          "--XXX-----XXXX-OOOXXOOOOOOOOOOXO-OOXXXXO-OOOXXXO---XOXX---X----- O",
          "---X-O----XXXO-XXXXXXXXXXOOXXOOXXOXOOOXXXXOOOO-XX--OOOO--------- O",
          "--OOOO--O-OOOO--OXXXOOO-OXXOXO--OOXXOXX-OOXXXX--O-XXX-----XX-O-- X",
          "--OOOO----OOOO---XOXXOOXOOXOOOOX-OOOOOXXXOOXXXXX--X-X----------- X",
          "O-OOOO--XOXXOX--XOOOXXX-XOOOXX--XOOXOX--XOXXX---X-XX------------ O",
          "O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X",
          "-OOOOO----OOOOX--OOOOOO-XXXXXOO--XXOOX--OOXOXX----OXXO---OOO--O- X",
          "--OOO-------XX-OOOOOOXOO-OOOOXOOX-OOOXXO---OOXOO---OOOXO--OOOO-- X",
          "--XXXXX---XXXX---OOOXX---OOXXXX--OOXXXO-OOOOXOO----XOX----XXXXX- O",
          "--O-X-O---O-XO-O-OOXXXOOOOOOXXXOOOOOXX--XXOOXO----XXXX-----XXX-- O",
          "---XXXX-X-XXXO--XXOXOO--XXXOXO--XXOXXO---OXXXOO-O-OOOO------OO-- X",
          "---XXX----OOOX----OOOXX--OOOOXXX--OOOOXX--OXOXXX--XXOO---XXXX-O- X",
          "-OOOOO----OOOO---OOOOX--XXXXXX---OXOOX--OOOXOX----OOXX----XXXX-- O",
          "-----X--X-XXX---XXXXOO--XOXOOXX-XOOXXX--XOOXX-----OOOX---XXXXXX- O",
          "--OX-O----XXOO--OOOOOXX-OOOOOX--OOOXOXX-OOOOXX-----OOX----X-O--- X",
          "----X-----XXX----OOOXOOO-OOOXOOO-OXOXOXO-OOXXOOO--OOXO----O--O-- X",
          "----O-X------X-----XXXO-OXXXXXOO-XXOOXOOXXOXXXOO--OOOO-O----OO-- O",
          "---X-------OX--X--XOOXXXXXXOXXXXXXXOOXXXXXXOOOXX--XO---X-------- O",
          "----OO-----OOO---XXXXOOO--XXOOXO-XXXXXOO--OOOXOO--X-OX-O-----X-- X",
          "--OOO---XXOO----XXXXOOOOXXXXOX--XXXOXX--XXOOO------OOO-----O---- X",
          "--------X-X------XXXXOOOOOXOXX--OOOXXXX-OOXXXX--O-OOOX-----OO--- O",
          "--XXXXX---XXXX---OOOXX---OOXOX---OXXXXX-OOOOOXO----OXX---------- O",
          "-------------------XXOOO--XXXOOO--XXOXOO-OOOXXXO--OXOO-O-OOOOO-- X",
          "--XOOO----OOO----OOOXOO--OOOOXO--OXOXXX-OOXXXX----X-XX---------- X",
          "-----------------------O--OOOOO---OOOOOXOOOOXXXX--XXOOXX--XX-O-X X"};
  
  
  final static String[] POSITIONS_AFTER_MOVES = {
    "XXXXXXXOXXOOOOOOXXXXXXOXXXXOXOXXOXXXOXXXXXXOXOXO-OXXOOXO-XOOOOX- O",
    "XXXXXXXOXXOOOOOOXXXXXXOX-XXOXOXXOXXXOXXX--XOXOXX-XXXOO---OOOOO-- O",
    "--OOOOO-OOOOOO-XOOXXOOXOOOOXOXOOOOXOXOOOOOOOOXOO-XXOXXX--XXXXX-- O",
    "-OOOOOO-O-OOOOOOOOXOOXOOOOOOOOXXXOXXOOXXXXXOOXXO-XXXXX----OOOOO- O",
    "---OOO--X-XXXOX-XXOOOOOOXOXOXOOXXOOXOOOXXOOOXXOXXOOOOXOX--XXXXXO O",
    "-OOOOOO-O-OOOOOOOXOOOXXOOOXOXXXOOOOXXOXO-OOOOOOO--XXXXX--XXXOOX- O",
    "-OOOOO--OOOOOO-XXOXXOXX-XXOXOXXOXXOOXOOOXXXXOO-OX-XOOO---XXXXX-- O",
    "-XXXXX--OOOOOOO-OXOXOOO-OXXOOOX-OXXXXXXXXXOXXOX--XOOOX---XXXXXX- O",
    "OOOOOO--XOXXXX--XXOOXXXXXXOXXXXXXXOOOXXX-XXXXXXX--XXOOO----XXOO- O",
    "---X-X--X-XXXX--XXXXOXXXXXXOOOXXXXOXOXOXXOXXXOOOXOOOOXO-XOOXXO-- O",
    "---O-O--O-OOOO--OOOOXOOOOOOXXXOOOOXOXOOOOXOOOXO-OXXXXOX-OXXOOX-- X",
    "OOOOOO--X-XXOOOO-XXXOOOO-OXOOOXOOOXOXXXOXOOXOXOO--XOXX----XXXX-- X",
    "-OOOO-----XXXXX-OXOXOXXOXOXOOXXOXOOXOXXXOOOXOXXO-OOOOX---XXXXX-- X",
    "--O--O--X-OOOOOOXXOOOXOOXXOXOXOOXXOXXOOOXXXXOOOO-XXXXO---XXXXXO- O",
    "--XXXXX-XXXXXX--XXOOXXXXXXXOXOXXOXOXOXXX-OOOXOXX--OXOOO--OOOOO-- O",
    "--XXXXX--XXXXX--OXOOXXXXXXXXOOOOOXOOXXOOOOOXXOOX-OOOOO----XXXXX- O",
    "----O-----XXXXX--OOOOXXXOOXXXOXOOOXOOXXOOOXOOOXOOOOXOOXO-OOOOOOX O",
    "-XXXXXX-X-XXXXXXXOXXXOXXXXOXXXOXXXXXXXXX--OOXXXX---OOO---OOOOOO- O",
    "-OOOOO--OOOOOO-XXOXOOOOOXXOXOXOOXXOOXOXOXXXXXXXOX-XOO----XXXX--- O",
    "-XXX----OOOOOOO-OOXOOOOXOXOXOOOXOXXOOOOOXXXOXOOX-XXXXO---OOOOO-- O",
    "OOOOOO--XOOOOOO-XOOXOXOXXOXOXXOXXXXXXXOX--XOOOOX--XXOOO----XXOO- O",
    "XXXOXXXXOXXXXXXXOOXXXXXXOOOXXXXXOOOXXOO-OOOOO---OOOOOOO-OOOOOOO- O",
    "OOOOOOOOXXXXXXOOXXOOXOOOXOXOOX--XOOOXX--XOOXOO--XOOOOO--XXXX---- O",
    "O-OOOOOOO-OOOXO-OXOOXOXXOXOOXXXXOXXXXXXXOXXOXOXX-XXXXX-X----X--- O"};
  
  EvaluatorLastMoves eval = new EvaluatorLastMoves();

  public Board readBoardFromFFOFormat(String ffoFormat) {
    String[] boards = ffoFormat.split(" ");
    return new Board(boards[0], boards[1].equals("X"));
  }

  public void run() {
    System.out.println(" num empties   lower lowEval   upper uppEval   total totEval    actual eval");
    OptimumAlphaBeta.computeOptimumLower(Board.randomBoard(1, 0), 1);
    for (int i = 0; i < POSITIONS_AFTER_MOVES.length; i++) {
      Board b = readBoardFromFFOFormat(POSITIONS_AFTER_MOVES[i]);
      System.out.print(String.format("%4d", i));
      System.out.print(String.format("%8d", b.getEmptySquares()));
      eval.resetNVisited();
      int result = eval.evaluatePosition(b, -6600, 6600, 0);
      long lower = OptimumAlphaBeta.computeOptimumLower(b, (int) result);
      long upper = OptimumAlphaBeta.computeOptimumUpper(b, (int) result);
      long lowerEval = OptimumAlphaBeta.computeOptimumLowerLast(b, (int) result);
      long upperEval = OptimumAlphaBeta.computeOptimumUpperLast(b, (int) result);
//
//      System.out.println(lower);
      System.out.println(String.format("%8d%8d%8d%8d%8d%8d%8d%8d", 
        lower, lowerEval,
        upper, upperEval, 
        lower + upper, lowerEval + upperEval, eval.getNVisited(),
        result));
    }
  }

  public void generateDataSet() {
    PossibleMovesFinderImproved mf = new PossibleMovesFinderImproved();
    EvaluatorMidgame eval = new EvaluatorMidgame();
    for (int i = 0; i < POSITIONS.length; i++) {
      Board b = readBoardFromFFOFormat(POSITIONS[i]);
      while (b.getEmptySquares() > 13) {
        long moves[] = mf.possibleMoves(b);
        if (moves.length == 0) {
          b = b.move(0);
          continue;
        }
        int bestEval = Integer.MIN_VALUE;
        Board best = new Board();
        
        for (long m : moves) {
          Board next = b.move(m);
          int curEval = eval.evaluatePosition(next, 6, -6400, 6400);
          if (curEval > bestEval) {
            bestEval = curEval;
            best = next;
          }
        }
        b = best;
      }
      System.out.println("\"" + b.toString().replace("\n", "") + " X" + "\",");
    }
  }
  
  public static void main(String args[]) {
    new EndgameUpperBound().run();
  }
}

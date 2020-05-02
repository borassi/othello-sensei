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
package evaluateposition;

import static org.junit.Assert.*;

import org.junit.Test;

import bitpattern.BitPattern;
import board.Board;
import board.PossibleMovesFinderImproved;
import constants.Constants;
import evaluateposition.HashMapVisitedPositions.PositionToImprove;
import java.util.ArrayList;

public class HashMapVisitedPositionsTest {
  PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER = new PossibleMovesFinderImproved();

  private short[] createSampleArray(int sample) {
    short[] result = new short[Constants.N_SAMPLES];
    result[0] = (short) sample;
    return result;
  }

  @Test
  public void testStartingPosition() {
    StoredBoard s = new StoredBoard(new Board(), 0, 0);
    s.eval = 123;
    StoredBoard t = new StoredBoard(new Board(12423, 45), 0, 0);
    t.eval = 456;
    HashMapVisitedPositions visitedPositions = new HashMapVisitedPositions(11, 5, POSSIBLE_MOVES_FINDER);
    
    assertEquals(0, visitedPositions.size);
    
    assertEquals(visitedPositions.get(s.player, s.opponent), null);
    assertEquals(visitedPositions.get(s.player, s.opponent), null);
    
    visitedPositions.addFirstPosition(s); 
    assertEquals(visitedPositions.get(s.player, s.opponent).eval, 123);
    
    s.updateEval(789, 0);//, new int[StoredBoard.N_SAMPLES]);
    assertEquals(visitedPositions.get(s.player, s.opponent).eval, 789);

    visitedPositions.add(t); 
    assertEquals(visitedPositions.get(t.player, t.opponent).eval, 456);
  }
  
  @Test
  public void testBoardChildrenAreCorrect() {
    HashMapVisitedPositions positions = new HashMapVisitedPositions(10, 5, POSSIBLE_MOVES_FINDER);

    StoredBoard firstMove = new StoredBoard(Board.e6(), 0, 0);
    positions.firstPosition = firstMove;
    firstMove.playerIsStartingPlayer = true;
    positions.add(firstMove);
    firstMove.lower = -1;
    firstMove.bestVariationPlayer = -3;
    firstMove.upper = 1;
    firstMove.bestVariationOpponent = 9;
    firstMove.descendants = 33;
 
    StoredBoard diag = new StoredBoard(Board.e6f6(), 0, 0);
    positions.add(diag);
    diag.playerIsStartingPlayer = false;
    diag.lower = -1;
    diag.bestVariationPlayer = -5;
    diag.upper = 1;
    diag.bestVariationOpponent = 3;
    diag.fathers.add(firstMove);
    diag.descendants = 10;
    
    StoredBoard perp = new StoredBoard(Board.e6f4(), 2, 0);
    positions.add(perp);
    perp.playerIsStartingPlayer = false;
    perp.samples = createSampleArray(9);
    perp.lower = 2;
    perp.upper = 2;
    perp.bestVariationPlayer = -9;
    perp.bestVariationOpponent = 7;
    perp.fathers.add(firstMove);
    perp.descendants = 11;
 
    StoredBoard par = new StoredBoard(Board.e6d6(), 8, 0);
    positions.add(par);
    par.playerIsStartingPlayer = false;
    par.samples = createSampleArray(8);
    par.lower = 8;
    par.upper = 8;
    par.bestVariationPlayer = 8;
    par.bestVariationOpponent = 8;
    par.fathers.add(firstMove);
    par.descendants = 12;

    firstMove.children = new StoredBoard[]{diag, perp, par};

    assert(positions.boardChildrenAreCorrect(firstMove));

    firstMove.eval = 1;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.eval = 0;
    
    firstMove.lower = 12;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.lower = -1;
    
//    firstMove.bestVariationLower = 12;
//    assert(!positions.boardChildrenAreCorrect(firstMove));
//    firstMove.bestVariationLower = -2;
    
    firstMove.upper = 124;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.upper = 1;
    
//    firstMove.bestVariationUpper = 12;
//    assert(!positions.boardChildrenAreCorrect(firstMove));
//    firstMove.bestVariationUpper = 5;
    
    diag.fathers = new ArrayList<>();
    assert(!positions.boardChildrenAreCorrect(firstMove));
    diag.fathers.add(firstMove);
    
    firstMove.children = new StoredBoard[] {par, perp};
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.children = new StoredBoard[] {par, par, perp};
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.children = new StoredBoard[] {par, par, diag, perp};
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.children = new StoredBoard[] {par, diag, perp};

    firstMove.descendants = 3;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.descendants = 33;
    par.descendants = 0;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    par.descendants = 12;
    
    par.playerIsStartingPlayer = true;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    par.playerIsStartingPlayer = false;
    
    positions.setEvalGoalNoUpdate(-6);
    assert(positions.boardChildrenAreCorrect(firstMove));
    
    firstMove.bestVariationPlayer = -5;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.bestVariationPlayer = -3;

    firstMove.bestVariationOpponent = 8;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.bestVariationOpponent = 9;
  }

  @Test
  public void testBoardIsCorrect() {
    HashMapVisitedPositions positions = new HashMapVisitedPositions(10, 5, POSSIBLE_MOVES_FINDER);
    StoredBoard firstMove = new StoredBoard(Board.e6(), 0, 0);
    StoredBoard start = new StoredBoard(new Board(), 0, 0);
    positions.add(firstMove);
    positions.add(start);
    
    firstMove.fathers.add(start);
    firstMove.eval = 0;

    assert(positions.boardIsCorrect(firstMove));

    firstMove.fathers.add(firstMove);
    assert(!positions.boardIsCorrect(firstMove));
    firstMove.fathers.remove(firstMove.fathers.size() - 1);
  }

  @Test
  public void testBoardChildrenAreCorrectPass() {
    HashMapVisitedPositions positions = new HashMapVisitedPositions(10, 5, POSSIBLE_MOVES_FINDER);
    StoredBoard pass = new StoredBoard(Board.pass(), 200, 0);
    positions.add(pass);

    StoredBoard afterPass = new StoredBoard(Board.pass().move(0), -200, 0);
    positions.add(afterPass);

    pass.children = new StoredBoard[] {afterPass};
    pass.playerIsStartingPlayer = true;
    pass.samples = createSampleArray(0);
    pass.bestVariationPlayer = 500;
    pass.bestVariationOpponent = 300;
//    pass.descendants = 2;

    afterPass.playerIsStartingPlayer = false;
    afterPass.eval = -200;
    afterPass.samples = createSampleArray(0);
    afterPass.bestVariationPlayer = -300;
    afterPass.bestVariationOpponent = -500;
    afterPass.fathers.add(pass);
//    afterPass.descendants = 1;

    assert(positions.boardChildrenAreCorrect(pass));

    pass.eval = 100;
    assert(!positions.boardChildrenAreCorrect(pass));
    pass.eval = -200;

    pass.bestVariationPlayer = 400;
    assert(!positions.boardChildrenAreCorrect(pass));
    pass.bestVariationPlayer = -300;
    
    afterPass.fathers = new ArrayList<>();
    assert(!positions.boardChildrenAreCorrect(pass));
    afterPass.fathers.add(pass);
    
    pass.children = new StoredBoard[0];
    assert(!positions.boardChildrenAreCorrect(pass));
    pass.children = new StoredBoard[] {pass};
    assert(!positions.boardChildrenAreCorrect(pass));
    pass.children = new StoredBoard[] {afterPass};
    
    StoredBoard bothPass = new StoredBoard(Board.bothPass(), -5600, 0);
    positions.add(bothPass);
    bothPass.eval = -5600;
    bothPass.lower = -5600;
    bothPass.upper = -5600;
    bothPass.children = new StoredBoard[0];
    for (int i = 0; i < Constants.N_SAMPLES; i++) {
      bothPass.samples[i] = -5600;
    }
    bothPass.bestVariationPlayer = -6600;
    bothPass.bestVariationOpponent = 6600;
    assert(positions.boardIsCorrect(bothPass));
    assert(positions.boardChildrenAreCorrect(bothPass));
    
    bothPass.eval = 500;
    assert(!positions.boardChildrenAreCorrect(bothPass));
    bothPass.eval = -5600;
    
    bothPass.bestVariationOpponent = 500;
    assert(!positions.boardChildrenAreCorrect(bothPass));
    bothPass.bestVariationOpponent = -5600;
    
    bothPass.children = new StoredBoard[] {bothPass};
    assert(!positions.boardChildrenAreCorrect(bothPass));
    bothPass.children = new StoredBoard[0];
  }
  
  
//  @Test
//  public void testMultipleInsert() {
//    Board e6f4 = Board.e6f4();
//    Board e6f4c3 = POSSIBLE_MOVES_FINDER.moveIfPossible(e6f4, "c3");
//    Board e6f4c3c4 = POSSIBLE_MOVES_FINDER.moveIfPossible(e6f4c3, "c4");
//    Board e6f4c3c4d3 = POSSIBLE_MOVES_FINDER.moveIfPossible(e6f4c3c4, "d3");
//    Board e6f4d3 = POSSIBLE_MOVES_FINDER.moveIfPossible(e6f4, "d3");
//    Board e6f4d3c4 = POSSIBLE_MOVES_FINDER.moveIfPossible(e6f4d3, "c4");
//    
//    HashMapVisitedPositions evaluator = new HashMapVisitedPositions(30, 30, POSSIBLE_MOVES_FINDER);
//    evaluator.setFirstPositionEval(e6f4);
//    evaluator.addPositionsIfPossible(evaluator.get(e6f4));
//    evaluator.addPositionsIfPossible(evaluator.get(e6f4c3));
//    evaluator.addPositionsIfPossible(evaluator.get(e6f4d3));
//    evaluator.addPositionsIfPossible(evaluator.get(e6f4c3c4));
//    
//    StoredBoard doubleFather = evaluator.get(e6f4c3c4d3);
//    assertEquals(1, doubleFather.fathers.size());
//    assertEquals(evaluator.get(e6f4c3c4).toString(), doubleFather.fathers.get(0).toString());
//
//    evaluator.addPositionsIfPossible(evaluator.get(e6f4d3c4));
//    
//    doubleFather = evaluator.get(e6f4c3c4d3);
//    assertEquals(2, doubleFather.fathers.size());
//    assertEquals(evaluator.get(e6f4c3c4).toString(), doubleFather.fathers.get(0).toString());
//    assertEquals(evaluator.get(e6f4d3c4).toString(), doubleFather.fathers.get(1).toString());
//  }

  @Test
  public void testNextPositionToImproveEndgame() {
    for (int i = 0; i < 10000; i++) {
      if (i % 1000 == 0) {
        System.out.println("Done " + i);
      }
      int nElements = 10 + (int) (Math.random() * 10);
      int totalSize = nElements + (int) (Math.random() * 10);

      HashMapVisitedPositions evaluator = new HashMapVisitedPositions(
        totalSize, nElements, POSSIBLE_MOVES_FINDER);
      Board start = Board.randomBoard();
      if (start.getPlayer() == 0 && start.getOpponent() == 0) {
        continue;
      }
      evaluator.addFirstPosition(new StoredBoard(start, 10, 10));
      
      for (int j = 0; j < nElements; j++) {
        PositionToImprove nextPos = evaluator.nextPositionToImproveRandom(true);
        if (nextPos == null) {
          break;
        }
        StoredBoard next = nextPos.board;
        long[] moves = POSSIBLE_MOVES_FINDER.possibleMovesAdvanced(next.player, next.opponent);
        if (moves.length == 0) {
          evaluator.updateSolved(next.player, next.opponent,
              BitPattern.getEvaluationGameOver(next.player, next.opponent));
          continue;
        }
        StoredBoard[] nextBoards = new StoredBoard[moves.length];
        for (int m = 0; m < moves.length; m++) {
          long move = moves[m];
          Board b = next.getBoard().move(move);
          int eval = (int) ((Math.random() - 0.5) * 6400);
          nextBoards[m] = new StoredBoard(b, eval, (int) (Math.random() * 100));
        }
        evaluator.add(nextBoards, next);
        if (!evaluator.allCorrect()) {
          assert(false);
        }
        if (Math.random() < 0.2) {
          evaluator.setEvalGoal((int) (Math.random() * 3200));
        }
        boolean playerVariates = Math.random() > 0.5;
        if (playerVariates && evaluator.firstPosition.bestVariationPlayer == -6600) {
          continue;
        }
        if (!playerVariates && evaluator.firstPosition.bestVariationOpponent == 6600) {
          continue;
        }
        ArrayList<StoredBoard> parents = new ArrayList<>();
        PositionToImprove sb = evaluator.nextPositionToImproveEndgame(evaluator.firstPosition, playerVariates, true, parents);

        for (int p = 0; p < sb.parents.size() - 1; ++p) {
          assert(sb.parents.get(p+1).fathers.contains(sb.parents.get(p)));
        }
        if (playerVariates) {
          if (sb.playerIsStartingPlayer) {
            if (sb.board.bestVariationPlayer != evaluator.firstPosition.bestVariationPlayer) {
              System.out.println(sb.board);
              System.out.println(evaluator.firstPosition);
              System.out.println(playerVariates);
              System.out.println(evaluator);
              assert(false);
            }
          } else {
            if (sb.board.bestVariationOpponent != -evaluator.firstPosition.bestVariationPlayer) {
              System.out.println(sb.board);
              System.out.println(evaluator.firstPosition);
              System.out.println(playerVariates);
              System.out.println(evaluator);
              assert(false);            
            }
          }
        } else {
          if (sb.playerIsStartingPlayer) {
            if (sb.board.bestVariationOpponent != evaluator.firstPosition.bestVariationOpponent) {
              System.out.println(sb.board);
              System.out.println(evaluator.firstPosition);
              System.out.println(playerVariates);
              System.out.println(evaluator);
              assert(false);
            }
          } else {
            if (sb.board.bestVariationPlayer != -evaluator.firstPosition.bestVariationOpponent) {
              System.out.println(sb.board);
              System.out.println(evaluator.firstPosition);
              System.out.println(playerVariates);
              System.out.println(evaluator);
              assert(false);            
            }
          }
        }
      }
    }
  }

  @Test
  public void testTreeIsCorrectAfterUpdates() {
    for (int i = 0; i < 10000; i++) {
      if (i % 1000 == 0) {
        System.out.println("Done " + i);
      }
      int nElements = 5 + (int) (Math.random() * 100);
      int totalSize = nElements + (int) (Math.random() * 100);

      HashMapVisitedPositions evaluator = new HashMapVisitedPositions(
        totalSize, nElements, POSSIBLE_MOVES_FINDER);
      Board start = Board.randomBoard();
      if (start.getPlayer() == 0 && start.getOpponent() == 0) {
        continue;
      }
      evaluator.addFirstPosition(new StoredBoard(start, 10, 10));
      
      for (int j = 0; j < nElements; j++) {
        PositionToImprove nextPos = evaluator.nextPositionToImproveRandom(true);

        if (nextPos == null) {
          break;
        }
        StoredBoard next = nextPos.board;
        double d = Math.random();
        
        int eval = (int) ((Math.random() - 0.5) * 6400);
        if (d <= 0.2) {
          evaluator.updateLower(next.player, next.opponent, eval);
          continue;
        } else if (d <= 0.4) {
          evaluator.updateUpper(next.player, next.opponent, eval);
          continue;
        } else if (d <= 0.6) {
          evaluator.updateSolved(next.player, next.opponent, eval);
          continue;
        }
        long[] moves = POSSIBLE_MOVES_FINDER.possibleMovesAdvanced(next.player, next.opponent);
        if (moves.length == 0) {
          evaluator.updateSolved(next.player, next.opponent,
              BitPattern.getEvaluationGameOver(next.player, next.opponent));
          continue;
        }
        StoredBoard[] nextBoards = new StoredBoard[moves.length];
        for (int m = 0; m < moves.length; m++) {
          long move = moves[m];
          Board b = next.getBoard().move(move);
          eval = (int) ((Math.random() - 0.5) * 6400);
          nextBoards[m] = new StoredBoard(b, eval, (int) (Math.random() * 100));
        }
        evaluator.add(nextBoards, next);
        if (Math.random() < 0.2) {
          evaluator.setEvalGoal((int) (Math.random() * 3200));
        }
        if (!evaluator.allCorrect()) {
          System.out.println(start.toString().replaceAll("\n", ""));
          System.out.println(evaluator);
          assert(false);
        }
      }
      boolean allLeaves = true;
      for (StoredBoard firstBoard : evaluator.evaluationsHashMap) {
        for (StoredBoard board = firstBoard; board != null; board = board.next) {
          if (!board.isLeaf()) {
            allLeaves = false;
            board.eval = (short) (board.eval + 10);
            for (int s = 0; s < Constants.N_SAMPLES; ++s) {
              board.samples[s] = (short) ((Math.random() - 0.5) * 6400);
            }
            board.bestVariationPlayer = (short) ((Math.random() - 0.5) * 6400);
            board.bestVariationOpponent = (short) ((Math.random() - 0.5) * 6400);
          }
        }
      }
      if (!allLeaves) {
//        assert(!evaluator.allCorrect());
        evaluator.updateAllDescendants();
        assert(evaluator.allCorrect());
      }
    }
  }
}
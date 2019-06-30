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
import evaluateposition.HashMapVisitedPositions.PositionToImprove;
import static evaluateposition.StoredBoard.N_SAMPLES;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;

public class HashMapVisitedPositionsTest {
  PossibleMovesFinderImproved POSSIBLE_MOVES_FINDER = new PossibleMovesFinderImproved();

  @Test
  public void testStartingPosition() {
    StoredBoard s = new StoredBoard(new Board(), 0, 0);
    s.eval = 123;
    StoredBoard t = new StoredBoard(new Board(12423, 45), 0, 0);
    t.eval = 456;
    HashMapVisitedPositions visitedPositions = new HashMapVisitedPositions(11, 5, POSSIBLE_MOVES_FINDER);
    
    assertEquals(0, visitedPositions.size());
    
    assertEquals(visitedPositions.get(s.player, s.opponent), null);
    assertEquals(visitedPositions.get(s.player, s.opponent), null);
    
    visitedPositions.addFirstPosition(s); 
    assertEquals(visitedPositions.get(s.player, s.opponent).eval, 123);
    
    s.updateEval(789, 0, new int[StoredBoard.N_SAMPLES]);
    assertEquals(visitedPositions.get(s.player, s.opponent).eval, 789);

    visitedPositions.add(t); 
    assertEquals(visitedPositions.get(t.player, t.opponent).eval, 456);
  }

  private short[] createSampleArray(int sample) {
    short[] result = new short[N_SAMPLES];
    result[0] = (short) sample;
    return result;
  }
  
  @Test
  public void testBoardChildrenAreCorrect() {
    HashMapVisitedPositions positions = new HashMapVisitedPositions(10, 5, POSSIBLE_MOVES_FINDER);

    StoredBoard firstMove = new StoredBoard(Board.e6(), 0, 0);
    positions.add(firstMove);
    firstMove.playerVariates = -2;
    firstMove.opponentVariates = -5;
    firstMove.descendants = 4;
 
    StoredBoard diag = new StoredBoard(Board.e6f6(), 0, 0);
    positions.add(diag);
    diag.playerVariates = 5;
    diag.opponentVariates = 3;
    diag.fathers.add(firstMove);

    StoredBoard perp = new StoredBoard(Board.e6f4(), 2, 0);
    positions.add(perp);
    perp.samples = createSampleArray(9);
    perp.playerVariates = 9;
    perp.opponentVariates = 7;
    perp.fathers.add(firstMove);
 
    StoredBoard par = new StoredBoard(Board.e6d6(), 8, 0);
    positions.add(par);
    par.samples = createSampleArray(8);
    par.playerVariates = 2;
    par.opponentVariates = 2;
    par.fathers.add(firstMove);

    firstMove.children = new StoredBoard[]{diag, perp, par};

    assert(positions.boardChildrenAreCorrect(firstMove));

    firstMove.eval = 1;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.eval = 0;
    
    firstMove.playerVariates = 12;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.playerVariates = -2;
    
    firstMove.opponentVariates = 12;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    firstMove.opponentVariates = -5;
    
    diag.fathers = new ObjectArrayList<>();
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
    firstMove.descendants = 4;
    par.descendants = 0;
    assert(!positions.boardChildrenAreCorrect(firstMove));
    par.descendants = 1;
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
    firstMove.fathers.pop();
  }

  @Test
  public void testBoardChildrenAreCorrectPass() {
    HashMapVisitedPositions positions = new HashMapVisitedPositions(10, 5, POSSIBLE_MOVES_FINDER);
    StoredBoard pass = new StoredBoard(Board.pass(), 200, 0);
    positions.add(pass);
    StoredBoard afterPass = new StoredBoard(Board.pass().move(0), 200, 0);
    positions.add(afterPass);

    pass.children = new StoredBoard[] {afterPass};
    pass.samples = createSampleArray(0);
    pass.playerVariates = 500;
    pass.opponentVariates = 300;
    pass.descendants = 2;

    afterPass.eval = -200;
    afterPass.samples = createSampleArray(0);
    afterPass.playerVariates = -300;
    afterPass.opponentVariates = -500;
    afterPass.fathers.add(pass);
    afterPass.descendants = 1;

    assert(positions.boardChildrenAreCorrect(pass));

    pass.eval = 100;
    assert(!positions.boardChildrenAreCorrect(pass));
    pass.eval = 200;

    pass.playerVariates = 400;
    assert(!positions.boardChildrenAreCorrect(pass));
    pass.playerVariates = -300;
    
    afterPass.fathers = new ObjectArrayList<>();
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
    bothPass.children = new StoredBoard[0];
    for (int i = 0; i < N_SAMPLES; i++) {
      bothPass.samples[i] = -5600;
    }
    bothPass.playerVariates = -6600;
    bothPass.opponentVariates = 6600;
    assert(positions.boardIsCorrect(bothPass));
    assert(positions.boardChildrenAreCorrect(bothPass));
    
    bothPass.eval = 500;
    assert(!positions.boardChildrenAreCorrect(bothPass));
    bothPass.eval = -5600;
    
    bothPass.opponentVariates = 500;
    assert(!positions.boardChildrenAreCorrect(bothPass));
    bothPass.opponentVariates = -5600;
    
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
        boolean playerVariates = Math.random() > 0.5;
        if (playerVariates && evaluator.firstPosition.playerVariates == -6600) {
          continue;
        }
        if (!playerVariates && evaluator.firstPosition.opponentVariates == 6600) {
          continue;
        }
        PositionToImprove sb = evaluator.nextPositionToImproveEndgame(evaluator.firstPosition, playerVariates, true);

        if (playerVariates) {
          if (sb.playerIsStartingPlayer) {
            if (sb.board.playerVariates != evaluator.firstPosition.playerVariates) {
              System.out.println(sb.board);
              System.out.println(evaluator.firstPosition);
              System.out.println(playerVariates);
              System.out.println(evaluator);
              assert(false);
            }
          } else {
            if (sb.board.opponentVariates != -evaluator.firstPosition.playerVariates) {
              System.out.println(sb.board);
              System.out.println(evaluator.firstPosition);
              System.out.println(playerVariates);
              System.out.println(evaluator);
              assert(false);            
            }
          }
        } else {
          if (sb.playerIsStartingPlayer) {
            if (sb.board.opponentVariates != evaluator.firstPosition.opponentVariates) {
              System.out.println(sb.board);
              System.out.println(evaluator.firstPosition);
              System.out.println(playerVariates);
              System.out.println(evaluator);
              assert(false);
            }
          } else {
            if (sb.board.playerVariates != -evaluator.firstPosition.opponentVariates) {
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
      System.out.println("Done " + i);
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
        if (!evaluator.allCorrect()) {
          System.out.println(start.toString().replaceAll("\n", ""));
          System.out.println(evaluator);
          assert(false);
        }
      }
    }
  }
}

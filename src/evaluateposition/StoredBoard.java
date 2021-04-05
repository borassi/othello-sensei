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

import bitpattern.BitPattern;
import board.Board;
import board.GetMoves;
import board.GetMovesCache;
import constants.Constants;
import evaluatedepthone.PatternEvaluatorImproved;
import helpers.Gaussian;
import helpers.Utils;
import java.util.ArrayList;
import java.util.HashSet;

public class StoredBoard {
  public class ExtraInfo {
    public float minProofGreaterEqual = Float.POSITIVE_INFINITY;
    public float minDisproofStrictlyGreater = Float.POSITIVE_INFINITY;
    public float minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
    public float minDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
    public boolean isFinished = false;
    public boolean proofBeforeFinished = false;
    public boolean disproofBeforeFinished = false;
  }
  protected static EvaluatorInterface EVALUATOR_MIDGAME = new EvaluatorMidgame(PatternEvaluatorImproved.load(), new HashMap(6000, 3000));
  
  private final long player;
  private final long opponent;

  static EndgameTimeEstimatorInterface endgameTimeEstimator = new EndgameTimeEstimator();
  final ArrayList<StoredBoard> fathers;
  StoredBoard[] children;
  
  int eval;
  int lower;
  int upper;
  int nEmpties;
  public float probGreaterEqual;
  public float probStrictlyGreater;
  // Positions to prove that eval >= evalGoal.
  float proofNumberGreaterEqual;
  // Positions to prove that eval <= evalGoal.
  float disproofNumberStrictlyGreater;
  public float maxLogDerivativeProbStrictlyGreater;
  public float maxLogDerivativeProbGreaterEqual;
  int evalGoal;
  public boolean playerIsStartingPlayer;
  private long descendants;

  StoredBoard next;
  StoredBoard prev;
  public ExtraInfo extraInfo;

  public final static GaussianNumberGenerator RANDOM = new GaussianNumberGenerator();

  protected StoredBoard(long player, long opponent, boolean playerIsStartingPlayer) {
    this.player = player;
    this.opponent = opponent;
    this.playerIsStartingPlayer = playerIsStartingPlayer;
    this.lower = -6400;
    this.upper = 6400;
    this.fathers = new ArrayList<>();
    this.children = null;
    this.prev = null;
    this.next = null;
    this.nEmpties = 64 - Long.bitCount(player | opponent);
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      extraInfo = new ExtraInfo();
    }
  }
  
  public static StoredBoard initialStoredBoard(Board b) {
    return initialStoredBoard(b.getPlayer(), b.getOpponent());
  }
  
  public static StoredBoard initialStoredBoard(long player, long opponent) {
    return new StoredBoard(player, opponent, true);
  }
  
  StoredBoard getChild(long player, long opponent) {
    return new StoredBoard(player, opponent, !playerIsStartingPlayer);
  }
  
  public void addDescendants(long newDescendants) {
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      if (extraInfo.isFinished) {
        return;
      }
    }
    descendants += newDescendants;
  }
  
  public void addChildren(EvaluatorMCTS evaluator) {
    GetMovesCache mover = new GetMovesCache();
    
    long curMoves = mover.getMoves(player, opponent);
    int nMoves = Long.bitCount(curMoves);
    
    if (nMoves == 0) {
      if (mover.getNMoves(opponent, player) == 0) {
        setSolved(BitPattern.getEvaluationGameOver(player, opponent));
        return;
      } else {
        ++nMoves;
      }
    }
    children = new StoredBoard[nMoves];

    for (int i = 0; i < nMoves; ++i) {
      int move = Long.numberOfTrailingZeros(curMoves);
      long moveBit = 1L << move;
      curMoves = curMoves & (~moveBit);
      long flip = move == 64 ? 0 : mover.getFlip(move) & (opponent | moveBit);
      long newPlayer = opponent & ~flip;
      long newOpponent = player | flip;
      StoredBoard child = evaluator.get(newPlayer, newOpponent);
      if (Constants.IGNORE_TRANSPOSITIONS) {
        child = null;
      }
      if (child == null) {
        child = getChild(newPlayer, newOpponent);
        child.evalGoal = -evalGoal;
        evaluator.add(child);
      } else {
        if (this.getEvalGoal() != -child.getEvalGoal()) {
          child.setEvalGoalRecursive(-this.getEvalGoal());
        }
      }
      child.addFather(this);
      if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
        child.extraInfo.isFinished = this.extraInfo.isFinished;
      }
      children[i] = child;
    }
  }
  
  public void setIsFinished(boolean newValue) {
    if (newValue == this.extraInfo.isFinished) {
      return;
    }
    this.extraInfo.isFinished = true;
    if (children != null) {
      for (StoredBoard child : children) {
        child.setIsFinished(newValue);
      }
    }
  }
  
  public void addFather(StoredBoard father) {
    fathers.add(father);
  }
  
  public boolean isLeaf() {
    return children == null;
  }
  
  public Board getBoard() {
    return new Board(player, opponent);
  }
  
  public long getPlayer() {
    return player;
  }
  
  public long getOpponent() {
    return opponent;
  }
  
  public int getEval() {
    return eval;
  }
  
  public float getProofNumberGreaterEqual() {
    return proofNumberGreaterEqual;
  }
  
  public float getDisproofNumberStrictlyGreater() {
    return disproofNumberStrictlyGreater;
  }
  
  public int getLower() {
    return lower;
  }
  
  public int getUpper() {
    return upper;
  }
  
  public long getDescendants() {
    return descendants;
  }
  
  public int getEvalGoal() {
    return evalGoal;
  }

  public final void setSolved(int newEval) {
    assert isLeaf();
    this.lower = newEval;
    this.eval = newEval;
    this.upper = newEval;
    this.setProofNumbersForLeaf();
//    this.updateFathers();
    assert areThisBoardEvalsOK();
  }

  public final void setLower(int newLower) {
    assert isLeaf();
    this.lower = (short) Math.max(lower, newLower);
    this.eval = (short) Math.max(newLower, eval);
    this.setProofNumbersForLeaf();
//    this.updateFathers();
    assert areThisBoardEvalsOK();
  }

  public final void setUpper(int newUpper) {
    assert isLeaf();
    this.upper = (short) Math.min(upper, newUpper);
    this.eval = (short) Math.min(newUpper, eval);
    this.setProofNumbersForLeaf();
//    this.updateFathers();
    assert areThisBoardEvalsOK();
  }
  
  public final void setEval(int newEval) {
    assert isLeaf();
    this.eval = (short) Math.max(this.lower, Math.min(this.upper, newEval));
    this.setProofNumbersForLeaf();
    assert areThisBoardEvalsOK();
  }
  
  protected void setEvalGoalRecursive(int evalGoal) {
    setEvalGoalRecursive(evalGoal, new HashSet<>());
  }
  protected void setEvalGoalRecursive(int evalGoal, HashSet<StoredBoard> done) {
    if (done.contains(this)) {
      return;
    }
    this.evalGoal = evalGoal;
    if (isLeaf()) {
      setProofNumbersForLeaf();
      return;
    }
    for (StoredBoard child : children) {
      child.setEvalGoalRecursive(-evalGoal, done);
    }
    updateFather();
    done.add(this);
    assert areChildrenOK();
  }

  private static float roundProb(float prob) {
    if (prob < Constants.PPN_MIN_COST_LEAF) {
      return 0;
    } else if (prob > 1 - Constants.PPN_MIN_COST_LEAF) {
      return 1;      
    }
    return prob;
  }
  
  public float lambda() {
    return -4 / Math.max(1, 2 * (float) Math.log(this.descendants)) - 1;
  }
  
  protected void updateFather() {
    assert !isLeaf();
    eval = Short.MIN_VALUE;
    lower = Short.MIN_VALUE;
    upper = Short.MIN_VALUE;
    probGreaterEqual = 1 - children.length;
    probStrictlyGreater = probGreaterEqual;
    proofNumberGreaterEqual = Float.POSITIVE_INFINITY;
    disproofNumberStrictlyGreater = 0;

    float lambda = lambda();
    for (StoredBoard child : children) {
      eval = Math.max(eval, -child.eval);
      lower = Math.max(lower, -child.upper);
      upper = Math.max(upper, -child.lower);
      probGreaterEqual += Math.pow(child.probStrictlyGreater, lambda);
      probStrictlyGreater += Math.pow(child.probGreaterEqual, lambda);
      proofNumberGreaterEqual = Math.min(proofNumberGreaterEqual, child.disproofNumberStrictlyGreater);
      disproofNumberStrictlyGreater += child.proofNumberGreaterEqual;
    }
    if (lower >= evalGoal) {
      probGreaterEqual = 1;
      assert proofNumberGreaterEqual == 0;
    } else if (upper < evalGoal) {
      probGreaterEqual = 0;
      assert proofNumberGreaterEqual == Float.POSITIVE_INFINITY;
    } else {
      probGreaterEqual = roundProb(1 - (float) Math.pow(probGreaterEqual, 1 / lambda));
      assert Float.isFinite(proofNumberGreaterEqual) && proofNumberGreaterEqual > 0;
    }
    if (lower > evalGoal) {
      probStrictlyGreater = 1;
      assert disproofNumberStrictlyGreater == Float.POSITIVE_INFINITY;
    } else if (upper <= evalGoal) {
      probStrictlyGreater = 0;
      assert disproofNumberStrictlyGreater == 0;
    } else {
      probStrictlyGreater = roundProb(1 - (float) Math.pow(probStrictlyGreater, 1 / lambda));
      assert Float.isFinite(disproofNumberStrictlyGreater) && disproofNumberStrictlyGreater > 0;
    }
    assert probGreaterEqual >= probStrictlyGreater;
    maxLogDerivativeProbStrictlyGreater = Float.NEGATIVE_INFINITY;
    maxLogDerivativeProbGreaterEqual = Float.NEGATIVE_INFINITY;

    for (StoredBoard child : children) {
      maxLogDerivativeProbStrictlyGreater = Math.max(maxLogDerivativeProbStrictlyGreater, logDerivativeProbStrictlyGreater(child));
      maxLogDerivativeProbGreaterEqual = Math.max(maxLogDerivativeProbGreaterEqual, logDerivativeProbGreaterEqual(child));
    }

    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      extraInfo.minProofGreaterEqual = Float.POSITIVE_INFINITY;
      extraInfo.minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
      extraInfo.minDisproofStrictlyGreater = 0;
      extraInfo.minDisproofStrictlyGreaterVar = 0;
      float extraCostDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
      extraInfo.proofBeforeFinished = false;
      extraInfo.disproofBeforeFinished = true;
      for (StoredBoard child : children) {
        extraInfo.proofBeforeFinished = extraInfo.proofBeforeFinished || child.extraInfo.disproofBeforeFinished;
        extraInfo.minProofGreaterEqual = Math.min(extraInfo.minProofGreaterEqual, child.extraInfo.minDisproofStrictlyGreater);
        extraInfo.minProofGreaterEqualVar = Math.min(extraInfo.minProofGreaterEqualVar, child.extraInfo.minDisproofStrictlyGreaterVar);
        extraInfo.disproofBeforeFinished =extraInfo.disproofBeforeFinished && child.extraInfo.proofBeforeFinished;
        extraInfo.minDisproofStrictlyGreater += child.extraInfo.minProofGreaterEqual;
        extraInfo.minDisproofStrictlyGreaterVar += Math.min(child.extraInfo.minProofGreaterEqual, child.extraInfo.minProofGreaterEqualVar);
        if (extraInfo.minDisproofStrictlyGreaterVar == Float.POSITIVE_INFINITY) {
          extraCostDisproofStrictlyGreaterVar = 0;
        } else {
          extraCostDisproofStrictlyGreaterVar = Math.min(extraCostDisproofStrictlyGreaterVar, child.extraInfo.minProofGreaterEqualVar - child.extraInfo.minProofGreaterEqual);
        }
      }
      extraInfo.minDisproofStrictlyGreaterVar += Math.max(0, extraCostDisproofStrictlyGreaterVar); 
      assert isExtraInfoOK();
    }
    assert isAllOK();
  }
  
  public float logDerivativeProbStrictlyGreater(StoredBoard child) {
    return child.maxLogDerivativeProbGreaterEqual + logDerivativeEdgeProbStrictlyGreater(child);
  }
  
  public float logDerivativeEdgeProbStrictlyGreater(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    assert evalGoal == -child.evalGoal;
    if (child.probGreaterEqual == 0) {
      return Float.NEGATIVE_INFINITY;
    }
    return Math.min(0, (float) Math.log((1 - probStrictlyGreater) / child.probGreaterEqual));
  }
  
  public float logDerivativeProbGreaterEqual(StoredBoard child) {
    return child.maxLogDerivativeProbStrictlyGreater + logDerivativeEdgeProbGreaterEqual(child);
  }
  
  public float logDerivativeEdgeProbGreaterEqual(StoredBoard child) {
    assert Utils.arrayContains(children, child);
    if (child.probStrictlyGreater == 0) {
      return Float.NEGATIVE_INFINITY;
    }
    return Math.min(0, (float) Math.log((1 - probGreaterEqual) / child.probStrictlyGreater));
  }

  protected void updateFathers() {
    if (!isLeaf()) {
      updateFather();
    }
    for (StoredBoard father : fathers) {
      father.updateFathers();
    }
  }
  
  public void setProofNumbersForLeaf() {
    assert this.isLeaf();
    assert evalGoal <= 6400 && evalGoal >= -6400;
    assert descendants > 0 || extraInfo.isFinished;
    probGreaterEqual = roundProb(1-(float) Gaussian.CDF(evalGoal-100, eval, 400));
    probStrictlyGreater = roundProb(1-(float) Gaussian.CDF(evalGoal+100, eval, 400));
    assert probGreaterEqual >= probStrictlyGreater;

    float mult = 0.6F;

    if (lower >= evalGoal) {
      proofNumberGreaterEqual = 0;
      probGreaterEqual = 1;
      maxLogDerivativeProbGreaterEqual = Float.NEGATIVE_INFINITY;
    } else if (upper < evalGoal) {
      proofNumberGreaterEqual = Float.POSITIVE_INFINITY;
      probGreaterEqual = 0;
      maxLogDerivativeProbGreaterEqual = Float.NEGATIVE_INFINITY;
    } else {
      this.maxLogDerivativeProbGreaterEqual = mult * (float) Math.log(probGreaterEqual * (1 - probGreaterEqual));
      proofNumberGreaterEqual = (float) (endgameTimeEstimator.proofNumber(player, opponent, evalGoal - 100, this.eval)
          / Math.max(Constants.PPN_MIN_COST_LEAF, probGreaterEqual));
      assert Float.isFinite(proofNumberGreaterEqual) && proofNumberGreaterEqual > 0;
    }
    
    if (upper <= evalGoal) {
      probStrictlyGreater = 0;
      disproofNumberStrictlyGreater = 0;
      maxLogDerivativeProbStrictlyGreater = Float.NEGATIVE_INFINITY;
    } else if (lower > evalGoal) {
      probStrictlyGreater = 1;
      disproofNumberStrictlyGreater = Float.POSITIVE_INFINITY;
      maxLogDerivativeProbStrictlyGreater = Float.NEGATIVE_INFINITY;
    } else {
      maxLogDerivativeProbStrictlyGreater = mult * (float) Math.log((1 - probStrictlyGreater) * (probStrictlyGreater));
      disproofNumberStrictlyGreater = (float) (endgameTimeEstimator.disproofNumber(player, opponent, evalGoal + 100, this.eval)
          / Math.max(Constants.PPN_MIN_COST_LEAF, 1 - probStrictlyGreater));
      assert Float.isFinite(disproofNumberStrictlyGreater) && disproofNumberStrictlyGreater > 0;
    }
    
    if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {     
      if (lower >= evalGoal) {
        if (!extraInfo.isFinished) {
          extraInfo.proofBeforeFinished = true;
        }
        EVALUATOR_MIDGAME.resetHashMap();
        EVALUATOR_MIDGAME.evaluatePosition(this.getPlayer(), this.getOpponent(), this.nEmpties, evalGoal-100, evalGoal-101);
        extraInfo.minProofGreaterEqual = Math.min(extraInfo.proofBeforeFinished ? descendants : Float.POSITIVE_INFINITY, EVALUATOR_MIDGAME.getNVisited());
        extraInfo.minProofGreaterEqualVar = Float.POSITIVE_INFINITY;
      } else {
        extraInfo.minProofGreaterEqual = Float.POSITIVE_INFINITY;
        extraInfo.minProofGreaterEqualVar = proofNumberGreaterEqual;
      }
      if (upper <= evalGoal) {
        if (!extraInfo.isFinished) {
          extraInfo.disproofBeforeFinished = true;
        }
        EVALUATOR_MIDGAME.resetHashMap();
        EVALUATOR_MIDGAME.evaluatePosition(this.getPlayer(), this.getOpponent(), this.nEmpties, evalGoal+100, evalGoal+101);
        extraInfo.minDisproofStrictlyGreater = Math.min(extraInfo.disproofBeforeFinished ? descendants : Float.POSITIVE_INFINITY, EVALUATOR_MIDGAME.getNVisited());
        extraInfo.minDisproofStrictlyGreaterVar = Float.POSITIVE_INFINITY;
      } else {
        extraInfo.minDisproofStrictlyGreater = Float.POSITIVE_INFINITY;
        extraInfo.minDisproofStrictlyGreaterVar = disproofNumberStrictlyGreater;
      }
    }
    assert areThisBoardEvalsOK();
  }
  
//  public boolean toBeSolved() {
//    if (nEmpties > Constants.EMPTIES_FOR_FORCED_MIDGAME + 3) {
//      return false;
//    } else if (nEmpties <= Constants.EMPTIES_FOR_FORCED_MIDGAME) {
//      return true;
//    }
////    System.out.println(probGreaterEqual);
//    return
//        (endgameTimeEstimator.proofNumber(player, opponent, evalGoal - 100, this.eval) 
//        < Constants.PROOF_NUMBER_FOR_ENDGAME && this.probGreaterEqual > 0.98);
////        (getDisproofNumberStrictlyGreater() < Constants.PROOF_NUMBER_FOR_ENDGAME && this.probGreaterEqual < 0.02);
////        (this.probGreaterEqual > 0.99 || this.probGreaterEqual < 0.01) &&
////        (getProofNumberGreaterEqual() * this.probGreaterEqual
////         + getDisproofNumberStrictlyGreater() * (1 - this.probGreaterEqual)
////         < Constants.PROOF_NUMBER_FOR_ENDGAME);
//  }

  @Override
  public String toString() {
    String str = new Board(player, opponent).toString() + eval + "\n";
    return str;// + "\n" + this.lower + "(" + this.bestVariationPlayer + ") " + this.upper + "(" + this.bestVariationOpponent + ")\n";
  }
  
  public boolean isPartiallySolved() {
    return probGreaterEqual > 1 - Constants.PPN_MIN_COST_LEAF && probStrictlyGreater < Constants.PPN_MIN_COST_LEAF;
  }
  
  public boolean isSolved() {
    return this.lower == this.upper;
  }
  
  public StoredBoard[] getChildren() {
    return children;
  }

//  public float childValueStrictlyGreater(StoredBoard child) {
//    assert Utils.arrayContains(children, child);
//    if (child.proofNumberGreaterEqual == 0 || disproofNumberStrictlyGreater == Float.POSITIVE_INFINITY) {
//      return Float.NEGATIVE_INFINITY;
//    }
////    if (child.probGreaterEqual > 0.9) {
////      return -1E30 * (1 + child.probGreaterEqual);
////    }
////    return child.probGreaterEqual;
////    return child.proofNumberGreaterEqual;
//    return child.proofNumberGreaterEqual / Math.min(1.E20, Math.exp(0.1 * Math.pow(disproofNumberStrictlyGreater, 0.35) / Math.sqrt(child.descendants)));
//  }
//  
//  public float childValueGreaterEqual(StoredBoard child) {
//    assert Utils.arrayContains(children, child);
//    if (proofNumberGreaterEqual == 0 || child.disproofNumberStrictlyGreater == Float.POSITIVE_INFINITY) {
//      return Float.NEGATIVE_INFINITY;
//    }
////    if (descendants >= proofNumberGreaterEqual * 0.01) {
////      return -child.disproofNumberStrictlyGreater;
////    }
////    if (child.probStrictlyGreater > 0.99) {
////      return -1E30 * (1 + child.probStrictlyGreater);
////    }
////    return -child.disproofNumberStrictlyGreater;
//    return -child.disproofNumberStrictlyGreater / Math.min(1.E20, Math.exp(0.1 * Math.pow(proofNumberGreaterEqual, 0.35) / Math.sqrt(child.descendants)));
//  }
  
  // TODO!!!
  public StoredBoard bestChildProofStrictlyGreater() {
    StoredBoard best = null;
    float bestValue = Float.POSITIVE_INFINITY;
    for (StoredBoard child : children) {
      assert child.extraInfo.minProofGreaterEqual < Float.POSITIVE_INFINITY;
      if (child.extraInfo.minProofGreaterEqualVar - child.extraInfo.minProofGreaterEqual < bestValue) {
        best = child;
        bestValue = child.extraInfo.minProofGreaterEqualVar - child.extraInfo.minProofGreaterEqual;
      }
    }
    return best;
  }
  
  public StoredBoard bestChildProofGreaterEqual() {
    StoredBoard best = null;
    float bestValue = Float.POSITIVE_INFINITY;
    for (StoredBoard child : children) {
      if (child.extraInfo.minDisproofStrictlyGreaterVar < bestValue) {
        best = child;
        bestValue = child.extraInfo.minDisproofStrictlyGreaterVar;
      }
    }
    return best;
  }

//  public StoredBoard bestChildEndgameStrictlyGreater() {
//    StoredBoard best = null;
//    float bestValue = Float.NEGATIVE_INFINITY;
//    for (StoredBoard child : children) {
//      if (childValueStrictlyGreater(child) > bestValue) {
//        best = child;
//        bestValue = childValueStrictlyGreater(child);
//      }
//    }
//    return best;
//  }
//  public StoredBoard bestChildEndgameGreaterEqual() {
//    StoredBoard best = null;
//    float bestValue = Float.NEGATIVE_INFINITY;
//    for (StoredBoard child : children) {
//      if (childValueGreaterEqual(child) > bestValue) {
//        best = child;
//        bestValue = childValueGreaterEqual(child);
//      }
//    }
//    return best;
//  }

  public StoredBoard bestChildMidgameStrictlyGreater() {
    assert !isLeaf();
    StoredBoard best = null;
    float bestValue = Float.NEGATIVE_INFINITY;
    float curValue;

    if (probStrictlyGreater == 0) {
      for (StoredBoard child : children) {
        curValue = child.proofNumberGreaterEqual;
        if (curValue > bestValue) {
          best = child;
          bestValue = curValue;
        }
      }
    } else { 
      for (StoredBoard child : children) {
        curValue = logDerivativeProbStrictlyGreater(child);
        if (logDerivativeProbStrictlyGreater(child) >= bestValue) {
          best = child;
          bestValue = curValue;
        }
      }
      assert this.maxLogDerivativeProbStrictlyGreater == bestValue;
    }
//    assert best != null;
//    assert logDerivativeProbStrictlyGreater(best) + best.maxLogDerivativeProbStrictlyGreater < 0;
    return best;
  }

  public StoredBoard bestChildMidgameGreaterEqual() {
    assert !isLeaf();
    StoredBoard best = null;
    float bestValue = Float.NEGATIVE_INFINITY;
    float curValue;

    if (probGreaterEqual == 1) {
      for (StoredBoard child : children) {
        curValue = -child.disproofNumberStrictlyGreater / (1-child.probStrictlyGreater);
        if (curValue > bestValue) {
          best = child;
          bestValue = curValue;
          assert bestValue < 0;
        }
      }
    } else {
      for (StoredBoard child : children) {
        curValue = logDerivativeProbGreaterEqual(child);
        if (curValue > bestValue) {
          best = child;
          bestValue = curValue;
        }
      }
      assert this.maxLogDerivativeProbGreaterEqual == bestValue;
    }
//    assert best != null;
    assert bestValue < 0;
    return best;
  }
  
  StoredBoard findLeastCommonAncestor() {
    if (fathers.isEmpty()) {
      return null;
    }
    if (fathers.size() == 1) {
      return fathers.get(0);
    }
    
    HashSet<StoredBoard> currentAncestors = new HashSet<>();
    HashSet<StoredBoard> nextAncestors = new HashSet<>();
    
    currentAncestors.add(this);
    
    while (true) {
      for (StoredBoard b : currentAncestors) {
        for (StoredBoard father : b.fathers) {
          if (father.nEmpties == b.nEmpties) {
            // father is a pass: it cannot be the least common ancestor,
            // because it has 1 child.
            assert(father.fathers.get(0).nEmpties == b.nEmpties + 1);
            nextAncestors.addAll(father.fathers);
          } else {
            assert(father.nEmpties == b.nEmpties + 1);
            nextAncestors.add(father);
          }
        }
      }
      if (nextAncestors.size() == 1) {
        return nextAncestors.iterator().next();
      }
      currentAncestors = nextAncestors;
      nextAncestors = new HashSet<>();
    }
  }

  boolean areThisBoardEvalsOK() {
    if (this.lower > this.eval || this.eval > this.upper) {
      throw new AssertionError("Wrong lower:" + lower + " / eval:" + eval + " / upper:" + upper + " for board\n" + this.getBoard());
    }
    return true;
  }

  boolean areDescendantsOK() {
    if (fathers.isEmpty()) {
      return true;
    }
    int maxDescendants = 0;
    String fatherDesc = "";
    for (StoredBoard father : fathers) {
      maxDescendants += father.getDescendants();
      fatherDesc += " " + father.getDescendants();
    }
    if (getDescendants() > maxDescendants) {
      throw new AssertionError(
          "Bad number of descendants " + getDescendants() + " > SUM_father descendants = "
              + maxDescendants + ". Father descendants: " + fatherDesc);
    }
    return true;
  }

  boolean isChildOK(StoredBoard child) {
    boolean found = false;
    for (StoredBoard father : child.fathers) {
      if (father == this) {
        if (found) {
          throw new AssertionError("Found the same father twice. Father:\n" + this + "\nchild:\n" + child);
        }
        found = true;
      }
    }
    if (!found) {
      throw new AssertionError("Did not find father\n" + child + " for board\n" + this);
    }
    if (eval < -child.eval || lower < -child.upper || upper < -child.lower) {
      throw new AssertionError(
          "Wrong eval=" + eval + "/lower=" + lower + "/upper=" + upper +
          " (should be eval>=" + (-child.eval) + "/lower>=" + (-child.upper) + "/upper>=" + (-child.lower) +
          ". Board:\n" + this + "\nchild:\n" + child);
    }
    return 
        child.playerIsStartingPlayer == !playerIsStartingPlayer
        && Utils.arrayContains(children, child);
  }

  boolean areChildrenOK() {
    if (children == null) {
      return true;
    }
    for (StoredBoard child : children) {
      if (!isChildOK(child)) {
        throw new AssertionError("Child\n" + child + "\n of board\n" + this + "\nis not OK");
      }
    }
    if (children.length != Long.bitCount(GetMoves.getMoves(player, opponent)) && !(
        Long.bitCount(GetMoves.getMoves(player, opponent)) == 0
        && Long.bitCount(GetMoves.getMoves(opponent, player)) != 0
        && children.length == 1)) {
      throw new AssertionError(
          "Board " + this + " should have " + Long.bitCount(GetMoves.getMoves(player, opponent))
              + " children. Found " + children.length);
    }
    return true;
  }

  boolean isPrevNextOK() {
    return 
        prev != this
        && next != this
        && (prev == null || prev.next == this)
        && (next == null || next.prev == this);
  }
  
  boolean isEvalOK() {
    if ((new GetMovesCache()).getNMoves(player, opponent) == 0
        && (new GetMovesCache()).getNMoves(opponent, player) == 0
        && (this.getProofNumberGreaterEqual() == 0 || this.getProofNumberGreaterEqual() == Float.POSITIVE_INFINITY)) {
      int correctEval = BitPattern.getEvaluationGameOver(player, opponent);
      if (eval != correctEval || lower != correctEval || upper != correctEval) {
        throw new AssertionError(
            "Got wrong eval/lower/upper = " + eval + "/" + lower + "/" + upper + 
            " (expected all " + correctEval + ") for board\n" + this.getBoard());
      }
      return true;
    }
    if (children == null) {
      return true;
    }
    int correctEval = Integer.MIN_VALUE;
    int correctLower = Integer.MIN_VALUE;
    int correctUpper = Integer.MIN_VALUE;
    String childEval = "";
    for (StoredBoard child : children) {
      correctEval = Math.max(correctEval, -child.getEval());
      correctLower = Math.max(correctLower, -child.getUpper());
      correctUpper = Math.max(correctUpper, -child.getLower());
    }
    if (eval != correctEval || lower != correctLower || upper != correctUpper) {
      throw new AssertionError(
          "Wrong eval/lower/upper. Expected: " + correctEval + "/" + correctLower + "/" + correctUpper + 
          ". Got: " + eval + "/" + lower + "/" + upper + ".\nChildren eval/lower/upper:\n" + childEval);
    }
    return true;
  }
  
  boolean isExtraInfoOK() {
    if (!Constants.FIND_BEST_PROOF_AFTER_EVAL) {
      return true;
    }
    if (Float.isNaN(extraInfo.minDisproofStrictlyGreaterVar) || Float.isNaN(extraInfo.minProofGreaterEqualVar)) {
      throw new AssertionError(
          "Got (minDisproofVar, minProofVar) = " + extraInfo.minDisproofStrictlyGreaterVar + " " + extraInfo.minProofGreaterEqualVar
      );      
    }
    
    return true;
  }
  
  boolean isAllOK() {
    if (!isPrevNextOK()) {
      throw new AssertionError("Wrong isPrevNextOK");
    }
    if (!areChildrenOK()) {
      throw new AssertionError("Wrong areChildrenOK");      
    }
    if (!areThisBoardEvalsOK() || !isEvalOK() || !areDescendantsOK()) {
      throw new AssertionError("Wrong areThisBoardEvalsOK or isEvalOK or areDescendantsOK.");
    }
    return isExtraInfoOK();// && isLeastSingleAncestorOK();
  }
}
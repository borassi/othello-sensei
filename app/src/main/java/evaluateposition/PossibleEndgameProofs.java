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

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.HashMap;
import java.util.List;
import java.util.Objects;
import java.util.PriorityQueue;

/**
 *
 * @author michele
 */
public class PossibleEndgameProofs {
  
  public ArrayList<EndgameProof> orClauses;
  final StoredBoard board;

  public static class EndgameProof {
    final long nPositions;
    final HashSet<PossibleEndgameProofs> children;

    public EndgameProof(long nPositions, PossibleEndgameProofs... children) {
      this(nPositions);
      this.children.addAll(Arrays.asList(children));
      assert isAllOK();
    }

    public EndgameProof(long nPositions, List<PossibleEndgameProofs> children) {
      this(nPositions);
      this.children.addAll(children);
      assert isAllOK();
    }

    public EndgameProof(long nPositions, HashSet<PossibleEndgameProofs> children) {
      this(nPositions);
      this.children.addAll(children);
      assert isAllOK();
    }
  
    public EndgameProof(long nPositions) {
      this.nPositions = nPositions;
      this.children = new HashSet<>();
      assert isAllOK();
    }

    public EndgameProof(EndgameProof other) {
      this(other.nPositions, other.children);
    }
    @Override
    public boolean equals(Object other) {
      if (!(other instanceof EndgameProof)) {
        return false;
      }
      EndgameProof otherConverted = (EndgameProof) other;
      return otherConverted.nPositions == this.nPositions && otherConverted.children.equals(children);
    }

    @Override
    public int hashCode() {
      int hash = 7;
      hash = 97 * hash + (int) (this.nPositions ^ (this.nPositions >>> 32));
      hash = 97 * hash + Objects.hashCode(this.children);
      return hash;
    }

    @Override
    public String toString() {
      String result = "[" + nPositions + " ";
      for (PossibleEndgameProofs p : children) {
        result += "(" + p.board.getPlayer() + " " + p.board.getOpponent() + ")";
      }
      return result + "]";
    }

    public boolean greaterEqual(EndgameProof other) {
      return nPositions >= other.nPositions && children.containsAll(other.children);
    }
  
    public boolean strictlyGreater(EndgameProof other) {
      return greaterEqual(other) && (nPositions > other.nPositions || children.size() > other.children.size());
    }
  
    public EndgameProof and(EndgameProof other) {
      assert other.isAllOK();
      HashSet<PossibleEndgameProofs> boardsUnion = new HashSet<>(children);
      boardsUnion.addAll(other.children);
      return new EndgameProof(nPositions + other.nPositions, boardsUnion);
    }
    
    private boolean isAllOK() {
      for (PossibleEndgameProofs child : children) {
        assert child.canProve();
      }
      return true;
    }
  }
  
  public PossibleEndgameProofs(StoredBoard board) {
    this.board = board;
    this.orClauses = new ArrayList<>();
  }
  
  public PossibleEndgameProofs(StoredBoard board, EndgameProof... orClauses) {
    this(board);
    this.orClauses.addAll(Arrays.asList(orClauses));
    assert this.areChildrenOK();
  }
  
  public PossibleEndgameProofs(StoredBoard board, List<EndgameProof> orClauses) {
    this(board);
    this.orClauses.addAll(orClauses);
    assert this.areChildrenOK();
  }
  
  public PossibleEndgameProofs(StoredBoard board, long nPositions) {
    this(board, new EndgameProof(nPositions));
    assert this.isSimplified();
  }

  public void simplify() {
    ArrayList<EndgameProof> newEndgameProofs = new ArrayList<>();

    for (int i = 0; i < orClauses.size(); ++i) {
      EndgameProof p1 = orClauses.get(i);
      boolean include = true;
      for (EndgameProof p2 : newEndgameProofs) {
        if (p1.greaterEqual(p2)) {
          include = false;
          break;
        }
      }
      if (!include) {
        continue;
      }
      for (int j = i+1; j < orClauses.size(); ++j) {
        EndgameProof p2 = orClauses.get(j);
        if (p1.strictlyGreater(p2)) {
          include = false;
          break;
        }
      }
      if (include) {
        newEndgameProofs.add(p1);
      }
    }
    orClauses = newEndgameProofs;
    assert this.areChildrenOK();
  }
  
  public void or(PossibleEndgameProofs other) {
    orClauses.addAll(other.orClauses);
    simplify();
    assert this.areChildrenOK();
    assert this.isSimplified();
  }
  
  public void and(PossibleEndgameProofs other) {
    ArrayList<EndgameProof> newOrClauses = new ArrayList<>();
    for (EndgameProof p1 : orClauses) {
      for (EndgameProof p2 : other.orClauses) {
        newOrClauses.add(p1.and(p2));
      }
    }
    orClauses = newOrClauses;
    simplify();
    assert this.areChildrenOK();
    assert this.isSimplified();
  }
  
  public void toNoProof() {
    orClauses.clear();
  }
  
  public void toTrivialProof() {
    orClauses.clear();
    orClauses.add(new EndgameProof(0));
  }
  
  public boolean canProve() {
    return !orClauses.isEmpty();
  }
  
  public EndgameProof get(int i) {
    return orClauses.get(i);
  }
  
  public long getMinNPositions() {
    if (!canProve()) {
      return Long.MAX_VALUE;
    }
    PossibleEndgameProofs simplifiedProofs = new PossibleEndgameProofs(board, orClauses);
    
    HashSet<PossibleEndgameProofs> unsolvedSet = new HashSet<>();

    for (EndgameProof orClause : simplifiedProofs.orClauses) {
      unsolvedSet.addAll(orClause.children);
    }
    PriorityQueue<PossibleEndgameProofs> unsolved = new PriorityQueue<>(
        (PossibleEndgameProofs p1, PossibleEndgameProofs p2) -> {
          return p2.board.nEmpties - p1.board.nEmpties;
        }
    );
    unsolved.addAll(unsolvedSet);
//        System.out.println("START");
    for (PossibleEndgameProofs solving = unsolved.poll(); solving != null; solving = unsolved.poll()) {
      assert solving.canProve();
      PossibleEndgameProofs unalteredProofs = new PossibleEndgameProofs(board);
      PossibleEndgameProofs newSimplifiedProofs = new PossibleEndgameProofs(board);
      for (EndgameProof orClause : simplifiedProofs.orClauses) {
        if (!orClause.children.contains(solving)) {
          unalteredProofs.orClauses.add(orClause);
          continue;
        }
        for (EndgameProof childOrClause : solving.orClauses) {
          for (PossibleEndgameProofs child : childOrClause.children) {
            if (!unsolvedSet.contains(child)) {
              unsolvedSet.add(child);
              unsolved.add(child);
            }
          }
        }
        EndgameProof newOrClause = new EndgameProof(orClause);
        newOrClause.children.remove(solving);
        newSimplifiedProofs.orClauses.add(newOrClause);
      }
      newSimplifiedProofs.and(solving);
      newSimplifiedProofs.or(unalteredProofs);
      simplifiedProofs = newSimplifiedProofs;
      simplifiedProofs.simplify();
    }
    if (!simplifiedProofs.canProve()) {
      return Long.MAX_VALUE;
    }
    if (simplifiedProofs.orClauses.size() != 1) {
      String s = "The number of clauses is " + simplifiedProofs.orClauses.size() + " instead of 1. Board:\n" +
          this.board + "Value: " + simplifiedProofs + "\nOriginal:" + orClauses;
      simplifiedProofs.simplify();
      throw new AssertionError(s + "\n" + simplifiedProofs);
    }
    assert simplifiedProofs.orClauses.size() == 1;
    if (!simplifiedProofs.get(0).children.isEmpty()) {
      throw new AssertionError("Wrong value of " + simplifiedProofs.get(0));
    }
    assert simplifiedProofs.get(0).children.isEmpty();    
    return simplifiedProofs.get(0).nPositions;
  }
  
  public HashMap<EndgameProof, Integer> toHashMap() {
    HashMap<EndgameProof, Integer> result = new HashMap<>();
    for (EndgameProof proof : this.orClauses) {
      result.put(proof, result.getOrDefault(proof, 0) + 1);
    }
    return result;
  }
  
  @Override
  public boolean equals(Object other) {
    if (!(other instanceof PossibleEndgameProofs)) {
      return false;
    }
    PossibleEndgameProofs otherConverted = (PossibleEndgameProofs) other;
    
    return board.equals(otherConverted.board);
  }

  @Override
  public int hashCode() {
    return board.hashCode();
  }
  
  @Override
  public String toString() {
    String result = "{";
    for (EndgameProof proof : orClauses) {
      result += proof + ", ";
    }
    return result + "}";
  }
  
  boolean isSimplified() {
    for (int i = 0; i < orClauses.size(); ++i) {
      EndgameProof clause1 = orClauses.get(i);
      for (int j = i+1; j < orClauses.size(); ++j) {
        EndgameProof clause2 = orClauses.get(j);
        if (clause1.greaterEqual(clause2) || clause2.greaterEqual(clause1)) {
          return false;
        } 
      }
    }
    return true;
  }
  
  boolean areChildrenOK() {
    for (EndgameProof orClause : orClauses) {
      for (PossibleEndgameProofs child : orClause.children) {
        assert child.canProve();
        if (board.nEmpties < child.board.nEmpties || board == child.board) {
          throw new AssertionError("Proof of board\n" + board + "cannot have child\n" + child.board);
        }
      }
    }
    return true;
  }
}

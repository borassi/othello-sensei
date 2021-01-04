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
import java.util.Objects;

/**
 *
 * @author michele
 */
public class PossibleEndgameProofs {
  
  ArrayList<EndgameProof> orClauses;
  StoredBoard board;

  public static class EndgameProof {
    long nPositions;
    HashSet<PossibleEndgameProofs> boards;

    public EndgameProof(long nPositions, PossibleEndgameProofs... boards) {
      this.nPositions = nPositions;
      this.boards = new HashSet<>(Arrays.asList(boards));
    }

    public EndgameProof(long nPositions, HashSet<PossibleEndgameProofs> boards) {
      this.nPositions = nPositions;
      this.boards = boards;
    }
  
    public EndgameProof(long nPositions) {
      this(nPositions, new HashSet<PossibleEndgameProofs>());
    }
  
    @Override
    public boolean equals(Object other) {
      if (!(other instanceof EndgameProof)) {
        return false;
      }
      EndgameProof otherConverted = (EndgameProof) other;
      return otherConverted.nPositions == this.nPositions && otherConverted.boards.equals(boards);
    }

    @Override
    public int hashCode() {
      int hash = 7;
      hash = 97 * hash + (int) (this.nPositions ^ (this.nPositions >>> 32));
      hash = 97 * hash + Objects.hashCode(this.boards);
      return hash;
    }

    @Override
    public String toString() {
      String result = "[" + nPositions + " ";
      for (PossibleEndgameProofs p : boards) {
        result += "(" + p.board.getPlayer() + " " + p.board.getOpponent() + ")";
      }
      return result + "]";
    }

    public boolean greaterEqual(EndgameProof other) {
      return nPositions >= other.nPositions && boards.containsAll(other.boards);
    }
  
    public boolean strictlyGreater(EndgameProof other) {
      return greaterEqual(other) && (nPositions > other.nPositions || boards.size() > other.boards.size());
    }
  
    public EndgameProof and(EndgameProof other) {
      HashSet<PossibleEndgameProofs> boardsUnion = new HashSet<>(boards);
      boardsUnion.addAll(other.boards);
      return new EndgameProof(nPositions + other.nPositions, boardsUnion);
    }
    public long getNPositions() {
      // TODO: Update!!!
      return nPositions;
    }
  }
  
  public PossibleEndgameProofs(StoredBoard board) {
    this.board = board;
    orClauses = new ArrayList<>();
  }
  
  public PossibleEndgameProofs(StoredBoard board, PossibleEndgameProofs other) {
    this(board);
    orClauses.addAll(other.orClauses);
  }
  
  public PossibleEndgameProofs(StoredBoard board, EndgameProof... proofs) {
    this(board);
    orClauses.addAll(Arrays.asList(proofs));
  }
  
  public PossibleEndgameProofs(StoredBoard board, long nPositions) {
    this(board, new EndgameProof(nPositions));
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
  }
  
  public void or(PossibleEndgameProofs other) {
    orClauses.addAll(other.orClauses);
    simplify();
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
    long minNPositions = Long.MAX_VALUE;
    for (EndgameProof proof : this.orClauses) {
      minNPositions = Math.min(minNPositions, proof.getNPositions());
    }
    return minNPositions;
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
}

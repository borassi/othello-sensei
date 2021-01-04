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
  
  private ArrayList<EndgameProof> endgameProofs;

  public static class EndgameProof {
    long nPositions;
    HashSet<StoredBoard> boards;

    public EndgameProof(long nPositions, StoredBoard... boards) {
      this.nPositions = nPositions;
      this.boards = new HashSet<>(Arrays.asList(boards));
    }

    public EndgameProof(long nPositions, HashSet<StoredBoard> boards) {
      this.nPositions = nPositions;
      this.boards = boards;
    }
  
    public EndgameProof(long nPositions) {
      this(nPositions, new HashSet<StoredBoard>());
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
      for (StoredBoard b : boards) {
        result += "(" + b.getPlayer() + " " + b.getOpponent() + ")";
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
      HashSet<StoredBoard> boardsUnion = new HashSet<>(boards);
      boardsUnion.addAll(other.boards);
      return new EndgameProof(nPositions + other.nPositions, boardsUnion);
    }
    public long getNPositions() {
      // TODO: Update!!!
      return nPositions;
    }
  }
  
  public PossibleEndgameProofs() {
    endgameProofs = new ArrayList<>();
  }
  
  public PossibleEndgameProofs(PossibleEndgameProofs other) {
    endgameProofs = new ArrayList<>(other.endgameProofs);
  }
  
  public PossibleEndgameProofs(EndgameProof... proofs) {
    this();
    endgameProofs.addAll(Arrays.asList(proofs));
  }
  
  public PossibleEndgameProofs(long nPositions) {
    this(new EndgameProof(nPositions));
  }
  
  public PossibleEndgameProofs(StoredBoard b) {
    this(new EndgameProof(0, b));
  }

  public void simplify() {
    ArrayList<EndgameProof> newEndgameProofs = new ArrayList<>();

    for (int i = 0; i < endgameProofs.size(); ++i) {
      EndgameProof p1 = endgameProofs.get(i);
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
      for (int j = i+1; j < endgameProofs.size(); ++j) {
        EndgameProof p2 = endgameProofs.get(j);
        if (p1.strictlyGreater(p2)) {
          include = false;
          break;
        }
      }
      if (include) {
        newEndgameProofs.add(p1);
      }
    }
    endgameProofs = newEndgameProofs;
  }
  
  public PossibleEndgameProofs or(PossibleEndgameProofs other) {
    PossibleEndgameProofs result = new PossibleEndgameProofs(this);
    result.endgameProofs.addAll(other.endgameProofs);
    result.simplify();
    return result;
  }
  
  public PossibleEndgameProofs and(PossibleEndgameProofs other) {
    PossibleEndgameProofs result = new PossibleEndgameProofs();
    for (EndgameProof p1 : endgameProofs) {
      for (EndgameProof p2 : other.endgameProofs) {
        result.endgameProofs.add(p1.and(p2));
      }
    }
    result.simplify();
    return result;
  }
  
  public void clear() {
    endgameProofs.clear();
  }
  
  public boolean canProve() {
    return !endgameProofs.isEmpty();
  }
  
  public EndgameProof get(int i) {
    return endgameProofs.get(i);
  }

  public long getMinNPositions() {
    long minNPositions = Long.MAX_VALUE;
    for (EndgameProof proof : this.endgameProofs) {
      minNPositions = Math.min(minNPositions, proof.getNPositions());
    }
    return minNPositions;
  }
  
  public HashMap<EndgameProof, Integer> toHashMap() {
    HashMap<EndgameProof, Integer> result = new HashMap<>();
    for (EndgameProof proof : this.endgameProofs) {
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
    
    return toHashMap().equals(otherConverted.toHashMap());
  }

  @Override
  public int hashCode() {
    return Objects.hashCode(this.toHashMap());
  }
  
  @Override
  public String toString() {
    String result = "{";
    for (EndgameProof proof : endgameProofs) {
      result += proof + ", ";
    }
    return result + "}";
  }
}

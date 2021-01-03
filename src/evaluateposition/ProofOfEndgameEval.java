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
import java.util.Objects;

/**
 *
 * @author michele
 */
public class ProofOfEndgameEval {
  long nPositions;
  HashSet<StoredBoard> boards;
  
  public ProofOfEndgameEval(long nPositions) {
    this(nPositions, new HashSet<StoredBoard>());
  }

  public ProofOfEndgameEval(long nPositions, HashSet<StoredBoard> boards) {
    this.nPositions = nPositions;
    this.boards = boards;
  }
  
  public static ArrayList<ProofOfEndgameEval> create(long nPositions) {
    ArrayList<ProofOfEndgameEval> result = new ArrayList<>();
    result.add(new ProofOfEndgameEval(nPositions));
    return result;
  }
  
  public static ArrayList<ProofOfEndgameEval> create(StoredBoard b) {
    ArrayList<ProofOfEndgameEval> result = new ArrayList<>();
    result.add(new ProofOfEndgameEval(0, new HashSet<>(Arrays.asList(b))));
    return result;
  }
  
  @Override
  public boolean equals(Object other) {
    if (!(other instanceof ProofOfEndgameEval)) {
      return false;
    }
    ProofOfEndgameEval otherConverted = (ProofOfEndgameEval) other;
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
  
  public static boolean greaterEqual(ProofOfEndgameEval p1, ProofOfEndgameEval p2) {
    return p1.nPositions >= p2.nPositions && p1.boards.containsAll(p2.boards);
  }
  
  public static boolean strictlyGreater(ProofOfEndgameEval p1, ProofOfEndgameEval p2) {
    return greaterEqual(p1, p2) && (p1.nPositions > p2.nPositions || p1.boards.size() > p2.boards.size());
  }

  public static ArrayList<ProofOfEndgameEval> simplify(ArrayList<ProofOfEndgameEval> values) {
    ArrayList<ProofOfEndgameEval> result = new ArrayList<>();

    for (int i = 0; i < values.size(); ++i) {
      ProofOfEndgameEval p1 = values.get(i);
      boolean include = true;
      for (ProofOfEndgameEval p2 : result) {
        if (greaterEqual(p1, p2)) {
          include = false;
          break;
        }
      }
      if (!include) {
        continue;
      }
      for (int j = i+1; j < values.size(); ++j) {
        ProofOfEndgameEval p2 = values.get(j);
        if (strictlyGreater(p1, p2)) {
          include = false;
          break;
        }
      }
      if (include) {
        result.add(p1);
      }
    }
    return result;
  }
  
  public static ArrayList<ProofOfEndgameEval> or(ArrayList<ProofOfEndgameEval> values1, ArrayList<ProofOfEndgameEval> values2) {
    ArrayList<ProofOfEndgameEval> result = new ArrayList<>(values1);
    result.addAll(values2);
    return simplify(result);
  }
  
  public static ArrayList<ProofOfEndgameEval> and(ArrayList<ProofOfEndgameEval> values1, ArrayList<ProofOfEndgameEval> values2) {
    ArrayList<ProofOfEndgameEval> result = new ArrayList<>();
    for (ProofOfEndgameEval p1 : values1) {
      for (ProofOfEndgameEval p2 : values2) {
        result.add(and(p1, p2));
      }
    }
    return simplify(result);
  }
  
  public static ProofOfEndgameEval and(ProofOfEndgameEval p1, ProofOfEndgameEval p2) {
    HashSet<StoredBoard> boardsUnion = new HashSet<>(p1.boards);
    boardsUnion.addAll(p2.boards);
    return new ProofOfEndgameEval(p1.nPositions + p2.nPositions, boardsUnion);
  }
}

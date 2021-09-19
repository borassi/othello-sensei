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

package bitpattern;

/**
 * A move, as a pair (i,j), where i and j range from 0 to 7.
 */
public class PositionIJ {
  /**
   * The row (from 0 to 7).
   */
  public final int i;
  /**
   * The column (from 0 to 7).
   */
  public final int j;
  
  /**
   * Constructor.
   * @param i The row (from 0 to 7).
   * @param j The column (from 0 to 7).
   */
  public PositionIJ(int i, int j) {
    this.i = i;
    this.j = j;
  }
  
  public PositionIJ(int move) {
    this(7 - move / 8, 7 - move % 8);
  }
  
  public PositionIJ(String move) {
    j = move.charAt(0) - 'a';
    i = move.charAt(1) - '1';
  }
  
  public int toMove() {
    return 7 - j + 8 * (7 - i);
  }
  
  @Override
  public boolean equals(Object other) {
    if (!PositionIJ.class.isAssignableFrom(other.getClass())) {
      return false;
    }
    final PositionIJ otherMove = (PositionIJ) other;
    return i == otherMove.i && j == otherMove.j;
  }

  @Override
  public int hashCode() {
    return this.i + 8 * this.j;
  }
  
  @Override
  public String toString() {
    return Character.toString((char) (j + 'a')) + (i + 1);
  }
}

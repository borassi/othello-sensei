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
package constants;

/**
 *
 * @author michele
 */
public class Stats {
  private static long nVisitedAlphaBetaSolve;
  private static long nAlphaBetaSolve;
  private static long nVisitedLastMoves;
  private static long nLastMoves;

  public static synchronized long getNVisitedAlphaBetaSolve() {
    return nVisitedAlphaBetaSolve;
  }

  public static synchronized long getNAlphaBetaSolve() {
    return nAlphaBetaSolve;
  }

  public static synchronized long getNVisitedLastMoves() {
    return nVisitedLastMoves;
  }

  public static synchronized long getNLastMoves() {
    return nLastMoves;
  }

  public static synchronized void addToNVisitedAlphaBetaSolve(long n) {
    Stats.nVisitedAlphaBetaSolve += n;
  }

  public static synchronized void addToNAlphaBetaSolve(long n) {
    Stats.nAlphaBetaSolve += n;
  }

  public static synchronized void addToNVisitedLastMoves(long n) {
    Stats.nVisitedLastMoves += n;
  }

  public static synchronized void addToNLastMoves(long n) {
    Stats.nLastMoves += n;
  }
  
  static {
    reset();
  }

  public static void reset() {
    nVisitedAlphaBetaSolve = 0;
    nAlphaBetaSolve = 0;
    nVisitedLastMoves = 0;
    nLastMoves = 0;
  }
}

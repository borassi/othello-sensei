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
public class Constants {
  public final static int WEIGHT_DEPTH_1 = 1;
  public final static int WEIGHT_DEPTH_0 = 2;
  public final static int EMPTIES_FOR_FORCED_MIDGAME = 14;
  public final static int PROOF_NUMBER_FOR_ENDGAME = 20000;
//  public final static int HASH_MAP_SIZE = 500000;
//  public final static int MCTS_SIZE = 2000000;
  public final static int HASH_MAP_SIZE = 1000000;
  public final static int MCTS_SIZE = 1000000;
  public final static int EMPTIES_FOR_ENDGAME = 8;
  public final static int TRY_SC_LAST_5_NO_CORNER = 1400;
  public final static int TRY_SC_LAST_5_CORNER = 2200;
  public final static int TRY_SC_LAST_4_NO_CORNER = 1000;
  public final static int TRY_SC_LAST_4_CORNER = 2000;
  public final static int TRY_SC_LAST_3_NO_CORNER = 600;
  public final static int TRY_SC_LAST_3_CORNER = 1600;
  
  public final static double PPN_MIN_COST_LEAF = 0.0001;
  public final static double PPN_LAMBDA = -4;
  public final static double PPN_MU = -6;

  public final static boolean ASSERT_EXTENDED = false;
  public final static boolean FIND_BEST_PROOF_AFTER_EVAL = false;
}

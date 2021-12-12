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

public class Constants {
  public static boolean MOBILE = false;

  public final static int WEIGHT_DEPTH_1 = 1;
  public final static int WEIGHT_DEPTH_0 = 2;
  public final static int EMPTIES_FOR_FORCED_MIDGAME = 14;
  public final static int VISITED_ENDGAME_GOAL = 150;
  public final static int EMPTIES_FOR_ENDGAME = 10;

  public final static int HASH_MAP_SIZE_MOBILE = 100000;
  public final static int HASH_MAP_SIZE_DESKTOP = 1000000;
  public final static int MCTS_SIZE_MOBILE = 100000;
  public final static int MCTS_SIZE_DESKTOP = 10000000;

  public final static int MAX_PARALLEL_TASKS = 1;
  public final static float LAMBDA = -0.45F;
  public final static float MIN_PROB = 0F;
  public final static float MIN_PROB_LEAF = 0F;
  public final static float PROB_RANDOM_VALUE = 0F;
  public final static float SIZE_FOR_APPROX = 0F;
  public final static float MULT_STDDEV = 0.8F;
  public final static float PROB_FOR_PROOF = 1F;
  public final static float ZERO_PERC_FOR_WEAK = 0.04F;

  public final static boolean ASSERT_EXTENDED = false;
  public final static boolean FIND_BEST_PROOF_AFTER_EVAL = false;
  public final static boolean IGNORE_TRANSPOSITIONS = false;
  public final static boolean APPROX_ONLY = false;

  public static int hashMapSize() {
    return MOBILE ? HASH_MAP_SIZE_MOBILE : HASH_MAP_SIZE_DESKTOP;
  }

  public static int MCTSSize() {
    return MOBILE ? MCTS_SIZE_MOBILE : MCTS_SIZE_DESKTOP;
  }
}

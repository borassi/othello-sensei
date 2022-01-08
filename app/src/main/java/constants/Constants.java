// Copyright 2022 Google LLC
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

import evaluateposition.ProbCombiner;
import evaluateposition.ProbCombinerExpPolyLog;

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
  public final static int MCTS_SIZE_DESKTOP = 14000000;

  public final static int MAX_PARALLEL_TASKS = 1;
  public final static float MIN_PROB_LEAF = 0F;
  public final static float SIZE_FOR_APPROX = 0F;
  public final static float MULT_STDDEV = 0.95F;
  public final static float LEAF_MULTIPLIER = 0.8F;
  public final static float PROB_FOR_PROOF = 0.03F;
  public final static float ZERO_PERC_FOR_WEAK = 0.05F;
  public final static ProbCombiner PROB_COMBINER = new ProbCombinerExpPolyLog(1.65);
  public final static float MIN_PROB_EVAL_GOAL = 0.03F;
  public final static float PROB_REDUCE_WEAK_EVAL = 0.01F;
  public final static float PROB_FOR_ENDGAME_ALPHA_BETA = 0.01F;
  public final static float PROB_INCREASE_WEAK_EVAL = 0.05F;

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

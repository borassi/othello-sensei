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
  public final static int EMPTIES_FOR_FORCED_MIDGAME = MOBILE ? 15 : 14;
  public final static float PROB_FOR_EARLY_MIDGAME = 0.05F;
  public final static int PROOF_NUMBER_GOAL_FOR_MIDGAME = MOBILE ? 20000 : 7000;
  public final static int VISITED_ENDGAME_GOAL = 150;
  public final static int EMPTIES_FOR_ENDGAME = 10;

  public final static int HASH_MAP_SIZE = MOBILE ? 300000 : 5000000;
  public final static int MCTS_SIZE = MOBILE ? 700000: 16000000;

  public final static int MAX_PARALLEL_TASKS = 1;//Runtime.getRuntime().availableProcessors();
  public final static float MIN_PROB_LEAF = 0F;
  public final static float SIZE_FOR_APPROX = 0F;
  public final static float MULT_STDDEV = 1F;
  public final static float LEAF_MULTIPLIER = 0.8F;
  public final static float PROB_FOR_PROOF = 0.03F;
  public final static float ZERO_PERC_FOR_WEAK = 0.05F;
  public final static ProbCombiner PROB_COMBINER = new ProbCombinerExpPolyLog(1.7);
  public final static float MIN_PROB_EVAL_GOAL = 0.03F;
  public final static float PROB_REDUCE_WEAK_EVAL = 0.01F;
  public final static float PROB_FOR_ENDGAME_ALPHA_BETA = 0.01F;
  public final static float PROB_INCREASE_WEAK_EVAL = 0.05F;

  public final static boolean ASSERT_EXTENDED = false;
  public final static boolean ASSERT_LOCKS = false;
  public final static boolean ASSERT_LOWER_UPPER = true;
  public final static boolean FIND_BEST_PROOF_AFTER_EVAL = false;
  public final static boolean IGNORE_TRANSPOSITIONS = false;
  public final static boolean APPROX_ONLY = false;
}

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
package ui;

/**
 *
 * @author michele
 */
public class CaseAnnotations {
  public float eval = 0;
  public boolean isBestMove = false;
  public float safeEval = Float.NEGATIVE_INFINITY;
  public double proofNumberCurEval = Double.NEGATIVE_INFINITY;
  public double proofNumberNextEval = Double.NEGATIVE_INFINITY;
  public double disproofNumberCurEval = Double.NEGATIVE_INFINITY;
  public double disproofNumberNextEval = Double.NEGATIVE_INFINITY;
  public float lower = Float.NEGATIVE_INFINITY;
  public float upper = Float.NEGATIVE_INFINITY;
  public float bestVariationPlayer = Float.NEGATIVE_INFINITY;
  public float bestVariationOpponent = Float.NEGATIVE_INFINITY;
  public long nVisited = 0;
  public String otherAnnotations = "";
}

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

package evaluatedepthone;

import board.Board;

/**
 * A board with the corresponding evaluation.
 * The evaluation is for the player who is currently playing.
 */
public class BoardWithEvaluation implements Comparable<BoardWithEvaluation> {
  public Board board;
  public int evaluation;
  
  public BoardWithEvaluation(Board board, int evaluation) {
    this.board = board;
    this.evaluation = evaluation;
  }

  public void update(Board newBoard, int newEvaluation) {
    board.setToOtherBoard(newBoard);
    this.evaluation = newEvaluation;
  }
  @Override
  public int compareTo(BoardWithEvaluation other) {
    return Integer.compare(evaluation, other.evaluation);
  }
}

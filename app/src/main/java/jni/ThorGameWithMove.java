// Copyright 2023 Michele Borassi
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

package jni;

import bitpattern.PositionIJ;

public class ThorGameWithMove {
  ThorGame game;
  Byte move;

  public ThorGameWithMove(ThorGame game, Byte move) {
    this.game = game;
    this.move = move;
  }
  public ThorGame game() { return game; }
  public Byte move() { return move; }

  @Override
  public String toString() {
    return new PositionIJ(move).toString() + " " + game.toString();
  }
}

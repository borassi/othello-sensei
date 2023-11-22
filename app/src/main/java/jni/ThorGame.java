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

public class ThorGame {
  String black;
  String white;
  String tournament;
  byte[] moves;
  short year;
  short blackScore;

  ThorGame(String black, String white, String tournament, byte[] moves, short year, short blackScore) {
    this.black = black;
    this.white = white;
    this.tournament = tournament;
    this.moves = moves;
    this.year = year;
    this.blackScore = blackScore;
  }
  public String black() { return black; }
  public String white() { return white; }
  public String tournament() { return tournament; }
  public byte[] moves() { return moves; }
  public short year() { return year; }
  public short blackScore() { return blackScore; }
}

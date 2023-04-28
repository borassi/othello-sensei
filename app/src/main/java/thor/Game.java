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
package thor;

import java.util.ArrayList;
import java.util.Arrays;

public class Game {
  private final byte[] moves;
  private final short year;
  private final String black;
  private final String white;
  private final String tournament;
  private final int score;

  public Game(byte[] buffer, int offset, short year, ArrayList<String> players, ArrayList<String> tournaments) {
    this(
        tournaments.get((buffer[offset] & 0xff) | ((buffer[offset + 1] & 0xff) << 8)),
        year,
        players.get((buffer[offset + 2] & 0xff) | ((buffer[offset + 3] & 0xff) << 8)),
        players.get((buffer[offset + 4] & 0xff) | ((buffer[offset + 5] & 0xff) << 8)),
        buffer[offset + 6],
        Arrays.copyOfRange(buffer, offset + 8, offset + 68)
    );
    assert(offset % 68 == 0);
  }
  public Game(String tournament, short year, String black, String white, byte score, byte[] moves) {
    this.black = black;
    this.white = white;
    this.year = year;
    this.tournament = tournament;
    this.score = score;
    this.moves = moves;
  }

  public String black() { return black; }
  public String white() { return white; }
  public String tournament() { return tournament; }
  public int year() { return year; }
  public int score() { return score; }

  public byte[] movesByte() { return moves; }

  public static String moveToString(byte move) {
    return String.valueOf((char) ((move % 10) - 1 + 'a')) + (move / 10);
  }

  public String moves() {
    String result = "";
    for (byte move : moves) {
      if (move == 0) {
        break;
      }
      result += moveToString(move);
    }
    return result;
  }

  @Override
  public String toString() {
    String result = black + " " + score + " - " + (64-score) + " " + white + " (" + tournament + "): ";

    for (byte move : moves) {
      result += moveToString(move);
    }
    return result;
  }
}

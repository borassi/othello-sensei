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

import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Paths;
import java.time.LocalDate;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.TreeSet;

import board.Board;
import helpers.FileAccessor;
import jni.JNI;

public class Thor {
  ArrayList<Game> games = new ArrayList<>();
  TreeMap<Short, ArrayList<Game>> gamesForYear = new TreeMap<>();
  HashMap<Board, ArrayList<Game>> gamesForPosition = new HashMap<>();

  public Thor(String folder) {
    for (String source : new String[] {"WTH", "PLAYOK", "QUEST"}) {
      ArrayList<String> files = new ArrayList<>();
      for (int i = 1977; i <= LocalDate.now().getYear(); ++i) {
        files.add(Paths.get(folder, source + "_" + i + ".wtb").toString());
      }
      loadFiles(
          files,
          Paths.get(folder, source + ".JOU").toString(),
          Paths.get(folder, source + ".TRN").toString()
      );
    }
    for (Short year : gamesForYear.descendingKeySet()) {
      ArrayList<Game> gamesCurYear = gamesForYear.get(year);
      for (int i = gamesCurYear.size() - 1; i >= 0; --i) {
        games.add(gamesCurYear.get(i));
      }
    }
  }

  public SortedSet<String> getTournaments() {
    SortedSet<String> result = new TreeSet<>();
    result.add("");
    for (Game game : games) {
      result.add(game.tournament());
    }
    return result;
  }

  public SortedSet<String> getPlayers() {
    SortedSet<String> result = new TreeSet<>();
    for (Game game : games) {
      result.add(game.black());
      result.add(game.white());
    }
    return result;
  }

  private void addGameToPosition(Board b, Game game) {
    if (!gamesForPosition.containsKey(b)) {
      gamesForPosition.put(b, new ArrayList<>());
    }
    gamesForPosition.get(b).add(game);
  }

  public void lookupPositions(Set<String> black, Set<String> white, Set<String> tournaments) {
    gamesForPosition.clear();
    for (Game game : games) {
      if ((!black.isEmpty() && !black.contains(game.black())) ||
          (!white.isEmpty() && !white.contains(game.white())) ||
          (!tournaments.isEmpty() && !tournaments.contains(game.tournament()))) {
        continue;
      }
      Board b = new Board();
      String moves = game.moves();
      addGameToPosition(b, game);
      for (int i = 0; i < moves.length(); i += 2) {
        b = b.move(moves.substring(i, i+2));
        if (JNI.haveToPass(b)) {
          b = b.move(0);
        }
        addGameToPosition(b, game);
      }
    }
  }

  public ArrayList<Game> getGames(Board child) {
    ArrayList<Game> result = new ArrayList<>();
    HashSet<Board> seenBoards = new HashSet<>();
    for (Board b : child.allTranspositions()) {
      if (seenBoards.contains(b)) {
        continue;
      }
      seenBoards.add(b);
      result.addAll(gamesForPosition.getOrDefault(b, new ArrayList<>()));
    }
    return result;
  }

  public int getNumGames(Board child) {
    return getGames(child).size();
  }

  public void loadFiles(List<String> gamesFilepath, String playersFilepath, String tournamentsFilepath) {
    ArrayList<String> players = new ArrayList<>();
    ArrayList<String> tournaments = new ArrayList<>();
    try {
      DataInputStream s = new DataInputStream(new FileInputStream(playersFilepath));
      s.skip(16);
      while (s.available() > 0) {
        int nextSize = Math.min(s.available(), 2000);
        byte[] buf = new byte[nextSize];
        s.read(buf);
        for (int i = 0; i < buf.length; i += 20) {
          players.add(new String(Arrays.copyOfRange(buf, i, i + 20), StandardCharsets.ISO_8859_1).replaceAll("\0*", ""));
        }
      }
    } catch (IOException e) {
      System.out.println("Cannot load Thor players!");
    }
    try {
      DataInputStream s = new DataInputStream(new FileInputStream(tournamentsFilepath));
      s.skip(16);
      while (s.available() > 0) {
        int nextSize = Math.min(s.available(), 1300);
        byte[] buf = new byte[nextSize];
        s.read(buf);
        for (int i = 0; i < buf.length; i += 26) {
          tournaments.add(new String(Arrays.copyOfRange(buf, i, i + 26), StandardCharsets.ISO_8859_1).replaceAll("\0*", ""));
        }
      }
    } catch (IOException e) {
      System.out.println("Cannot load Thor tournaments!");
    }
    for (String gameFilepath : gamesFilepath) {
      loadGames(gameFilepath, players, tournaments);
    }
  }

  public void loadGames(String filepath, ArrayList<String> players, ArrayList<String> tournaments) {
    try {
      DataInputStream s = new DataInputStream(new FileInputStream(filepath));
      s.skip(10);
      short year = Short.reverseBytes(s.readShort());
      ArrayList<Game> games = gamesForYear.getOrDefault(year, new ArrayList<>());
      s.skip(4);
      while (s.available() > 0) {
        int nextSize = Math.min(s.available(), 6800);
        byte[] buf = new byte[nextSize];
        s.read(buf);
        for (int i = 0; i < buf.length; i += 68) {
          games.add(new Game(buf, i, year, players, tournaments));
        }
      }
      gamesForYear.put(year, games);
    } catch (IOException e) {}
  }
}

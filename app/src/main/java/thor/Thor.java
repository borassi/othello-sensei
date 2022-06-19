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
package thor;

import java.io.DataInputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

import board.Board;
import board.GetMovesCache;
import helpers.FileAccessor;

public class Thor {
  ArrayList<Game> games = new ArrayList<>();
  HashMap<Board, ArrayList<Game>> gamesForPosition = new HashMap<>();

  public Thor() {
    ArrayList<String> thorGames = new ArrayList<>();
    for (int i = 1977; i < 2100; ++i) {
      thorGames.add("games/WTH_" + i + ".wtb");
    }
    loadFiles(thorGames, "games/WTHOR.JOU", "games/WTHOR.TRN");
    loadFiles(Arrays.asList(new String[]{"games/PLAYOK.wtb"}), "games/PLAYOK.JOU", "games/PLAYOK.TRN");
    loadFiles(Arrays.asList(new String[]{"games/QUEST.wtb"}), "games/QUEST.JOU", "games/QUEST.TRN");
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
    result.add("");
    for (Game game : games) {
      result.add(game.black());
      result.add(game.white());
    }
    return result;
  }

  private void addGameToPosition(Board b, Game game) {
    ArrayList<Game> gamesCurrentPosition = gamesForPosition.getOrDefault(b, new ArrayList<>());
    gamesCurrentPosition.add(game);
    gamesForPosition.put(b, gamesCurrentPosition);
  }

  public void lookupPositions(Set<String> black, Set<String> white, Set<String> tournaments) {
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
        if (GetMovesCache.haveToPass(b)) {
          b = b.move(0);
        }
        addGameToPosition(b, game);
      }
    }
  }

  public int getNumGames(Board child) {
    int result = 0;
    HashSet<Board> seenBoards = new HashSet<>();
    for (Board b : child.allTranspositions()) {
      if (seenBoards.contains(b)) {
        continue;
      }
      seenBoards.add(b);
      result += gamesForPosition.getOrDefault(b, new ArrayList<>()).size();
    }
    return result;
  }

  public void loadFiles(List<String> gamesFilepath, String playersFilepath, String tournamentsFilepath) {
    ArrayList<String> players = new ArrayList<>();
    ArrayList<String> tournaments = new ArrayList<>();

    try {
      byte[] playerName = new byte[20];
      DataInputStream s = new DataInputStream(FileAccessor.fileInputStream(playersFilepath));
      s.skip(16);
      while (s.available() > 0) {
        s.read(playerName);
        players.add(new String(playerName, StandardCharsets.ISO_8859_1).replaceAll("\0*", ""));
      }
    } catch (IOException e) {
      System.out.println("Cannot load Thor players!");
    }
    try {
      byte[] tournamentName = new byte[26];
      DataInputStream s = new DataInputStream(FileAccessor.fileInputStream(tournamentsFilepath));
      s.skip(16);
      while (s.available() > 0) {
        s.read(tournamentName);
        tournaments.add(new String(tournamentName, StandardCharsets.ISO_8859_1).replaceAll("\0*", ""));
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
      DataInputStream s = new DataInputStream(FileAccessor.fileInputStream(filepath));
      s.skip(16);
      while (s.available() > 0) {
        String tournament = tournaments.get(Short.reverseBytes(s.readShort()));
        String black = players.get(Short.reverseBytes(s.readShort()));
        String white = players.get(Short.reverseBytes(s.readShort()));
        byte blackScore = s.readByte();
        byte blackScoreTheoretical = s.readByte();
        byte[] moves = new byte[60];
        s.readFully(moves);

        games.add(new Game(tournament, black, white, blackScoreTheoretical, blackScore, moves));
      }
    } catch (IOException e) {
      System.out.println("Cannot load Thor games " + filepath + ".");
    }
  }

  public static void main(String args[]) {
    Thor t = new Thor();
    t.lookupPositions(new HashSet<>(), new HashSet<>(), new HashSet<>());
    System.out.println(t.getNumGames(new Board()));
  }
}

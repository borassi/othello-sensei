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

package helpers;

import bitpattern.BitPattern;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;

import board.Board;
import evaluatedepthone.BoardWithEvaluation;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;

public class LoadDataset {


  /**
   * Helper function to convert little to big endian.
   * @param i the number to convert.
   * @return the converted number.
   */
  public static long littleToBigEndianLong(long i) {
    return (i & 0xffL) << 56 | (i & 0xff00L) << 40 | 
            (i & 0xff0000L) << 24 | (i & 0xff000000L) << 8
            | (i & 0xff00000000L) >>> 8 | (i & 0xff0000000000L) >>> 24 | 
            (i & 0xff000000000000L) >>> 40 | i >>> 56;
  }

  /**
   * Helper function to convert little to big endian.
   * @param i the number to convert.
   * @return the converted number.
   */
  public static int littleToBigEndian(int i) {
    return (i & 0xff) << 24 | (i & 0xff00) << 8 | (i & 0xff0000) >>> 8
            | (i >>> 24) & 0xff;
  }

  public static final ArrayList<BoardWithEvaluation> loadTestingSet() {
    return loadTrainingSet(2015, 2016);
  }
  public static final ArrayList<BoardWithEvaluation> loadTrainingSet() {
    return loadTrainingSet(1977, 2014);
  }
  
  public static final ArrayList<BoardWithEvaluation> loadTrainingSet(
      ObjectArrayList<String>files) {
    ArrayList<BoardWithEvaluation> trainingSet = new ArrayList<>();
    for (String file : files) {
      try (DataInputStream stream = new DataInputStream(new FileInputStream(file))) {
        System.out.println(file);
        System.out.println(stream.available());
        while (stream.available() > 0) {
          Byte b = -1;
          long player = littleToBigEndianLong(stream.readLong());
          long opponent = littleToBigEndianLong(stream.readLong());
          int eval = littleToBigEndian(stream.readInt()) * 100;
          trainingSet.add(new BoardWithEvaluation(new Board(
            player, opponent), eval));
        }
      } catch (IOException ex) {
        ex.printStackTrace();
      }      
    }
    return trainingSet;
  }
  /**
   * Loads the training set as an ArrayList<BoardWithEvaluation>.
   * @return The training set
   */
  public static final ArrayList<BoardWithEvaluation> loadTrainingSet(int startYear, int endYear) {
    ObjectArrayList<String> files = new ObjectArrayList<>();
    for (int i = startYear; i <= endYear; i++) {
      files.add("training_set/TrainingSet" + i + ".tmp");
    }
    return loadTrainingSet(files);
  }
  public static final ArrayList<BoardWithEvaluation> loadOMGSet(int endFile) {
    ObjectArrayList<String> files = new ObjectArrayList<>();
    for (int i = 0; i <= endFile; i++) {
      files.add("tmp/weird_positions_result_" + i + ".tmp");
    }
    return loadTrainingSet(files);
  }
}

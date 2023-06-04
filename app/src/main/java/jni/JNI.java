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

import java.util.ArrayList;

import board.Board;
import constants.Constants;
import evaluateposition.Status;
import helpers.FileAccessor;

public class JNI {
  private long pointer;

  public long getPointer() {
    return pointer;
  }

  public JNI(FileAccessor fileAccessor) {
    pointer = 0;
    create(fileAccessor);
  }
  public native void create(FileAccessor fileAccessor);

  @Override
  protected native void finalize();

  private void setup(long pointer) {
    this.pointer = pointer;
  }

  static {
    if (Constants.MOBILE) {
      System.loadLibrary("jni");
    } else {
      System.load(System.getProperty("user.dir") + "/build/jni/libjni.so");
    }
  }

  public native void evaluate(
      ArrayList<Board> boards, int lower,
    int upper, long maxNVisited, int maxTimeMillis, float gap, boolean approx);

  public native void empty();
  public native void stop();
  public native Status getStatus();
  public native TreeNodeCPP getFirstPosition();
  public native TreeNodeCPP get(long player, long opponent);
  public native TreeNodeCPP getFromBook(long player, long opponent);

  public native boolean finished(long maxNVisited);

  public static native Board getEndgameBoard(int i);

  public static native Board move(Board board, int move);

  public static native boolean haveToPass(Board board);

  public static native ArrayList<Board> children(Board board);

  public static native ArrayList<Board> uniqueChildren(Board board);

  public native void addToBook(Board father, ArrayList<Board> parents);

  public native static String prettyPrintDouble(double d);

  public static boolean isGameOver(Board b) {
    return haveToPass(b) && haveToPass(b.move(0));
  }
}

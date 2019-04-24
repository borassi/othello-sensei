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

package bitpattern;

import java.util.ArrayList;

public class BitPatternInt {
  public final static int SIGN_BIT_PATTERN = parsePattern(
          "X-------\n" +
          "--------\n" +
          "--------\n" +
          "--------\n");
  public final static int VALUE_BIT_PATTERN = parsePattern(
          "X-------\n" +
          "--------\n" +
          "--------\n" +
          "XXXXXXXX\n");

  /**
   * Parses a string pattern into a 32-bit int, as follows: 
   * - it removes any newline. 
   * - it sets '1' to the bits corresponding to the input
   *   variable 'letter', '0' to all the other bits.
   * 
   * @param pattern
   *          the string pattern
   * @param letter
   *          the letter to be used
   * @return the converted string
   */
  public final static int parsePattern(String pattern, char letter) {
    int result = 0;
    pattern = pattern.replace("\n", "");
    for (int i = 0; i < 32; i++) {
      result = result << 1;
      if (pattern.charAt(i) == letter) {
        result = result | 1;
      }
    }
    return result;
  }

  /**
   * See parsePattern(String pattern, char letter).
   * 
   * @param pattern
   * @return
   */
  public final static int parsePattern(String pattern) {
    return parsePattern(pattern, 'X');
  }

  /**
   * Converts a pattern to a string, by putting 'X' where the corresponding
   * bit is 1, '-' where the corresponding bit is 0
   * @param pattern the bit pattern
   * @return the string version of the pattern.
   */
  public final static String patternToString(int pattern) {
    String result = "";
    for (int i = 0; i < 32; i++) {
      if (pattern >>> 31 == 1) {
        result += "X";
      } else {
        result += "-";
      }
      pattern = pattern << 1;
      if (i % 8 == 7) {
        result += "\n";
      }
    }
    return result;
  }
}

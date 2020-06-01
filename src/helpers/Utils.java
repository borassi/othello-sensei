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

public class Utils {
  public static String prettyPrintDouble(double l) {
    if (l == Double.POSITIVE_INFINITY) {
      return "+Inf";
    } else if (l == Double.NEGATIVE_INFINITY) {
      return "-Inf";
    } else if (l == 0) {
      return "0";
    }
    String sign = "";
    if (l < 0) {
      sign = "-";
      l = -l;
    }
    String[] suffixes = {"", "k", "M", "G", "T", "P", "E", "Z", "Y"};
    int orderOfMagnitude = (int) Math.log10(l);
    
    if (orderOfMagnitude / 3 >= suffixes.length) {
      return String.format("%s%.1f", sign, l);
    }
    String suffix = suffixes[orderOfMagnitude / 3];
    
    double rescaledL = l / Math.pow(10, (orderOfMagnitude / 3) * 3);
    
    if (orderOfMagnitude % 3 == 0) {
      return String.format("%s%.1f%s", sign, rescaledL, suffix);
    }
    return String.format("%s%.0f%s", sign, rescaledL, suffix);
  }
  
  public static boolean arrayContains(Object[] array, Object elem) {
    for (Object obj : array) {
      if (elem == obj) {
        return true;
      }
    }
    return false;
  }
}

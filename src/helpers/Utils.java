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

import java.math.BigDecimal;
import java.math.MathContext;

public class Utils {
  public static String prettyPrintDouble(double d) {
    if (d == Double.POSITIVE_INFINITY) {
      return "+Inf";
    } else if (d == Double.NEGATIVE_INFINITY) {
      return "-Inf";
    } else if (Math.abs(d) < 1.E-40) {
      return "0";
    }
    if (d < 0) {
      return "-" + prettyPrintDouble(-d);
    }
    BigDecimal bd = new BigDecimal(d);
    bd = bd.round(new MathContext(2));
    d = bd.doubleValue();
    String[] suffixes = {"p", "n", "μ", "m", "", "k", "M", "G", "T", "P", "E", "Z", "Y"};
    int orderOfMagnitude = (int) Math.floor(Math.log10(d));
    int suffixesPosition = (int) (orderOfMagnitude + 12) / 3;

    if (suffixesPosition >= suffixes.length || suffixesPosition < 0) {
      return String.format("%.1E", d);
    }
    String suffix = suffixes[suffixesPosition];
    
    double rescaledL = d / Math.pow(10, ((orderOfMagnitude + 300) / 3) * 3 - 300);
    
    if (orderOfMagnitude % 3 == 0) {
      return String.format("%.1f%s", rescaledL, suffix);
    }
    return String.format("%.0f%s", rescaledL, suffix);
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

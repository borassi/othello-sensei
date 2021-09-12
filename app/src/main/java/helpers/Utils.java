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

import board.GetMovesCache;
import java.math.BigDecimal;
import java.math.MathContext;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Utils {
  public static String prettyPrintDouble(double d) {
    if (d == Double.POSITIVE_INFINITY) {
      return "+Inf";
    } else if (d == Double.NEGATIVE_INFINITY) {
      return "-Inf";
    } else if (Double.isNaN(d)) {
      return "NaN";
    } else if (Math.abs(d) < 1.E-20) {
      return "0";
    }
    if (d < 0) {
      return "-" + prettyPrintDouble(-d);
    }
    BigDecimal bd = new BigDecimal(d);
    bd = bd.round(new MathContext(3));
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
  
  
  public static long testFunctionsInParallel(int nTasks, List<Runnable> functions) {
    ExecutorService es = Executors.newFixedThreadPool(nTasks);
    long t = -System.currentTimeMillis();
    for (Runnable function : functions) {
      es.submit(function);
    }
    es.shutdown();
    try {
      es.awaitTermination(1000, TimeUnit.HOURS);
    } catch (InterruptedException ex) {
      Logger.getLogger(Utils.class.getName()).log(Level.SEVERE, null, ex);
    }
    t += System.currentTimeMillis();
    return t;
  }
}

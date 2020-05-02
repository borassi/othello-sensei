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

/**
 *
 * @author michele
 */

public class Gaussian {

    // See https://math.stackexchange.com/questions/2006392/taylor-expansion-of-normal-cdf.
    public static double CDFStd(double x) {
      if (x < 0) {
        return 1 - CDFStd(-x);
      }
      double k = 1 / (1 + 0.2316419 * x);
      double k2 = k * k;
      double k3 = k2 * k;
      double k4 = k3 * k;
      double k5 = k4 * k;
      
      return 1 - 1 / Math.sqrt(2 * Math.PI) * Math.exp(- x * x / 2) * (
        0.319381530 * k - 0.356563782 * k2 + 1.781477937 * k3 - 1.821255978 * k4 + 1.330274429 * k5);
    }

    public static double CDF(double x, double mean, double stddev) {
        return CDFStd((x - mean) / stddev);
    }

}

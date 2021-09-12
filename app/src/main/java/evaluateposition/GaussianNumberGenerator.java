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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package evaluateposition;

import java.util.Random;

/**
 *
 * @author michele
 */
public class GaussianNumberGenerator {
  public float nums[] = new float[100000];
  public int currentNumber = 0;
  
  public GaussianNumberGenerator() {
    Random random = new Random();
    for (int i = 0; i < nums.length; i++) {
      nums[i] = (float) random.nextGaussian();
    }
  }
  
  public float next() {
    if (currentNumber >= nums.length - 1) {
      currentNumber = -1;
    }
    return nums[++currentNumber];
  }
}

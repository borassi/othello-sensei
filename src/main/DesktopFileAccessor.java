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
package main;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

/**
 *
 * @author michele
 */
public class DesktopFileAccessor implements FileAccessor {
  @Override
  public ObjectInputStream inputFile(String filename) throws IOException {
    return new ObjectInputStream(new FileInputStream(filename));
  }

  @Override
  public ObjectOutputStream outputFile(String filename) throws IOException {
    return new ObjectOutputStream(new FileOutputStream(filename));
  }
}
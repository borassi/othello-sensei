// Copyright 2021 Google LLC
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

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

public class FileAccessor {
  public Object getAssetManager() { return null; }

  public InputStream fileInputStream(String filepath) throws IOException {
//    InputStream result = Thread.currentThread().getContextClassLoader().getResourceAsStream(filepath);
//    if (result != null) {
//      return result;
//    }
    return new FileInputStream("app/src/main/assets/" + filepath);
  }

  public FileOutputStream fileOutputStream(String filepath) throws FileNotFoundException {
    return new FileOutputStream("app/src/main/assets/" + filepath);
  }

  public ObjectOutputStream objectOutputStream(String filepath) throws IOException {
    return new ObjectOutputStream(fileOutputStream(filepath));
  }

  public ObjectInputStream objectInputStream(String filepath) throws IOException {
    return new ObjectInputStream(fileInputStream(filepath));
  }
}

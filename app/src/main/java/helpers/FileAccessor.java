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

import constants.Constants;
import ui.MobileUI;

public class FileAccessor {

  public static InputStream fileInputStream(String filepath) throws IOException {
    if (Constants.MOBILE) {
      return MobileUI.context.getAssets().open(filepath);
    } else {
      return new FileInputStream("app/desktop_res/" + filepath);
    }
  }

  public static FileOutputStream fileOutputStream(String filepath) throws FileNotFoundException {
    if (Constants.MOBILE) {
      return null;
    } else {
      return new FileOutputStream("app/desktop_res/" + filepath);
    }
  }

  public static ObjectOutputStream objectOutputStream(String filepath) throws IOException {
    return new ObjectOutputStream(fileOutputStream(filepath));
  }

  public static ObjectInputStream objectInputStream(String filepath) throws IOException {
    return new ObjectInputStream(fileInputStream(filepath));
  }
}

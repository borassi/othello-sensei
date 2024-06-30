/*
 * Copyright 2023 Michele Borassi
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "files.h"

void CreateFileIfNotExists(const std::string& filepath) {
  if (std::fstream(filepath, std::ios::in).is_open()) {
    return;
  }
  CreateEmptyFileWithDirectories(filepath);
}

void CreateEmptyFileWithDirectories(const std::string& filepath) {
  fs::create_directories(fs::path(filepath).remove_filename());
  std::ofstream(filepath, std::ios::out).close();
}

FileOffset FileLength(std::fstream& file) {
  assert(file.is_open());
  auto position = file.tellg();
  file.seekg(0, std::ios_base::end);
  auto result = file.tellg();
  file.seekg(position, std::ios_base::beg);
  return (FileOffset) result;
}
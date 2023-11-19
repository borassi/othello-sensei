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

#include <experimental/filesystem>

#include "files.h"

void CreateFileIfNotExists(std::string filepath) {
  if (std::fstream(filepath, std::ios::in).is_open()) {
    return;
  }
  ResetFile(filepath);
}


void ResetFile(std::string filepath) {
  auto path = std::experimental::filesystem::path(filepath);
  std::experimental::filesystem::create_directories(path.remove_filename());
  std::ofstream(filepath, std::ios::out).close();
//  file_ = std::fstream(filepath_, std::ios::binary | std::ios::out | std::ios::in);
//  file_.seekg(0, std::ios::end);
//  assert(file_.is_open());
}

FileOffset FileLength(std::fstream& file) {
  assert(file.is_open());
  auto position = file.tellg();
  file.seekg(0, std::ios_base::end);
  auto result = file.tellg();
  file.seekg(position, std::ios_base::beg);
  return result;
}
/*
 * Copyright 2022 Google LLC
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

#include "file.h"

#include <fstream>


std::fstream OpenFile(const std::string& filename) {
  std::fstream file(filename, std::ios::binary | std::ios::out | std::ios::in);
  if (!file) {
    throw std::invalid_argument("Missing " + filename);
  }
  return file;
}

int FileSize(std::fstream& file) {
  int position = file.tellg();
  file.seekg(0, std::ios::end);
  int size = file.tellg();
  file.seekg(position, std::ios::beg);
  return size;
}
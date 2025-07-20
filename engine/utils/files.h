/*
 * Copyright 2023-2025 Michele Borassi
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

#ifndef OTHELLOSENSEI_APP_SRC_MAIN_CPP_UTILS_FILES_H_
#define OTHELLOSENSEI_APP_SRC_MAIN_CPP_UTILS_FILES_H_

#include <assert.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

constexpr char kAssetFilepath[] = "app/src/main/assets";

typedef long FileOffset;

namespace fs = ::std::filesystem;

void CreateEmptyFileWithDirectories(const std::string& filepath);

void CreateFileIfNotExists(const std::string& filepath);

FileOffset FileLength(std::fstream& file);

template<typename T>
std::vector<T> ReadFile(const std::string& filename) {
  std::fstream file = std::fstream(filename, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    return {};
  }
  std::vector<T> result;
  FileOffset length = FileLength(file);
  result.resize(length * sizeof(char) / sizeof(T));
  file.read((char*) result.data(), length);
  return result;
}

std::vector<std::string> GetAllFilesMostRecentFirst(const std::string& directory, bool include_files, bool include_directories);

std::vector<std::string> GetAllFiles(const std::string& directory, bool include_files, bool include_directories);

bool FileExists(const std::string& filename);

std::string Filename(const std::string& filepath);

std::string LoadTextFile(const std::string& filepath);

#endif //OTHELLOSENSEI_APP_SRC_MAIN_CPP_UTILS_FILES_H_

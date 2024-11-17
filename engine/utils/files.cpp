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

#if __APPLE__
#include <dirent.h>
#include <sys/stat.h>
#endif

#include "files.h"
#include "misc.h"

constexpr char kPathSeparator = '/';

void CreateFileIfNotExists(const std::string& filepath) {
  if (FileExists(filepath)) {
    return;
  }
  CreateEmptyFileWithDirectories(filepath);
}

void CreateEmptyFileWithDirectories(const std::string& filepath) {
#if __APPLE__
  auto split_filepath = Split(filepath, kPathSeparator);
  std::string path_so_far = split_filepath[0];
  int mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
  struct stat sb;

  for (int i = 1; i < split_filepath.size() - 1; ++i) {
    path_so_far = path_so_far + kPathSeparator + split_filepath[i];
    if (stat(path_so_far.c_str(), &sb) == -1) {
      mkdir(path_so_far.c_str(), mode);
    } else {
      mode = sb.st_mode;
    }
  }
#else
  // Works only after MacOS 10.15, we want to be compatible with MacOS 10.11.
  fs::create_directories(fs::path(filepath).remove_filename());
#endif
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

std::vector<std::string> GetAllFiles(const std::string& directory, bool include_files, bool include_directories) {
  std::vector<std::string> result;
#if __APPLE__
  struct dirent *entry = nullptr;
  DIR *dp = nullptr;

  dp = opendir(directory.c_str());
  if (dp == nullptr) {
    return result;
  }
  while ((entry = readdir(dp))) {
    if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
      continue;
    }
    bool is_file = entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN;
    bool is_directory = entry->d_type == DT_DIR;
    if ((is_file && include_files) || (is_directory && include_directories)) {
      result.push_back(directory + (EndsWith(directory, {kPathSeparator}) ? "" : std::string{kPathSeparator}) + entry->d_name);
    }
  }
#else
  for (const auto& entry : fs::directory_iterator(directory)) {
    if ((entry.is_regular_file() && include_files) || (entry.is_directory() && include_directories)) {
      result.push_back(entry.path().string());
    }
  }
#endif
  return result;
}

bool FileExists(const std::string& filename) {
  return std::fstream(filename, std::ios::in).is_open();
}

std::string Filename(const std::string& filepath) {
  auto split = Split(filepath, kPathSeparator);
  return split.back();
}
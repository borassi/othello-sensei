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

#ifdef _MSC_VER
constexpr char kPathSeparator = '\\';
#else
constexpr char kPathSeparator = '/';
#endif

void CreateFileIfNotExists(const std::string& filepath) {
  if (FileExists(filepath)) {
    return;
  }
  CreateEmptyFileWithDirectories(filepath);
}

void CreateEmptyFileWithDirectories(const std::string& filepath) {
  fs::create_directories(fs::path(filepath).remove_filename());
  std::ofstream(filepath, std::ios::out | std::ios::binary).close();
}

FileOffset FileLength(std::fstream& file) {
  assert(file.is_open());
  auto position = file.tellg();
  file.seekg(0, std::ios_base::end);
  auto result = file.tellg();
  file.seekg(position, std::ios_base::beg);
  return (FileOffset) result;
}

namespace {
// Structure to hold file system entry information for sorting
struct FileWithWriteTime {
  std::string path;
  fs::file_time_type last_write_time;

  // Custom comparison operator for sorting in decreasing order of modification time
  bool operator>(const FileWithWriteTime& other) const {
    return last_write_time > other.last_write_time;
  }
};
}  // namespace

/**
 * @brief Fetches paths of file system entries (files and directories) in a given directory,
 * sorted in decreasing order of modification time.
 *
 * @param directory The path to the directory to search.
 * @param include_files Whether to include files.
 * @param include_directories Whether to include directories.
 * @return A vector of strings representing the paths, sorted by modification time (newest first).
 * Returns an empty vector if the directory does not exist or is not accessible.
 */
std::vector<std::string> GetAllFilesMostRecentFirst(const std::string& directory, bool include_files, bool include_directories) {
  std::vector<FileWithWriteTime> entries;
  std::vector<std::string> sortedPaths;
  fs::path dir_path(directory);

  try {
    for (const auto& path : GetAllFiles(directory, include_files, include_directories)) {
      try {
        entries.push_back({path, fs::last_write_time(path)});
      } catch (const fs::filesystem_error& e) {
        std::cerr << "Error accessing entry " << path << ": " << e.what() << std::endl;
      }
    }

    // Sort the temporary entries vector using the custom comparison operator
    std::sort(entries.begin(), entries.end(), std::greater<FileWithWriteTime>());

    // Extract just the paths into the result vector
    for (const auto& entry : entries) {
      sortedPaths.push_back(entry.path);
    }
  } catch (const fs::filesystem_error& e) {
    std::cerr << "Error iterating directory " << directory << ": " << e.what() << std::endl;
  }

  return sortedPaths;
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
  std::error_code error;
  for (const auto& entry : fs::directory_iterator(directory, error)) {
    if ((entry.is_regular_file() && include_files) || (entry.is_directory() && include_directories)) {
      result.push_back(entry.path().string());
    }
  }
#endif
  return result;
}

bool FileExists(const std::string& filename) {
  return std::fstream(filename, std::ios::in | std::ios::binary).is_open();
}

std::string Filename(const std::string& filepath) {
  auto split = Split(filepath, kPathSeparator);
  return split.back();
}

std::string LoadTextFile(const std::string& filepath) {
  std::ifstream ifstream(filepath);
  if (!ifstream.is_open()) {
    return "";
  }
  return std::string(std::istreambuf_iterator<char>(ifstream),
                     std::istreambuf_iterator<char>());
}
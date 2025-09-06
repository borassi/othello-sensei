/*
 * Copyright 2025 Michele Borassi
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

#include "fix_xot_source.h"

namespace {
FindXOTFileResults FixXOTFile(const XOT& xot, const std::string& input_filepath, const std::string& output_filepath) {
  std::vector<char> input = ReadFile<char>(input_filepath);
  std::ofstream output(output_filepath, std::ios::binary);
  output.write(input.data(), 16);
  FindXOTFileResults results;

  for (int i = 16; i < input.size(); i += 68) {
    Sequence sequence = Sequence::FromThor((Square*) input.data() + i + 8);
    if (sequence.Size() > 52 || sequence.IsValid()) {
      assert(sequence.IsValid());
      ++results.ok;
      output.write(input.data() + i, 68);
      continue;
    }
    std::vector<Sequence> starts = xot.FindStart(sequence, 2);
    if (starts.size() == 0) {
      ++results.not_found;
    } else if (starts.size() == 1) {
      ++results.fixed;
      Sequence final = starts[0];
      final.Extend(sequence);
      auto final_thor = final.ToThor();
      output.write(input.data() + i, 8);
      output.write((char*) final_thor.data(), 60);
    } else {
      ++results.multiple_found;
    }
  }
  return results;
}
}  // namespace

std::ostream& operator<<(std::ostream& stream, const FindXOTFileResults& s) {
  stream << "ok: " << s.ok << "  fixed: " << s.fixed << "  not_found: " << s.not_found << "  multiple_found: " << s.multiple_found;
  return stream;
}

FindXOTFileResults FixXOTSource(
    const XOT& xot,
    const std::string& input_folder,
    const std::string& output_folder) {
  fs::create_directories(output_folder);

  FindXOTFileResults results;
  for (const std::string& input_filepath : GetAllFiles(input_folder, /*include_files=*/true, /*include_directories=*/false)) {
    fs::path output_filepath = fs::path(output_folder) / fs::path(input_filepath).filename();
    if (EndsWith(ToLower(input_filepath), ".wtb")) {
      results.Merge(FixXOTFile(xot, input_filepath, output_filepath.string()));
    } else {
      fs::copy(input_filepath, output_filepath);
    }
  }
  return results;
}
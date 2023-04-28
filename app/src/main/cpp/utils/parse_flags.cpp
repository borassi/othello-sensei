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
 #include "parse_flags.h"

ParseFlags::ParseFlags(int argc, const char* const argv[]) {
  std::string last_flag;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    std::smatch matches;
    if (std::regex_match(arg, matches, std::regex("--?([^=]*)(?:=(.*))?"))) {
      flags_[matches[1]] = matches[2];
      last_flag = matches[1];
    } else {
      if (last_flag.empty() || flags_[last_flag].empty()) {
        throw ParseFlagsException(
            "Flag " + arg + " is not in the format --flag_name [flag_value].");
      }
      flags_[last_flag] = arg;
      last_flag = "";
    }
  }
}

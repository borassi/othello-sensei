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

#include "pattern.h"

std::size_t std::hash<Pattern>::operator()(const Pattern& f) const noexcept {
  stringstream s;
  s << f;
  return std::hash<std::string>{}(s.str());
}

std::array<Pattern, kMaxPatternTranspositions> Pattern::AllTranspositions(
    BitPattern p) {
  Pattern result_array[8];
  std::array<Pattern, kMaxPatternTranspositions> result;
  AllTranspositions(p, result_array);
  for (int i = 0; i < kMaxPatternTranspositions; ++i) {
    result[i] = result_array[i];
  }
  return result;
}

std::ostream& operator<<(std::ostream& s, const Pattern& f) {
  s << "(";
  for (int i = 0; i < f.size_; ++i) {
    s << (int) f.squares_[i] << " ";
  }
  s << ")";
  return s;
}
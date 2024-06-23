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

// A constexpr random generator.

#ifndef RANDOM_H
#define RANDOM_H

#include <cstdint>

struct Random {
 public:
  constexpr Random() :
    last_value_(1940869496),
    a_(2891336453),
    c_(7) {}

  constexpr uint32_t next() {
    last_value_ = (a_ * last_value_ + c_);
    return last_value_;
  }
 private:
  uint32_t last_value_;
  const uint32_t a_;
  const uint32_t c_;
};

#endif  // RANDOM_H
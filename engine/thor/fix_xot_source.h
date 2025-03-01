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

#ifndef OTHELLO_SENSEI_THOR_FIX_XOT_SOURCE_H
#define OTHELLO_SENSEI_THOR_FIX_XOT_SOURCE_H

#include <string>
#include "source.h"
#include "../xot/xot.h"
#include "../utils/files.h"

struct FindXOTFileResults {
  long ok = 0;
  long not_found = 0;
  long fixed = 0;
  long multiple_found = 0;

  void Merge(const FindXOTFileResults& other) {
    ok += other.ok;
    not_found += other.not_found;
    fixed += other.fixed;
    multiple_found += other.multiple_found;
  }
};

std::ostream& operator<<(std::ostream& stream, const FindXOTFileResults& s);

FindXOTFileResults FixXOTSource(
    const XOT& xot,
    const std::string& input_folder,
    const std::string& output_folder);

#endif  // OTHELLO_SENSEI_THOR_FIX_XOT_SOURCE_H
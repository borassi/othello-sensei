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

#include <array>
#include "../board/bitpattern.h"

constexpr BitPattern kCorner4x4 = ParsePattern("--------"
                                     "--------"
                                     "--------"
                                     "--------"
                                     "------XX"
                                     "-----XXX"
                                     "----XXXX"
                                     "----XXXX");

constexpr BitPattern kCornerSided = ParsePattern("--------"
                                       "--------"
                                       "--------"
                                       "-------X"
                                       "-------X"
                                       "-------X"
                                       "------XX"
                                       "---XXXXX");

constexpr BitPattern kDiagonalImproved = ParsePattern("XX------"
                                             "XX------"
                                             "--X-----"
                                             "---X----"
                                             "----X---"
                                             "-----X--"
                                             "------XX"
                                             "------XX");

constexpr BitPattern kCorner = ParsePattern("--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "-X----X-"
                                  "X------X");

constexpr BitPattern kLastRowSmall = ParsePattern("--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "--------"
                                  "-XXXXXX-");

constexpr BitPattern k2LastRowSmall = ParsePattern("--------"
                                        "--------"
                                        "--------"
                                        "--------"
                                        "--------"
                                        "--------"
                                        "--XXXX--"
                                        "--------");

constexpr BitPattern k3LastRowSmall = ParsePattern("--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "--XXXX--"
                                         "--------"
                                         "--------");

constexpr BitPattern k4LastRowSmall = ParsePattern("--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "--XXXX--"
                                         "--------"
                                         "--------"
                                         "--------");

constexpr BitPattern kCorner5x2 = ParsePattern("--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "--------"
                                         "---XXXXX"
                                         "---XXXXX");

std::vector<std::vector<BitPattern>> kFeatureGroups = {
  {kCorner, kLastRowSmall, k2LastRowSmall, k3LastRowSmall, k4LastRowSmall},
  {kDiagonalImproved},
  {kCorner4x4},
  {kLastRowPattern << 16},
  {kLastRowPattern << 24},
  {kMainDiag7Pattern << 8},
  {kMainDiag7Pattern << 16},
};

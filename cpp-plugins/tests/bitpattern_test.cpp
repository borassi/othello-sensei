/*
 * Copyright 2021 Google LLC
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

#include "bitpattern_test.h"
#include "../bitpattern.h"


CPPUNIT_TEST_SUITE_REGISTRATION(bitpattern_test);

void bitpattern_test::testNeighbors() {
  BitPattern p = ParsePattern("X--X---X"
                              "--------" 
                              "--------"
                              "X-------"
                              "-------X"
                              "--------"
                              "-------X"
                              "XXX-----");
  BitPattern expected = ParsePattern("-XX-X-X-"
                                     "XXXXX-XX" 
                                     "XX------"
                                     "-X----XX"
                                     "XX----X-"
                                     "------XX"
                                     "XXXX--X-"
                                     "XXXX--XX");
  std::cout << PatternToString(Neighbors(p));
  CPPUNIT_ASSERT(Neighbors(p) == expected);
}
void bitpattern_test::testUniqueInEdges() {
  BitPattern p1 = ParsePattern("XXXX-XXX"
                               "--------" 
                               "X------X"
                               "X-------"
                               "X------X"
                               "-------X"
                               "X------X"
                               "XXXXXXXX");
  BitPattern expected1 = ParsePattern("----X---"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------");
  std::cout << PatternToString(FirstLastInEdges(~p1));
  BitPattern expectedFirstLast = ParsePattern("----X---"
                                              "X------X"
                                              "--------"
                                              "-------X"
                                              "--------"
                                              "X-------"
                                              "--------"
                                              "--------");
  CPPUNIT_ASSERT(FirstLastInEdges(~p1) == expectedFirstLast);
  CPPUNIT_ASSERT(UniqueInEdges(~p1) == expected1);
  BitPattern p2 = ParsePattern("X-XXXXXX"
                               "X-------" 
                               "X------X"
                               "X------X"
                               "X------X"
                               "-------X"
                               "X------X"
                               "XXXXXXX-");
  BitPattern expected2 = ParsePattern("-X------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "--------"
                                      "X-------"
                                      "--------"
                                      "-------X");
  std::cout << PatternToString(UniqueInEdges(~p2));
  CPPUNIT_ASSERT(UniqueInEdges(~p2) == expected2);
}


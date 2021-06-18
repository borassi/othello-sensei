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

#ifndef STABLE_TEST_H
#define STABLE_TEST_H

#include <cppunit/extensions/HelperMacros.h>

class stable : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(stable);

  CPPUNIT_TEST(testSimple);
  CPPUNIT_TEST(testLeft);
  CPPUNIT_TEST(testCenter);
  CPPUNIT_TEST(testUpdate);
  CPPUNIT_TEST(testGetFullRowsColumns);
  CPPUNIT_TEST(testAll);

  CPPUNIT_TEST_SUITE_END();

private:
  void testSimple();
  void testLeft();
  void testCenter();
  void testUpdate();
  void testGetFullRowsColumns();
  void testAll();
};

#endif /* STABLE_TEST_H */


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

#ifndef NEWTESTCLASS1_H
#define NEWTESTCLASS1_H

#include <cppunit/extensions/HelperMacros.h>

class evaluator_last_moves_test : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(evaluator_last_moves_test);

  CPPUNIT_TEST(testDoublePass);
  CPPUNIT_TEST(testManyPass);
  CPPUNIT_TEST(testE2E);

  CPPUNIT_TEST_SUITE_END();

private:
  void testDoublePass();
  void testManyPass();
  void testE2E();
};

#endif /* NEWTESTCLASS1_H */


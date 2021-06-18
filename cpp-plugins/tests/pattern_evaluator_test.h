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

#ifndef PATTERN_EVALUATOR_TEST_H
#define PATTERN_EVALUATOR_TEST_H

#include <cppunit/extensions/HelperMacros.h>

class pattern_evaluator_test : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(pattern_evaluator_test);

  CPPUNIT_TEST(testMethod);
  CPPUNIT_TEST(testFailedMethod);

  CPPUNIT_TEST_SUITE_END();

public:
  pattern_evaluator_test();
  virtual ~pattern_evaluator_test();
  void setUp();
  void tearDown();

private:
  void testMethod();
  void testFailedMethod();
};

#endif /* PATTERN_EVALUATOR_TEST_H */


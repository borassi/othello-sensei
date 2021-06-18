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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <cppunit/portability/Stream.h>

class ProgressListener : public CPPUNIT_NS::TestListener {
public:

  ProgressListener()
  : m_lastTestFailed(false) {
  }

  ~ProgressListener() {
  }

  void startTest(CPPUNIT_NS::Test *test) {
    CPPUNIT_NS::stdCOut() << test->getName();
    CPPUNIT_NS::stdCOut() << "\n";
    CPPUNIT_NS::stdCOut().flush();

    m_lastTestFailed = false;
  }

  void addFailure(const CPPUNIT_NS::TestFailure &failure) {
    CPPUNIT_NS::stdCOut() << " : " << (failure.isError() ? "error" : "assertion");
    m_lastTestFailed = true;
  }

  void endTest(CPPUNIT_NS::Test *test) {
    if (!m_lastTestFailed)
      CPPUNIT_NS::stdCOut() << " : OK";
    CPPUNIT_NS::stdCOut() << "\n";
  }

private:
  /// Prevents the use of the copy constructor.
  ProgressListener(const ProgressListener &copy);

  /// Prevents the use of the copy operator.
  void operator=(const ProgressListener &copy);

private:
  bool m_lastTestFailed;
};

int main() {
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener(&result);

  // Add a listener that print dots as test run.
  ProgressListener progress;
  controller.addListener(&progress);

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
  runner.run(controller);

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter(&result, CPPUNIT_NS::stdCOut());
  outputter.write();

  return result.wasSuccessful() ? 0 : 1;
}

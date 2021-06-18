/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   newtestclass.h
 * Author: michele
 *
 * Created on May 16, 2021, 10:39:43 AM
 */

#ifndef NEWTESTCLASS_H
#define NEWTESTCLASS_H

#include <cppunit/extensions/HelperMacros.h>

class newtestclass : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(newtestclass);

  CPPUNIT_TEST(testMethod);
  CPPUNIT_TEST(testFailedMethod);

  CPPUNIT_TEST_SUITE_END();

public:
  newtestclass();
  virtual ~newtestclass();
  void setUp();
  void tearDown();

private:
  void testMethod();
  void testFailedMethod();
};

#endif /* NEWTESTCLASS_H */


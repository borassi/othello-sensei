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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <math.h>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include "misc.h"

using ::testing::ElementsAre;

TEST(ConstexprMath, Log) {
  for (double x : {1E-30, 0.000001, 0.01, 0.5, 1.0, 1.5, 2.0, 10.0, 10000.0, 100000000.0, DBL_MAX}) {
    EXPECT_FLOAT_EQ(ConstexprLog(x), log(x));
  }
}

//TEST(ConstexprMath, Exp) {
//  for (double x : {-DBL_MAX, -10.5, -4.0, -0.01, 1E-30, 0.0, 0.1, DBL_MAX}) {
//    EXPECT_FLOAT_EQ(ConstexprExp(x), exp(x));
//  }
//}

TEST(ConstexprMath, Pow) {
  for (double x : {0.01, 0.1, 1.0, 2.3, 10.6}) {
    for (double y : {-5.0, -2.0, -1.5, -1.0, -0.01, 0.0, 0.01, 1.0, 10.0}) {
      EXPECT_FLOAT_EQ(ConstexprPow(x, y), pow(x, y));
    }
  }
}

TEST(Vector, VectorContains) {
  EXPECT_TRUE(Contains<int>({1, 2, 3}, 2));
  EXPECT_FALSE(Contains<int>({1, 2, 3}, 4));
}

TEST(Misc, GetOrDefault) {
  std::unordered_map<std::string, int> map;
  map["luke"] = 1;
  map["vader"] = 2;
  EXPECT_EQ(1, (GetOrDefault(map, std::string("luke"), 10)));
  EXPECT_EQ(2, (GetOrDefault(map, std::string("vader"), 10)));
  EXPECT_EQ(10, (GetOrDefault(map, std::string("leila"), 10)));
}

TEST(Misc, ContainsMap) {
  std::unordered_map<std::string, int> map;
  map["luke"] = 1;
  map["vader"] = 2;
  EXPECT_TRUE(Contains(map, std::string("luke")));
  EXPECT_FALSE(Contains(map, std::string("leila")));
}

TEST(Misc, ContainsSet) {
  std::unordered_set<std::string> set;
  set.insert("luke");
  set.insert("vader");
  EXPECT_TRUE(Contains(set, std::string("luke")));
  EXPECT_FALSE(Contains(set, std::string("leila")));
}

TEST(Misc, PrettyPrintDouble) {
  EXPECT_EQ("1.0E-16", PrettyPrintDouble(1E-16));
  EXPECT_EQ("1.0E+30", PrettyPrintDouble(1E30));
  EXPECT_EQ("1.0", PrettyPrintDouble(1));
  EXPECT_EQ("1.0", PrettyPrintDouble(1));
  EXPECT_EQ("9.9", PrettyPrintDouble(9.949));
  EXPECT_EQ("10", PrettyPrintDouble(9.951));
  EXPECT_EQ("10", PrettyPrintDouble(10.49));
  EXPECT_EQ("11", PrettyPrintDouble(10.51));
  EXPECT_EQ("99", PrettyPrintDouble(99));
  EXPECT_EQ("100", PrettyPrintDouble(99.5));
  EXPECT_EQ("999", PrettyPrintDouble(999.49));
  EXPECT_EQ("1.0k", PrettyPrintDouble(999.51));
  EXPECT_EQ("1.1k", PrettyPrintDouble(1051));
  EXPECT_EQ("999k", PrettyPrintDouble(999499));
  EXPECT_EQ("1.0M", PrettyPrintDouble(999501));
  EXPECT_EQ("1.0G", PrettyPrintDouble(999500001));
}

TEST(Misc, Indent) {
  EXPECT_EQ(Indent("abc\ndef\n", "  "), "  abc\n  def\n");
}

TEST(Misc, LeftStrip) {
  EXPECT_EQ(LeftStrip("  \n\tabc"), "abc");
  EXPECT_EQ(LeftStrip("  \n\tabc "), "abc ");
  EXPECT_EQ(LeftStrip("  \n\ta b c "), "a b c ");
  EXPECT_EQ(LeftStrip("\n\t  a b c "), "a b c ");
}

TEST(Misc, RightStrip) {
  EXPECT_EQ(RightStrip("abc  \n\t"), "abc");
  EXPECT_EQ(RightStrip(" abc  \n\t"), " abc");
  EXPECT_EQ(RightStrip(" a b c  \n\t"), " a b c");
  EXPECT_EQ(RightStrip(" a b c \n\t "), " a b c");
}

TEST(Misc, ToLower) {
  EXPECT_EQ(ToLower("AB.c D \n\t"), "ab.c d \n\t");
}

TEST(Misc, EndsWith) {
  EXPECT_FALSE(EndsWith("abcd", "zabcd"));
  EXPECT_FALSE(EndsWith("abcd", "ccd"));
  EXPECT_TRUE(EndsWith("abcd", "bcd"));
  EXPECT_TRUE(EndsWith("abcd", "abcd"));
}

TEST(Misc, Split) {
  EXPECT_THAT(Split("abc  , def  ", ',', true), ElementsAre("abc", "def"));
  EXPECT_THAT(Split("abc  , def  ,", ',', true), ElementsAre("abc", "def"));
  EXPECT_THAT(Split(",abc  , def  ,", ',', true), ElementsAre("abc", "def"));
}
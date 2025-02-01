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
#include "parse_flags.h"

TEST(ParseFlags, NoFlag) {
  char const* flags[] = {"run_my_program"};
  ParseFlags parse_flags(0, flags);
  EXPECT_EQ(parse_flags.NumFlags(), 0);
  EXPECT_THROW(parse_flags.GetFlag("no_flag"), ParseFlagsException);
}
TEST(ParseFlags, SingleFlag) {
  char const* flags[] = {"run_my_program", "--approx"};
  ParseFlags parse_flags(2, flags);
  EXPECT_THROW(parse_flags.GetFlag("no_flag"), ParseFlagsException);
  EXPECT_EQ(parse_flags.GetFlagOrDefault("no_flag", "default"), "default");
  EXPECT_EQ(parse_flags.GetFlag("approx"), "");
}
TEST(ParseFlags, ManyFlags) {
  char const* flags[] = {"run_my_program", "--approx", "--n_threads", "2", "--tasks=100", "--dbl=2.0", "--happy", "--glad=True", "--tired=FALSE"};
  ParseFlags parse_flags(8, flags);
  EXPECT_THROW(parse_flags.GetFlag("no_flag"), ParseFlagsException);
  EXPECT_EQ(parse_flags.GetFlag("approx"), "");
  EXPECT_DOUBLE_EQ(parse_flags.GetDoubleFlag("dbl"), 2.0);
  EXPECT_EQ(parse_flags.GetFlag("n_threads"), "2");
  EXPECT_EQ(parse_flags.GetIntFlag("n_threads"), 2);
  EXPECT_EQ(parse_flags.GetIntFlag("tasks"), 100);
  EXPECT_EQ(parse_flags.GetIntFlagOrDefault("no_flag", 100), 100);
  EXPECT_EQ(parse_flags.GetBoolFlag("happy"), true);
  EXPECT_EQ(parse_flags.GetBoolFlag("glad"), true);
  EXPECT_EQ(parse_flags.GetBoolFlag("tired"), false);
  EXPECT_EQ(parse_flags.GetBoolFlagOrDefault("no_flag", true), true);
  EXPECT_THROW(parse_flags.GetBoolFlag("n_threads"), ParseFlagsException);
}
TEST(ParseFlags, WrongFlags) {
  char const* flags[] = {"run_my_program", "--approx=2", "2"};
  EXPECT_THROW(ParseFlags(3, flags), ParseFlagsException);
  char const* flags1[] = {"run_my_program", "2"};
  EXPECT_THROW(ParseFlags(2, flags1), ParseFlagsException);
}
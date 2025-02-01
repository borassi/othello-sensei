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
#ifndef PARSE_FLAGS_H_
#define PARSE_FLAGS_H_

#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>

class ParseFlagsException : public std::exception {
 public:
  ParseFlagsException(const std::string& msg) : msg_(msg) {}
  const char* what() const noexcept override {
    return msg_.c_str();
  }
 private:
  std::string msg_;
};

class ParseFlags {
 public:
  ParseFlags(int argc, const char* const argv[]);

  std::string GetFlag(const std::string& name) {
    auto flag = flags_.find(name);
    if (flag == flags_.end()) {
      throw ParseFlagsException("Missing flag " + name);
    }
    return flag->second;
  }
  std::string GetFlagOrDefault(const std::string& name, const std::string& default_value) {
    if (flags_.count(name)) {
      return GetFlag(name);
    } else {
      return default_value;
    }
  }
  int GetIntFlag(const std::string& name) { return stoi(GetFlag(name)); }
  int GetIntFlagOrDefault(const std::string& name, int default_value) {
    if (flags_.count(name)) {
      return stoi(GetFlag(name));
    } else {
      return default_value;
    }
  }
  long long GetLongLongFlag(const std::string& name) { return stoull(GetFlag(name)); }
  long long GetLongLongFlagOrDefault(const std::string& name, long long default_value) {
    if (flags_.count(name)) {
      return GetLongLongFlag(name);
    } else {
      return default_value;
    }
  }
  bool GetBoolFlag(const std::string& name) {
    std::string flag = GetFlag(name);
    if (flag == "" || flag == "true" || flag == "True" || flag == "TRUE") {
      return true;
    }
    if (flag == "false" || flag == "False" || flag == "FALSE") {
      return false;
    }
    throw ParseFlagsException("Cannot convert flag value '" + flag + "' to boolean.");
  }
  bool GetBoolFlagOrDefault(const std::string& name, int default_value) {
    if (flags_.count(name)) {
      return GetBoolFlag(name);
    } else {
      return default_value;
    }
  }
  double GetDoubleFlag(const std::string& name) { return stof(GetFlag(name)); }
  double GetDoubleFlagOrDefault(const std::string& name, double default_value) {
    if (flags_.count(name)) {
      return GetDoubleFlag(name);
    } else {
      return default_value;
    }
  }
  int NumFlags() { return (int) flags_.size(); }

 private:
  std::unordered_map<std::string, std::string> flags_;
};



#endif //PARSE_FLAGS_H_

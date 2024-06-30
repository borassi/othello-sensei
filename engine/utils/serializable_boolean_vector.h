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

#ifndef OTHELLO_SENSEI_SERIALIZABLE_BOOLEAN_VECTOR_H
#define OTHELLO_SENSEI_SERIALIZABLE_BOOLEAN_VECTOR_H

#include <vector>
#include <cstdint>

class SerializableBooleanVector {
 public:
  SerializableBooleanVector() : memory_(), size_(0) {}
  template<class InputIt>
  SerializableBooleanVector(InputIt begin, InputIt end, size_t size)
      : memory_(begin, end), size_((int) size) {}

  void PushBack(bool b);
  void PushBack(uint32_t value, int num_bits);
  int Size() const { return (int) size_; };
  const std::vector<char>& Serialize() const { return memory_; }
  bool Get(int i) const;
  uint32_t Get(int i, int num_bits) const;

 private:
  std::vector<char> memory_;
  int size_;
};



#endif //OTHELLO_SENSEI_SERIALIZABLE_BOOLEAN_VECTOR_H

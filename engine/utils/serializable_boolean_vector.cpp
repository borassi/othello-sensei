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

#include <cassert>
#include <iostream>
#include "serializable_boolean_vector.h"

constexpr int kBitPerChar = 8;

int PositionInByte(int i) {
  return kBitPerChar - 1 - (i % kBitPerChar);
}

void SerializableBooleanVector::PushBack(bool b) {
  if (size_ % kBitPerChar == 0) {
    memory_.push_back(0);
  }
  if (b) {
    memory_[size_ / kBitPerChar] |= 1 << PositionInByte(size_);
  }
  ++size_;
}

void SerializableBooleanVector::PushBack(uint32_t value, int num_bits) {
  assert(num_bits >= 1 && num_bits <= 32);
  assert(num_bits == 32 || value < (1U << num_bits));
  for (int offset = num_bits - 1; offset >= 0; --offset) {
    PushBack((value >> offset) & 1);
  }
}

bool SerializableBooleanVector::Get(int i) const {
  assert(i < size_);
  int value = memory_[i / kBitPerChar];
  return (value & (1 << PositionInByte(i))) != 0;
}

uint32_t SerializableBooleanVector::Get(int i, int num_bits) const {
  assert(num_bits >= 1 && num_bits <= 32);
  assert(i + num_bits - 1 < size_);
  uint32_t value = 0;
  for (int offset = num_bits - 1; offset >= 0; --offset) {
    if (Get(i + (num_bits - 1 - offset))) {
      value = value | (1U << offset);
    }
  }
  return value;
}
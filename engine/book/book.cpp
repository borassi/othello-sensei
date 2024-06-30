/*
 * Copyright 2022-2024 Michele Borassi
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
#include "book.h"

std::ostream& operator<<(std::ostream& stream, const HashMapNode& n) {
  stream << "[" << (int) n.Size() << " " << n.Offset() << "]";
  return stream;
}

// static
uint8_t HashMapNode::SizeToByte(int size) {
  assert(size >= 0);
  if (size == 0) {
    return 0;
  } else if (size <= kMinFileSize) {
    return 1;
  }
  int size_shifted = size - kMinFileSize + 1;
  if (size_shifted <= 255 - kNumDoubling) {
    return size_shifted;
  } else {
    return 255 - kNumDoubling + (int) (ceil(log(size / (255.0 - kNumDoubling)) / log(2)));
  }
}
// static
int HashMapNode::ByteToSize(uint8_t b) {
  if (b == 0) {
    return 0;
  } else if (b == 1) {
    return kMinFileSize;
  }
  int byte_shifted = b + kMinFileSize - 1;
  if (b <= 255 - kNumDoubling) {
    return byte_shifted;
  } else {
    return (255 - kNumDoubling) * (int) pow(2, b - 255 + kNumDoubling);
  }
}


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

#include "sequence.h"

Sequence::Sequence(const std::string& moves) : Sequence(moves.size() / 2) {
  assert(moves.size() % 2 == 0);
  assert(moves.size() <= 120);
  for (int i = 0; i < moves.length(); i += 2) {
    moves_[i / 2] = MoveToSquare(moves.substr(i, 2));
  }
}

void Sequence::ToCanonicalFirstMoveInplace() {
  if (Size() == 0 || moves_[0] == 19) {
    return;
  } else if (moves_[0] == 26) {
    for (int i = 0; i < Size(); ++i) {
      moves_[i] = Diag9MirrorMove(moves_[i]);
    }
  } else if (moves_[0] == 37) {
    for (int i = 0; i < Size(); ++i) {
      moves_[i] = Diag7MirrorMove(moves_[i]);
    }
  } else if (moves_[0] == 44) {
    for (int i = 0; i < Size(); ++i) {
      moves_[i] = Rotate180Move(moves_[i]);
    }
  } else {
    assert(false);
  }
  assert(moves_[0] == 19);
}

void Sequence::ToCanonicalDiagonalInplace() {
  if (Size() < 4 || moves_[0] != 19 || moves_[1] != 18 || moves_[2] != 26 || moves_[3] != 34) {
    return;
  }
  for (int i = 0; i < Size(); ++i) {
    moves_[i] = Diag9MirrorMove(moves_[i]);
  }
  moves_[0] = 19;
  moves_[1] = 18;
  moves_[2] = 26;
  assert(moves_[3] == 20);
}

std::ostream& operator<<(std::ostream& stream, const Sequence& s) {
  stream << s.ToString();
  return stream;
}
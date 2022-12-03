/*
 * Copyright 2022 Google LLC
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

#include "evaluation.h"

PN ProofNumberToByte(float proof_number) {
  assert(proof_number == 0 || proof_number >= 1);
  if (proof_number <= 1E-8) {
    return 0;
  } else if (proof_number > kMaxProofNumber) {
    return kProofNumberStep;
  }
  float rescaled_proof_number = log(proof_number) / log(kBaseLogProofNumber) + 1;
  assert(rescaled_proof_number >= 0.5 && rescaled_proof_number <= kProofNumberStep - 0.5);
  return (PN) round(rescaled_proof_number);
}

float ByteToProofNumber(PN byte) {
  if (byte == 0) {
    return 0;
  } else if (byte == kProofNumberStep) {
    return INFINITY;
  }
  return pow(kBaseLogProofNumber, byte - 1);
}

std::ostream& operator<<(std::ostream& stream, const Evaluation& e) {
  stream << e.ProbGreaterEqual() << " " << e.MaxLogDerivative() << " ["
         << e.ProofNumber() << " " << e.DisproofNumber() << "]";
  return stream;
}
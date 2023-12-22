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

#include <functional>
#include <memory>

#ifndef EVALUATE_DEPTH_ONE_EVALUATOR_DEPTH_ONE_BASE_H
#define EVALUATE_DEPTH_ONE_EVALUATOR_DEPTH_ONE_BASE_H

class EvaluatorDepthOneBase {

 public:
  virtual void Setup(BitPattern player, BitPattern opponent) = 0;
  virtual void Update(BitPattern square, BitPattern flip) = 0;
  virtual void UndoUpdate(BitPattern square, BitPattern flip) = 0;
  virtual void Invert() = 0;
  virtual EvalLarge Evaluate() const = 0;
  virtual ~EvaluatorDepthOneBase() {}

};

typedef std::function<std::unique_ptr<EvaluatorDepthOneBase>()> EvaluatorFactory;
#endif
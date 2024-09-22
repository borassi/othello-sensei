/*
 * Copyright 2024 Michele Borassi
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

#ifndef OTHELLO_SENSEI_BINDINGS_H
#define OTHELLO_SENSEI_BINDINGS_H

#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default"))) __attribute__((used))
#endif

EXPORT
bool CPUHasBMI2();

EXPORT
bool CPUHasPopcnt();

#ifdef __cplusplus
}
#endif

#endif
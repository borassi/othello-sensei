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

#ifndef GET_FLIP_H
#define GET_FLIP_H

#include <iostream>
#include "bitpattern.h"
#include "../utils/constants.h"
#if __BMI2__
#include <immintrin.h>
#endif

constexpr uint8_t kOutflank[] = {
    0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 16, 0, 0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 32, 0,
    0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 16, 0, 0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 64, 0,
    0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 16, 0, 0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 32, 0,
    0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 16, 0, 0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 128, 0,
    0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 16, 0, 0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 32, 0,
    0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 16, 0, 0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 64, 0,
    0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 16, 0, 0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 32, 0,
    0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 16, 0, 0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 32, 32, 0, 0,
    0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 64, 64, 0, 0,
    0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 32, 32, 0, 0,
    0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 128, 128, 0, 0,
    0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 32, 32, 0, 0,
    0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 64, 64, 0, 0,
    0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 32, 32, 0, 0,
    0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 16, 16, 17, 16, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 32, 32, 33, 32, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 16, 16, 17, 16, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 64, 64, 65, 64, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 16, 16, 17, 16, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 32, 32, 33, 32, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 16, 16, 17, 16, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 128, 128, 129, 128, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 16, 16, 17, 16, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 32, 32, 33, 32, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 16, 16, 17, 16, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 64, 64, 65, 64, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 16, 16, 17, 16, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 32, 32, 33, 32, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 16, 16, 17, 16, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 32, 32, 34, 34, 33, 32, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 64, 64, 64, 66, 66, 65, 64, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 32, 32, 34, 34, 33, 32, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 130, 130, 129, 128, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 32, 32, 34, 34, 33, 32, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 64, 64, 64, 66, 66, 65, 64, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 32, 32, 34, 34, 33, 32, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 64, 64, 64, 64, 64, 64, 64, 68, 68, 68, 68, 66, 66, 65, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    128, 128, 128, 128, 128, 128, 128, 128, 132, 132, 132, 132, 130, 130, 129, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 64, 64, 64, 64, 64, 64, 64, 68, 68, 68, 68, 66, 66, 65, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 2, 2, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 136, 136, 136, 136, 136, 136, 136, 136, 132, 132, 132, 132, 130, 130, 129, 128,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 2, 2, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 2, 2, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 2, 2, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 2, 2, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 2, 2, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 

constexpr uint8_t kFlip[] = {
    // Move 0
    0, 0, 0, 0, 3, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Move 1
    0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Move 2
    0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    28, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    60, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    124, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Move 3
    0, 14, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    24, 30, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    56, 62, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    120, 126, 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Move 4
    0, 30, 28, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    48, 62, 60, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    112, 126, 124, 0, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Move 5
    0, 62, 60, 0, 56, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    96, 126, 124, 0, 120, 0, 0, 0, 112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Move 6
    0, 126, 124, 0, 120, 0, 0, 0, 112, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Move 7
    0, 254, 252, 0, 248, 0, 0, 0, 240, 0, 0, 0, 0, 0, 0, 0, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


constexpr BitPattern PExt(BitPattern x, BitPattern mask) {
  BitPattern res = 0;
  for (BitPattern bb = 1; mask != 0; bb += bb) {
    if (x & mask & (0 - mask)) {
      res |= bb;
    }
    mask &= (mask - 1);
  }
  return res;
}
constexpr MoveShift GetPositionInPattern(Square move, BitPattern pattern) {
  return (MoveShift) CountTrailingZerosConstexpr(PExt(1ULL << move, pattern));
}

struct MoveMetadata {
  BitPattern row;
  BitPattern column;
  BitPattern diag7;
  BitPattern diag9;
  MoveShift position_in_row;
  BitPattern neighbors;
  MoveShift position_in_column;
  MoveShift position_in_diag7;
  MoveShift position_in_diag9;

#if !__BMI2__
  MoveShift row_shift;
  MoveShift column_shift;
#endif

  constexpr MoveMetadata(Square move) :
    row(GetRow(move)),
    column(GetColumn(move)),
    diag7(GetDiag7(move)),
    diag9(GetDiag9(move)),
    position_in_row(GetPositionInPattern(move, GetRow(move)) << 8),
    neighbors(Neighbors(1ULL << move)),
#if __BMI2__
    position_in_column(GetPositionInPattern(move, GetColumn(move)) << 8),
    position_in_diag7(GetPositionInPattern(move, GetDiag7(move)) << 8),
    position_in_diag9(GetPositionInPattern(move, GetDiag9(move)) << 8)
#else
    // Moving the column / diag does not respect the order of bits. For this
    // reason, we have to fix it.
    position_in_column((7 - GetPositionInPattern(move, GetColumn(move))) << 8),
    position_in_diag7((move % 8) << 8),
    position_in_diag9((move % 8) << 8),
    row_shift(GetPositionInPattern(move, GetColumn(move)) * 8),
    column_shift(GetPositionInPattern(move, GetRow(move)))
#endif
  {}
};

constexpr MoveMetadata kMoveMetadata[] = {
    MoveMetadata(0), MoveMetadata(1), MoveMetadata(2), MoveMetadata(3),
    MoveMetadata(4), MoveMetadata(5), MoveMetadata(6), MoveMetadata(7),
    MoveMetadata(8), MoveMetadata(9), MoveMetadata(10), MoveMetadata(11),
    MoveMetadata(12), MoveMetadata(13), MoveMetadata(14), MoveMetadata(15),
    MoveMetadata(16), MoveMetadata(17), MoveMetadata(18), MoveMetadata(19),
    MoveMetadata(20), MoveMetadata(21), MoveMetadata(22), MoveMetadata(23),
    MoveMetadata(24), MoveMetadata(25), MoveMetadata(26), MoveMetadata(27),
    MoveMetadata(28), MoveMetadata(29), MoveMetadata(30), MoveMetadata(31),
    MoveMetadata(32), MoveMetadata(33), MoveMetadata(34), MoveMetadata(35),
    MoveMetadata(36), MoveMetadata(37), MoveMetadata(38), MoveMetadata(39),
    MoveMetadata(40), MoveMetadata(41), MoveMetadata(42), MoveMetadata(43),
    MoveMetadata(44), MoveMetadata(45), MoveMetadata(46), MoveMetadata(47),
    MoveMetadata(48), MoveMetadata(49), MoveMetadata(50), MoveMetadata(51),
    MoveMetadata(52), MoveMetadata(53), MoveMetadata(54), MoveMetadata(55),
    MoveMetadata(56), MoveMetadata(57), MoveMetadata(58), MoveMetadata(59),
    MoveMetadata(60), MoveMetadata(61), MoveMetadata(62), MoveMetadata(63)};

forceinline(BitPattern GetFlip(Square move, BitPattern player, BitPattern opponent) noexcept);
inline BitPattern GetFlip(Square move, BitPattern player, BitPattern opponent) noexcept {
  assert(((1ULL << move) & (player | opponent)) == 0);
  const MoveMetadata* m = kMoveMetadata + move;
#if __BMI2__
  return _pdep_u64(kFlip[m->position_in_row | (kOutflank[m->position_in_row | _pext_u64(opponent, m->row)] & _pext_u64(player, m->row))], m->row)
      | _pdep_u64(kFlip[m->position_in_column | (kOutflank[m->position_in_column | _pext_u64(opponent, m->column)] & _pext_u64(player, m->column))], m->column)
      | _pdep_u64(kFlip[m->position_in_diag7 | (kOutflank[m->position_in_diag7 | _pext_u64(opponent, m->diag7)] & _pext_u64(player, m->diag7))], m->diag7)
      | _pdep_u64(kFlip[m->position_in_diag9 | (kOutflank[m->position_in_diag9 | _pext_u64(opponent, m->diag9)] & _pext_u64(player, m->diag9))], m->diag9);
#else
  return LastRowToRow(
      kFlip[m->position_in_row | (kOutflank[m->position_in_row | RowToLastRow(opponent, m->row, m->row_shift)] & RowToLastRow(player, m->row, m->row_shift))],
          m->row_shift)
      | LastRowToColumn(
      kFlip[m->position_in_column | (
          kOutflank[m->position_in_column | ColumnToLastRow(opponent, m->column, m->column_shift)] & ColumnToLastRow(player, m->column, m->column_shift))],
          m->column_shift)
      | LastRowToDiagonal(
      kFlip[m->position_in_diag7 | (kOutflank[m->position_in_diag7 | DiagonalToLastRow(opponent, m->diag7)] & DiagonalToLastRow(player, m->diag7))],
          m->diag7)
      | LastRowToDiagonal(
      kFlip[m->position_in_diag9 | (kOutflank[m->position_in_diag9 | DiagonalToLastRow(opponent, m->diag9)] & DiagonalToLastRow(player, m->diag9))],
          m->diag9);
#endif
}

forceinline(BitPattern NewPlayer(BitPattern flip, BitPattern opponent) noexcept);
inline BitPattern NewPlayer(BitPattern flip, BitPattern opponent) noexcept {
  return opponent & ~flip;
}
forceinline(BitPattern NewOpponent(BitPattern flip, BitPattern player) noexcept);
inline BitPattern NewOpponent(BitPattern flip, BitPattern player) noexcept {
  return player | flip;
}


constexpr BitPattern GetFlipBasic(Square move, BitPattern player, BitPattern opponent) {
  assert(((1ULL << move) & (player | opponent)) == 0);
  int directions[] = {-9, -8, -7, -1, 1, 7, 8, 9};
  BitPattern flip = 0;
  
  for (int direction : directions) {
    if (move + direction < 0 || (opponent & (1ULL << (move + direction))) == 0) {
      continue;
    }
    BitPattern cur_flip = (1ULL << (move + direction)) | (1ULL << move);
    int max_num_steps = 7;
    if ((direction + 16) % 8 == 7) {
      max_num_steps = std::min(max_num_steps, move % 8);
    } else if ((direction + 16) % 8 == 1) {
      max_num_steps = std::min(max_num_steps, 7 - move % 8);      
    }
    for (int steps = 2; steps <= max_num_steps; steps++) {
      if (move + steps * direction < 0 || move + steps * direction > 63) {
        break;
      }
      BitPattern cur_case = (1ULL << (move + steps * direction));
      if ((player & cur_case) != 0) {
        flip |= cur_flip;
        break;
      } else if ((opponent & cur_case) == 0) {
        // Also if cur_case is 0, because we moved outside of the board.
        break;
      } else {    
        cur_flip = cur_flip | cur_case;
      }              
    }
  }
  return flip;
}

void PrintOutflank();
void PrintFlip();
void PrintMoveMetadata();
#endif /* GET_FLIP_H */


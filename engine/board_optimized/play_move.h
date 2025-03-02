/*
 * Copyright 2025 Michele Borassi
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


#include "compressed_flip.h"
#include "flip_optimized.h"


void (*flip_row_a1_h1[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G1, &Flip_F1G1, &Flip_E1G1, &Flip_D1G1, &Flip_C1G1, &Flip_B1G1,
    &Flip_F1, &Flip_E1F1, &Flip_D1F1, &Flip_C1F1, &Flip_B1F1, &Flip_E1,
    &Flip_D1E1, &Flip_C1E1, &Flip_B1E1, &Flip_D1, &Flip_C1D1, &Flip_B1D1,
    &Flip_C1, &Flip_B1C1, &Flip_B1, &Flip_B1E1_G1, &Flip_B1D1_F1G1, &Flip_B1C1_E1G1,
    &Flip_B1_D1G1, &Flip_C1E1_G1, &Flip_C1D1_F1G1, &Flip_C1_E1G1, &Flip_B1D1_F1, &Flip_B1C1_E1F1,
    &Flip_B1_D1F1, &Flip_D1E1_G1, &Flip_D1_F1G1, &Flip_C1D1_F1, &Flip_C1_E1F1, &Flip_B1C1_E1,
    &Flip_B1_D1E1, &Flip_E1_G1, &Flip_D1_F1, &Flip_C1_E1, &Flip_B1_D1,
};

void (*flip_row_a2_h2[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G2, &Flip_F2G2, &Flip_E2G2, &Flip_D2G2, &Flip_C2G2, &Flip_B2G2,
    &Flip_F2, &Flip_E2F2, &Flip_D2F2, &Flip_C2F2, &Flip_B2F2, &Flip_E2,
    &Flip_D2E2, &Flip_C2E2, &Flip_B2E2, &Flip_D2, &Flip_C2D2, &Flip_B2D2,
    &Flip_C2, &Flip_B2C2, &Flip_B2, &Flip_B2E2_G2, &Flip_B2D2_F2G2, &Flip_B2C2_E2G2,
    &Flip_B2_D2G2, &Flip_C2E2_G2, &Flip_C2D2_F2G2, &Flip_C2_E2G2, &Flip_B2D2_F2, &Flip_B2C2_E2F2,
    &Flip_B2_D2F2, &Flip_D2E2_G2, &Flip_D2_F2G2, &Flip_C2D2_F2, &Flip_C2_E2F2, &Flip_B2C2_E2,
    &Flip_B2_D2E2, &Flip_E2_G2, &Flip_D2_F2, &Flip_C2_E2, &Flip_B2_D2,
};

void (*flip_row_a3_h3[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G3, &Flip_F3G3, &Flip_E3G3, &Flip_D3G3, &Flip_C3G3, &Flip_B3G3,
    &Flip_F3, &Flip_E3F3, &Flip_D3F3, &Flip_C3F3, &Flip_B3F3, &Flip_E3,
    &Flip_D3E3, &Flip_C3E3, &Flip_B3E3, &Flip_D3, &Flip_C3D3, &Flip_B3D3,
    &Flip_C3, &Flip_B3C3, &Flip_B3, &Flip_B3E3_G3, &Flip_B3D3_F3G3, &Flip_B3C3_E3G3,
    &Flip_B3_D3G3, &Flip_C3E3_G3, &Flip_C3D3_F3G3, &Flip_C3_E3G3, &Flip_B3D3_F3, &Flip_B3C3_E3F3,
    &Flip_B3_D3F3, &Flip_D3E3_G3, &Flip_D3_F3G3, &Flip_C3D3_F3, &Flip_C3_E3F3, &Flip_B3C3_E3,
    &Flip_B3_D3E3, &Flip_E3_G3, &Flip_D3_F3, &Flip_C3_E3, &Flip_B3_D3,
};

void (*flip_row_a4_h4[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G4, &Flip_F4G4, &Flip_E4G4, &Flip_D4G4, &Flip_C4G4, &Flip_B4G4,
    &Flip_F4, &Flip_E4F4, &Flip_D4F4, &Flip_C4F4, &Flip_B4F4, &Flip_E4,
    &Flip_D4E4, &Flip_C4E4, &Flip_B4E4, &Flip_D4, &Flip_C4D4, &Flip_B4D4,
    &Flip_C4, &Flip_B4C4, &Flip_B4, &Flip_B4E4_G4, &Flip_B4D4_F4G4, &Flip_B4C4_E4G4,
    &Flip_B4_D4G4, &Flip_C4E4_G4, &Flip_C4D4_F4G4, &Flip_C4_E4G4, &Flip_B4D4_F4, &Flip_B4C4_E4F4,
    &Flip_B4_D4F4, &Flip_D4E4_G4, &Flip_D4_F4G4, &Flip_C4D4_F4, &Flip_C4_E4F4, &Flip_B4C4_E4,
    &Flip_B4_D4E4, &Flip_E4_G4, &Flip_D4_F4, &Flip_C4_E4, &Flip_B4_D4,
};

void (*flip_row_a5_h5[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G5, &Flip_F5G5, &Flip_E5G5, &Flip_D5G5, &Flip_C5G5, &Flip_B5G5,
    &Flip_F5, &Flip_E5F5, &Flip_D5F5, &Flip_C5F5, &Flip_B5F5, &Flip_E5,
    &Flip_D5E5, &Flip_C5E5, &Flip_B5E5, &Flip_D5, &Flip_C5D5, &Flip_B5D5,
    &Flip_C5, &Flip_B5C5, &Flip_B5, &Flip_B5E5_G5, &Flip_B5D5_F5G5, &Flip_B5C5_E5G5,
    &Flip_B5_D5G5, &Flip_C5E5_G5, &Flip_C5D5_F5G5, &Flip_C5_E5G5, &Flip_B5D5_F5, &Flip_B5C5_E5F5,
    &Flip_B5_D5F5, &Flip_D5E5_G5, &Flip_D5_F5G5, &Flip_C5D5_F5, &Flip_C5_E5F5, &Flip_B5C5_E5,
    &Flip_B5_D5E5, &Flip_E5_G5, &Flip_D5_F5, &Flip_C5_E5, &Flip_B5_D5,
};

void (*flip_row_a6_h6[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G6, &Flip_F6G6, &Flip_E6G6, &Flip_D6G6, &Flip_C6G6, &Flip_B6G6,
    &Flip_F6, &Flip_E6F6, &Flip_D6F6, &Flip_C6F6, &Flip_B6F6, &Flip_E6,
    &Flip_D6E6, &Flip_C6E6, &Flip_B6E6, &Flip_D6, &Flip_C6D6, &Flip_B6D6,
    &Flip_C6, &Flip_B6C6, &Flip_B6, &Flip_B6E6_G6, &Flip_B6D6_F6G6, &Flip_B6C6_E6G6,
    &Flip_B6_D6G6, &Flip_C6E6_G6, &Flip_C6D6_F6G6, &Flip_C6_E6G6, &Flip_B6D6_F6, &Flip_B6C6_E6F6,
    &Flip_B6_D6F6, &Flip_D6E6_G6, &Flip_D6_F6G6, &Flip_C6D6_F6, &Flip_C6_E6F6, &Flip_B6C6_E6,
    &Flip_B6_D6E6, &Flip_E6_G6, &Flip_D6_F6, &Flip_C6_E6, &Flip_B6_D6,
};

void (*flip_row_a7_h7[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G7, &Flip_F7G7, &Flip_E7G7, &Flip_D7G7, &Flip_C7G7, &Flip_B7G7,
    &Flip_F7, &Flip_E7F7, &Flip_D7F7, &Flip_C7F7, &Flip_B7F7, &Flip_E7,
    &Flip_D7E7, &Flip_C7E7, &Flip_B7E7, &Flip_D7, &Flip_C7D7, &Flip_B7D7,
    &Flip_C7, &Flip_B7C7, &Flip_B7, &Flip_B7E7_G7, &Flip_B7D7_F7G7, &Flip_B7C7_E7G7,
    &Flip_B7_D7G7, &Flip_C7E7_G7, &Flip_C7D7_F7G7, &Flip_C7_E7G7, &Flip_B7D7_F7, &Flip_B7C7_E7F7,
    &Flip_B7_D7F7, &Flip_D7E7_G7, &Flip_D7_F7G7, &Flip_C7D7_F7, &Flip_C7_E7F7, &Flip_B7C7_E7,
    &Flip_B7_D7E7, &Flip_E7_G7, &Flip_D7_F7, &Flip_C7_E7, &Flip_B7_D7,
};

void (*flip_row_a8_h8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G8, &Flip_F8G8, &Flip_E8G8, &Flip_D8G8, &Flip_C8G8, &Flip_B8G8,
    &Flip_F8, &Flip_E8F8, &Flip_D8F8, &Flip_C8F8, &Flip_B8F8, &Flip_E8,
    &Flip_D8E8, &Flip_C8E8, &Flip_B8E8, &Flip_D8, &Flip_C8D8, &Flip_B8D8,
    &Flip_C8, &Flip_B8C8, &Flip_B8, &Flip_B8E8_G8, &Flip_B8D8_F8G8, &Flip_B8C8_E8G8,
    &Flip_B8_D8G8, &Flip_C8E8_G8, &Flip_C8D8_F8G8, &Flip_C8_E8G8, &Flip_B8D8_F8, &Flip_B8C8_E8F8,
    &Flip_B8_D8F8, &Flip_D8E8_G8, &Flip_D8_F8G8, &Flip_C8D8_F8, &Flip_C8_E8F8, &Flip_B8C8_E8,
    &Flip_B8_D8E8, &Flip_E8_G8, &Flip_D8_F8, &Flip_C8_E8, &Flip_B8_D8,
};

void (*flip_column_a1_a8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_A7, &Flip_A6A7, &Flip_A5A7, &Flip_A4A7, &Flip_A3A7, &Flip_A2A7,
    &Flip_A6, &Flip_A5A6, &Flip_A4A6, &Flip_A3A6, &Flip_A2A6, &Flip_A5,
    &Flip_A4A5, &Flip_A3A5, &Flip_A2A5, &Flip_A4, &Flip_A3A4, &Flip_A2A4,
    &Flip_A3, &Flip_A2A3, &Flip_A2, &Flip_A2A5_A7, &Flip_A2A4_A6A7, &Flip_A2A3_A5A7,
    &Flip_A2_A4A7, &Flip_A3A5_A7, &Flip_A3A4_A6A7, &Flip_A3_A5A7, &Flip_A2A4_A6, &Flip_A2A3_A5A6,
    &Flip_A2_A4A6, &Flip_A4A5_A7, &Flip_A4_A6A7, &Flip_A3A4_A6, &Flip_A3_A5A6, &Flip_A2A3_A5,
    &Flip_A2_A4A5, &Flip_A5_A7, &Flip_A4_A6, &Flip_A3_A5, &Flip_A2_A4,
};

void (*flip_column_b1_b8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_B7, &Flip_B6B7, &Flip_B5B7, &Flip_B4B7, &Flip_B3B7, &Flip_B2B7,
    &Flip_B6, &Flip_B5B6, &Flip_B4B6, &Flip_B3B6, &Flip_B2B6, &Flip_B5,
    &Flip_B4B5, &Flip_B3B5, &Flip_B2B5, &Flip_B4, &Flip_B3B4, &Flip_B2B4,
    &Flip_B3, &Flip_B2B3, &Flip_B2, &Flip_B2B5_B7, &Flip_B2B4_B6B7, &Flip_B2B3_B5B7,
    &Flip_B2_B4B7, &Flip_B3B5_B7, &Flip_B3B4_B6B7, &Flip_B3_B5B7, &Flip_B2B4_B6, &Flip_B2B3_B5B6,
    &Flip_B2_B4B6, &Flip_B4B5_B7, &Flip_B4_B6B7, &Flip_B3B4_B6, &Flip_B3_B5B6, &Flip_B2B3_B5,
    &Flip_B2_B4B5, &Flip_B5_B7, &Flip_B4_B6, &Flip_B3_B5, &Flip_B2_B4,
};

void (*flip_column_c1_c8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_C7, &Flip_C6C7, &Flip_C5C7, &Flip_C4C7, &Flip_C3C7, &Flip_C2C7,
    &Flip_C6, &Flip_C5C6, &Flip_C4C6, &Flip_C3C6, &Flip_C2C6, &Flip_C5,
    &Flip_C4C5, &Flip_C3C5, &Flip_C2C5, &Flip_C4, &Flip_C3C4, &Flip_C2C4,
    &Flip_C3, &Flip_C2C3, &Flip_C2, &Flip_C2C5_C7, &Flip_C2C4_C6C7, &Flip_C2C3_C5C7,
    &Flip_C2_C4C7, &Flip_C3C5_C7, &Flip_C3C4_C6C7, &Flip_C3_C5C7, &Flip_C2C4_C6, &Flip_C2C3_C5C6,
    &Flip_C2_C4C6, &Flip_C4C5_C7, &Flip_C4_C6C7, &Flip_C3C4_C6, &Flip_C3_C5C6, &Flip_C2C3_C5,
    &Flip_C2_C4C5, &Flip_C5_C7, &Flip_C4_C6, &Flip_C3_C5, &Flip_C2_C4,
};

void (*flip_column_d1_d8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_D7, &Flip_D6D7, &Flip_D5D7, &Flip_D4D7, &Flip_D3D7, &Flip_D2D7,
    &Flip_D6, &Flip_D5D6, &Flip_D4D6, &Flip_D3D6, &Flip_D2D6, &Flip_D5,
    &Flip_D4D5, &Flip_D3D5, &Flip_D2D5, &Flip_D4, &Flip_D3D4, &Flip_D2D4,
    &Flip_D3, &Flip_D2D3, &Flip_D2, &Flip_D2D5_D7, &Flip_D2D4_D6D7, &Flip_D2D3_D5D7,
    &Flip_D2_D4D7, &Flip_D3D5_D7, &Flip_D3D4_D6D7, &Flip_D3_D5D7, &Flip_D2D4_D6, &Flip_D2D3_D5D6,
    &Flip_D2_D4D6, &Flip_D4D5_D7, &Flip_D4_D6D7, &Flip_D3D4_D6, &Flip_D3_D5D6, &Flip_D2D3_D5,
    &Flip_D2_D4D5, &Flip_D5_D7, &Flip_D4_D6, &Flip_D3_D5, &Flip_D2_D4,
};

void (*flip_column_e1_e8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_E7, &Flip_E6E7, &Flip_E5E7, &Flip_E4E7, &Flip_E3E7, &Flip_E2E7,
    &Flip_E6, &Flip_E5E6, &Flip_E4E6, &Flip_E3E6, &Flip_E2E6, &Flip_E5,
    &Flip_E4E5, &Flip_E3E5, &Flip_E2E5, &Flip_E4, &Flip_E3E4, &Flip_E2E4,
    &Flip_E3, &Flip_E2E3, &Flip_E2, &Flip_E2E5_E7, &Flip_E2E4_E6E7, &Flip_E2E3_E5E7,
    &Flip_E2_E4E7, &Flip_E3E5_E7, &Flip_E3E4_E6E7, &Flip_E3_E5E7, &Flip_E2E4_E6, &Flip_E2E3_E5E6,
    &Flip_E2_E4E6, &Flip_E4E5_E7, &Flip_E4_E6E7, &Flip_E3E4_E6, &Flip_E3_E5E6, &Flip_E2E3_E5,
    &Flip_E2_E4E5, &Flip_E5_E7, &Flip_E4_E6, &Flip_E3_E5, &Flip_E2_E4,
};

void (*flip_column_f1_f8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_F7, &Flip_F6F7, &Flip_F5F7, &Flip_F4F7, &Flip_F3F7, &Flip_F2F7,
    &Flip_F6, &Flip_F5F6, &Flip_F4F6, &Flip_F3F6, &Flip_F2F6, &Flip_F5,
    &Flip_F4F5, &Flip_F3F5, &Flip_F2F5, &Flip_F4, &Flip_F3F4, &Flip_F2F4,
    &Flip_F3, &Flip_F2F3, &Flip_F2, &Flip_F2F5_F7, &Flip_F2F4_F6F7, &Flip_F2F3_F5F7,
    &Flip_F2_F4F7, &Flip_F3F5_F7, &Flip_F3F4_F6F7, &Flip_F3_F5F7, &Flip_F2F4_F6, &Flip_F2F3_F5F6,
    &Flip_F2_F4F6, &Flip_F4F5_F7, &Flip_F4_F6F7, &Flip_F3F4_F6, &Flip_F3_F5F6, &Flip_F2F3_F5,
    &Flip_F2_F4F5, &Flip_F5_F7, &Flip_F4_F6, &Flip_F3_F5, &Flip_F2_F4,
};

void (*flip_column_g1_g8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G7, &Flip_G6G7, &Flip_G5G7, &Flip_G4G7, &Flip_G3G7, &Flip_G2G7,
    &Flip_G6, &Flip_G5G6, &Flip_G4G6, &Flip_G3G6, &Flip_G2G6, &Flip_G5,
    &Flip_G4G5, &Flip_G3G5, &Flip_G2G5, &Flip_G4, &Flip_G3G4, &Flip_G2G4,
    &Flip_G3, &Flip_G2G3, &Flip_G2, &Flip_G2G5_G7, &Flip_G2G4_G6G7, &Flip_G2G3_G5G7,
    &Flip_G2_G4G7, &Flip_G3G5_G7, &Flip_G3G4_G6G7, &Flip_G3_G5G7, &Flip_G2G4_G6, &Flip_G2G3_G5G6,
    &Flip_G2_G4G6, &Flip_G4G5_G7, &Flip_G4_G6G7, &Flip_G3G4_G6, &Flip_G3_G5G6, &Flip_G2G3_G5,
    &Flip_G2_G4G5, &Flip_G5_G7, &Flip_G4_G6, &Flip_G3_G5, &Flip_G2_G4,
};

void (*flip_column_h1_h8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_H7, &Flip_H6H7, &Flip_H5H7, &Flip_H4H7, &Flip_H3H7, &Flip_H2H7,
    &Flip_H6, &Flip_H5H6, &Flip_H4H6, &Flip_H3H6, &Flip_H2H6, &Flip_H5,
    &Flip_H4H5, &Flip_H3H5, &Flip_H2H5, &Flip_H4, &Flip_H3H4, &Flip_H2H4,
    &Flip_H3, &Flip_H2H3, &Flip_H2, &Flip_H2H5_H7, &Flip_H2H4_H6H7, &Flip_H2H3_H5H7,
    &Flip_H2_H4H7, &Flip_H3H5_H7, &Flip_H3H4_H6H7, &Flip_H3_H5H7, &Flip_H2H4_H6, &Flip_H2H3_H5H6,
    &Flip_H2_H4H6, &Flip_H4H5_H7, &Flip_H4_H6H7, &Flip_H3H4_H6, &Flip_H3_H5H6, &Flip_H2H3_H5,
    &Flip_H2_H4H5, &Flip_H5_H7, &Flip_H4_H6, &Flip_H3_H5, &Flip_H2_H4,
};

void (*flip_diag7_c1_a3[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_B2, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag7_d1_a4[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_B3, &Flip_C2B3, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &Flip_C2, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag7_e1_a5[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_B4, &Flip_C3B4, &Flip_D2B4, &NoFlip, &NoFlip, &NoFlip,
    &Flip_C3, &Flip_D2C3, &NoFlip, &NoFlip, &NoFlip, &Flip_D2,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_D2_B4, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag7_h6_f8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G7, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag7_f1_a6[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_B5, &Flip_C4B5, &Flip_D3B5, &Flip_E2B5, &NoFlip, &NoFlip,
    &Flip_C4, &Flip_D3C4, &Flip_E2C4, &NoFlip, &NoFlip, &Flip_D3,
    &Flip_E2D3, &NoFlip, &NoFlip, &Flip_E2, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_E2D3_B5, &Flip_E2_C4B5, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_D3_B5, &Flip_E2_C4, &NoFlip, &NoFlip,
};

void (*flip_diag7_g1_a7[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_B6, &Flip_C5B6, &Flip_D4B6, &Flip_E3B6, &Flip_F2B6, &NoFlip,
    &Flip_C5, &Flip_D4C5, &Flip_E3C5, &Flip_F2C5, &NoFlip, &Flip_D4,
    &Flip_E3D4, &Flip_F2D4, &NoFlip, &Flip_E3, &Flip_F2E3, &NoFlip,
    &Flip_F2, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_F2D4_B6, &Flip_F2E3_C5B6, &Flip_F2_D4B6, &NoFlip, &NoFlip,
    &NoFlip, &Flip_E3D4_B6, &Flip_E3_C5B6, &Flip_F2E3_C5, &Flip_F2_D4C5, &NoFlip,
    &NoFlip, &Flip_D4_B6, &Flip_E3_C5, &Flip_F2_D4, &NoFlip,
};

void (*flip_diag7_h1_a8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_B7, &Flip_C6B7, &Flip_D5B7, &Flip_E4B7, &Flip_F3B7, &Flip_G2B7,
    &Flip_C6, &Flip_D5C6, &Flip_E4C6, &Flip_F3C6, &Flip_G2C6, &Flip_D5,
    &Flip_E4D5, &Flip_F3D5, &Flip_G2D5, &Flip_E4, &Flip_F3E4, &Flip_G2E4,
    &Flip_F3, &Flip_G2F3, &Flip_G2, &Flip_G2D5_B7, &Flip_G2E4_C6B7, &Flip_G2F3_D5B7,
    &Flip_G2_E4B7, &Flip_F3D5_B7, &Flip_F3E4_C6B7, &Flip_F3_D5B7, &Flip_G2E4_C6, &Flip_G2F3_D5C6,
    &Flip_G2_E4C6, &Flip_E4D5_B7, &Flip_E4_C6B7, &Flip_F3E4_C6, &Flip_F3_D5C6, &Flip_G2F3_D5,
    &Flip_G2_E4D5, &Flip_D5_B7, &Flip_E4_C6, &Flip_F3_D5, &Flip_G2_E4,
};

void (*flip_diag7_h2_b8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_C7, &Flip_D6C7, &Flip_E5C7, &Flip_F4C7, &Flip_G3C7, &NoFlip,
    &Flip_D6, &Flip_E5D6, &Flip_F4D6, &Flip_G3D6, &NoFlip, &Flip_E5,
    &Flip_F4E5, &Flip_G3E5, &NoFlip, &Flip_F4, &Flip_G3F4, &NoFlip,
    &Flip_G3, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_G3E5_C7, &Flip_G3F4_D6C7, &Flip_G3_E5C7, &NoFlip, &NoFlip,
    &NoFlip, &Flip_F4E5_C7, &Flip_F4_D6C7, &Flip_G3F4_D6, &Flip_G3_E5D6, &NoFlip,
    &NoFlip, &Flip_E5_C7, &Flip_F4_D6, &Flip_G3_E5, &NoFlip,
};

void (*flip_diag7_h3_c8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_D7, &Flip_E6D7, &Flip_F5D7, &Flip_G4D7, &NoFlip, &NoFlip,
    &Flip_E6, &Flip_F5E6, &Flip_G4E6, &NoFlip, &NoFlip, &Flip_F5,
    &Flip_G4F5, &NoFlip, &NoFlip, &Flip_G4, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_G4F5_D7, &Flip_G4_E6D7, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_F5_D7, &Flip_G4_E6, &NoFlip, &NoFlip,
};

void (*flip_diag7_h4_d8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_E7, &Flip_F6E7, &Flip_G5E7, &NoFlip, &NoFlip, &NoFlip,
    &Flip_F6, &Flip_G5F6, &NoFlip, &NoFlip, &NoFlip, &Flip_G5,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_G5_E7, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag7_h5_e8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_F7, &Flip_G6F7, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &Flip_G6, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag9_f1_h3[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G2, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag9_e1_h4[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G3, &Flip_F2G3, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &Flip_F2, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag9_d1_h5[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G4, &Flip_F3G4, &Flip_E2G4, &NoFlip, &NoFlip, &NoFlip,
    &Flip_F3, &Flip_E2F3, &NoFlip, &NoFlip, &NoFlip, &Flip_E2,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_E2_G4, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag9_a6_c8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_B7, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag9_c1_h6[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G5, &Flip_F4G5, &Flip_E3G5, &Flip_D2G5, &NoFlip, &NoFlip,
    &Flip_F4, &Flip_E3F4, &Flip_D2F4, &NoFlip, &NoFlip, &Flip_E3,
    &Flip_D2E3, &NoFlip, &NoFlip, &Flip_D2, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_D2E3_G5, &Flip_D2_F4G5, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_E3_G5, &Flip_D2_F4, &NoFlip, &NoFlip,
};

void (*flip_diag9_b1_h7[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G6, &Flip_F5G6, &Flip_E4G6, &Flip_D3G6, &Flip_C2G6, &NoFlip,
    &Flip_F5, &Flip_E4F5, &Flip_D3F5, &Flip_C2F5, &NoFlip, &Flip_E4,
    &Flip_D3E4, &Flip_C2E4, &NoFlip, &Flip_D3, &Flip_C2D3, &NoFlip,
    &Flip_C2, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_C2E4_G6, &Flip_C2D3_F5G6, &Flip_C2_E4G6, &NoFlip, &NoFlip,
    &NoFlip, &Flip_D3E4_G6, &Flip_D3_F5G6, &Flip_C2D3_F5, &Flip_C2_E4F5, &NoFlip,
    &NoFlip, &Flip_E4_G6, &Flip_D3_F5, &Flip_C2_E4, &NoFlip,
};

void (*flip_diag9_a1_h8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_G7, &Flip_F6G7, &Flip_E5G7, &Flip_D4G7, &Flip_C3G7, &Flip_B2G7,
    &Flip_F6, &Flip_E5F6, &Flip_D4F6, &Flip_C3F6, &Flip_B2F6, &Flip_E5,
    &Flip_D4E5, &Flip_C3E5, &Flip_B2E5, &Flip_D4, &Flip_C3D4, &Flip_B2D4,
    &Flip_C3, &Flip_B2C3, &Flip_B2, &Flip_B2E5_G7, &Flip_B2D4_F6G7, &Flip_B2C3_E5G7,
    &Flip_B2_D4G7, &Flip_C3E5_G7, &Flip_C3D4_F6G7, &Flip_C3_E5G7, &Flip_B2D4_F6, &Flip_B2C3_E5F6,
    &Flip_B2_D4F6, &Flip_D4E5_G7, &Flip_D4_F6G7, &Flip_C3D4_F6, &Flip_C3_E5F6, &Flip_B2C3_E5,
    &Flip_B2_D4E5, &Flip_E5_G7, &Flip_D4_F6, &Flip_C3_E5, &Flip_B2_D4,
};

void (*flip_diag9_a2_g8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_F7, &Flip_E6F7, &Flip_D5F7, &Flip_C4F7, &Flip_B3F7, &NoFlip,
    &Flip_E6, &Flip_D5E6, &Flip_C4E6, &Flip_B3E6, &NoFlip, &Flip_D5,
    &Flip_C4D5, &Flip_B3D5, &NoFlip, &Flip_C4, &Flip_B3C4, &NoFlip,
    &Flip_B3, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_B3D5_F7, &Flip_B3C4_E6F7, &Flip_B3_D5F7, &NoFlip, &NoFlip,
    &NoFlip, &Flip_C4D5_F7, &Flip_C4_E6F7, &Flip_B3C4_E6, &Flip_B3_D5E6, &NoFlip,
    &NoFlip, &Flip_D5_F7, &Flip_C4_E6, &Flip_B3_D5, &NoFlip,
};

void (*flip_diag9_a3_f8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_E7, &Flip_D6E7, &Flip_C5E7, &Flip_B4E7, &NoFlip, &NoFlip,
    &Flip_D6, &Flip_C5D6, &Flip_B4D6, &NoFlip, &NoFlip, &Flip_C5,
    &Flip_B4C5, &NoFlip, &NoFlip, &Flip_B4, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_B4C5_E7, &Flip_B4_D6E7, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_C5_E7, &Flip_B4_D6, &NoFlip, &NoFlip,
};

void (*flip_diag9_a4_e8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_D7, &Flip_C6D7, &Flip_B5D7, &NoFlip, &NoFlip, &NoFlip,
    &Flip_C6, &Flip_B5C6, &NoFlip, &NoFlip, &NoFlip, &Flip_B5,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &Flip_B5_D7, &NoFlip, &NoFlip, &NoFlip,
};

void (*flip_diag9_a5_d8[kNumCompressedFlipValues])() = {
    &NoFlip,
    &Flip_C7, &Flip_B6C7, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &Flip_B6, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
    &NoFlip, &NoFlip, &NoFlip, &NoFlip, &NoFlip,
};

void PlayA1() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a1_h1[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*row_a1_h1]]();
  flip_column_a1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*column_a1_a8]]();
  flip_diag9_a1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*diag9_a1_h8]]();

  if (flipped) {
    Set_A1();
    action_stack.value[depth][action_stack_count++] = &Set_A1;
    turn = -turn;
    ++depth;
  }
}

void PlayB1() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a1_h1[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*row_a1_h1]]();
  flip_column_b1_b8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*column_b1_b8]]();
  flip_diag9_b1_h7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][6][*diag9_b1_h7]]();

  if (flipped) {
    Set_B1();
    action_stack.value[depth][action_stack_count++] = &Set_B1;
    turn = -turn;
    ++depth;
  }
}

void PlayC1() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a1_h1[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*row_a1_h1]]();
  flip_column_c1_c8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*column_c1_c8]]();
  flip_diag7_c1_a3[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][2][*diag7_c1_a3]]();
  flip_diag9_c1_h6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][5][*diag9_c1_h6]]();

  if (flipped) {
    Set_C1();
    action_stack.value[depth][action_stack_count++] = &Set_C1;
    turn = -turn;
    ++depth;
  }
}

void PlayD1() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a1_h1[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*row_a1_h1]]();
  flip_column_d1_d8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*column_d1_d8]]();
  flip_diag7_d1_a4[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][3][*diag7_d1_a4]]();
  flip_diag9_d1_h5[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][4][*diag9_d1_h5]]();

  if (flipped) {
    Set_D1();
    action_stack.value[depth][action_stack_count++] = &Set_D1;
    turn = -turn;
    ++depth;
  }
}

void PlayE1() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a1_h1[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*row_a1_h1]]();
  flip_column_e1_e8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*column_e1_e8]]();
  flip_diag7_e1_a5[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][4][*diag7_e1_a5]]();
  flip_diag9_e1_h4[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][3][*diag9_e1_h4]]();

  if (flipped) {
    Set_E1();
    action_stack.value[depth][action_stack_count++] = &Set_E1;
    turn = -turn;
    ++depth;
  }
}

void PlayF1() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a1_h1[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*row_a1_h1]]();
  flip_column_f1_f8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*column_f1_f8]]();
  flip_diag7_f1_a6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][5][*diag7_f1_a6]]();
  flip_diag9_f1_h3[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][2][*diag9_f1_h3]]();

  if (flipped) {
    Set_F1();
    action_stack.value[depth][action_stack_count++] = &Set_F1;
    turn = -turn;
    ++depth;
  }
}

void PlayG1() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a1_h1[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*row_a1_h1]]();
  flip_column_g1_g8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*column_g1_g8]]();
  flip_diag7_g1_a7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][6][*diag7_g1_a7]]();

  if (flipped) {
    Set_G1();
    action_stack.value[depth][action_stack_count++] = &Set_G1;
    turn = -turn;
    ++depth;
  }
}

void PlayH1() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a1_h1[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*row_a1_h1]]();
  flip_column_h1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*column_h1_h8]]();
  flip_diag7_h1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*diag7_h1_a8]]();

  if (flipped) {
    Set_H1();
    action_stack.value[depth][action_stack_count++] = &Set_H1;
    turn = -turn;
    ++depth;
  }
}

void PlayA2() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a2_h2[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*row_a2_h2]]();
  flip_column_a1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*column_a1_a8]]();
  flip_diag9_a2_g8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][6][*diag9_a2_g8]]();

  if (flipped) {
    Set_A2();
    action_stack.value[depth][action_stack_count++] = &Set_A2;
    turn = -turn;
    ++depth;
  }
}

void PlayB2() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a2_h2[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*row_a2_h2]]();
  flip_column_b1_b8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*column_b1_b8]]();
  flip_diag7_c1_a3[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][1][*diag7_c1_a3]]();
  flip_diag9_a1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*diag9_a1_h8]]();

  if (flipped) {
    Set_B2();
    action_stack.value[depth][action_stack_count++] = &Set_B2;
    turn = -turn;
    ++depth;
  }
}

void PlayC2() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a2_h2[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*row_a2_h2]]();
  flip_column_c1_c8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*column_c1_c8]]();
  flip_diag7_d1_a4[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][2][*diag7_d1_a4]]();
  flip_diag9_b1_h7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][5][*diag9_b1_h7]]();

  if (flipped) {
    Set_C2();
    action_stack.value[depth][action_stack_count++] = &Set_C2;
    turn = -turn;
    ++depth;
  }
}

void PlayD2() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a2_h2[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*row_a2_h2]]();
  flip_column_d1_d8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*column_d1_d8]]();
  flip_diag7_e1_a5[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][3][*diag7_e1_a5]]();
  flip_diag9_c1_h6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][4][*diag9_c1_h6]]();

  if (flipped) {
    Set_D2();
    action_stack.value[depth][action_stack_count++] = &Set_D2;
    turn = -turn;
    ++depth;
  }
}

void PlayE2() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a2_h2[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*row_a2_h2]]();
  flip_column_e1_e8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*column_e1_e8]]();
  flip_diag7_f1_a6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][4][*diag7_f1_a6]]();
  flip_diag9_d1_h5[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][3][*diag9_d1_h5]]();

  if (flipped) {
    Set_E2();
    action_stack.value[depth][action_stack_count++] = &Set_E2;
    turn = -turn;
    ++depth;
  }
}

void PlayF2() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a2_h2[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*row_a2_h2]]();
  flip_column_f1_f8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*column_f1_f8]]();
  flip_diag7_g1_a7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][5][*diag7_g1_a7]]();
  flip_diag9_e1_h4[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][2][*diag9_e1_h4]]();

  if (flipped) {
    Set_F2();
    action_stack.value[depth][action_stack_count++] = &Set_F2;
    turn = -turn;
    ++depth;
  }
}

void PlayG2() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a2_h2[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*row_a2_h2]]();
  flip_column_g1_g8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*column_g1_g8]]();
  flip_diag7_h1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*diag7_h1_a8]]();
  flip_diag9_f1_h3[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][1][*diag9_f1_h3]]();

  if (flipped) {
    Set_G2();
    action_stack.value[depth][action_stack_count++] = &Set_G2;
    turn = -turn;
    ++depth;
  }
}

void PlayH2() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a2_h2[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*row_a2_h2]]();
  flip_column_h1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*column_h1_h8]]();
  flip_diag7_h2_b8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][6][*diag7_h2_b8]]();

  if (flipped) {
    Set_H2();
    action_stack.value[depth][action_stack_count++] = &Set_H2;
    turn = -turn;
    ++depth;
  }
}

void PlayA3() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a3_h3[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*row_a3_h3]]();
  flip_column_a1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*column_a1_a8]]();
  flip_diag7_c1_a3[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][0][*diag7_c1_a3]]();
  flip_diag9_a3_f8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][5][*diag9_a3_f8]]();

  if (flipped) {
    Set_A3();
    action_stack.value[depth][action_stack_count++] = &Set_A3;
    turn = -turn;
    ++depth;
  }
}

void PlayB3() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a3_h3[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*row_a3_h3]]();
  flip_column_b1_b8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*column_b1_b8]]();
  flip_diag7_d1_a4[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][1][*diag7_d1_a4]]();
  flip_diag9_a2_g8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][5][*diag9_a2_g8]]();

  if (flipped) {
    Set_B3();
    action_stack.value[depth][action_stack_count++] = &Set_B3;
    turn = -turn;
    ++depth;
  }
}

void PlayC3() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a3_h3[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*row_a3_h3]]();
  flip_column_c1_c8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*column_c1_c8]]();
  flip_diag7_e1_a5[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][2][*diag7_e1_a5]]();
  flip_diag9_a1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*diag9_a1_h8]]();

  if (flipped) {
    Set_C3();
    action_stack.value[depth][action_stack_count++] = &Set_C3;
    turn = -turn;
    ++depth;
  }
}

void PlayD3() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a3_h3[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*row_a3_h3]]();
  flip_column_d1_d8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*column_d1_d8]]();
  flip_diag7_f1_a6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][3][*diag7_f1_a6]]();
  flip_diag9_b1_h7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][4][*diag9_b1_h7]]();

  if (flipped) {
    Set_D3();
    action_stack.value[depth][action_stack_count++] = &Set_D3;
    turn = -turn;
    ++depth;
  }
}

void PlayE3() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a3_h3[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*row_a3_h3]]();
  flip_column_e1_e8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*column_e1_e8]]();
  flip_diag7_g1_a7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][4][*diag7_g1_a7]]();
  flip_diag9_c1_h6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][3][*diag9_c1_h6]]();

  if (flipped) {
    Set_E3();
    action_stack.value[depth][action_stack_count++] = &Set_E3;
    turn = -turn;
    ++depth;
  }
}

void PlayF3() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a3_h3[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*row_a3_h3]]();
  flip_column_f1_f8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*column_f1_f8]]();
  flip_diag7_h1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*diag7_h1_a8]]();
  flip_diag9_d1_h5[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][2][*diag9_d1_h5]]();

  if (flipped) {
    Set_F3();
    action_stack.value[depth][action_stack_count++] = &Set_F3;
    turn = -turn;
    ++depth;
  }
}

void PlayG3() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a3_h3[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*row_a3_h3]]();
  flip_column_g1_g8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*column_g1_g8]]();
  flip_diag7_h2_b8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][5][*diag7_h2_b8]]();
  flip_diag9_e1_h4[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][1][*diag9_e1_h4]]();

  if (flipped) {
    Set_G3();
    action_stack.value[depth][action_stack_count++] = &Set_G3;
    turn = -turn;
    ++depth;
  }
}

void PlayH3() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a3_h3[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*row_a3_h3]]();
  flip_column_h1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*column_h1_h8]]();
  flip_diag7_h3_c8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][5][*diag7_h3_c8]]();
  flip_diag9_f1_h3[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][0][*diag9_f1_h3]]();

  if (flipped) {
    Set_H3();
    action_stack.value[depth][action_stack_count++] = &Set_H3;
    turn = -turn;
    ++depth;
  }
}

void PlayA4() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a4_h4[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*row_a4_h4]]();
  flip_column_a1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*column_a1_a8]]();
  flip_diag7_d1_a4[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][0][*diag7_d1_a4]]();
  flip_diag9_a4_e8[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][4][*diag9_a4_e8]]();

  if (flipped) {
    Set_A4();
    action_stack.value[depth][action_stack_count++] = &Set_A4;
    turn = -turn;
    ++depth;
  }
}

void PlayB4() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a4_h4[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*row_a4_h4]]();
  flip_column_b1_b8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*column_b1_b8]]();
  flip_diag7_e1_a5[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][1][*diag7_e1_a5]]();
  flip_diag9_a3_f8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][4][*diag9_a3_f8]]();

  if (flipped) {
    Set_B4();
    action_stack.value[depth][action_stack_count++] = &Set_B4;
    turn = -turn;
    ++depth;
  }
}

void PlayC4() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a4_h4[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*row_a4_h4]]();
  flip_column_c1_c8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*column_c1_c8]]();
  flip_diag7_f1_a6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][2][*diag7_f1_a6]]();
  flip_diag9_a2_g8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][4][*diag9_a2_g8]]();

  if (flipped) {
    Set_C4();
    action_stack.value[depth][action_stack_count++] = &Set_C4;
    turn = -turn;
    ++depth;
  }
}

void PlayD4() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a4_h4[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*row_a4_h4]]();
  flip_column_d1_d8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*column_d1_d8]]();
  flip_diag7_g1_a7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][3][*diag7_g1_a7]]();
  flip_diag9_a1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*diag9_a1_h8]]();

  if (flipped) {
    Set_D4();
    action_stack.value[depth][action_stack_count++] = &Set_D4;
    turn = -turn;
    ++depth;
  }
}

void PlayE4() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a4_h4[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*row_a4_h4]]();
  flip_column_e1_e8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*column_e1_e8]]();
  flip_diag7_h1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*diag7_h1_a8]]();
  flip_diag9_b1_h7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][3][*diag9_b1_h7]]();

  if (flipped) {
    Set_E4();
    action_stack.value[depth][action_stack_count++] = &Set_E4;
    turn = -turn;
    ++depth;
  }
}

void PlayF4() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a4_h4[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*row_a4_h4]]();
  flip_column_f1_f8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*column_f1_f8]]();
  flip_diag7_h2_b8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][4][*diag7_h2_b8]]();
  flip_diag9_c1_h6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][2][*diag9_c1_h6]]();

  if (flipped) {
    Set_F4();
    action_stack.value[depth][action_stack_count++] = &Set_F4;
    turn = -turn;
    ++depth;
  }
}

void PlayG4() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a4_h4[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*row_a4_h4]]();
  flip_column_g1_g8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*column_g1_g8]]();
  flip_diag7_h3_c8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][4][*diag7_h3_c8]]();
  flip_diag9_d1_h5[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][1][*diag9_d1_h5]]();

  if (flipped) {
    Set_G4();
    action_stack.value[depth][action_stack_count++] = &Set_G4;
    turn = -turn;
    ++depth;
  }
}

void PlayH4() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a4_h4[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*row_a4_h4]]();
  flip_column_h1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*column_h1_h8]]();
  flip_diag7_h4_d8[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][4][*diag7_h4_d8]]();
  flip_diag9_e1_h4[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][0][*diag9_e1_h4]]();

  if (flipped) {
    Set_H4();
    action_stack.value[depth][action_stack_count++] = &Set_H4;
    turn = -turn;
    ++depth;
  }
}

void PlayA5() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a5_h5[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*row_a5_h5]]();
  flip_column_a1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*column_a1_a8]]();
  flip_diag7_e1_a5[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][0][*diag7_e1_a5]]();
  flip_diag9_a5_d8[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][3][*diag9_a5_d8]]();

  if (flipped) {
    Set_A5();
    action_stack.value[depth][action_stack_count++] = &Set_A5;
    turn = -turn;
    ++depth;
  }
}

void PlayB5() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a5_h5[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*row_a5_h5]]();
  flip_column_b1_b8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*column_b1_b8]]();
  flip_diag7_f1_a6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][1][*diag7_f1_a6]]();
  flip_diag9_a4_e8[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][3][*diag9_a4_e8]]();

  if (flipped) {
    Set_B5();
    action_stack.value[depth][action_stack_count++] = &Set_B5;
    turn = -turn;
    ++depth;
  }
}

void PlayC5() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a5_h5[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*row_a5_h5]]();
  flip_column_c1_c8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*column_c1_c8]]();
  flip_diag7_g1_a7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][2][*diag7_g1_a7]]();
  flip_diag9_a3_f8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][3][*diag9_a3_f8]]();

  if (flipped) {
    Set_C5();
    action_stack.value[depth][action_stack_count++] = &Set_C5;
    turn = -turn;
    ++depth;
  }
}

void PlayD5() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a5_h5[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*row_a5_h5]]();
  flip_column_d1_d8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*column_d1_d8]]();
  flip_diag7_h1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*diag7_h1_a8]]();
  flip_diag9_a2_g8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][3][*diag9_a2_g8]]();

  if (flipped) {
    Set_D5();
    action_stack.value[depth][action_stack_count++] = &Set_D5;
    turn = -turn;
    ++depth;
  }
}

void PlayE5() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a5_h5[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*row_a5_h5]]();
  flip_column_e1_e8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*column_e1_e8]]();
  flip_diag7_h2_b8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][3][*diag7_h2_b8]]();
  flip_diag9_a1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*diag9_a1_h8]]();

  if (flipped) {
    Set_E5();
    action_stack.value[depth][action_stack_count++] = &Set_E5;
    turn = -turn;
    ++depth;
  }
}

void PlayF5() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a5_h5[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*row_a5_h5]]();
  flip_column_f1_f8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*column_f1_f8]]();
  flip_diag7_h3_c8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][3][*diag7_h3_c8]]();
  flip_diag9_b1_h7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][2][*diag9_b1_h7]]();

  if (flipped) {
    Set_F5();
    action_stack.value[depth][action_stack_count++] = &Set_F5;
    turn = -turn;
    ++depth;
  }
}

void PlayG5() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a5_h5[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*row_a5_h5]]();
  flip_column_g1_g8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*column_g1_g8]]();
  flip_diag7_h4_d8[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][3][*diag7_h4_d8]]();
  flip_diag9_c1_h6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][1][*diag9_c1_h6]]();

  if (flipped) {
    Set_G5();
    action_stack.value[depth][action_stack_count++] = &Set_G5;
    turn = -turn;
    ++depth;
  }
}

void PlayH5() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a5_h5[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*row_a5_h5]]();
  flip_column_h1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*column_h1_h8]]();
  flip_diag7_h5_e8[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][3][*diag7_h5_e8]]();
  flip_diag9_d1_h5[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][0][*diag9_d1_h5]]();

  if (flipped) {
    Set_H5();
    action_stack.value[depth][action_stack_count++] = &Set_H5;
    turn = -turn;
    ++depth;
  }
}

void PlayA6() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a6_h6[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*row_a6_h6]]();
  flip_column_a1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*column_a1_a8]]();
  flip_diag7_f1_a6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][0][*diag7_f1_a6]]();
  flip_diag9_a6_c8[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][2][*diag9_a6_c8]]();

  if (flipped) {
    Set_A6();
    action_stack.value[depth][action_stack_count++] = &Set_A6;
    turn = -turn;
    ++depth;
  }
}

void PlayB6() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a6_h6[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*row_a6_h6]]();
  flip_column_b1_b8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*column_b1_b8]]();
  flip_diag7_g1_a7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][1][*diag7_g1_a7]]();
  flip_diag9_a5_d8[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][2][*diag9_a5_d8]]();

  if (flipped) {
    Set_B6();
    action_stack.value[depth][action_stack_count++] = &Set_B6;
    turn = -turn;
    ++depth;
  }
}

void PlayC6() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a6_h6[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*row_a6_h6]]();
  flip_column_c1_c8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*column_c1_c8]]();
  flip_diag7_h1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*diag7_h1_a8]]();
  flip_diag9_a4_e8[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][2][*diag9_a4_e8]]();

  if (flipped) {
    Set_C6();
    action_stack.value[depth][action_stack_count++] = &Set_C6;
    turn = -turn;
    ++depth;
  }
}

void PlayD6() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a6_h6[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*row_a6_h6]]();
  flip_column_d1_d8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*column_d1_d8]]();
  flip_diag7_h2_b8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][2][*diag7_h2_b8]]();
  flip_diag9_a3_f8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][2][*diag9_a3_f8]]();

  if (flipped) {
    Set_D6();
    action_stack.value[depth][action_stack_count++] = &Set_D6;
    turn = -turn;
    ++depth;
  }
}

void PlayE6() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a6_h6[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*row_a6_h6]]();
  flip_column_e1_e8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*column_e1_e8]]();
  flip_diag7_h3_c8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][2][*diag7_h3_c8]]();
  flip_diag9_a2_g8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][2][*diag9_a2_g8]]();

  if (flipped) {
    Set_E6();
    action_stack.value[depth][action_stack_count++] = &Set_E6;
    turn = -turn;
    ++depth;
  }
}

void PlayF6() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a6_h6[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*row_a6_h6]]();
  flip_column_f1_f8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*column_f1_f8]]();
  flip_diag7_h4_d8[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][2][*diag7_h4_d8]]();
  flip_diag9_a1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*diag9_a1_h8]]();

  if (flipped) {
    Set_F6();
    action_stack.value[depth][action_stack_count++] = &Set_F6;
    turn = -turn;
    ++depth;
  }
}

void PlayG6() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a6_h6[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*row_a6_h6]]();
  flip_column_g1_g8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*column_g1_g8]]();
  flip_diag7_h5_e8[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][2][*diag7_h5_e8]]();
  flip_diag9_b1_h7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][1][*diag9_b1_h7]]();

  if (flipped) {
    Set_G6();
    action_stack.value[depth][action_stack_count++] = &Set_G6;
    turn = -turn;
    ++depth;
  }
}

void PlayH6() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a6_h6[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*row_a6_h6]]();
  flip_column_h1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*column_h1_h8]]();
  flip_diag7_h6_f8[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][2][*diag7_h6_f8]]();
  flip_diag9_c1_h6[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][0][*diag9_c1_h6]]();

  if (flipped) {
    Set_H6();
    action_stack.value[depth][action_stack_count++] = &Set_H6;
    turn = -turn;
    ++depth;
  }
}

void PlayA7() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a7_h7[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*row_a7_h7]]();
  flip_column_a1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*column_a1_a8]]();
  flip_diag7_g1_a7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][0][*diag7_g1_a7]]();

  if (flipped) {
    Set_A7();
    action_stack.value[depth][action_stack_count++] = &Set_A7;
    turn = -turn;
    ++depth;
  }
}

void PlayB7() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a7_h7[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*row_a7_h7]]();
  flip_column_b1_b8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*column_b1_b8]]();
  flip_diag7_h1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*diag7_h1_a8]]();
  flip_diag9_a6_c8[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][1][*diag9_a6_c8]]();

  if (flipped) {
    Set_B7();
    action_stack.value[depth][action_stack_count++] = &Set_B7;
    turn = -turn;
    ++depth;
  }
}

void PlayC7() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a7_h7[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*row_a7_h7]]();
  flip_column_c1_c8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*column_c1_c8]]();
  flip_diag7_h2_b8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][1][*diag7_h2_b8]]();
  flip_diag9_a5_d8[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][1][*diag9_a5_d8]]();

  if (flipped) {
    Set_C7();
    action_stack.value[depth][action_stack_count++] = &Set_C7;
    turn = -turn;
    ++depth;
  }
}

void PlayD7() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a7_h7[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*row_a7_h7]]();
  flip_column_d1_d8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*column_d1_d8]]();
  flip_diag7_h3_c8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][1][*diag7_h3_c8]]();
  flip_diag9_a4_e8[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][1][*diag9_a4_e8]]();

  if (flipped) {
    Set_D7();
    action_stack.value[depth][action_stack_count++] = &Set_D7;
    turn = -turn;
    ++depth;
  }
}

void PlayE7() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a7_h7[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*row_a7_h7]]();
  flip_column_e1_e8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*column_e1_e8]]();
  flip_diag7_h4_d8[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][1][*diag7_h4_d8]]();
  flip_diag9_a3_f8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][1][*diag9_a3_f8]]();

  if (flipped) {
    Set_E7();
    action_stack.value[depth][action_stack_count++] = &Set_E7;
    turn = -turn;
    ++depth;
  }
}

void PlayF7() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a7_h7[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*row_a7_h7]]();
  flip_column_f1_f8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*column_f1_f8]]();
  flip_diag7_h5_e8[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][1][*diag7_h5_e8]]();
  flip_diag9_a2_g8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][1][*diag9_a2_g8]]();

  if (flipped) {
    Set_F7();
    action_stack.value[depth][action_stack_count++] = &Set_F7;
    turn = -turn;
    ++depth;
  }
}

void PlayG7() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a7_h7[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*row_a7_h7]]();
  flip_column_g1_g8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*column_g1_g8]]();
  flip_diag7_h6_f8[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][1][*diag7_h6_f8]]();
  flip_diag9_a1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*diag9_a1_h8]]();

  if (flipped) {
    Set_G7();
    action_stack.value[depth][action_stack_count++] = &Set_G7;
    turn = -turn;
    ++depth;
  }
}

void PlayH7() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a7_h7[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*row_a7_h7]]();
  flip_column_h1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*column_h1_h8]]();
  flip_diag9_b1_h7[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][0][*diag9_b1_h7]]();

  if (flipped) {
    Set_H7();
    action_stack.value[depth][action_stack_count++] = &Set_H7;
    turn = -turn;
    ++depth;
  }
}

void PlayA8() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a8_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][7][*row_a8_h8]]();
  flip_column_a1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*column_a1_a8]]();
  flip_diag7_h1_a8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*diag7_h1_a8]]();

  if (flipped) {
    Set_A8();
    action_stack.value[depth][action_stack_count++] = &Set_A8;
    turn = -turn;
    ++depth;
  }
}

void PlayB8() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a8_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][6][*row_a8_h8]]();
  flip_column_b1_b8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*column_b1_b8]]();
  flip_diag7_h2_b8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][0][*diag7_h2_b8]]();

  if (flipped) {
    Set_B8();
    action_stack.value[depth][action_stack_count++] = &Set_B8;
    turn = -turn;
    ++depth;
  }
}

void PlayC8() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a8_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][5][*row_a8_h8]]();
  flip_column_c1_c8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*column_c1_c8]]();
  flip_diag7_h3_c8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][0][*diag7_h3_c8]]();
  flip_diag9_a6_c8[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][0][*diag9_a6_c8]]();

  if (flipped) {
    Set_C8();
    action_stack.value[depth][action_stack_count++] = &Set_C8;
    turn = -turn;
    ++depth;
  }
}

void PlayD8() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a8_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][4][*row_a8_h8]]();
  flip_column_d1_d8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*column_d1_d8]]();
  flip_diag7_h4_d8[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][0][*diag7_h4_d8]]();
  flip_diag9_a5_d8[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][0][*diag9_a5_d8]]();

  if (flipped) {
    Set_D8();
    action_stack.value[depth][action_stack_count++] = &Set_D8;
    turn = -turn;
    ++depth;
  }
}

void PlayE8() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a8_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][3][*row_a8_h8]]();
  flip_column_e1_e8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*column_e1_e8]]();
  flip_diag7_h5_e8[kSquareCompressedPatternToFlip.value_4[TurnToOffset(turn)][0][*diag7_h5_e8]]();
  flip_diag9_a4_e8[kSquareCompressedPatternToFlip.value_5[TurnToOffset(turn)][0][*diag9_a4_e8]]();

  if (flipped) {
    Set_E8();
    action_stack.value[depth][action_stack_count++] = &Set_E8;
    turn = -turn;
    ++depth;
  }
}

void PlayF8() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a8_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][2][*row_a8_h8]]();
  flip_column_f1_f8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*column_f1_f8]]();
  flip_diag7_h6_f8[kSquareCompressedPatternToFlip.value_3[TurnToOffset(turn)][0][*diag7_h6_f8]]();
  flip_diag9_a3_f8[kSquareCompressedPatternToFlip.value_6[TurnToOffset(turn)][0][*diag9_a3_f8]]();

  if (flipped) {
    Set_F8();
    action_stack.value[depth][action_stack_count++] = &Set_F8;
    turn = -turn;
    ++depth;
  }
}

void PlayG8() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a8_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][1][*row_a8_h8]]();
  flip_column_g1_g8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*column_g1_g8]]();
  flip_diag9_a2_g8[kSquareCompressedPatternToFlip.value_7[TurnToOffset(turn)][0][*diag9_a2_g8]]();

  if (flipped) {
    Set_G8();
    action_stack.value[depth][action_stack_count++] = &Set_G8;
    turn = -turn;
    ++depth;
  }
}

void PlayH8() {
  flipped = false;
  action_stack_count = 0;
  flip_row_a8_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*row_a8_h8]]();
  flip_column_h1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*column_h1_h8]]();
  flip_diag9_a1_h8[kSquareCompressedPatternToFlip.value_8[TurnToOffset(turn)][0][*diag9_a1_h8]]();

  if (flipped) {
    Set_H8();
    action_stack.value[depth][action_stack_count++] = &Set_H8;
    turn = -turn;
    ++depth;
  }
}

void (*play_move[64])() = {
    &PlayH8, &PlayG8, &PlayF8, &PlayE8, &PlayD8, &PlayC8,
    &PlayB8, &PlayA8, &PlayH7, &PlayG7, &PlayF7, &PlayE7,
    &PlayD7, &PlayC7, &PlayB7, &PlayA7, &PlayH6, &PlayG6,
    &PlayF6, &PlayE6, &PlayD6, &PlayC6, &PlayB6, &PlayA6,
    &PlayH5, &PlayG5, &PlayF5, &PlayE5, &PlayD5, &PlayC5,
    &PlayB5, &PlayA5, &PlayH4, &PlayG4, &PlayF4, &PlayE4,
    &PlayD4, &PlayC4, &PlayB4, &PlayA4, &PlayH3, &PlayG3,
    &PlayF3, &PlayE3, &PlayD3, &PlayC3, &PlayB3, &PlayA3,
    &PlayH2, &PlayG2, &PlayF2, &PlayE2, &PlayD2, &PlayC2,
    &PlayB2, &PlayA2, &PlayH1, &PlayG1, &PlayF1, &PlayE1,
    &PlayD1, &PlayC1, &PlayB1, &PlayA1
};

void Undo() {
  --depth;
  action_stack.value[depth][0]();
  action_stack.value[depth][0] = &NoFlip;
  action_stack.value[depth][1]();
  action_stack.value[depth][1] = &NoFlip;
  action_stack.value[depth][2]();
  action_stack.value[depth][2] = &NoFlip;
  action_stack.value[depth][3]();
  action_stack.value[depth][3] = &NoFlip;
  action_stack.value[depth][4]();
  action_stack.value[depth][4] = &NoFlip;
  turn = -turn;
}

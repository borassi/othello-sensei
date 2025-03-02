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

#ifndef SENSEI_BOARD_OPTIMIZED_FLIP_OPTIMIZED_H
#define SENSEI_BOARD_OPTIMIZED_FLIP_OPTIMIZED_H

#include "board_optimized.h"


void Flip_B1_ValuesOnly() {
  top_rows += turn * 410390516044136448ULL;
  left_columns += turn * 18786186952704ULL;
  top_diag9s += turn * 1458ULL;
  disk_difference += turn * 2;
}

void UndoFlip_B1() {
  Flip_B1_ValuesOnly();
}

void Flip_B1() {
  Flip_B1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1;
  flipped = true;
}

void Flip_B1_D1_ValuesOnly() {
  top_rows += turn * 455989462271262720ULL;
  left_columns += turn * 18786186957078ULL;
  top_diag9s += turn * 178120883701170ULL;
  top_diag7s += turn * 15199648742375424ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B1_D1() {
  Flip_B1_D1_ValuesOnly();
}

void Flip_B1_D1() {
  Flip_B1_D1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1_D1;
  flipped = true;
}

void Flip_B1_D1E1_ValuesOnly() {
  top_rows += turn * 471189111013638144ULL;
  left_columns += turn * 18786186957078ULL;
  top_diag9s += turn * 15377769626076594ULL;
  top_diag7s += turn * 15377769626075136ULL;
  right_columns += turn * 1231171548132409344ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B1_D1E1() {
  Flip_B1_D1E1_ValuesOnly();
}

void Flip_B1_D1E1() {
  Flip_B1_D1E1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1_D1E1;
  flipped = true;
}

void Flip_B1_D1F1_ValuesOnly() {
  top_rows += turn * 476255660594429952ULL;
  left_columns += turn * 18786186957078ULL;
  top_diag9s += turn * 1312414462308779442ULL;
  top_diag7s += turn * 15377769657925632ULL;
  right_columns += turn * 1231190334319362048ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B1_D1F1() {
  Flip_B1_D1F1_ValuesOnly();
}

void Flip_B1_D1F1() {
  Flip_B1_D1F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1_D1F1;
  flipped = true;
}

void Flip_B1_D1G1_ValuesOnly() {
  top_rows += turn * 477944510454693888ULL;
  left_columns += turn * 18786186957078ULL;
  top_diag9s += turn * 1312414462308779442ULL;
  top_diag7s += turn * 15377769657927090ULL;
  right_columns += turn * 1231190334606016512ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B1_D1G1() {
  Flip_B1_D1G1_ValuesOnly();
}

void Flip_B1_D1G1() {
  Flip_B1_D1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1_D1G1;
  flipped = true;
}

void Flip_B1C1_ValuesOnly() {
  top_rows += turn * 547187354725515264ULL;
  left_columns += turn * 18786473607168ULL;
  top_diag9s += turn * 31851954ULL;
  top_diag7s += turn * 1297036692682702848ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B1C1() {
  Flip_B1C1_ValuesOnly();
}

void Flip_B1C1() {
  Flip_B1C1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1C1;
  flipped = true;
}

void Flip_B1C1_E1_ValuesOnly() {
  top_rows += turn * 562387003467890688ULL;
  left_columns += turn * 18786473607168ULL;
  top_diag9s += turn * 15199648774227378ULL;
  top_diag7s += turn * 1297214813566402560ULL;
  right_columns += turn * 1231171548132409344ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B1C1_E1() {
  Flip_B1C1_E1_ValuesOnly();
}

void Flip_B1C1_E1() {
  Flip_B1C1_E1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1C1_E1;
  flipped = true;
}

void Flip_B1C1_E1F1_ValuesOnly() {
  top_rows += turn * 567453553048682496ULL;
  left_columns += turn * 18786473607168ULL;
  top_diag9s += turn * 1312236341456930226ULL;
  top_diag7s += turn * 1297214813598253056ULL;
  right_columns += turn * 1231190334319362048ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B1C1_E1F1() {
  Flip_B1C1_E1F1_ValuesOnly();
}

void Flip_B1C1_E1F1() {
  Flip_B1C1_E1F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1C1_E1F1;
  flipped = true;
}

void Flip_B1C1_E1G1_ValuesOnly() {
  top_rows += turn * 569142402908946432ULL;
  left_columns += turn * 18786473607168ULL;
  top_diag9s += turn * 1312236341456930226ULL;
  top_diag7s += turn * 1297214813598254514ULL;
  right_columns += turn * 1231190334606016512ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B1C1_E1G1() {
  Flip_B1C1_E1G1_ValuesOnly();
}

void Flip_B1C1_E1G1() {
  Flip_B1C1_E1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1C1_E1G1;
  flipped = true;
}

void Flip_B1D1_ValuesOnly() {
  top_rows += turn * 592786300952641536ULL;
  left_columns += turn * 18786473611542ULL;
  top_diag9s += turn * 178120915551666ULL;
  top_diag7s += turn * 1312236341425078272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B1D1() {
  Flip_B1D1_ValuesOnly();
}

void Flip_B1D1() {
  Flip_B1D1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1D1;
  flipped = true;
}

void Flip_B1D1_F1_ValuesOnly() {
  top_rows += turn * 597852850533433344ULL;
  left_columns += turn * 18786473611542ULL;
  top_diag9s += turn * 1297214813598254514ULL;
  top_diag7s += turn * 1312236341456928768ULL;
  right_columns += turn * 18786186952704ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B1D1_F1() {
  Flip_B1D1_F1_ValuesOnly();
}

void Flip_B1D1_F1() {
  Flip_B1D1_F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1D1_F1;
  flipped = true;
}

void Flip_B1D1_F1G1_ValuesOnly() {
  top_rows += turn * 599541700393697280ULL;
  left_columns += turn * 18786473611542ULL;
  top_diag9s += turn * 1297214813598254514ULL;
  top_diag7s += turn * 1312236341456930226ULL;
  right_columns += turn * 18786473607168ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B1D1_F1G1() {
  Flip_B1D1_F1G1_ValuesOnly();
}

void Flip_B1D1_F1G1() {
  Flip_B1D1_F1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1D1_F1G1;
  flipped = true;
}

void Flip_B1E1_ValuesOnly() {
  top_rows += turn * 607985949695016960ULL;
  left_columns += turn * 18786473611542ULL;
  top_diag9s += turn * 15377769657927090ULL;
  top_diag7s += turn * 1312414462308777984ULL;
  right_columns += turn * 1231171548132409344ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B1E1() {
  Flip_B1E1_ValuesOnly();
}

void Flip_B1E1() {
  Flip_B1E1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1E1;
  flipped = true;
}

void Flip_B1E1_G1_ValuesOnly() {
  top_rows += turn * 609674799555280896ULL;
  left_columns += turn * 18786473611542ULL;
  top_diag9s += turn * 15377769657927090ULL;
  top_diag7s += turn * 1312414462308779442ULL;
  right_columns += turn * 1231171548419063808ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B1E1_G1() {
  Flip_B1E1_G1_ValuesOnly();
}

void Flip_B1E1_G1() {
  Flip_B1E1_G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1E1_G1;
  flipped = true;
}

void Flip_B1F1_ValuesOnly() {
  top_rows += turn * 613052499275808768ULL;
  left_columns += turn * 18786473611542ULL;
  top_diag9s += turn * 1312414462340629938ULL;
  top_diag7s += turn * 1312414462340628480ULL;
  right_columns += turn * 1231190334319362048ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B1F1() {
  Flip_B1F1_ValuesOnly();
}

void Flip_B1F1() {
  Flip_B1F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1F1;
  flipped = true;
}

void Flip_B1G1_ValuesOnly() {
  top_rows += turn * 614741349136072704ULL;
  left_columns += turn * 18786473611542ULL;
  top_diag9s += turn * 1312414462340629938ULL;
  top_diag7s += turn * 1312414462340629938ULL;
  right_columns += turn * 1231190334606016512ULL;
  disk_difference += turn * 12;
}

void UndoFlip_B1G1() {
  Flip_B1G1_ValuesOnly();
}

void Flip_B1G1() {
  Flip_B1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1G1;
  flipped = true;
}

void Flip_C1_ValuesOnly() {
  top_rows += turn * 136796838681378816ULL;
  left_columns += turn * 286654464ULL;
  top_diag9s += turn * 31850496ULL;
  top_diag7s += turn * 1297036692682702848ULL;
  disk_difference += turn * 2;
}

void UndoFlip_C1() {
  Flip_C1_ValuesOnly();
}

void Flip_C1() {
  Flip_C1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1;
  flipped = true;
}

void Flip_C1_E1_ValuesOnly() {
  top_rows += turn * 151996487423754240ULL;
  left_columns += turn * 286654464ULL;
  top_diag9s += turn * 15199648774225920ULL;
  top_diag7s += turn * 1297214813566402560ULL;
  right_columns += turn * 1231171548132409344ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C1_E1() {
  Flip_C1_E1_ValuesOnly();
}

void Flip_C1_E1() {
  Flip_C1_E1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1_E1;
  flipped = true;
}

void Flip_C1_E1F1_ValuesOnly() {
  top_rows += turn * 157063037004546048ULL;
  left_columns += turn * 286654464ULL;
  top_diag9s += turn * 1312236341456928768ULL;
  top_diag7s += turn * 1297214813598253056ULL;
  right_columns += turn * 1231190334319362048ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C1_E1F1() {
  Flip_C1_E1F1_ValuesOnly();
}

void Flip_C1_E1F1() {
  Flip_C1_E1F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1_E1F1;
  flipped = true;
}

void Flip_C1_E1G1_ValuesOnly() {
  top_rows += turn * 158751886864809984ULL;
  left_columns += turn * 286654464ULL;
  top_diag9s += turn * 1312236341456928768ULL;
  top_diag7s += turn * 1297214813598254514ULL;
  right_columns += turn * 1231190334606016512ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C1_E1G1() {
  Flip_C1_E1G1_ValuesOnly();
}

void Flip_C1_E1G1() {
  Flip_C1_E1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1_E1G1;
  flipped = true;
}

void Flip_C1D1_ValuesOnly() {
  top_rows += turn * 182395784908505088ULL;
  left_columns += turn * 286658838ULL;
  top_diag9s += turn * 178120915550208ULL;
  top_diag7s += turn * 1312236341425078272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C1D1() {
  Flip_C1D1_ValuesOnly();
}

void Flip_C1D1() {
  Flip_C1D1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1D1;
  flipped = true;
}

void Flip_C1D1_F1_ValuesOnly() {
  top_rows += turn * 187462334489296896ULL;
  left_columns += turn * 286658838ULL;
  top_diag9s += turn * 1297214813598253056ULL;
  top_diag7s += turn * 1312236341456928768ULL;
  right_columns += turn * 18786186952704ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C1D1_F1() {
  Flip_C1D1_F1_ValuesOnly();
}

void Flip_C1D1_F1() {
  Flip_C1D1_F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1D1_F1;
  flipped = true;
}

void Flip_C1D1_F1G1_ValuesOnly() {
  top_rows += turn * 189151184349560832ULL;
  left_columns += turn * 286658838ULL;
  top_diag9s += turn * 1297214813598253056ULL;
  top_diag7s += turn * 1312236341456930226ULL;
  right_columns += turn * 18786473607168ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C1D1_F1G1() {
  Flip_C1D1_F1G1_ValuesOnly();
}

void Flip_C1D1_F1G1() {
  Flip_C1D1_F1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1D1_F1G1;
  flipped = true;
}

void Flip_C1E1_ValuesOnly() {
  top_rows += turn * 197595433650880512ULL;
  left_columns += turn * 286658838ULL;
  top_diag9s += turn * 15377769657925632ULL;
  top_diag7s += turn * 1312414462308777984ULL;
  right_columns += turn * 1231171548132409344ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C1E1() {
  Flip_C1E1_ValuesOnly();
}

void Flip_C1E1() {
  Flip_C1E1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1E1;
  flipped = true;
}

void Flip_C1E1_G1_ValuesOnly() {
  top_rows += turn * 199284283511144448ULL;
  left_columns += turn * 286658838ULL;
  top_diag9s += turn * 15377769657925632ULL;
  top_diag7s += turn * 1312414462308779442ULL;
  right_columns += turn * 1231171548419063808ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C1E1_G1() {
  Flip_C1E1_G1_ValuesOnly();
}

void Flip_C1E1_G1() {
  Flip_C1E1_G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1E1_G1;
  flipped = true;
}

void Flip_C1F1_ValuesOnly() {
  top_rows += turn * 202661983231672320ULL;
  left_columns += turn * 286658838ULL;
  top_diag9s += turn * 1312414462340628480ULL;
  top_diag7s += turn * 1312414462340628480ULL;
  right_columns += turn * 1231190334319362048ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C1F1() {
  Flip_C1F1_ValuesOnly();
}

void Flip_C1F1() {
  Flip_C1F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1F1;
  flipped = true;
}

void Flip_C1G1_ValuesOnly() {
  top_rows += turn * 204350833091936256ULL;
  left_columns += turn * 286658838ULL;
  top_diag9s += turn * 1312414462340628480ULL;
  top_diag7s += turn * 1312414462340629938ULL;
  right_columns += turn * 1231190334606016512ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C1G1() {
  Flip_C1G1_ValuesOnly();
}

void Flip_C1G1() {
  Flip_C1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1G1;
  flipped = true;
}

void Flip_D1_ValuesOnly() {
  top_rows += turn * 45598946227126272ULL;
  left_columns += turn * 4374ULL;
  top_diag9s += turn * 178120883699712ULL;
  top_diag7s += turn * 15199648742375424ULL;
  disk_difference += turn * 2;
}

void UndoFlip_D1() {
  Flip_D1_ValuesOnly();
}

void Flip_D1() {
  Flip_D1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1;
  flipped = true;
}

void Flip_D1_F1_ValuesOnly() {
  top_rows += turn * 50665495807918080ULL;
  left_columns += turn * 4374ULL;
  top_diag9s += turn * 1297214813566402560ULL;
  top_diag7s += turn * 15199648774225920ULL;
  right_columns += turn * 18786186952704ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D1_F1() {
  Flip_D1_F1_ValuesOnly();
}

void Flip_D1_F1() {
  Flip_D1_F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1_F1;
  flipped = true;
}

void Flip_D1_F1G1_ValuesOnly() {
  top_rows += turn * 52354345668182016ULL;
  left_columns += turn * 4374ULL;
  top_diag9s += turn * 1297214813566402560ULL;
  top_diag7s += turn * 15199648774227378ULL;
  right_columns += turn * 18786473607168ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D1_F1G1() {
  Flip_D1_F1G1_ValuesOnly();
}

void Flip_D1_F1G1() {
  Flip_D1_F1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1_F1G1;
  flipped = true;
}

void Flip_D1E1_ValuesOnly() {
  top_rows += turn * 60798594969501696ULL;
  left_columns += turn * 4374ULL;
  top_diag9s += turn * 15377769626075136ULL;
  top_diag7s += turn * 15377769626075136ULL;
  right_columns += turn * 1231171548132409344ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D1E1() {
  Flip_D1E1_ValuesOnly();
}

void Flip_D1E1() {
  Flip_D1E1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1E1;
  flipped = true;
}

void Flip_D1E1_G1_ValuesOnly() {
  top_rows += turn * 62487444829765632ULL;
  left_columns += turn * 4374ULL;
  top_diag9s += turn * 15377769626075136ULL;
  top_diag7s += turn * 15377769626076594ULL;
  right_columns += turn * 1231171548419063808ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D1E1_G1() {
  Flip_D1E1_G1_ValuesOnly();
}

void Flip_D1E1_G1() {
  Flip_D1E1_G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1E1_G1;
  flipped = true;
}

void Flip_D1F1_ValuesOnly() {
  top_rows += turn * 65865144550293504ULL;
  left_columns += turn * 4374ULL;
  top_diag9s += turn * 1312414462308777984ULL;
  top_diag7s += turn * 15377769657925632ULL;
  right_columns += turn * 1231190334319362048ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D1F1() {
  Flip_D1F1_ValuesOnly();
}

void Flip_D1F1() {
  Flip_D1F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1F1;
  flipped = true;
}

void Flip_D1G1_ValuesOnly() {
  top_rows += turn * 67553994410557440ULL;
  left_columns += turn * 4374ULL;
  top_diag9s += turn * 1312414462308777984ULL;
  top_diag7s += turn * 15377769657927090ULL;
  right_columns += turn * 1231190334606016512ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D1G1() {
  Flip_D1G1_ValuesOnly();
}

void Flip_D1G1() {
  Flip_D1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1G1;
  flipped = true;
}

void Flip_E1_ValuesOnly() {
  top_rows += turn * 15199648742375424ULL;
  top_diag9s += turn * 15199648742375424ULL;
  top_diag7s += turn * 178120883699712ULL;
  right_columns += turn * 1231171548132409344ULL;
  disk_difference += turn * 2;
}

void UndoFlip_E1() {
  Flip_E1_ValuesOnly();
}

void Flip_E1() {
  Flip_E1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E1;
  flipped = true;
}

void Flip_E1_G1_ValuesOnly() {
  top_rows += turn * 16888498602639360ULL;
  top_diag9s += turn * 15199648742375424ULL;
  top_diag7s += turn * 178120883701170ULL;
  right_columns += turn * 1231171548419063808ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E1_G1() {
  Flip_E1_G1_ValuesOnly();
}

void Flip_E1_G1() {
  Flip_E1_G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E1_G1;
  flipped = true;
}

void Flip_E1F1_ValuesOnly() {
  top_rows += turn * 20266198323167232ULL;
  top_diag9s += turn * 1312236341425078272ULL;
  top_diag7s += turn * 178120915550208ULL;
  right_columns += turn * 1231190334319362048ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E1F1() {
  Flip_E1F1_ValuesOnly();
}

void Flip_E1F1() {
  Flip_E1F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E1F1;
  flipped = true;
}

void Flip_E1G1_ValuesOnly() {
  top_rows += turn * 21955048183431168ULL;
  top_diag9s += turn * 1312236341425078272ULL;
  top_diag7s += turn * 178120915551666ULL;
  right_columns += turn * 1231190334606016512ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E1G1() {
  Flip_E1G1_ValuesOnly();
}

void Flip_E1G1() {
  Flip_E1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E1G1;
  flipped = true;
}

void Flip_F1_ValuesOnly() {
  top_rows += turn * 5066549580791808ULL;
  top_diag9s += turn * 1297036692682702848ULL;
  top_diag7s += turn * 31850496ULL;
  right_columns += turn * 18786186952704ULL;
  disk_difference += turn * 2;
}

void UndoFlip_F1() {
  Flip_F1_ValuesOnly();
}

void Flip_F1() {
  Flip_F1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F1;
  flipped = true;
}

void Flip_F1G1_ValuesOnly() {
  top_rows += turn * 6755399441055744ULL;
  top_diag9s += turn * 1297036692682702848ULL;
  top_diag7s += turn * 31851954ULL;
  right_columns += turn * 18786473607168ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F1G1() {
  Flip_F1G1_ValuesOnly();
}

void Flip_F1G1() {
  Flip_F1G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F1G1;
  flipped = true;
}

void Flip_G1_ValuesOnly() {
  top_rows += turn * 1688849860263936ULL;
  top_diag7s += turn * 1458ULL;
  right_columns += turn * 286654464ULL;
  disk_difference += turn * 2;
}

void UndoFlip_G1() {
  Flip_G1_ValuesOnly();
}

void Flip_G1() {
  Flip_G1_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G1;
  flipped = true;
}

void Flip_B2_ValuesOnly() {
  top_rows += turn * 6262062317568ULL;
  left_columns += turn * 6262062317568ULL;
  top_diag7s += turn * 432345564227567616ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 2;
}

void UndoFlip_B2() {
  Flip_B2_ValuesOnly();
}

void Flip_B2() {
  Flip_B2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2;
  flipped = true;
}

void Flip_B2_D2_ValuesOnly() {
  top_rows += turn * 6957847019520ULL;
  left_columns += turn * 6262062319026ULL;
  top_diag9s += turn * 10616832ULL;
  top_diag7s += turn * 432404937855467520ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B2_D2() {
  Flip_B2_D2_ValuesOnly();
}

void Flip_B2_D2() {
  Flip_B2_D2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D2;
  flipped = true;
}

void Flip_B2_D2E2_ValuesOnly() {
  top_rows += turn * 7189775253504ULL;
  left_columns += turn * 6262062319026ULL;
  top_diag9s += turn * 59373638516736ULL;
  top_diag7s += turn * 432404937866084352ULL;
  right_columns += turn * 410390516044136448ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B2_D2E2() {
  Flip_B2_D2E2_ValuesOnly();
}

void Flip_B2_D2E2() {
  Flip_B2_D2E2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D2E2;
  flipped = true;
}

void Flip_B2_D2F2_ValuesOnly() {
  top_rows += turn * 7267084664832ULL;
  left_columns += turn * 6262062319026ULL;
  top_diag9s += turn * 5125923219308544ULL;
  top_diag7s += turn * 432404937866084838ULL;
  right_columns += turn * 410396778106454016ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2_D2F2() {
  Flip_B2_D2F2_ValuesOnly();
}

void Flip_B2_D2F2() {
  Flip_B2_D2F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D2F2;
  flipped = true;
}

void Flip_B2_D2G2_ValuesOnly() {
  top_rows += turn * 7292854468608ULL;
  left_columns += turn * 6262062319026ULL;
  top_diag9s += turn * 437471487446876160ULL;
  top_diag7s += turn * 432404937866084838ULL;
  right_columns += turn * 410396778202005504ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2_D2G2() {
  Flip_B2_D2G2_ValuesOnly();
}

void Flip_B2_D2G2() {
  Flip_B2_D2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D2G2;
  flipped = true;
}

void Flip_B2C2_ValuesOnly() {
  top_rows += turn * 8349416423424ULL;
  left_columns += turn * 6262157869056ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 437412113808359424ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B2C2() {
  Flip_B2C2_ValuesOnly();
}

void Flip_B2C2() {
  Flip_B2C2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C2;
  flipped = true;
}

void Flip_B2C2_E2_ValuesOnly() {
  top_rows += turn * 8581344657408ULL;
  left_columns += turn * 6262157869056ULL;
  top_diag9s += turn * 59373627900390ULL;
  top_diag7s += turn * 437412113818976256ULL;
  right_columns += turn * 410390516044136448ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B2C2_E2() {
  Flip_B2C2_E2_ValuesOnly();
}

void Flip_B2C2_E2() {
  Flip_B2C2_E2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C2_E2;
  flipped = true;
}

void Flip_B2C2_E2F2_ValuesOnly() {
  top_rows += turn * 8658654068736ULL;
  left_columns += turn * 6262157869056ULL;
  top_diag9s += turn * 5125923208692198ULL;
  top_diag7s += turn * 437412113818976742ULL;
  right_columns += turn * 410396778106454016ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2C2_E2F2() {
  Flip_B2C2_E2F2_ValuesOnly();
}

void Flip_B2C2_E2F2() {
  Flip_B2C2_E2F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C2_E2F2;
  flipped = true;
}

void Flip_B2C2_E2G2_ValuesOnly() {
  top_rows += turn * 8684423872512ULL;
  left_columns += turn * 6262157869056ULL;
  top_diag9s += turn * 437471487436259814ULL;
  top_diag7s += turn * 437412113818976742ULL;
  right_columns += turn * 410396778202005504ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2C2_E2G2() {
  Flip_B2C2_E2G2_ValuesOnly();
}

void Flip_B2C2_E2G2() {
  Flip_B2C2_E2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C2_E2G2;
  flipped = true;
}

void Flip_B2D2_ValuesOnly() {
  top_rows += turn * 9045201125376ULL;
  left_columns += turn * 6262157870514ULL;
  top_diag9s += turn * 10617318ULL;
  top_diag7s += turn * 437471487436259328ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B2D2() {
  Flip_B2D2_ValuesOnly();
}

void Flip_B2D2() {
  Flip_B2D2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D2;
  flipped = true;
}

void Flip_B2D2_F2_ValuesOnly() {
  top_rows += turn * 9122510536704ULL;
  left_columns += turn * 6262157870514ULL;
  top_diag9s += turn * 5066549591409126ULL;
  top_diag7s += turn * 437471487436259814ULL;
  right_columns += turn * 6262062317568ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2D2_F2() {
  Flip_B2D2_F2_ValuesOnly();
}

void Flip_B2D2_F2() {
  Flip_B2D2_F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D2_F2;
  flipped = true;
}

void Flip_B2D2_F2G2_ValuesOnly() {
  top_rows += turn * 9148280340480ULL;
  left_columns += turn * 6262157870514ULL;
  top_diag9s += turn * 437412113818976742ULL;
  top_diag7s += turn * 437471487436259814ULL;
  right_columns += turn * 6262157869056ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2D2_F2G2() {
  Flip_B2D2_F2G2_ValuesOnly();
}

void Flip_B2D2_F2G2() {
  Flip_B2D2_F2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D2_F2G2;
  flipped = true;
}

void Flip_B2E2_ValuesOnly() {
  top_rows += turn * 9277129359360ULL;
  left_columns += turn * 6262157870514ULL;
  top_diag9s += turn * 59373638517222ULL;
  top_diag7s += turn * 437471487446876160ULL;
  right_columns += turn * 410390516044136448ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2E2() {
  Flip_B2E2_ValuesOnly();
}

void Flip_B2E2() {
  Flip_B2E2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2E2;
  flipped = true;
}

void Flip_B2E2_G2_ValuesOnly() {
  top_rows += turn * 9302899163136ULL;
  left_columns += turn * 6262157870514ULL;
  top_diag9s += turn * 432404937866084838ULL;
  top_diag7s += turn * 437471487446876160ULL;
  right_columns += turn * 410390516139687936ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2E2_G2() {
  Flip_B2E2_G2_ValuesOnly();
}

void Flip_B2E2_G2() {
  Flip_B2E2_G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2E2_G2;
  flipped = true;
}

void Flip_B2F2_ValuesOnly() {
  top_rows += turn * 9354438770688ULL;
  left_columns += turn * 6262157870514ULL;
  top_diag9s += turn * 5125923219309030ULL;
  top_diag7s += turn * 437471487446876646ULL;
  right_columns += turn * 410396778106454016ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2F2() {
  Flip_B2F2_ValuesOnly();
}

void Flip_B2F2() {
  Flip_B2F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2F2;
  flipped = true;
}

void Flip_B2G2_ValuesOnly() {
  top_rows += turn * 9380208574464ULL;
  left_columns += turn * 6262157870514ULL;
  top_diag9s += turn * 437471487446876646ULL;
  top_diag7s += turn * 437471487446876646ULL;
  right_columns += turn * 410396778202005504ULL;
  bottom_diag9s += turn * 410390516044136448ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 12;
}

void UndoFlip_B2G2() {
  Flip_B2G2_ValuesOnly();
}

void Flip_B2G2() {
  Flip_B2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2G2;
  flipped = true;
}

void Flip_C2_ValuesOnly() {
  top_rows += turn * 2087354105856ULL;
  left_columns += turn * 95551488ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5066549580791808ULL;
  disk_difference += turn * 2;
}

void UndoFlip_C2() {
  Flip_C2_ValuesOnly();
}

void Flip_C2() {
  Flip_C2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2;
  flipped = true;
}

void Flip_C2_E2_ValuesOnly() {
  top_rows += turn * 2319282339840ULL;
  left_columns += turn * 95551488ULL;
  top_diag9s += turn * 59373627900390ULL;
  top_diag7s += turn * 5066549591408640ULL;
  right_columns += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C2_E2() {
  Flip_C2_E2_ValuesOnly();
}

void Flip_C2_E2() {
  Flip_C2_E2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E2;
  flipped = true;
}

void Flip_C2_E2F2_ValuesOnly() {
  top_rows += turn * 2396591751168ULL;
  left_columns += turn * 95551488ULL;
  top_diag9s += turn * 5125923208692198ULL;
  top_diag7s += turn * 5066549591409126ULL;
  right_columns += turn * 410396778106454016ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C2_E2F2() {
  Flip_C2_E2F2_ValuesOnly();
}

void Flip_C2_E2F2() {
  Flip_C2_E2F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E2F2;
  flipped = true;
}

void Flip_C2_E2G2_ValuesOnly() {
  top_rows += turn * 2422361554944ULL;
  left_columns += turn * 95551488ULL;
  top_diag9s += turn * 437471487436259814ULL;
  top_diag7s += turn * 5066549591409126ULL;
  right_columns += turn * 410396778202005504ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2_E2G2() {
  Flip_C2_E2G2_ValuesOnly();
}

void Flip_C2_E2G2() {
  Flip_C2_E2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E2G2;
  flipped = true;
}

void Flip_C2D2_ValuesOnly() {
  top_rows += turn * 2783138807808ULL;
  left_columns += turn * 95552946ULL;
  top_diag9s += turn * 10617318ULL;
  top_diag7s += turn * 5125923208691712ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C2D2() {
  Flip_C2D2_ValuesOnly();
}

void Flip_C2D2() {
  Flip_C2D2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D2;
  flipped = true;
}

void Flip_C2D2_F2_ValuesOnly() {
  top_rows += turn * 2860448219136ULL;
  left_columns += turn * 95552946ULL;
  top_diag9s += turn * 5066549591409126ULL;
  top_diag7s += turn * 5125923208692198ULL;
  right_columns += turn * 6262062317568ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C2D2_F2() {
  Flip_C2D2_F2_ValuesOnly();
}

void Flip_C2D2_F2() {
  Flip_C2D2_F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D2_F2;
  flipped = true;
}

void Flip_C2D2_F2G2_ValuesOnly() {
  top_rows += turn * 2886218022912ULL;
  left_columns += turn * 95552946ULL;
  top_diag9s += turn * 437412113818976742ULL;
  top_diag7s += turn * 5125923208692198ULL;
  right_columns += turn * 6262157869056ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2D2_F2G2() {
  Flip_C2D2_F2G2_ValuesOnly();
}

void Flip_C2D2_F2G2() {
  Flip_C2D2_F2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D2_F2G2;
  flipped = true;
}

void Flip_C2E2_ValuesOnly() {
  top_rows += turn * 3015067041792ULL;
  left_columns += turn * 95552946ULL;
  top_diag9s += turn * 59373638517222ULL;
  top_diag7s += turn * 5125923219308544ULL;
  right_columns += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C2E2() {
  Flip_C2E2_ValuesOnly();
}

void Flip_C2E2() {
  Flip_C2E2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2E2;
  flipped = true;
}

void Flip_C2E2_G2_ValuesOnly() {
  top_rows += turn * 3040836845568ULL;
  left_columns += turn * 95552946ULL;
  top_diag9s += turn * 432404937866084838ULL;
  top_diag7s += turn * 5125923219308544ULL;
  right_columns += turn * 410390516139687936ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2E2_G2() {
  Flip_C2E2_G2_ValuesOnly();
}

void Flip_C2E2_G2() {
  Flip_C2E2_G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2E2_G2;
  flipped = true;
}

void Flip_C2F2_ValuesOnly() {
  top_rows += turn * 3092376453120ULL;
  left_columns += turn * 95552946ULL;
  top_diag9s += turn * 5125923219309030ULL;
  top_diag7s += turn * 5125923219309030ULL;
  right_columns += turn * 410396778106454016ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2F2() {
  Flip_C2F2_ValuesOnly();
}

void Flip_C2F2() {
  Flip_C2F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2F2;
  flipped = true;
}

void Flip_C2G2_ValuesOnly() {
  top_rows += turn * 3118146256896ULL;
  left_columns += turn * 95552946ULL;
  top_diag9s += turn * 437471487446876646ULL;
  top_diag7s += turn * 5125923219309030ULL;
  right_columns += turn * 410396778202005504ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C2G2() {
  Flip_C2G2_ValuesOnly();
}

void Flip_C2G2() {
  Flip_C2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2G2;
  flipped = true;
}

void Flip_D2_ValuesOnly() {
  top_rows += turn * 695784701952ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 10616832ULL;
  top_diag7s += turn * 59373627899904ULL;
  disk_difference += turn * 2;
}

void UndoFlip_D2() {
  Flip_D2_ValuesOnly();
}

void Flip_D2() {
  Flip_D2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2;
  flipped = true;
}

void Flip_D2_F2_ValuesOnly() {
  top_rows += turn * 773094113280ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 5066549591408640ULL;
  top_diag7s += turn * 59373627900390ULL;
  right_columns += turn * 6262062317568ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D2_F2() {
  Flip_D2_F2_ValuesOnly();
}

void Flip_D2_F2() {
  Flip_D2_F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_F2;
  flipped = true;
}

void Flip_D2_F2G2_ValuesOnly() {
  top_rows += turn * 798863917056ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 437412113818976256ULL;
  top_diag7s += turn * 59373627900390ULL;
  right_columns += turn * 6262157869056ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D2_F2G2() {
  Flip_D2_F2G2_ValuesOnly();
}

void Flip_D2_F2G2() {
  Flip_D2_F2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_F2G2;
  flipped = true;
}

void Flip_D2E2_ValuesOnly() {
  top_rows += turn * 927712935936ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 59373638516736ULL;
  top_diag7s += turn * 59373638516736ULL;
  right_columns += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D2E2() {
  Flip_D2E2_ValuesOnly();
}

void Flip_D2E2() {
  Flip_D2E2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2E2;
  flipped = true;
}

void Flip_D2E2_G2_ValuesOnly() {
  top_rows += turn * 953482739712ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 432404937866084352ULL;
  top_diag7s += turn * 59373638516736ULL;
  right_columns += turn * 410390516139687936ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D2E2_G2() {
  Flip_D2E2_G2_ValuesOnly();
}

void Flip_D2E2_G2() {
  Flip_D2E2_G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2E2_G2;
  flipped = true;
}

void Flip_D2F2_ValuesOnly() {
  top_rows += turn * 1005022347264ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 5125923219308544ULL;
  top_diag7s += turn * 59373638517222ULL;
  right_columns += turn * 410396778106454016ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D2F2() {
  Flip_D2F2_ValuesOnly();
}

void Flip_D2F2() {
  Flip_D2F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2F2;
  flipped = true;
}

void Flip_D2G2_ValuesOnly() {
  top_rows += turn * 1030792151040ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 437471487446876160ULL;
  top_diag7s += turn * 59373638517222ULL;
  right_columns += turn * 410396778202005504ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D2G2() {
  Flip_D2G2_ValuesOnly();
}

void Flip_D2G2() {
  Flip_D2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2G2;
  flipped = true;
}

void Flip_E2_ValuesOnly() {
  top_rows += turn * 231928233984ULL;
  top_diag9s += turn * 59373627899904ULL;
  top_diag7s += turn * 10616832ULL;
  right_columns += turn * 410390516044136448ULL;
  disk_difference += turn * 2;
}

void UndoFlip_E2() {
  Flip_E2_ValuesOnly();
}

void Flip_E2() {
  Flip_E2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2;
  flipped = true;
}

void Flip_E2_G2_ValuesOnly() {
  top_rows += turn * 257698037760ULL;
  top_diag9s += turn * 432404937855467520ULL;
  top_diag7s += turn * 10616832ULL;
  right_columns += turn * 410390516139687936ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E2_G2() {
  Flip_E2_G2_ValuesOnly();
}

void Flip_E2_G2() {
  Flip_E2_G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_G2;
  flipped = true;
}

void Flip_E2F2_ValuesOnly() {
  top_rows += turn * 309237645312ULL;
  top_diag9s += turn * 5125923208691712ULL;
  top_diag7s += turn * 10617318ULL;
  right_columns += turn * 410396778106454016ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E2F2() {
  Flip_E2F2_ValuesOnly();
}

void Flip_E2F2() {
  Flip_E2F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2F2;
  flipped = true;
}

void Flip_E2G2_ValuesOnly() {
  top_rows += turn * 335007449088ULL;
  top_diag9s += turn * 437471487436259328ULL;
  top_diag7s += turn * 10617318ULL;
  right_columns += turn * 410396778202005504ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E2G2() {
  Flip_E2G2_ValuesOnly();
}

void Flip_E2G2() {
  Flip_E2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2G2;
  flipped = true;
}

void Flip_F2_ValuesOnly() {
  top_rows += turn * 77309411328ULL;
  top_diag9s += turn * 5066549580791808ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 6262062317568ULL;
  disk_difference += turn * 2;
}

void UndoFlip_F2() {
  Flip_F2_ValuesOnly();
}

void Flip_F2() {
  Flip_F2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2;
  flipped = true;
}

void Flip_F2G2_ValuesOnly() {
  top_rows += turn * 103079215104ULL;
  top_diag9s += turn * 437412113808359424ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 6262157869056ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F2G2() {
  Flip_F2G2_ValuesOnly();
}

void Flip_F2G2() {
  Flip_F2G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2G2;
  flipped = true;
}

void Flip_G2_ValuesOnly() {
  top_rows += turn * 25769803776ULL;
  top_diag9s += turn * 432345564227567616ULL;
  right_columns += turn * 95551488ULL;
  bottom_diag7s += turn * 410390516044136448ULL;
  disk_difference += turn * 2;
}

void UndoFlip_G2() {
  Flip_G2_ValuesOnly();
}

void Flip_G2() {
  Flip_G2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2;
  flipped = true;
}

void Flip_B3_ValuesOnly() {
  top_rows += turn * 95551488ULL;
  left_columns += turn * 2087354105856ULL;
  top_diag7s += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 2087354105856ULL;
  disk_difference += turn * 2;
}

void UndoFlip_B3() {
  Flip_B3_ValuesOnly();
}

void Flip_B3() {
  Flip_B3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3;
  flipped = true;
}

void Flip_B3_D3_ValuesOnly() {
  top_rows += turn * 106168320ULL;
  left_columns += turn * 2087354106342ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 1688849863802880ULL;
  bottom_diag9s += turn * 2087354105856ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B3_D3() {
  Flip_B3_D3_ValuesOnly();
}

void Flip_B3_D3() {
  Flip_B3_D3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D3;
  flipped = true;
}

void Flip_B3_D3E3_ValuesOnly() {
  top_rows += turn * 109707264ULL;
  left_columns += turn * 2087354106342ULL;
  top_diag9s += turn * 3539106ULL;
  top_diag7s += turn * 1688849863803042ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 2087354105856ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B3_D3E3() {
  Flip_B3_D3E3_ValuesOnly();
}

void Flip_B3_D3E3() {
  Flip_B3_D3E3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D3E3;
  flipped = true;
}

void Flip_B3_D3F3_ValuesOnly() {
  top_rows += turn * 110886912ULL;
  left_columns += turn * 2087354106342ULL;
  top_diag9s += turn * 19791212839074ULL;
  top_diag7s += turn * 1688849863803042ULL;
  right_columns += turn * 136798926035484672ULL;
  bottom_diag9s += turn * 2087354105856ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3_D3F3() {
  Flip_B3_D3F3_ValuesOnly();
}

void Flip_B3_D3F3() {
  Flip_B3_D3F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D3F3;
  flipped = true;
}

void Flip_B3_D3G3_ValuesOnly() {
  top_rows += turn * 111280128ULL;
  left_columns += turn * 2087354106342ULL;
  top_diag9s += turn * 1708641073103010ULL;
  top_diag7s += turn * 1688849863803042ULL;
  right_columns += turn * 136798926067335168ULL;
  bottom_diag9s += turn * 2087354105856ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B3_D3G3() {
  Flip_B3_D3G3_ValuesOnly();
}

void Flip_B3_D3G3() {
  Flip_B3_D3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D3G3;
  flipped = true;
}

void Flip_B3C3_ValuesOnly() {
  top_rows += turn * 127401984ULL;
  left_columns += turn * 2087385956352ULL;
  top_diag7s += turn * 1708641069563904ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B3C3() {
  Flip_B3C3_ValuesOnly();
}

void Flip_B3C3() {
  Flip_B3C3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C3;
  flipped = true;
}

void Flip_B3C3_E3_ValuesOnly() {
  top_rows += turn * 130940928ULL;
  left_columns += turn * 2087385956352ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 1708641069564066ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B3C3_E3() {
  Flip_B3C3_E3_ValuesOnly();
}

void Flip_B3C3_E3() {
  Flip_B3C3_E3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C3_E3;
  flipped = true;
}

void Flip_B3C3_E3F3_ValuesOnly() {
  top_rows += turn * 132120576ULL;
  left_columns += turn * 2087385956352ULL;
  top_diag9s += turn * 19791212838912ULL;
  top_diag7s += turn * 1708641069564066ULL;
  right_columns += turn * 136798926035484672ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3C3_E3F3() {
  Flip_B3C3_E3F3_ValuesOnly();
}

void Flip_B3C3_E3F3() {
  Flip_B3C3_E3F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C3_E3F3;
  flipped = true;
}

void Flip_B3C3_E3G3_ValuesOnly() {
  top_rows += turn * 132513792ULL;
  left_columns += turn * 2087385956352ULL;
  top_diag9s += turn * 1708641073102848ULL;
  top_diag7s += turn * 1708641069564066ULL;
  right_columns += turn * 136798926067335168ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B3C3_E3G3() {
  Flip_B3C3_E3G3_ValuesOnly();
}

void Flip_B3C3_E3G3() {
  Flip_B3C3_E3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C3_E3G3;
  flipped = true;
}

void Flip_B3D3_ValuesOnly() {
  top_rows += turn * 138018816ULL;
  left_columns += turn * 2087385956838ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 1708641073102848ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B3D3() {
  Flip_B3D3_ValuesOnly();
}

void Flip_B3D3() {
  Flip_B3D3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3D3;
  flipped = true;
}

void Flip_B3D3_F3_ValuesOnly() {
  top_rows += turn * 139198464ULL;
  left_columns += turn * 2087385956838ULL;
  top_diag9s += turn * 19791209300130ULL;
  top_diag7s += turn * 1708641073102848ULL;
  right_columns += turn * 2087354105856ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3D3_F3() {
  Flip_B3D3_F3_ValuesOnly();
}

void Flip_B3D3_F3() {
  Flip_B3D3_F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3D3_F3;
  flipped = true;
}

void Flip_B3D3_F3G3_ValuesOnly() {
  top_rows += turn * 139591680ULL;
  left_columns += turn * 2087385956838ULL;
  top_diag9s += turn * 1708641069564066ULL;
  top_diag7s += turn * 1708641073102848ULL;
  right_columns += turn * 2087385956352ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B3D3_F3G3() {
  Flip_B3D3_F3G3_ValuesOnly();
}

void Flip_B3D3_F3G3() {
  Flip_B3D3_F3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3D3_F3G3;
  flipped = true;
}

void Flip_B3E3_ValuesOnly() {
  top_rows += turn * 141557760ULL;
  left_columns += turn * 2087385956838ULL;
  top_diag9s += turn * 3539106ULL;
  top_diag7s += turn * 1708641073103010ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3E3() {
  Flip_B3E3_ValuesOnly();
}

void Flip_B3E3() {
  Flip_B3E3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3E3;
  flipped = true;
}

void Flip_B3E3_G3_ValuesOnly() {
  top_rows += turn * 141950976ULL;
  left_columns += turn * 2087385956838ULL;
  top_diag9s += turn * 1688849863803042ULL;
  top_diag7s += turn * 1708641073103010ULL;
  right_columns += turn * 136796838713229312ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  bottom_diag7s += turn * 2087354105856ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B3E3_G3() {
  Flip_B3E3_G3_ValuesOnly();
}

void Flip_B3E3_G3() {
  Flip_B3E3_G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3E3_G3;
  flipped = true;
}

void Flip_B3F3_ValuesOnly() {
  top_rows += turn * 142737408ULL;
  left_columns += turn * 2087385956838ULL;
  top_diag9s += turn * 19791212839074ULL;
  top_diag7s += turn * 1708641073103010ULL;
  right_columns += turn * 136798926035484672ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B3F3() {
  Flip_B3F3_ValuesOnly();
}

void Flip_B3F3() {
  Flip_B3F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3F3;
  flipped = true;
}

void Flip_B3G3_ValuesOnly() {
  top_rows += turn * 143130624ULL;
  left_columns += turn * 2087385956838ULL;
  top_diag9s += turn * 1708641073103010ULL;
  top_diag7s += turn * 1708641073103010ULL;
  right_columns += turn * 136798926067335168ULL;
  bottom_diag9s += turn * 136798926035484672ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 12;
}

void UndoFlip_B3G3() {
  Flip_B3G3_ValuesOnly();
}

void Flip_B3G3() {
  Flip_B3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3G3;
  flipped = true;
}

void Flip_C3_ValuesOnly() {
  top_rows += turn * 31850496ULL;
  left_columns += turn * 31850496ULL;
  top_diag7s += turn * 19791209299968ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  disk_difference += turn * 2;
}

void UndoFlip_C3() {
  Flip_C3_ValuesOnly();
}

void Flip_C3() {
  Flip_C3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3;
  flipped = true;
}

void Flip_C3_E3_ValuesOnly() {
  top_rows += turn * 35389440ULL;
  left_columns += turn * 31850496ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 19791209300130ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C3_E3() {
  Flip_C3_E3_ValuesOnly();
}

void Flip_C3_E3() {
  Flip_C3_E3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E3;
  flipped = true;
}

void Flip_C3_E3F3_ValuesOnly() {
  top_rows += turn * 36569088ULL;
  left_columns += turn * 31850496ULL;
  top_diag9s += turn * 19791212838912ULL;
  top_diag7s += turn * 19791209300130ULL;
  right_columns += turn * 136798926035484672ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C3_E3F3() {
  Flip_C3_E3F3_ValuesOnly();
}

void Flip_C3_E3F3() {
  Flip_C3_E3F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E3F3;
  flipped = true;
}

void Flip_C3_E3G3_ValuesOnly() {
  top_rows += turn * 36962304ULL;
  left_columns += turn * 31850496ULL;
  top_diag9s += turn * 1708641073102848ULL;
  top_diag7s += turn * 19791209300130ULL;
  right_columns += turn * 136798926067335168ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3_E3G3() {
  Flip_C3_E3G3_ValuesOnly();
}

void Flip_C3_E3G3() {
  Flip_C3_E3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E3G3;
  flipped = true;
}

void Flip_C3D3_ValuesOnly() {
  top_rows += turn * 42467328ULL;
  left_columns += turn * 31850982ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 19791212838912ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C3D3() {
  Flip_C3D3_ValuesOnly();
}

void Flip_C3D3() {
  Flip_C3D3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D3;
  flipped = true;
}

void Flip_C3D3_F3_ValuesOnly() {
  top_rows += turn * 43646976ULL;
  left_columns += turn * 31850982ULL;
  top_diag9s += turn * 19791209300130ULL;
  top_diag7s += turn * 19791212838912ULL;
  right_columns += turn * 2087354105856ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C3D3_F3() {
  Flip_C3D3_F3_ValuesOnly();
}

void Flip_C3D3_F3() {
  Flip_C3D3_F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D3_F3;
  flipped = true;
}

void Flip_C3D3_F3G3_ValuesOnly() {
  top_rows += turn * 44040192ULL;
  left_columns += turn * 31850982ULL;
  top_diag9s += turn * 1708641069564066ULL;
  top_diag7s += turn * 19791212838912ULL;
  right_columns += turn * 2087385956352ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3D3_F3G3() {
  Flip_C3D3_F3G3_ValuesOnly();
}

void Flip_C3D3_F3G3() {
  Flip_C3D3_F3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D3_F3G3;
  flipped = true;
}

void Flip_C3E3_ValuesOnly() {
  top_rows += turn * 46006272ULL;
  left_columns += turn * 31850982ULL;
  top_diag9s += turn * 3539106ULL;
  top_diag7s += turn * 19791212839074ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C3E3() {
  Flip_C3E3_ValuesOnly();
}

void Flip_C3E3() {
  Flip_C3E3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3E3;
  flipped = true;
}

void Flip_C3E3_G3_ValuesOnly() {
  top_rows += turn * 46399488ULL;
  left_columns += turn * 31850982ULL;
  top_diag9s += turn * 1688849863803042ULL;
  top_diag7s += turn * 19791212839074ULL;
  right_columns += turn * 136796838713229312ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  bottom_diag7s += turn * 2087354105856ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3E3_G3() {
  Flip_C3E3_G3_ValuesOnly();
}

void Flip_C3E3_G3() {
  Flip_C3E3_G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3E3_G3;
  flipped = true;
}

void Flip_C3F3_ValuesOnly() {
  top_rows += turn * 47185920ULL;
  left_columns += turn * 31850982ULL;
  top_diag9s += turn * 19791212839074ULL;
  top_diag7s += turn * 19791212839074ULL;
  right_columns += turn * 136798926035484672ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3F3() {
  Flip_C3F3_ValuesOnly();
}

void Flip_C3F3() {
  Flip_C3F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3F3;
  flipped = true;
}

void Flip_C3G3_ValuesOnly() {
  top_rows += turn * 47579136ULL;
  left_columns += turn * 31850982ULL;
  top_diag9s += turn * 1708641073103010ULL;
  top_diag7s += turn * 19791212839074ULL;
  right_columns += turn * 136798926067335168ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C3G3() {
  Flip_C3G3_ValuesOnly();
}

void Flip_C3G3() {
  Flip_C3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3G3;
  flipped = true;
}

void Flip_D3_ValuesOnly() {
  top_rows += turn * 10616832ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538944ULL;
  disk_difference += turn * 2;
}

void UndoFlip_D3() {
  Flip_D3_ValuesOnly();
}

void Flip_D3() {
  Flip_D3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3;
  flipped = true;
}

void Flip_D3_F3_ValuesOnly() {
  top_rows += turn * 11796480ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 19791209300130ULL;
  top_diag7s += turn * 3538944ULL;
  right_columns += turn * 2087354105856ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D3_F3() {
  Flip_D3_F3_ValuesOnly();
}

void Flip_D3_F3() {
  Flip_D3_F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_F3;
  flipped = true;
}

void Flip_D3_F3G3_ValuesOnly() {
  top_rows += turn * 12189696ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 1708641069564066ULL;
  top_diag7s += turn * 3538944ULL;
  right_columns += turn * 2087385956352ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D3_F3G3() {
  Flip_D3_F3G3_ValuesOnly();
}

void Flip_D3_F3G3() {
  Flip_D3_F3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_F3G3;
  flipped = true;
}

void Flip_D3E3_ValuesOnly() {
  top_rows += turn * 14155776ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 3539106ULL;
  top_diag7s += turn * 3539106ULL;
  right_columns += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D3E3() {
  Flip_D3E3_ValuesOnly();
}

void Flip_D3E3() {
  Flip_D3E3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3E3;
  flipped = true;
}

void Flip_D3E3_G3_ValuesOnly() {
  top_rows += turn * 14548992ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 1688849863803042ULL;
  top_diag7s += turn * 3539106ULL;
  right_columns += turn * 136796838713229312ULL;
  bottom_diag7s += turn * 2087354105856ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D3E3_G3() {
  Flip_D3E3_G3_ValuesOnly();
}

void Flip_D3E3_G3() {
  Flip_D3E3_G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3E3_G3;
  flipped = true;
}

void Flip_D3F3_ValuesOnly() {
  top_rows += turn * 15335424ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 19791212839074ULL;
  top_diag7s += turn * 3539106ULL;
  right_columns += turn * 136798926035484672ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D3F3() {
  Flip_D3F3_ValuesOnly();
}

void Flip_D3F3() {
  Flip_D3F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3F3;
  flipped = true;
}

void Flip_D3G3_ValuesOnly() {
  top_rows += turn * 15728640ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 1708641073103010ULL;
  top_diag7s += turn * 3539106ULL;
  right_columns += turn * 136798926067335168ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D3G3() {
  Flip_D3G3_ValuesOnly();
}

void Flip_D3G3() {
  Flip_D3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3G3;
  flipped = true;
}

void Flip_E3_ValuesOnly() {
  top_rows += turn * 3538944ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 136796838681378816ULL;
  disk_difference += turn * 2;
}

void UndoFlip_E3() {
  Flip_E3_ValuesOnly();
}

void Flip_E3() {
  Flip_E3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3;
  flipped = true;
}

void Flip_E3_G3_ValuesOnly() {
  top_rows += turn * 3932160ULL;
  top_diag9s += turn * 1688849863802880ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 136796838713229312ULL;
  bottom_diag7s += turn * 2087354105856ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E3_G3() {
  Flip_E3_G3_ValuesOnly();
}

void Flip_E3_G3() {
  Flip_E3_G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_G3;
  flipped = true;
}

void Flip_E3F3_ValuesOnly() {
  top_rows += turn * 4718592ULL;
  top_diag9s += turn * 19791212838912ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 136798926035484672ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E3F3() {
  Flip_E3F3_ValuesOnly();
}

void Flip_E3F3() {
  Flip_E3F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3F3;
  flipped = true;
}

void Flip_E3G3_ValuesOnly() {
  top_rows += turn * 5111808ULL;
  top_diag9s += turn * 1708641073102848ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 136798926067335168ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E3G3() {
  Flip_E3G3_ValuesOnly();
}

void Flip_E3G3() {
  Flip_E3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3G3;
  flipped = true;
}

void Flip_F3_ValuesOnly() {
  top_rows += turn * 1179648ULL;
  top_diag9s += turn * 19791209299968ULL;
  right_columns += turn * 2087354105856ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 2;
}

void UndoFlip_F3() {
  Flip_F3_ValuesOnly();
}

void Flip_F3() {
  Flip_F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3;
  flipped = true;
}

void Flip_F3G3_ValuesOnly() {
  top_rows += turn * 1572864ULL;
  top_diag9s += turn * 1708641069563904ULL;
  right_columns += turn * 2087385956352ULL;
  bottom_diag7s += turn * 136798926035484672ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F3G3() {
  Flip_F3G3_ValuesOnly();
}

void Flip_F3G3() {
  Flip_F3G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3G3;
  flipped = true;
}

void Flip_G3_ValuesOnly() {
  top_rows += turn * 393216ULL;
  top_diag9s += turn * 1688849860263936ULL;
  right_columns += turn * 31850496ULL;
  bottom_diag7s += turn * 2087354105856ULL;
  disk_difference += turn * 2;
}

void UndoFlip_G3() {
  Flip_G3_ValuesOnly();
}

void Flip_G3() {
  Flip_G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3;
  flipped = true;
}

void Flip_B4_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 695784701952ULL;
  top_diag7s += turn * 6597069766656ULL;
  bottom_diag9s += turn * 10616832ULL;
  disk_difference += turn * 2;
}

void UndoFlip_B4() {
  Flip_B4_ValuesOnly();
}

void Flip_B4() {
  Flip_B4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4;
  flipped = true;
}

void Flip_B4_D4_ValuesOnly() {
  top_rows += turn * 1620ULL;
  left_columns += turn * 695784702114ULL;
  top_diag7s += turn * 6597069766710ULL;
  bottom_diag9s += turn * 45598946237743104ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B4_D4() {
  Flip_B4_D4_ValuesOnly();
}

void Flip_B4_D4() {
  Flip_B4_D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D4;
  flipped = true;
}

void Flip_B4_D4E4_ValuesOnly() {
  top_rows += turn * 1674ULL;
  left_columns += turn * 695784702114ULL;
  top_diag9s += turn * 54ULL;
  top_diag7s += turn * 6597069766710ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 45598946237743104ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B4_D4E4() {
  Flip_B4_D4E4_ValuesOnly();
}

void Flip_B4_D4E4() {
  Flip_B4_D4E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D4E4;
  flipped = true;
}

void Flip_B4_D4F4_ValuesOnly() {
  top_rows += turn * 1692ULL;
  left_columns += turn * 695784702114ULL;
  top_diag9s += turn * 1179702ULL;
  top_diag7s += turn * 6597069766710ULL;
  right_columns += turn * 45599642011828224ULL;
  bottom_diag9s += turn * 45598946237743104ULL;
  bottom_diag7s += turn * 45599642011828224ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B4_D4F4() {
  Flip_B4_D4F4_ValuesOnly();
}

void Flip_B4_D4F4() {
  Flip_B4_D4F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D4F4;
  flipped = true;
}

void Flip_B4_D4G4_ValuesOnly() {
  top_rows += turn * 1698ULL;
  left_columns += turn * 695784702114ULL;
  top_diag9s += turn * 6597070946358ULL;
  top_diag7s += turn * 6597069766710ULL;
  right_columns += turn * 45599642022445056ULL;
  bottom_diag9s += turn * 45598946237743104ULL;
  bottom_diag7s += turn * 45599642022445056ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B4_D4G4() {
  Flip_B4_D4G4_ValuesOnly();
}

void Flip_B4_D4G4() {
  Flip_B4_D4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D4G4;
  flipped = true;
}

void Flip_B4C4_ValuesOnly() {
  top_rows += turn * 1944ULL;
  left_columns += turn * 695795318784ULL;
  top_diag7s += turn * 6597070946304ULL;
  bottom_diag9s += turn * 695795318784ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B4C4() {
  Flip_B4C4_ValuesOnly();
}

void Flip_B4C4() {
  Flip_B4C4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C4;
  flipped = true;
}

void Flip_B4C4_E4_ValuesOnly() {
  top_rows += turn * 1998ULL;
  left_columns += turn * 695795318784ULL;
  top_diag9s += turn * 54ULL;
  top_diag7s += turn * 6597070946304ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 695795318784ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B4C4_E4() {
  Flip_B4C4_E4_ValuesOnly();
}

void Flip_B4C4_E4() {
  Flip_B4C4_E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C4_E4;
  flipped = true;
}

void Flip_B4C4_E4F4_ValuesOnly() {
  top_rows += turn * 2016ULL;
  left_columns += turn * 695795318784ULL;
  top_diag9s += turn * 1179702ULL;
  top_diag7s += turn * 6597070946304ULL;
  right_columns += turn * 45599642011828224ULL;
  bottom_diag9s += turn * 695795318784ULL;
  bottom_diag7s += turn * 45599642011828224ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B4C4_E4F4() {
  Flip_B4C4_E4F4_ValuesOnly();
}

void Flip_B4C4_E4F4() {
  Flip_B4C4_E4F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C4_E4F4;
  flipped = true;
}

void Flip_B4C4_E4G4_ValuesOnly() {
  top_rows += turn * 2022ULL;
  left_columns += turn * 695795318784ULL;
  top_diag9s += turn * 6597070946358ULL;
  top_diag7s += turn * 6597070946304ULL;
  right_columns += turn * 45599642022445056ULL;
  bottom_diag9s += turn * 695795318784ULL;
  bottom_diag7s += turn * 45599642022445056ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B4C4_E4G4() {
  Flip_B4C4_E4G4_ValuesOnly();
}

void Flip_B4C4_E4G4() {
  Flip_B4C4_E4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C4_E4G4;
  flipped = true;
}

void Flip_B4D4_ValuesOnly() {
  top_rows += turn * 2106ULL;
  left_columns += turn * 695795318946ULL;
  top_diag7s += turn * 6597070946358ULL;
  bottom_diag9s += turn * 45599642022445056ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B4D4() {
  Flip_B4D4_ValuesOnly();
}

void Flip_B4D4() {
  Flip_B4D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4D4;
  flipped = true;
}

void Flip_B4D4_F4_ValuesOnly() {
  top_rows += turn * 2124ULL;
  left_columns += turn * 695795318946ULL;
  top_diag9s += turn * 1179648ULL;
  top_diag7s += turn * 6597070946358ULL;
  right_columns += turn * 695784701952ULL;
  bottom_diag9s += turn * 45599642022445056ULL;
  bottom_diag7s += turn * 695784701952ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B4D4_F4() {
  Flip_B4D4_F4_ValuesOnly();
}

void Flip_B4D4_F4() {
  Flip_B4D4_F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4D4_F4;
  flipped = true;
}

void Flip_B4D4_F4G4_ValuesOnly() {
  top_rows += turn * 2130ULL;
  left_columns += turn * 695795318946ULL;
  top_diag9s += turn * 6597070946304ULL;
  top_diag7s += turn * 6597070946358ULL;
  right_columns += turn * 695795318784ULL;
  bottom_diag9s += turn * 45599642022445056ULL;
  bottom_diag7s += turn * 695795318784ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B4D4_F4G4() {
  Flip_B4D4_F4G4_ValuesOnly();
}

void Flip_B4D4_F4G4() {
  Flip_B4D4_F4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4D4_F4G4;
  flipped = true;
}

void Flip_B4E4_ValuesOnly() {
  top_rows += turn * 2160ULL;
  left_columns += turn * 695795318946ULL;
  top_diag9s += turn * 54ULL;
  top_diag7s += turn * 6597070946358ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 45599642022445056ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B4E4() {
  Flip_B4E4_ValuesOnly();
}

void Flip_B4E4() {
  Flip_B4E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4E4;
  flipped = true;
}

void Flip_B4E4_G4_ValuesOnly() {
  top_rows += turn * 2166ULL;
  left_columns += turn * 695795318946ULL;
  top_diag9s += turn * 6597069766710ULL;
  top_diag7s += turn * 6597070946358ULL;
  right_columns += turn * 45598946237743104ULL;
  bottom_diag9s += turn * 45599642022445056ULL;
  bottom_diag7s += turn * 45598946237743104ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B4E4_G4() {
  Flip_B4E4_G4_ValuesOnly();
}

void Flip_B4E4_G4() {
  Flip_B4E4_G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4E4_G4;
  flipped = true;
}

void Flip_B4F4_ValuesOnly() {
  top_rows += turn * 2178ULL;
  left_columns += turn * 695795318946ULL;
  top_diag9s += turn * 1179702ULL;
  top_diag7s += turn * 6597070946358ULL;
  right_columns += turn * 45599642011828224ULL;
  bottom_diag9s += turn * 45599642022445056ULL;
  bottom_diag7s += turn * 45599642011828224ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B4F4() {
  Flip_B4F4_ValuesOnly();
}

void Flip_B4F4() {
  Flip_B4F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4F4;
  flipped = true;
}

void Flip_B4G4_ValuesOnly() {
  top_rows += turn * 2184ULL;
  left_columns += turn * 695795318946ULL;
  top_diag9s += turn * 6597070946358ULL;
  top_diag7s += turn * 6597070946358ULL;
  right_columns += turn * 45599642022445056ULL;
  bottom_diag9s += turn * 45599642022445056ULL;
  bottom_diag7s += turn * 45599642022445056ULL;
  disk_difference += turn * 12;
}

void UndoFlip_B4G4() {
  Flip_B4G4_ValuesOnly();
}

void Flip_B4G4() {
  Flip_B4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4G4;
  flipped = true;
}

void Flip_C4_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 10616832ULL;
  top_diag7s += turn * 1179648ULL;
  bottom_diag9s += turn * 695784701952ULL;
  disk_difference += turn * 2;
}

void UndoFlip_C4() {
  Flip_C4_ValuesOnly();
}

void Flip_C4() {
  Flip_C4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4;
  flipped = true;
}

void Flip_C4_E4_ValuesOnly() {
  top_rows += turn * 540ULL;
  left_columns += turn * 10616832ULL;
  top_diag9s += turn * 54ULL;
  top_diag7s += turn * 1179648ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 695784701952ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C4_E4() {
  Flip_C4_E4_ValuesOnly();
}

void Flip_C4_E4() {
  Flip_C4_E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_E4;
  flipped = true;
}

void Flip_C4_E4F4_ValuesOnly() {
  top_rows += turn * 558ULL;
  left_columns += turn * 10616832ULL;
  top_diag9s += turn * 1179702ULL;
  top_diag7s += turn * 1179648ULL;
  right_columns += turn * 45599642011828224ULL;
  bottom_diag9s += turn * 695784701952ULL;
  bottom_diag7s += turn * 45599642011828224ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C4_E4F4() {
  Flip_C4_E4F4_ValuesOnly();
}

void Flip_C4_E4F4() {
  Flip_C4_E4F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_E4F4;
  flipped = true;
}

void Flip_C4_E4G4_ValuesOnly() {
  top_rows += turn * 564ULL;
  left_columns += turn * 10616832ULL;
  top_diag9s += turn * 6597070946358ULL;
  top_diag7s += turn * 1179648ULL;
  right_columns += turn * 45599642022445056ULL;
  bottom_diag9s += turn * 695784701952ULL;
  bottom_diag7s += turn * 45599642022445056ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C4_E4G4() {
  Flip_C4_E4G4_ValuesOnly();
}

void Flip_C4_E4G4() {
  Flip_C4_E4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_E4G4;
  flipped = true;
}

void Flip_C4D4_ValuesOnly() {
  top_rows += turn * 648ULL;
  left_columns += turn * 10616994ULL;
  top_diag7s += turn * 1179702ULL;
  bottom_diag9s += turn * 45599642011828224ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C4D4() {
  Flip_C4D4_ValuesOnly();
}

void Flip_C4D4() {
  Flip_C4D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4D4;
  flipped = true;
}

void Flip_C4D4_F4_ValuesOnly() {
  top_rows += turn * 666ULL;
  left_columns += turn * 10616994ULL;
  top_diag9s += turn * 1179648ULL;
  top_diag7s += turn * 1179702ULL;
  right_columns += turn * 695784701952ULL;
  bottom_diag9s += turn * 45599642011828224ULL;
  bottom_diag7s += turn * 695784701952ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C4D4_F4() {
  Flip_C4D4_F4_ValuesOnly();
}

void Flip_C4D4_F4() {
  Flip_C4D4_F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4D4_F4;
  flipped = true;
}

void Flip_C4D4_F4G4_ValuesOnly() {
  top_rows += turn * 672ULL;
  left_columns += turn * 10616994ULL;
  top_diag9s += turn * 6597070946304ULL;
  top_diag7s += turn * 1179702ULL;
  right_columns += turn * 695795318784ULL;
  bottom_diag9s += turn * 45599642011828224ULL;
  bottom_diag7s += turn * 695795318784ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C4D4_F4G4() {
  Flip_C4D4_F4G4_ValuesOnly();
}

void Flip_C4D4_F4G4() {
  Flip_C4D4_F4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4D4_F4G4;
  flipped = true;
}

void Flip_C4E4_ValuesOnly() {
  top_rows += turn * 702ULL;
  left_columns += turn * 10616994ULL;
  top_diag9s += turn * 54ULL;
  top_diag7s += turn * 1179702ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 45599642011828224ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C4E4() {
  Flip_C4E4_ValuesOnly();
}

void Flip_C4E4() {
  Flip_C4E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4E4;
  flipped = true;
}

void Flip_C4E4_G4_ValuesOnly() {
  top_rows += turn * 708ULL;
  left_columns += turn * 10616994ULL;
  top_diag9s += turn * 6597069766710ULL;
  top_diag7s += turn * 1179702ULL;
  right_columns += turn * 45598946237743104ULL;
  bottom_diag9s += turn * 45599642011828224ULL;
  bottom_diag7s += turn * 45598946237743104ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C4E4_G4() {
  Flip_C4E4_G4_ValuesOnly();
}

void Flip_C4E4_G4() {
  Flip_C4E4_G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4E4_G4;
  flipped = true;
}

void Flip_C4F4_ValuesOnly() {
  top_rows += turn * 720ULL;
  left_columns += turn * 10616994ULL;
  top_diag9s += turn * 1179702ULL;
  top_diag7s += turn * 1179702ULL;
  right_columns += turn * 45599642011828224ULL;
  bottom_diag9s += turn * 45599642011828224ULL;
  bottom_diag7s += turn * 45599642011828224ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C4F4() {
  Flip_C4F4_ValuesOnly();
}

void Flip_C4F4() {
  Flip_C4F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4F4;
  flipped = true;
}

void Flip_C4G4_ValuesOnly() {
  top_rows += turn * 726ULL;
  left_columns += turn * 10616994ULL;
  top_diag9s += turn * 6597070946358ULL;
  top_diag7s += turn * 1179702ULL;
  right_columns += turn * 45599642022445056ULL;
  bottom_diag9s += turn * 45599642011828224ULL;
  bottom_diag7s += turn * 45599642022445056ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C4G4() {
  Flip_C4G4_ValuesOnly();
}

void Flip_C4G4() {
  Flip_C4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4G4;
  flipped = true;
}

void Flip_D4_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 162ULL;
  top_diag7s += turn * 54ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  disk_difference += turn * 2;
}

void UndoFlip_D4() {
  Flip_D4_ValuesOnly();
}

void Flip_D4() {
  Flip_D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4;
  flipped = true;
}

void Flip_D4_F4_ValuesOnly() {
  top_rows += turn * 180ULL;
  left_columns += turn * 162ULL;
  top_diag9s += turn * 1179648ULL;
  top_diag7s += turn * 54ULL;
  right_columns += turn * 695784701952ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  bottom_diag7s += turn * 695784701952ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D4_F4() {
  Flip_D4_F4_ValuesOnly();
}

void Flip_D4_F4() {
  Flip_D4_F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_F4;
  flipped = true;
}

void Flip_D4_F4G4_ValuesOnly() {
  top_rows += turn * 186ULL;
  left_columns += turn * 162ULL;
  top_diag9s += turn * 6597070946304ULL;
  top_diag7s += turn * 54ULL;
  right_columns += turn * 695795318784ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  bottom_diag7s += turn * 695795318784ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D4_F4G4() {
  Flip_D4_F4G4_ValuesOnly();
}

void Flip_D4_F4G4() {
  Flip_D4_F4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_F4G4;
  flipped = true;
}

void Flip_D4E4_ValuesOnly() {
  top_rows += turn * 216ULL;
  left_columns += turn * 162ULL;
  top_diag9s += turn * 54ULL;
  top_diag7s += turn * 54ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D4E4() {
  Flip_D4E4_ValuesOnly();
}

void Flip_D4E4() {
  Flip_D4E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4E4;
  flipped = true;
}

void Flip_D4E4_G4_ValuesOnly() {
  top_rows += turn * 222ULL;
  left_columns += turn * 162ULL;
  top_diag9s += turn * 6597069766710ULL;
  top_diag7s += turn * 54ULL;
  right_columns += turn * 45598946237743104ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  bottom_diag7s += turn * 45598946237743104ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D4E4_G4() {
  Flip_D4E4_G4_ValuesOnly();
}

void Flip_D4E4_G4() {
  Flip_D4E4_G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4E4_G4;
  flipped = true;
}

void Flip_D4F4_ValuesOnly() {
  top_rows += turn * 234ULL;
  left_columns += turn * 162ULL;
  top_diag9s += turn * 1179702ULL;
  top_diag7s += turn * 54ULL;
  right_columns += turn * 45599642011828224ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  bottom_diag7s += turn * 45599642011828224ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D4F4() {
  Flip_D4F4_ValuesOnly();
}

void Flip_D4F4() {
  Flip_D4F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4F4;
  flipped = true;
}

void Flip_D4G4_ValuesOnly() {
  top_rows += turn * 240ULL;
  left_columns += turn * 162ULL;
  top_diag9s += turn * 6597070946358ULL;
  top_diag7s += turn * 54ULL;
  right_columns += turn * 45599642022445056ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  bottom_diag7s += turn * 45599642022445056ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D4G4() {
  Flip_D4G4_ValuesOnly();
}

void Flip_D4G4() {
  Flip_D4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4G4;
  flipped = true;
}

void Flip_E4_ValuesOnly() {
  top_rows += turn * 54ULL;
  top_diag9s += turn * 54ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 2;
}

void UndoFlip_E4() {
  Flip_E4_ValuesOnly();
}

void Flip_E4() {
  Flip_E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4;
  flipped = true;
}

void Flip_E4_G4_ValuesOnly() {
  top_rows += turn * 60ULL;
  top_diag9s += turn * 6597069766710ULL;
  right_columns += turn * 45598946237743104ULL;
  bottom_diag7s += turn * 45598946237743104ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E4_G4() {
  Flip_E4_G4_ValuesOnly();
}

void Flip_E4_G4() {
  Flip_E4_G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_G4;
  flipped = true;
}

void Flip_E4F4_ValuesOnly() {
  top_rows += turn * 72ULL;
  top_diag9s += turn * 1179702ULL;
  right_columns += turn * 45599642011828224ULL;
  bottom_diag7s += turn * 45599642011828224ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E4F4() {
  Flip_E4F4_ValuesOnly();
}

void Flip_E4F4() {
  Flip_E4F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4F4;
  flipped = true;
}

void Flip_E4G4_ValuesOnly() {
  top_rows += turn * 78ULL;
  top_diag9s += turn * 6597070946358ULL;
  right_columns += turn * 45599642022445056ULL;
  bottom_diag7s += turn * 45599642022445056ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E4G4() {
  Flip_E4G4_ValuesOnly();
}

void Flip_E4G4() {
  Flip_E4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4G4;
  flipped = true;
}

void Flip_F4_ValuesOnly() {
  top_rows += turn * 18ULL;
  top_diag9s += turn * 1179648ULL;
  right_columns += turn * 695784701952ULL;
  bottom_diag7s += turn * 695784701952ULL;
  disk_difference += turn * 2;
}

void UndoFlip_F4() {
  Flip_F4_ValuesOnly();
}

void Flip_F4() {
  Flip_F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4;
  flipped = true;
}

void Flip_F4G4_ValuesOnly() {
  top_rows += turn * 24ULL;
  top_diag9s += turn * 6597070946304ULL;
  right_columns += turn * 695795318784ULL;
  bottom_diag7s += turn * 695795318784ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F4G4() {
  Flip_F4G4_ValuesOnly();
}

void Flip_F4G4() {
  Flip_F4G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4G4;
  flipped = true;
}

void Flip_G4_ValuesOnly() {
  top_rows += turn * 6ULL;
  top_diag9s += turn * 6597069766656ULL;
  right_columns += turn * 10616832ULL;
  bottom_diag7s += turn * 10616832ULL;
  disk_difference += turn * 2;
}

void UndoFlip_G4() {
  Flip_G4_ValuesOnly();
}

void Flip_G4() {
  Flip_G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4;
  flipped = true;
}

void Flip_B5_ValuesOnly() {
  left_columns += turn * 231928233984ULL;
  top_diag7s += turn * 393216ULL;
  bottom_diag9s += turn * 13824ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 2;
}

void UndoFlip_B5() {
  Flip_B5_ValuesOnly();
}

void Flip_B5() {
  Flip_B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5;
  flipped = true;
}

void Flip_B5_D5_ValuesOnly() {
  left_columns += turn * 231928234038ULL;
  top_diag7s += turn * 393216ULL;
  bottom_diag9s += turn * 231928247808ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 455989462271262720ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B5_D5() {
  Flip_B5_D5_ValuesOnly();
}

void Flip_B5_D5() {
  Flip_B5_D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_D5;
  flipped = true;
}

void Flip_B5_D5E5_ValuesOnly() {
  left_columns += turn * 231928234038ULL;
  top_diag7s += turn * 393216ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 15199880670623232ULL;
  bottom_diag7s += turn * 15199880670609408ULL;
  bottom_rows += turn * 471189111013638144ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B5_D5E5() {
  Flip_B5_D5E5_ValuesOnly();
}

void Flip_B5_D5E5() {
  Flip_B5_D5E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_D5E5;
  flipped = true;
}

void Flip_B5_D5F5_ValuesOnly() {
  left_columns += turn * 231928234038ULL;
  top_diag9s += turn * 18ULL;
  top_diag7s += turn * 393216ULL;
  right_columns += turn * 15199880670609408ULL;
  bottom_diag9s += turn * 15199880670623232ULL;
  bottom_diag7s += turn * 15199880674148352ULL;
  bottom_rows += turn * 476255660594429952ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B5_D5F5() {
  Flip_B5_D5F5_ValuesOnly();
}

void Flip_B5_D5F5() {
  Flip_B5_D5F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_D5F5;
  flipped = true;
}

void Flip_B5_D5G5_ValuesOnly() {
  left_columns += turn * 231928234038ULL;
  top_diag9s += turn * 393234ULL;
  top_diag7s += turn * 393216ULL;
  right_columns += turn * 15199880674148352ULL;
  bottom_diag9s += turn * 15199880670623232ULL;
  bottom_diag7s += turn * 15199880674162176ULL;
  bottom_rows += turn * 477944510454693888ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B5_D5G5() {
  Flip_B5_D5G5_ValuesOnly();
}

void Flip_B5_D5G5() {
  Flip_B5_D5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_D5G5;
  flipped = true;
}

void Flip_B5C5_ValuesOnly() {
  left_columns += turn * 231931772928ULL;
  top_diag7s += turn * 393234ULL;
  bottom_diag9s += turn * 3552768ULL;
  bottom_rows += turn * 547187354725515264ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B5C5() {
  Flip_B5C5_ValuesOnly();
}

void Flip_B5C5() {
  Flip_B5C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5C5;
  flipped = true;
}

void Flip_B5C5_E5_ValuesOnly() {
  left_columns += turn * 231931772928ULL;
  top_diag7s += turn * 393234ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 15199648745928192ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 562387003467890688ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B5C5_E5() {
  Flip_B5C5_E5_ValuesOnly();
}

void Flip_B5C5_E5() {
  Flip_B5C5_E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5C5_E5;
  flipped = true;
}

void Flip_B5C5_E5F5_ValuesOnly() {
  left_columns += turn * 231931772928ULL;
  top_diag9s += turn * 18ULL;
  top_diag7s += turn * 393234ULL;
  right_columns += turn * 15199880670609408ULL;
  bottom_diag9s += turn * 15199648745928192ULL;
  bottom_diag7s += turn * 231931772928ULL;
  bottom_rows += turn * 567453553048682496ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B5C5_E5F5() {
  Flip_B5C5_E5F5_ValuesOnly();
}

void Flip_B5C5_E5F5() {
  Flip_B5C5_E5F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5C5_E5F5;
  flipped = true;
}

void Flip_B5C5_E5G5_ValuesOnly() {
  left_columns += turn * 231931772928ULL;
  top_diag9s += turn * 393234ULL;
  top_diag7s += turn * 393234ULL;
  right_columns += turn * 15199880674148352ULL;
  bottom_diag9s += turn * 15199648745928192ULL;
  bottom_diag7s += turn * 231931786752ULL;
  bottom_rows += turn * 569142402908946432ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B5C5_E5G5() {
  Flip_B5C5_E5G5_ValuesOnly();
}

void Flip_B5C5_E5G5() {
  Flip_B5C5_E5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5C5_E5G5;
  flipped = true;
}

void Flip_B5D5_ValuesOnly() {
  left_columns += turn * 231931772982ULL;
  top_diag7s += turn * 393234ULL;
  bottom_diag9s += turn * 231931786752ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 592786300952641536ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B5D5() {
  Flip_B5D5_ValuesOnly();
}

void Flip_B5D5() {
  Flip_B5D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5D5;
  flipped = true;
}

void Flip_B5D5_F5_ValuesOnly() {
  left_columns += turn * 231931772982ULL;
  top_diag9s += turn * 18ULL;
  top_diag7s += turn * 393234ULL;
  right_columns += turn * 231928233984ULL;
  bottom_diag9s += turn * 231931786752ULL;
  bottom_diag7s += turn * 15199648745914368ULL;
  bottom_rows += turn * 597852850533433344ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B5D5_F5() {
  Flip_B5D5_F5_ValuesOnly();
}

void Flip_B5D5_F5() {
  Flip_B5D5_F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5D5_F5;
  flipped = true;
}

void Flip_B5D5_F5G5_ValuesOnly() {
  left_columns += turn * 231931772982ULL;
  top_diag9s += turn * 393234ULL;
  top_diag7s += turn * 393234ULL;
  right_columns += turn * 231931772928ULL;
  bottom_diag9s += turn * 231931786752ULL;
  bottom_diag7s += turn * 15199648745928192ULL;
  bottom_rows += turn * 599541700393697280ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B5D5_F5G5() {
  Flip_B5D5_F5G5_ValuesOnly();
}

void Flip_B5D5_F5G5() {
  Flip_B5D5_F5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5D5_F5G5;
  flipped = true;
}

void Flip_B5E5_ValuesOnly() {
  left_columns += turn * 231931772982ULL;
  top_diag7s += turn * 393234ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 15199880674162176ULL;
  bottom_diag7s += turn * 15199880670609408ULL;
  bottom_rows += turn * 607985949695016960ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B5E5() {
  Flip_B5E5_ValuesOnly();
}

void Flip_B5E5() {
  Flip_B5E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5E5;
  flipped = true;
}

void Flip_B5E5_G5_ValuesOnly() {
  left_columns += turn * 231931772982ULL;
  top_diag9s += turn * 393216ULL;
  top_diag7s += turn * 393234ULL;
  right_columns += turn * 15199648745914368ULL;
  bottom_diag9s += turn * 15199880674162176ULL;
  bottom_diag7s += turn * 15199880670623232ULL;
  bottom_rows += turn * 609674799555280896ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B5E5_G5() {
  Flip_B5E5_G5_ValuesOnly();
}

void Flip_B5E5_G5() {
  Flip_B5E5_G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5E5_G5;
  flipped = true;
}

void Flip_B5F5_ValuesOnly() {
  left_columns += turn * 231931772982ULL;
  top_diag9s += turn * 18ULL;
  top_diag7s += turn * 393234ULL;
  right_columns += turn * 15199880670609408ULL;
  bottom_diag9s += turn * 15199880674162176ULL;
  bottom_diag7s += turn * 15199880674148352ULL;
  bottom_rows += turn * 613052499275808768ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B5F5() {
  Flip_B5F5_ValuesOnly();
}

void Flip_B5F5() {
  Flip_B5F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5F5;
  flipped = true;
}

void Flip_B5G5_ValuesOnly() {
  left_columns += turn * 231931772982ULL;
  top_diag9s += turn * 393234ULL;
  top_diag7s += turn * 393234ULL;
  right_columns += turn * 15199880674148352ULL;
  bottom_diag9s += turn * 15199880674162176ULL;
  bottom_diag7s += turn * 15199880674162176ULL;
  bottom_rows += turn * 614741349136072704ULL;
  disk_difference += turn * 12;
}

void UndoFlip_B5G5() {
  Flip_B5G5_ValuesOnly();
}

void Flip_B5G5() {
  Flip_B5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5G5;
  flipped = true;
}

void Flip_C5_ValuesOnly() {
  left_columns += turn * 3538944ULL;
  top_diag7s += turn * 18ULL;
  bottom_diag9s += turn * 3538944ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 2;
}

void UndoFlip_C5() {
  Flip_C5_ValuesOnly();
}

void Flip_C5() {
  Flip_C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5;
  flipped = true;
}

void Flip_C5_E5_ValuesOnly() {
  left_columns += turn * 3538944ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 15199648745914368ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 151996487423754240ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C5_E5() {
  Flip_C5_E5_ValuesOnly();
}

void Flip_C5_E5() {
  Flip_C5_E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5_E5;
  flipped = true;
}

void Flip_C5_E5F5_ValuesOnly() {
  left_columns += turn * 3538944ULL;
  top_diag9s += turn * 18ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 15199880670609408ULL;
  bottom_diag9s += turn * 15199648745914368ULL;
  bottom_diag7s += turn * 231931772928ULL;
  bottom_rows += turn * 157063037004546048ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C5_E5F5() {
  Flip_C5_E5F5_ValuesOnly();
}

void Flip_C5_E5F5() {
  Flip_C5_E5F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5_E5F5;
  flipped = true;
}

void Flip_C5_E5G5_ValuesOnly() {
  left_columns += turn * 3538944ULL;
  top_diag9s += turn * 393234ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 15199880674148352ULL;
  bottom_diag9s += turn * 15199648745914368ULL;
  bottom_diag7s += turn * 231931786752ULL;
  bottom_rows += turn * 158751886864809984ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C5_E5G5() {
  Flip_C5_E5G5_ValuesOnly();
}

void Flip_C5_E5G5() {
  Flip_C5_E5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5_E5G5;
  flipped = true;
}

void Flip_C5D5_ValuesOnly() {
  left_columns += turn * 3538998ULL;
  top_diag7s += turn * 18ULL;
  bottom_diag9s += turn * 231931772928ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 182395784908505088ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C5D5() {
  Flip_C5D5_ValuesOnly();
}

void Flip_C5D5() {
  Flip_C5D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5D5;
  flipped = true;
}

void Flip_C5D5_F5_ValuesOnly() {
  left_columns += turn * 3538998ULL;
  top_diag9s += turn * 18ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 231928233984ULL;
  bottom_diag9s += turn * 231931772928ULL;
  bottom_diag7s += turn * 15199648745914368ULL;
  bottom_rows += turn * 187462334489296896ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C5D5_F5() {
  Flip_C5D5_F5_ValuesOnly();
}

void Flip_C5D5_F5() {
  Flip_C5D5_F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5D5_F5;
  flipped = true;
}

void Flip_C5D5_F5G5_ValuesOnly() {
  left_columns += turn * 3538998ULL;
  top_diag9s += turn * 393234ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 231931772928ULL;
  bottom_diag9s += turn * 231931772928ULL;
  bottom_diag7s += turn * 15199648745928192ULL;
  bottom_rows += turn * 189151184349560832ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C5D5_F5G5() {
  Flip_C5D5_F5G5_ValuesOnly();
}

void Flip_C5D5_F5G5() {
  Flip_C5D5_F5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5D5_F5G5;
  flipped = true;
}

void Flip_C5E5_ValuesOnly() {
  left_columns += turn * 3538998ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 15199880674148352ULL;
  bottom_diag7s += turn * 15199880670609408ULL;
  bottom_rows += turn * 197595433650880512ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C5E5() {
  Flip_C5E5_ValuesOnly();
}

void Flip_C5E5() {
  Flip_C5E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5E5;
  flipped = true;
}

void Flip_C5E5_G5_ValuesOnly() {
  left_columns += turn * 3538998ULL;
  top_diag9s += turn * 393216ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 15199648745914368ULL;
  bottom_diag9s += turn * 15199880674148352ULL;
  bottom_diag7s += turn * 15199880670623232ULL;
  bottom_rows += turn * 199284283511144448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C5E5_G5() {
  Flip_C5E5_G5_ValuesOnly();
}

void Flip_C5E5_G5() {
  Flip_C5E5_G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5E5_G5;
  flipped = true;
}

void Flip_C5F5_ValuesOnly() {
  left_columns += turn * 3538998ULL;
  top_diag9s += turn * 18ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 15199880670609408ULL;
  bottom_diag9s += turn * 15199880674148352ULL;
  bottom_diag7s += turn * 15199880674148352ULL;
  bottom_rows += turn * 202661983231672320ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C5F5() {
  Flip_C5F5_ValuesOnly();
}

void Flip_C5F5() {
  Flip_C5F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5F5;
  flipped = true;
}

void Flip_C5G5_ValuesOnly() {
  left_columns += turn * 3538998ULL;
  top_diag9s += turn * 393234ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 15199880674148352ULL;
  bottom_diag9s += turn * 15199880674148352ULL;
  bottom_diag7s += turn * 15199880674162176ULL;
  bottom_rows += turn * 204350833091936256ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C5G5() {
  Flip_C5G5_ValuesOnly();
}

void Flip_C5G5() {
  Flip_C5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5G5;
  flipped = true;
}

void Flip_D5_ValuesOnly() {
  left_columns += turn * 54ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 2;
}

void UndoFlip_D5() {
  Flip_D5_ValuesOnly();
}

void Flip_D5() {
  Flip_D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5;
  flipped = true;
}

void Flip_D5_F5_ValuesOnly() {
  left_columns += turn * 54ULL;
  top_diag9s += turn * 18ULL;
  right_columns += turn * 231928233984ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 15199648745914368ULL;
  bottom_rows += turn * 50665495807918080ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D5_F5() {
  Flip_D5_F5_ValuesOnly();
}

void Flip_D5_F5() {
  Flip_D5_F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5_F5;
  flipped = true;
}

void Flip_D5_F5G5_ValuesOnly() {
  left_columns += turn * 54ULL;
  top_diag9s += turn * 393234ULL;
  right_columns += turn * 231931772928ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 15199648745928192ULL;
  bottom_rows += turn * 52354345668182016ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D5_F5G5() {
  Flip_D5_F5G5_ValuesOnly();
}

void Flip_D5_F5G5() {
  Flip_D5_F5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5_F5G5;
  flipped = true;
}

void Flip_D5E5_ValuesOnly() {
  left_columns += turn * 54ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 15199880670609408ULL;
  bottom_diag7s += turn * 15199880670609408ULL;
  bottom_rows += turn * 60798594969501696ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D5E5() {
  Flip_D5E5_ValuesOnly();
}

void Flip_D5E5() {
  Flip_D5E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5E5;
  flipped = true;
}

void Flip_D5E5_G5_ValuesOnly() {
  left_columns += turn * 54ULL;
  top_diag9s += turn * 393216ULL;
  right_columns += turn * 15199648745914368ULL;
  bottom_diag9s += turn * 15199880670609408ULL;
  bottom_diag7s += turn * 15199880670623232ULL;
  bottom_rows += turn * 62487444829765632ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D5E5_G5() {
  Flip_D5E5_G5_ValuesOnly();
}

void Flip_D5E5_G5() {
  Flip_D5E5_G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5E5_G5;
  flipped = true;
}

void Flip_D5F5_ValuesOnly() {
  left_columns += turn * 54ULL;
  top_diag9s += turn * 18ULL;
  right_columns += turn * 15199880670609408ULL;
  bottom_diag9s += turn * 15199880670609408ULL;
  bottom_diag7s += turn * 15199880674148352ULL;
  bottom_rows += turn * 65865144550293504ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D5F5() {
  Flip_D5F5_ValuesOnly();
}

void Flip_D5F5() {
  Flip_D5F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5F5;
  flipped = true;
}

void Flip_D5G5_ValuesOnly() {
  left_columns += turn * 54ULL;
  top_diag9s += turn * 393234ULL;
  right_columns += turn * 15199880674148352ULL;
  bottom_diag9s += turn * 15199880670609408ULL;
  bottom_diag7s += turn * 15199880674162176ULL;
  bottom_rows += turn * 67553994410557440ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D5G5() {
  Flip_D5G5_ValuesOnly();
}

void Flip_D5G5() {
  Flip_D5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5G5;
  flipped = true;
}

void Flip_E5_ValuesOnly() {
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 2;
}

void UndoFlip_E5() {
  Flip_E5_ValuesOnly();
}

void Flip_E5() {
  Flip_E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5;
  flipped = true;
}

void Flip_E5_G5_ValuesOnly() {
  top_diag9s += turn * 393216ULL;
  right_columns += turn * 15199648745914368ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 231928247808ULL;
  bottom_rows += turn * 16888498602639360ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E5_G5() {
  Flip_E5_G5_ValuesOnly();
}

void Flip_E5_G5() {
  Flip_E5_G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5_G5;
  flipped = true;
}

void Flip_E5F5_ValuesOnly() {
  top_diag9s += turn * 18ULL;
  right_columns += turn * 15199880670609408ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 231931772928ULL;
  bottom_rows += turn * 20266198323167232ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E5F5() {
  Flip_E5F5_ValuesOnly();
}

void Flip_E5F5() {
  Flip_E5F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5F5;
  flipped = true;
}

void Flip_E5G5_ValuesOnly() {
  top_diag9s += turn * 393234ULL;
  right_columns += turn * 15199880674148352ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 231931786752ULL;
  bottom_rows += turn * 21955048183431168ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E5G5() {
  Flip_E5G5_ValuesOnly();
}

void Flip_E5G5() {
  Flip_E5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5G5;
  flipped = true;
}

void Flip_F5_ValuesOnly() {
  top_diag9s += turn * 18ULL;
  right_columns += turn * 231928233984ULL;
  bottom_diag7s += turn * 3538944ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 2;
}

void UndoFlip_F5() {
  Flip_F5_ValuesOnly();
}

void Flip_F5() {
  Flip_F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5;
  flipped = true;
}

void Flip_F5G5_ValuesOnly() {
  top_diag9s += turn * 393234ULL;
  right_columns += turn * 231931772928ULL;
  bottom_diag7s += turn * 3552768ULL;
  bottom_rows += turn * 6755399441055744ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F5G5() {
  Flip_F5G5_ValuesOnly();
}

void Flip_F5G5() {
  Flip_F5G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5G5;
  flipped = true;
}

void Flip_G5_ValuesOnly() {
  top_diag9s += turn * 393216ULL;
  right_columns += turn * 3538944ULL;
  bottom_diag7s += turn * 13824ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 2;
}

void UndoFlip_G5() {
  Flip_G5_ValuesOnly();
}

void Flip_G5() {
  Flip_G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5;
  flipped = true;
}

void Flip_B6_ValuesOnly() {
  left_columns += turn * 77309411328ULL;
  top_diag7s += turn * 6ULL;
  bottom_diag9s += turn * 18ULL;
  bottom_rows += turn * 6262062317568ULL;
  disk_difference += turn * 2;
}

void UndoFlip_B6() {
  Flip_B6_ValuesOnly();
}

void Flip_B6() {
  Flip_B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6;
  flipped = true;
}

void Flip_B6_D6_ValuesOnly() {
  left_columns += turn * 77309411346ULL;
  top_diag7s += turn * 6ULL;
  bottom_diag9s += turn * 1179666ULL;
  bottom_diag7s += turn * 77309411328ULL;
  bottom_rows += turn * 6957847019520ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B6_D6() {
  Flip_B6_D6_ValuesOnly();
}

void Flip_B6_D6() {
  Flip_B6_D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6_D6;
  flipped = true;
}

void Flip_B6_D6E6_ValuesOnly() {
  left_columns += turn * 77309411346ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 77310590994ULL;
  bottom_diag7s += turn * 77310590976ULL;
  bottom_rows += turn * 7189775253504ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B6_D6E6() {
  Flip_B6_D6E6_ValuesOnly();
}

void Flip_B6_D6E6() {
  Flip_B6_D6E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6_D6E6;
  flipped = true;
}

void Flip_B6_D6F6_ValuesOnly() {
  left_columns += turn * 77309411346ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 5066626890203136ULL;
  bottom_diag9s += turn * 5066626891382802ULL;
  bottom_diag7s += turn * 77310595584ULL;
  bottom_rows += turn * 7267084664832ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B6_D6F6() {
  Flip_B6_D6F6_ValuesOnly();
}

void Flip_B6_D6F6() {
  Flip_B6_D6F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6_D6F6;
  flipped = true;
}

void Flip_B6_D6G6_ValuesOnly() {
  left_columns += turn * 77309411346ULL;
  top_diag9s += turn * 6ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 5066626891382784ULL;
  bottom_diag9s += turn * 5066626891382802ULL;
  bottom_diag7s += turn * 77310595602ULL;
  bottom_rows += turn * 7292854468608ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B6_D6G6() {
  Flip_B6_D6G6_ValuesOnly();
}

void Flip_B6_D6G6() {
  Flip_B6_D6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6_D6G6;
  flipped = true;
}

void Flip_B6C6_ValuesOnly() {
  left_columns += turn * 77310590976ULL;
  top_diag7s += turn * 6ULL;
  bottom_diag9s += turn * 4626ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 8349416423424ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B6C6() {
  Flip_B6C6_ValuesOnly();
}

void Flip_B6C6() {
  Flip_B6C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6C6;
  flipped = true;
}

void Flip_B6C6_E6_ValuesOnly() {
  left_columns += turn * 77310590976ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 77309415954ULL;
  bottom_diag7s += turn * 5066549581971456ULL;
  bottom_rows += turn * 8581344657408ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B6C6_E6() {
  Flip_B6C6_E6_ValuesOnly();
}

void Flip_B6C6_E6() {
  Flip_B6C6_E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6C6_E6;
  flipped = true;
}

void Flip_B6C6_E6F6_ValuesOnly() {
  left_columns += turn * 77310590976ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 5066626890203136ULL;
  bottom_diag9s += turn * 5066626890207762ULL;
  bottom_diag7s += turn * 5066549581976064ULL;
  bottom_rows += turn * 8658654068736ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B6C6_E6F6() {
  Flip_B6C6_E6F6_ValuesOnly();
}

void Flip_B6C6_E6F6() {
  Flip_B6C6_E6F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6C6_E6F6;
  flipped = true;
}

void Flip_B6C6_E6G6_ValuesOnly() {
  left_columns += turn * 77310590976ULL;
  top_diag9s += turn * 6ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 5066626891382784ULL;
  bottom_diag9s += turn * 5066626890207762ULL;
  bottom_diag7s += turn * 5066549581976082ULL;
  bottom_rows += turn * 8684423872512ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B6C6_E6G6() {
  Flip_B6C6_E6G6_ValuesOnly();
}

void Flip_B6C6_E6G6() {
  Flip_B6C6_E6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6C6_E6G6;
  flipped = true;
}

void Flip_B6D6_ValuesOnly() {
  left_columns += turn * 77310590994ULL;
  top_diag7s += turn * 6ULL;
  bottom_diag9s += turn * 1184274ULL;
  bottom_diag7s += turn * 5066626890203136ULL;
  bottom_rows += turn * 9045201125376ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B6D6() {
  Flip_B6D6_ValuesOnly();
}

void Flip_B6D6() {
  Flip_B6D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6D6;
  flipped = true;
}

void Flip_B6D6_F6_ValuesOnly() {
  left_columns += turn * 77310590994ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 77309411328ULL;
  bottom_diag9s += turn * 5066549581976082ULL;
  bottom_diag7s += turn * 5066626890207744ULL;
  bottom_rows += turn * 9122510536704ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B6D6_F6() {
  Flip_B6D6_F6_ValuesOnly();
}

void Flip_B6D6_F6() {
  Flip_B6D6_F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6D6_F6;
  flipped = true;
}

void Flip_B6D6_F6G6_ValuesOnly() {
  left_columns += turn * 77310590994ULL;
  top_diag9s += turn * 6ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 77310590976ULL;
  bottom_diag9s += turn * 5066549581976082ULL;
  bottom_diag7s += turn * 5066626890207762ULL;
  bottom_rows += turn * 9148280340480ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B6D6_F6G6() {
  Flip_B6D6_F6G6_ValuesOnly();
}

void Flip_B6D6_F6G6() {
  Flip_B6D6_F6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6D6_F6G6;
  flipped = true;
}

void Flip_B6E6_ValuesOnly() {
  left_columns += turn * 77310590994ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 77310595602ULL;
  bottom_diag7s += turn * 5066626891382784ULL;
  bottom_rows += turn * 9277129359360ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B6E6() {
  Flip_B6E6_ValuesOnly();
}

void Flip_B6E6() {
  Flip_B6E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6E6;
  flipped = true;
}

void Flip_B6E6_G6_ValuesOnly() {
  left_columns += turn * 77310590994ULL;
  top_diag9s += turn * 6ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 5066549581971456ULL;
  bottom_diag9s += turn * 77310595602ULL;
  bottom_diag7s += turn * 5066626891382802ULL;
  bottom_rows += turn * 9302899163136ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B6E6_G6() {
  Flip_B6E6_G6_ValuesOnly();
}

void Flip_B6E6_G6() {
  Flip_B6E6_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6E6_G6;
  flipped = true;
}

void Flip_B6F6_ValuesOnly() {
  left_columns += turn * 77310590994ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 5066626890203136ULL;
  bottom_diag9s += turn * 5066626891387410ULL;
  bottom_diag7s += turn * 5066626891387392ULL;
  bottom_rows += turn * 9354438770688ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B6F6() {
  Flip_B6F6_ValuesOnly();
}

void Flip_B6F6() {
  Flip_B6F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6F6;
  flipped = true;
}

void Flip_B6G6_ValuesOnly() {
  left_columns += turn * 77310590994ULL;
  top_diag9s += turn * 6ULL;
  top_diag7s += turn * 6ULL;
  right_columns += turn * 5066626891382784ULL;
  bottom_diag9s += turn * 5066626891387410ULL;
  bottom_diag7s += turn * 5066626891387410ULL;
  bottom_rows += turn * 9380208574464ULL;
  disk_difference += turn * 12;
}

void UndoFlip_B6G6() {
  Flip_B6G6_ValuesOnly();
}

void Flip_B6G6() {
  Flip_B6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6G6;
  flipped = true;
}

void Flip_C6_ValuesOnly() {
  left_columns += turn * 1179648ULL;
  bottom_diag9s += turn * 4608ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 2087354105856ULL;
  disk_difference += turn * 2;
}

void UndoFlip_C6() {
  Flip_C6_ValuesOnly();
}

void Flip_C6() {
  Flip_C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6;
  flipped = true;
}

void Flip_C6_E6_ValuesOnly() {
  left_columns += turn * 1179648ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 77309415936ULL;
  bottom_diag7s += turn * 5066549581971456ULL;
  bottom_rows += turn * 2319282339840ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C6_E6() {
  Flip_C6_E6_ValuesOnly();
}

void Flip_C6_E6() {
  Flip_C6_E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6_E6;
  flipped = true;
}

void Flip_C6_E6F6_ValuesOnly() {
  left_columns += turn * 1179648ULL;
  right_columns += turn * 5066626890203136ULL;
  bottom_diag9s += turn * 5066626890207744ULL;
  bottom_diag7s += turn * 5066549581976064ULL;
  bottom_rows += turn * 2396591751168ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C6_E6F6() {
  Flip_C6_E6F6_ValuesOnly();
}

void Flip_C6_E6F6() {
  Flip_C6_E6F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6_E6F6;
  flipped = true;
}

void Flip_C6_E6G6_ValuesOnly() {
  left_columns += turn * 1179648ULL;
  top_diag9s += turn * 6ULL;
  right_columns += turn * 5066626891382784ULL;
  bottom_diag9s += turn * 5066626890207744ULL;
  bottom_diag7s += turn * 5066549581976082ULL;
  bottom_rows += turn * 2422361554944ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C6_E6G6() {
  Flip_C6_E6G6_ValuesOnly();
}

void Flip_C6_E6G6() {
  Flip_C6_E6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6_E6G6;
  flipped = true;
}

void Flip_C6D6_ValuesOnly() {
  left_columns += turn * 1179666ULL;
  bottom_diag9s += turn * 1184256ULL;
  bottom_diag7s += turn * 5066626890203136ULL;
  bottom_rows += turn * 2783138807808ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C6D6() {
  Flip_C6D6_ValuesOnly();
}

void Flip_C6D6() {
  Flip_C6D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6D6;
  flipped = true;
}

void Flip_C6D6_F6_ValuesOnly() {
  left_columns += turn * 1179666ULL;
  right_columns += turn * 77309411328ULL;
  bottom_diag9s += turn * 5066549581976064ULL;
  bottom_diag7s += turn * 5066626890207744ULL;
  bottom_rows += turn * 2860448219136ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C6D6_F6() {
  Flip_C6D6_F6_ValuesOnly();
}

void Flip_C6D6_F6() {
  Flip_C6D6_F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6D6_F6;
  flipped = true;
}

void Flip_C6D6_F6G6_ValuesOnly() {
  left_columns += turn * 1179666ULL;
  top_diag9s += turn * 6ULL;
  right_columns += turn * 77310590976ULL;
  bottom_diag9s += turn * 5066549581976064ULL;
  bottom_diag7s += turn * 5066626890207762ULL;
  bottom_rows += turn * 2886218022912ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C6D6_F6G6() {
  Flip_C6D6_F6G6_ValuesOnly();
}

void Flip_C6D6_F6G6() {
  Flip_C6D6_F6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6D6_F6G6;
  flipped = true;
}

void Flip_C6E6_ValuesOnly() {
  left_columns += turn * 1179666ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 77310595584ULL;
  bottom_diag7s += turn * 5066626891382784ULL;
  bottom_rows += turn * 3015067041792ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C6E6() {
  Flip_C6E6_ValuesOnly();
}

void Flip_C6E6() {
  Flip_C6E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6E6;
  flipped = true;
}

void Flip_C6E6_G6_ValuesOnly() {
  left_columns += turn * 1179666ULL;
  top_diag9s += turn * 6ULL;
  right_columns += turn * 5066549581971456ULL;
  bottom_diag9s += turn * 77310595584ULL;
  bottom_diag7s += turn * 5066626891382802ULL;
  bottom_rows += turn * 3040836845568ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C6E6_G6() {
  Flip_C6E6_G6_ValuesOnly();
}

void Flip_C6E6_G6() {
  Flip_C6E6_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6E6_G6;
  flipped = true;
}

void Flip_C6F6_ValuesOnly() {
  left_columns += turn * 1179666ULL;
  right_columns += turn * 5066626890203136ULL;
  bottom_diag9s += turn * 5066626891387392ULL;
  bottom_diag7s += turn * 5066626891387392ULL;
  bottom_rows += turn * 3092376453120ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C6F6() {
  Flip_C6F6_ValuesOnly();
}

void Flip_C6F6() {
  Flip_C6F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6F6;
  flipped = true;
}

void Flip_C6G6_ValuesOnly() {
  left_columns += turn * 1179666ULL;
  top_diag9s += turn * 6ULL;
  right_columns += turn * 5066626891382784ULL;
  bottom_diag9s += turn * 5066626891387392ULL;
  bottom_diag7s += turn * 5066626891387410ULL;
  bottom_rows += turn * 3118146256896ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C6G6() {
  Flip_C6G6_ValuesOnly();
}

void Flip_C6G6() {
  Flip_C6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6G6;
  flipped = true;
}

void Flip_D6_ValuesOnly() {
  left_columns += turn * 18ULL;
  bottom_diag9s += turn * 1179648ULL;
  bottom_diag7s += turn * 77309411328ULL;
  bottom_rows += turn * 695784701952ULL;
  disk_difference += turn * 2;
}

void UndoFlip_D6() {
  Flip_D6_ValuesOnly();
}

void Flip_D6() {
  Flip_D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6;
  flipped = true;
}

void Flip_D6_F6_ValuesOnly() {
  left_columns += turn * 18ULL;
  right_columns += turn * 77309411328ULL;
  bottom_diag9s += turn * 5066549581971456ULL;
  bottom_diag7s += turn * 77309415936ULL;
  bottom_rows += turn * 773094113280ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D6_F6() {
  Flip_D6_F6_ValuesOnly();
}

void Flip_D6_F6() {
  Flip_D6_F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6_F6;
  flipped = true;
}

void Flip_D6_F6G6_ValuesOnly() {
  left_columns += turn * 18ULL;
  top_diag9s += turn * 6ULL;
  right_columns += turn * 77310590976ULL;
  bottom_diag9s += turn * 5066549581971456ULL;
  bottom_diag7s += turn * 77309415954ULL;
  bottom_rows += turn * 798863917056ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D6_F6G6() {
  Flip_D6_F6G6_ValuesOnly();
}

void Flip_D6_F6G6() {
  Flip_D6_F6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6_F6G6;
  flipped = true;
}

void Flip_D6E6_ValuesOnly() {
  left_columns += turn * 18ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 77310590976ULL;
  bottom_diag7s += turn * 77310590976ULL;
  bottom_rows += turn * 927712935936ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D6E6() {
  Flip_D6E6_ValuesOnly();
}

void Flip_D6E6() {
  Flip_D6E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6E6;
  flipped = true;
}

void Flip_D6E6_G6_ValuesOnly() {
  left_columns += turn * 18ULL;
  top_diag9s += turn * 6ULL;
  right_columns += turn * 5066549581971456ULL;
  bottom_diag9s += turn * 77310590976ULL;
  bottom_diag7s += turn * 77310590994ULL;
  bottom_rows += turn * 953482739712ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D6E6_G6() {
  Flip_D6E6_G6_ValuesOnly();
}

void Flip_D6E6_G6() {
  Flip_D6E6_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6E6_G6;
  flipped = true;
}

void Flip_D6F6_ValuesOnly() {
  left_columns += turn * 18ULL;
  right_columns += turn * 5066626890203136ULL;
  bottom_diag9s += turn * 5066626891382784ULL;
  bottom_diag7s += turn * 77310595584ULL;
  bottom_rows += turn * 1005022347264ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D6F6() {
  Flip_D6F6_ValuesOnly();
}

void Flip_D6F6() {
  Flip_D6F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6F6;
  flipped = true;
}

void Flip_D6G6_ValuesOnly() {
  left_columns += turn * 18ULL;
  top_diag9s += turn * 6ULL;
  right_columns += turn * 5066626891382784ULL;
  bottom_diag9s += turn * 5066626891382784ULL;
  bottom_diag7s += turn * 77310595602ULL;
  bottom_rows += turn * 1030792151040ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D6G6() {
  Flip_D6G6_ValuesOnly();
}

void Flip_D6G6() {
  Flip_D6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6G6;
  flipped = true;
}

void Flip_E6_ValuesOnly() {
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 77309411328ULL;
  bottom_diag7s += turn * 1179648ULL;
  bottom_rows += turn * 231928233984ULL;
  disk_difference += turn * 2;
}

void UndoFlip_E6() {
  Flip_E6_ValuesOnly();
}

void Flip_E6() {
  Flip_E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6;
  flipped = true;
}

void Flip_E6_G6_ValuesOnly() {
  top_diag9s += turn * 6ULL;
  right_columns += turn * 5066549581971456ULL;
  bottom_diag9s += turn * 77309411328ULL;
  bottom_diag7s += turn * 1179666ULL;
  bottom_rows += turn * 257698037760ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E6_G6() {
  Flip_E6_G6_ValuesOnly();
}

void Flip_E6_G6() {
  Flip_E6_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6_G6;
  flipped = true;
}

void Flip_E6F6_ValuesOnly() {
  right_columns += turn * 5066626890203136ULL;
  bottom_diag9s += turn * 5066626890203136ULL;
  bottom_diag7s += turn * 1184256ULL;
  bottom_rows += turn * 309237645312ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E6F6() {
  Flip_E6F6_ValuesOnly();
}

void Flip_E6F6() {
  Flip_E6F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6F6;
  flipped = true;
}

void Flip_E6G6_ValuesOnly() {
  top_diag9s += turn * 6ULL;
  right_columns += turn * 5066626891382784ULL;
  bottom_diag9s += turn * 5066626890203136ULL;
  bottom_diag7s += turn * 1184274ULL;
  bottom_rows += turn * 335007449088ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E6G6() {
  Flip_E6G6_ValuesOnly();
}

void Flip_E6G6() {
  Flip_E6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6G6;
  flipped = true;
}

void Flip_F6_ValuesOnly() {
  right_columns += turn * 77309411328ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 4608ULL;
  bottom_rows += turn * 77309411328ULL;
  disk_difference += turn * 2;
}

void UndoFlip_F6() {
  Flip_F6_ValuesOnly();
}

void Flip_F6() {
  Flip_F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F6;
  flipped = true;
}

void Flip_F6G6_ValuesOnly() {
  top_diag9s += turn * 6ULL;
  right_columns += turn * 77310590976ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 4626ULL;
  bottom_rows += turn * 103079215104ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F6G6() {
  Flip_F6G6_ValuesOnly();
}

void Flip_F6G6() {
  Flip_F6G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F6G6;
  flipped = true;
}

void Flip_G6_ValuesOnly() {
  top_diag9s += turn * 6ULL;
  right_columns += turn * 1179648ULL;
  bottom_diag7s += turn * 18ULL;
  bottom_rows += turn * 25769803776ULL;
  disk_difference += turn * 2;
}

void UndoFlip_G6() {
  Flip_G6_ValuesOnly();
}

void Flip_G6() {
  Flip_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G6;
  flipped = true;
}

void Flip_B7_ValuesOnly() {
  left_columns += turn * 25769803776ULL;
  top_diag9s += turn * 25769803776ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 95551488ULL;
  disk_difference += turn * 2;
}

void UndoFlip_B7() {
  Flip_B7_ValuesOnly();
}

void Flip_B7() {
  Flip_B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7;
  flipped = true;
}

void Flip_B7_D7_ValuesOnly() {
  left_columns += turn * 25769803782ULL;
  top_diag9s += turn * 25769803776ULL;
  bottom_diag9s += turn * 1536ULL;
  bottom_diag7s += turn * 1688849860657152ULL;
  bottom_rows += turn * 106168320ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B7_D7() {
  Flip_B7_D7_ValuesOnly();
}

void Flip_B7_D7() {
  Flip_B7_D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7_D7;
  flipped = true;
}

void Flip_B7_D7E7_ValuesOnly() {
  left_columns += turn * 25769803782ULL;
  top_diag9s += turn * 25769803776ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 394752ULL;
  bottom_diag7s += turn * 1688849860658688ULL;
  bottom_rows += turn * 109707264ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B7_D7E7() {
  Flip_B7_D7E7_ValuesOnly();
}

void Flip_B7_D7E7() {
  Flip_B7_D7E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7_D7E7;
  flipped = true;
}

void Flip_B7_D7F7_ValuesOnly() {
  left_columns += turn * 25769803782ULL;
  top_diag9s += turn * 25769803776ULL;
  right_columns += turn * 1688875630067712ULL;
  bottom_diag9s += turn * 25770198528ULL;
  bottom_diag7s += turn * 1688849860658694ULL;
  bottom_rows += turn * 110886912ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B7_D7F7() {
  Flip_B7_D7F7_ValuesOnly();
}

void Flip_B7_D7F7() {
  Flip_B7_D7F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7_D7F7;
  flipped = true;
}

void Flip_B7_D7G7_ValuesOnly() {
  left_columns += turn * 25769803782ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688875630460928ULL;
  bottom_diag9s += turn * 1688875630462464ULL;
  bottom_diag7s += turn * 1688849860658694ULL;
  bottom_rows += turn * 111280128ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B7_D7G7() {
  Flip_B7_D7G7_ValuesOnly();
}

void Flip_B7_D7G7() {
  Flip_B7_D7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7_D7G7;
  flipped = true;
}

void Flip_B7C7_ValuesOnly() {
  left_columns += turn * 25770196992ULL;
  top_diag9s += turn * 25769803776ULL;
  bottom_diag9s += turn * 6ULL;
  bottom_diag7s += turn * 1688875630067712ULL;
  bottom_rows += turn * 127401984ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B7C7() {
  Flip_B7C7_ValuesOnly();
}

void Flip_B7C7() {
  Flip_B7C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7C7;
  flipped = true;
}

void Flip_B7C7_E7_ValuesOnly() {
  left_columns += turn * 25770196992ULL;
  top_diag9s += turn * 25769803776ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 393222ULL;
  bottom_diag7s += turn * 1688875630069248ULL;
  bottom_rows += turn * 130940928ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B7C7_E7() {
  Flip_B7C7_E7_ValuesOnly();
}

void Flip_B7C7_E7() {
  Flip_B7C7_E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7C7_E7;
  flipped = true;
}

void Flip_B7C7_E7F7_ValuesOnly() {
  left_columns += turn * 25770196992ULL;
  top_diag9s += turn * 25769803776ULL;
  right_columns += turn * 1688875630067712ULL;
  bottom_diag9s += turn * 25770196998ULL;
  bottom_diag7s += turn * 1688875630069254ULL;
  bottom_rows += turn * 132120576ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B7C7_E7F7() {
  Flip_B7C7_E7F7_ValuesOnly();
}

void Flip_B7C7_E7F7() {
  Flip_B7C7_E7F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7C7_E7F7;
  flipped = true;
}

void Flip_B7C7_E7G7_ValuesOnly() {
  left_columns += turn * 25770196992ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688875630460928ULL;
  bottom_diag9s += turn * 1688875630460934ULL;
  bottom_diag7s += turn * 1688875630069254ULL;
  bottom_rows += turn * 132513792ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B7C7_E7G7() {
  Flip_B7C7_E7G7_ValuesOnly();
}

void Flip_B7C7_E7G7() {
  Flip_B7C7_E7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7C7_E7G7;
  flipped = true;
}

void Flip_B7D7_ValuesOnly() {
  left_columns += turn * 25770196998ULL;
  top_diag9s += turn * 25769803776ULL;
  bottom_diag9s += turn * 1542ULL;
  bottom_diag7s += turn * 1688875630460928ULL;
  bottom_rows += turn * 138018816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B7D7() {
  Flip_B7D7_ValuesOnly();
}

void Flip_B7D7() {
  Flip_B7D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7D7;
  flipped = true;
}

void Flip_B7D7_F7_ValuesOnly() {
  left_columns += turn * 25770196998ULL;
  top_diag9s += turn * 25769803776ULL;
  right_columns += turn * 25769803776ULL;
  bottom_diag9s += turn * 25769805318ULL;
  bottom_diag7s += turn * 1688875630460934ULL;
  bottom_rows += turn * 139198464ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B7D7_F7() {
  Flip_B7D7_F7_ValuesOnly();
}

void Flip_B7D7_F7() {
  Flip_B7D7_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7D7_F7;
  flipped = true;
}

void Flip_B7D7_F7G7_ValuesOnly() {
  left_columns += turn * 25770196998ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 25770196992ULL;
  bottom_diag9s += turn * 1688875630069254ULL;
  bottom_diag7s += turn * 1688875630460934ULL;
  bottom_rows += turn * 139591680ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B7D7_F7G7() {
  Flip_B7D7_F7G7_ValuesOnly();
}

void Flip_B7D7_F7G7() {
  Flip_B7D7_F7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7D7_F7G7;
  flipped = true;
}

void Flip_B7E7_ValuesOnly() {
  left_columns += turn * 25770196998ULL;
  top_diag9s += turn * 25769803776ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 394758ULL;
  bottom_diag7s += turn * 1688875630462464ULL;
  bottom_rows += turn * 141557760ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B7E7() {
  Flip_B7E7_ValuesOnly();
}

void Flip_B7E7() {
  Flip_B7E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7E7;
  flipped = true;
}

void Flip_B7E7_G7_ValuesOnly() {
  left_columns += turn * 25770196998ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688849860657152ULL;
  bottom_diag9s += turn * 1688849860658694ULL;
  bottom_diag7s += turn * 1688875630462464ULL;
  bottom_rows += turn * 141950976ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B7E7_G7() {
  Flip_B7E7_G7_ValuesOnly();
}

void Flip_B7E7_G7() {
  Flip_B7E7_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7E7_G7;
  flipped = true;
}

void Flip_B7F7_ValuesOnly() {
  left_columns += turn * 25770196998ULL;
  top_diag9s += turn * 25769803776ULL;
  right_columns += turn * 1688875630067712ULL;
  bottom_diag9s += turn * 25770198534ULL;
  bottom_diag7s += turn * 1688875630462470ULL;
  bottom_rows += turn * 142737408ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B7F7() {
  Flip_B7F7_ValuesOnly();
}

void Flip_B7F7() {
  Flip_B7F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7F7;
  flipped = true;
}

void Flip_B7G7_ValuesOnly() {
  left_columns += turn * 25770196998ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688875630460928ULL;
  bottom_diag9s += turn * 1688875630462470ULL;
  bottom_diag7s += turn * 1688875630462470ULL;
  bottom_rows += turn * 143130624ULL;
  disk_difference += turn * 12;
}

void UndoFlip_B7G7() {
  Flip_B7G7_ValuesOnly();
}

void Flip_B7G7() {
  Flip_B7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7G7;
  flipped = true;
}

void Flip_C7_ValuesOnly() {
  left_columns += turn * 393216ULL;
  bottom_diag9s += turn * 6ULL;
  bottom_diag7s += turn * 25769803776ULL;
  bottom_rows += turn * 31850496ULL;
  disk_difference += turn * 2;
}

void UndoFlip_C7() {
  Flip_C7_ValuesOnly();
}

void Flip_C7() {
  Flip_C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7;
  flipped = true;
}

void Flip_C7_E7_ValuesOnly() {
  left_columns += turn * 393216ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 393222ULL;
  bottom_diag7s += turn * 25769805312ULL;
  bottom_rows += turn * 35389440ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C7_E7() {
  Flip_C7_E7_ValuesOnly();
}

void Flip_C7_E7() {
  Flip_C7_E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7_E7;
  flipped = true;
}

void Flip_C7_E7F7_ValuesOnly() {
  left_columns += turn * 393216ULL;
  right_columns += turn * 1688875630067712ULL;
  bottom_diag9s += turn * 25770196998ULL;
  bottom_diag7s += turn * 25769805318ULL;
  bottom_rows += turn * 36569088ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C7_E7F7() {
  Flip_C7_E7F7_ValuesOnly();
}

void Flip_C7_E7F7() {
  Flip_C7_E7F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7_E7F7;
  flipped = true;
}

void Flip_C7_E7G7_ValuesOnly() {
  left_columns += turn * 393216ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688875630460928ULL;
  bottom_diag9s += turn * 1688875630460934ULL;
  bottom_diag7s += turn * 25769805318ULL;
  bottom_rows += turn * 36962304ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C7_E7G7() {
  Flip_C7_E7G7_ValuesOnly();
}

void Flip_C7_E7G7() {
  Flip_C7_E7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7_E7G7;
  flipped = true;
}

void Flip_C7D7_ValuesOnly() {
  left_columns += turn * 393222ULL;
  bottom_diag9s += turn * 1542ULL;
  bottom_diag7s += turn * 25770196992ULL;
  bottom_rows += turn * 42467328ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C7D7() {
  Flip_C7D7_ValuesOnly();
}

void Flip_C7D7() {
  Flip_C7D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7D7;
  flipped = true;
}

void Flip_C7D7_F7_ValuesOnly() {
  left_columns += turn * 393222ULL;
  right_columns += turn * 25769803776ULL;
  bottom_diag9s += turn * 25769805318ULL;
  bottom_diag7s += turn * 25770196998ULL;
  bottom_rows += turn * 43646976ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C7D7_F7() {
  Flip_C7D7_F7_ValuesOnly();
}

void Flip_C7D7_F7() {
  Flip_C7D7_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7D7_F7;
  flipped = true;
}

void Flip_C7D7_F7G7_ValuesOnly() {
  left_columns += turn * 393222ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 25770196992ULL;
  bottom_diag9s += turn * 1688875630069254ULL;
  bottom_diag7s += turn * 25770196998ULL;
  bottom_rows += turn * 44040192ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C7D7_F7G7() {
  Flip_C7D7_F7G7_ValuesOnly();
}

void Flip_C7D7_F7G7() {
  Flip_C7D7_F7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7D7_F7G7;
  flipped = true;
}

void Flip_C7E7_ValuesOnly() {
  left_columns += turn * 393222ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 394758ULL;
  bottom_diag7s += turn * 25770198528ULL;
  bottom_rows += turn * 46006272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C7E7() {
  Flip_C7E7_ValuesOnly();
}

void Flip_C7E7() {
  Flip_C7E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7E7;
  flipped = true;
}

void Flip_C7E7_G7_ValuesOnly() {
  left_columns += turn * 393222ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688849860657152ULL;
  bottom_diag9s += turn * 1688849860658694ULL;
  bottom_diag7s += turn * 25770198528ULL;
  bottom_rows += turn * 46399488ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C7E7_G7() {
  Flip_C7E7_G7_ValuesOnly();
}

void Flip_C7E7_G7() {
  Flip_C7E7_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7E7_G7;
  flipped = true;
}

void Flip_C7F7_ValuesOnly() {
  left_columns += turn * 393222ULL;
  right_columns += turn * 1688875630067712ULL;
  bottom_diag9s += turn * 25770198534ULL;
  bottom_diag7s += turn * 25770198534ULL;
  bottom_rows += turn * 47185920ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C7F7() {
  Flip_C7F7_ValuesOnly();
}

void Flip_C7F7() {
  Flip_C7F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7F7;
  flipped = true;
}

void Flip_C7G7_ValuesOnly() {
  left_columns += turn * 393222ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688875630460928ULL;
  bottom_diag9s += turn * 1688875630462470ULL;
  bottom_diag7s += turn * 25770198534ULL;
  bottom_rows += turn * 47579136ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C7G7() {
  Flip_C7G7_ValuesOnly();
}

void Flip_C7G7() {
  Flip_C7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7G7;
  flipped = true;
}

void Flip_D7_ValuesOnly() {
  left_columns += turn * 6ULL;
  bottom_diag9s += turn * 1536ULL;
  bottom_diag7s += turn * 393216ULL;
  bottom_rows += turn * 10616832ULL;
  disk_difference += turn * 2;
}

void UndoFlip_D7() {
  Flip_D7_ValuesOnly();
}

void Flip_D7() {
  Flip_D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7;
  flipped = true;
}

void Flip_D7_F7_ValuesOnly() {
  left_columns += turn * 6ULL;
  right_columns += turn * 25769803776ULL;
  bottom_diag9s += turn * 25769805312ULL;
  bottom_diag7s += turn * 393222ULL;
  bottom_rows += turn * 11796480ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D7_F7() {
  Flip_D7_F7_ValuesOnly();
}

void Flip_D7_F7() {
  Flip_D7_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7_F7;
  flipped = true;
}

void Flip_D7_F7G7_ValuesOnly() {
  left_columns += turn * 6ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 25770196992ULL;
  bottom_diag9s += turn * 1688875630069248ULL;
  bottom_diag7s += turn * 393222ULL;
  bottom_rows += turn * 12189696ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D7_F7G7() {
  Flip_D7_F7G7_ValuesOnly();
}

void Flip_D7_F7G7() {
  Flip_D7_F7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7_F7G7;
  flipped = true;
}

void Flip_D7E7_ValuesOnly() {
  left_columns += turn * 6ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 394752ULL;
  bottom_diag7s += turn * 394752ULL;
  bottom_rows += turn * 14155776ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D7E7() {
  Flip_D7E7_ValuesOnly();
}

void Flip_D7E7() {
  Flip_D7E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7E7;
  flipped = true;
}

void Flip_D7E7_G7_ValuesOnly() {
  left_columns += turn * 6ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688849860657152ULL;
  bottom_diag9s += turn * 1688849860658688ULL;
  bottom_diag7s += turn * 394752ULL;
  bottom_rows += turn * 14548992ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D7E7_G7() {
  Flip_D7E7_G7_ValuesOnly();
}

void Flip_D7E7_G7() {
  Flip_D7E7_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7E7_G7;
  flipped = true;
}

void Flip_D7F7_ValuesOnly() {
  left_columns += turn * 6ULL;
  right_columns += turn * 1688875630067712ULL;
  bottom_diag9s += turn * 25770198528ULL;
  bottom_diag7s += turn * 394758ULL;
  bottom_rows += turn * 15335424ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D7F7() {
  Flip_D7F7_ValuesOnly();
}

void Flip_D7F7() {
  Flip_D7F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7F7;
  flipped = true;
}

void Flip_D7G7_ValuesOnly() {
  left_columns += turn * 6ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688875630460928ULL;
  bottom_diag9s += turn * 1688875630462464ULL;
  bottom_diag7s += turn * 394758ULL;
  bottom_rows += turn * 15728640ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D7G7() {
  Flip_D7G7_ValuesOnly();
}

void Flip_D7G7() {
  Flip_D7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7G7;
  flipped = true;
}

void Flip_E7_ValuesOnly() {
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 393216ULL;
  bottom_diag7s += turn * 1536ULL;
  bottom_rows += turn * 3538944ULL;
  disk_difference += turn * 2;
}

void UndoFlip_E7() {
  Flip_E7_ValuesOnly();
}

void Flip_E7() {
  Flip_E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E7;
  flipped = true;
}

void Flip_E7_G7_ValuesOnly() {
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688849860657152ULL;
  bottom_diag9s += turn * 1688849860657152ULL;
  bottom_diag7s += turn * 1536ULL;
  bottom_rows += turn * 3932160ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E7_G7() {
  Flip_E7_G7_ValuesOnly();
}

void Flip_E7_G7() {
  Flip_E7_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E7_G7;
  flipped = true;
}

void Flip_E7F7_ValuesOnly() {
  right_columns += turn * 1688875630067712ULL;
  bottom_diag9s += turn * 25770196992ULL;
  bottom_diag7s += turn * 1542ULL;
  bottom_rows += turn * 4718592ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E7F7() {
  Flip_E7F7_ValuesOnly();
}

void Flip_E7F7() {
  Flip_E7F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E7F7;
  flipped = true;
}

void Flip_E7G7_ValuesOnly() {
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1688875630460928ULL;
  bottom_diag9s += turn * 1688875630460928ULL;
  bottom_diag7s += turn * 1542ULL;
  bottom_rows += turn * 5111808ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E7G7() {
  Flip_E7G7_ValuesOnly();
}

void Flip_E7G7() {
  Flip_E7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E7G7;
  flipped = true;
}

void Flip_F7_ValuesOnly() {
  right_columns += turn * 25769803776ULL;
  bottom_diag9s += turn * 25769803776ULL;
  bottom_diag7s += turn * 6ULL;
  bottom_rows += turn * 1179648ULL;
  disk_difference += turn * 2;
}

void UndoFlip_F7() {
  Flip_F7_ValuesOnly();
}

void Flip_F7() {
  Flip_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F7;
  flipped = true;
}

void Flip_F7G7_ValuesOnly() {
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 25770196992ULL;
  bottom_diag9s += turn * 1688875630067712ULL;
  bottom_diag7s += turn * 6ULL;
  bottom_rows += turn * 1572864ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F7G7() {
  Flip_F7G7_ValuesOnly();
}

void Flip_F7G7() {
  Flip_F7G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F7G7;
  flipped = true;
}

void Flip_G7_ValuesOnly() {
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 393216ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_rows += turn * 393216ULL;
  disk_difference += turn * 2;
}

void UndoFlip_G7() {
  Flip_G7_ValuesOnly();
}

void Flip_G7() {
  Flip_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G7;
  flipped = true;
}

void Flip_B8_ValuesOnly() {
  left_columns += turn * 8589934592ULL;
  bottom_diag7s += turn * 8589934592ULL;
  bottom_rows += turn * 1458ULL;
  disk_difference += turn * 2;
}

void UndoFlip_B8() {
  Flip_B8_ValuesOnly();
}

void Flip_B8() {
  Flip_B8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8;
  flipped = true;
}

void Flip_B8_D8_ValuesOnly() {
  left_columns += turn * 8589934594ULL;
  bottom_diag9s += turn * 2ULL;
  bottom_diag7s += turn * 8589935104ULL;
  bottom_rows += turn * 1620ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B8_D8() {
  Flip_B8_D8_ValuesOnly();
}

void Flip_B8_D8() {
  Flip_B8_D8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8_D8;
  flipped = true;
}

void Flip_B8_D8E8_ValuesOnly() {
  left_columns += turn * 8589934594ULL;
  right_columns += turn * 562949953421312ULL;
  bottom_diag9s += turn * 514ULL;
  bottom_diag7s += turn * 8589935106ULL;
  bottom_rows += turn * 1674ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B8_D8E8() {
  Flip_B8_D8E8_ValuesOnly();
}

void Flip_B8_D8E8() {
  Flip_B8_D8E8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8_D8E8;
  flipped = true;
}

void Flip_B8_D8F8_ValuesOnly() {
  left_columns += turn * 8589934594ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543355904ULL;
  bottom_diag9s += turn * 131586ULL;
  bottom_diag7s += turn * 8589935106ULL;
  bottom_rows += turn * 1692ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B8_D8F8() {
  Flip_B8_D8F8_ValuesOnly();
}

void Flip_B8_D8F8() {
  Flip_B8_D8F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8_D8F8;
  flipped = true;
}

void Flip_B8_D8G8_ValuesOnly() {
  left_columns += turn * 8589934594ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543486976ULL;
  bottom_diag9s += turn * 8590066178ULL;
  bottom_diag7s += turn * 8589935106ULL;
  bottom_rows += turn * 1698ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B8_D8G8() {
  Flip_B8_D8G8_ValuesOnly();
}

void Flip_B8_D8G8() {
  Flip_B8_D8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8_D8G8;
  flipped = true;
}

void Flip_B8C8_ValuesOnly() {
  left_columns += turn * 8590065664ULL;
  top_diag9s += turn * 8589934592ULL;
  bottom_diag7s += turn * 8590065664ULL;
  bottom_rows += turn * 1944ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B8C8() {
  Flip_B8C8_ValuesOnly();
}

void Flip_B8C8() {
  Flip_B8C8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8C8;
  flipped = true;
}

void Flip_B8C8_E8_ValuesOnly() {
  left_columns += turn * 8590065664ULL;
  top_diag9s += turn * 8589934592ULL;
  right_columns += turn * 562949953421312ULL;
  bottom_diag9s += turn * 512ULL;
  bottom_diag7s += turn * 8590065666ULL;
  bottom_rows += turn * 1998ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B8C8_E8() {
  Flip_B8C8_E8_ValuesOnly();
}

void Flip_B8C8_E8() {
  Flip_B8C8_E8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8C8_E8;
  flipped = true;
}

void Flip_B8C8_E8F8_ValuesOnly() {
  left_columns += turn * 8590065664ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543355904ULL;
  bottom_diag9s += turn * 131584ULL;
  bottom_diag7s += turn * 8590065666ULL;
  bottom_rows += turn * 2016ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B8C8_E8F8() {
  Flip_B8C8_E8F8_ValuesOnly();
}

void Flip_B8C8_E8F8() {
  Flip_B8C8_E8F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8C8_E8F8;
  flipped = true;
}

void Flip_B8C8_E8G8_ValuesOnly() {
  left_columns += turn * 8590065664ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543486976ULL;
  bottom_diag9s += turn * 8590066176ULL;
  bottom_diag7s += turn * 8590065666ULL;
  bottom_rows += turn * 2022ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B8C8_E8G8() {
  Flip_B8C8_E8G8_ValuesOnly();
}

void Flip_B8C8_E8G8() {
  Flip_B8C8_E8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8C8_E8G8;
  flipped = true;
}

void Flip_B8D8_ValuesOnly() {
  left_columns += turn * 8590065666ULL;
  top_diag9s += turn * 8589934592ULL;
  bottom_diag9s += turn * 2ULL;
  bottom_diag7s += turn * 8590066176ULL;
  bottom_rows += turn * 2106ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B8D8() {
  Flip_B8D8_ValuesOnly();
}

void Flip_B8D8() {
  Flip_B8D8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8D8;
  flipped = true;
}

void Flip_B8D8_F8_ValuesOnly() {
  left_columns += turn * 8590065666ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 8589934592ULL;
  bottom_diag9s += turn * 131074ULL;
  bottom_diag7s += turn * 8590066176ULL;
  bottom_rows += turn * 2124ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B8D8_F8() {
  Flip_B8D8_F8_ValuesOnly();
}

void Flip_B8D8_F8() {
  Flip_B8D8_F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8D8_F8;
  flipped = true;
}

void Flip_B8D8_F8G8_ValuesOnly() {
  left_columns += turn * 8590065666ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 8590065664ULL;
  bottom_diag9s += turn * 8590065666ULL;
  bottom_diag7s += turn * 8590066176ULL;
  bottom_rows += turn * 2130ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B8D8_F8G8() {
  Flip_B8D8_F8G8_ValuesOnly();
}

void Flip_B8D8_F8G8() {
  Flip_B8D8_F8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8D8_F8G8;
  flipped = true;
}

void Flip_B8E8_ValuesOnly() {
  left_columns += turn * 8590065666ULL;
  top_diag9s += turn * 8589934592ULL;
  right_columns += turn * 562949953421312ULL;
  bottom_diag9s += turn * 514ULL;
  bottom_diag7s += turn * 8590066178ULL;
  bottom_rows += turn * 2160ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B8E8() {
  Flip_B8E8_ValuesOnly();
}

void Flip_B8E8() {
  Flip_B8E8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8E8;
  flipped = true;
}

void Flip_B8E8_G8_ValuesOnly() {
  left_columns += turn * 8590065666ULL;
  top_diag9s += turn * 8589934592ULL;
  right_columns += turn * 562949953552384ULL;
  bottom_diag9s += turn * 8589935106ULL;
  bottom_diag7s += turn * 8590066178ULL;
  bottom_rows += turn * 2166ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B8E8_G8() {
  Flip_B8E8_G8_ValuesOnly();
}

void Flip_B8E8_G8() {
  Flip_B8E8_G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8E8_G8;
  flipped = true;
}

void Flip_B8F8_ValuesOnly() {
  left_columns += turn * 8590065666ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543355904ULL;
  bottom_diag9s += turn * 131586ULL;
  bottom_diag7s += turn * 8590066178ULL;
  bottom_rows += turn * 2178ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B8F8() {
  Flip_B8F8_ValuesOnly();
}

void Flip_B8F8() {
  Flip_B8F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8F8;
  flipped = true;
}

void Flip_B8G8_ValuesOnly() {
  left_columns += turn * 8590065666ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543486976ULL;
  bottom_diag9s += turn * 8590066178ULL;
  bottom_diag7s += turn * 8590066178ULL;
  bottom_rows += turn * 2184ULL;
  disk_difference += turn * 12;
}

void UndoFlip_B8G8() {
  Flip_B8G8_ValuesOnly();
}

void Flip_B8G8() {
  Flip_B8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8G8;
  flipped = true;
}

void Flip_C8_ValuesOnly() {
  left_columns += turn * 131072ULL;
  top_diag9s += turn * 8589934592ULL;
  bottom_diag7s += turn * 131072ULL;
  bottom_rows += turn * 486ULL;
  disk_difference += turn * 2;
}

void UndoFlip_C8() {
  Flip_C8_ValuesOnly();
}

void Flip_C8() {
  Flip_C8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8;
  flipped = true;
}

void Flip_C8_E8_ValuesOnly() {
  left_columns += turn * 131072ULL;
  top_diag9s += turn * 8589934592ULL;
  right_columns += turn * 562949953421312ULL;
  bottom_diag9s += turn * 512ULL;
  bottom_diag7s += turn * 131074ULL;
  bottom_rows += turn * 540ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C8_E8() {
  Flip_C8_E8_ValuesOnly();
}

void Flip_C8_E8() {
  Flip_C8_E8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8_E8;
  flipped = true;
}

void Flip_C8_E8F8_ValuesOnly() {
  left_columns += turn * 131072ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543355904ULL;
  bottom_diag9s += turn * 131584ULL;
  bottom_diag7s += turn * 131074ULL;
  bottom_rows += turn * 558ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C8_E8F8() {
  Flip_C8_E8F8_ValuesOnly();
}

void Flip_C8_E8F8() {
  Flip_C8_E8F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8_E8F8;
  flipped = true;
}

void Flip_C8_E8G8_ValuesOnly() {
  left_columns += turn * 131072ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543486976ULL;
  bottom_diag9s += turn * 8590066176ULL;
  bottom_diag7s += turn * 131074ULL;
  bottom_rows += turn * 564ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C8_E8G8() {
  Flip_C8_E8G8_ValuesOnly();
}

void Flip_C8_E8G8() {
  Flip_C8_E8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8_E8G8;
  flipped = true;
}

void Flip_C8D8_ValuesOnly() {
  left_columns += turn * 131074ULL;
  top_diag9s += turn * 8589934592ULL;
  bottom_diag9s += turn * 2ULL;
  bottom_diag7s += turn * 131584ULL;
  bottom_rows += turn * 648ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C8D8() {
  Flip_C8D8_ValuesOnly();
}

void Flip_C8D8() {
  Flip_C8D8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8D8;
  flipped = true;
}

void Flip_C8D8_F8_ValuesOnly() {
  left_columns += turn * 131074ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 8589934592ULL;
  bottom_diag9s += turn * 131074ULL;
  bottom_diag7s += turn * 131584ULL;
  bottom_rows += turn * 666ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C8D8_F8() {
  Flip_C8D8_F8_ValuesOnly();
}

void Flip_C8D8_F8() {
  Flip_C8D8_F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8D8_F8;
  flipped = true;
}

void Flip_C8D8_F8G8_ValuesOnly() {
  left_columns += turn * 131074ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 8590065664ULL;
  bottom_diag9s += turn * 8590065666ULL;
  bottom_diag7s += turn * 131584ULL;
  bottom_rows += turn * 672ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C8D8_F8G8() {
  Flip_C8D8_F8G8_ValuesOnly();
}

void Flip_C8D8_F8G8() {
  Flip_C8D8_F8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8D8_F8G8;
  flipped = true;
}

void Flip_C8E8_ValuesOnly() {
  left_columns += turn * 131074ULL;
  top_diag9s += turn * 8589934592ULL;
  right_columns += turn * 562949953421312ULL;
  bottom_diag9s += turn * 514ULL;
  bottom_diag7s += turn * 131586ULL;
  bottom_rows += turn * 702ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C8E8() {
  Flip_C8E8_ValuesOnly();
}

void Flip_C8E8() {
  Flip_C8E8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8E8;
  flipped = true;
}

void Flip_C8E8_G8_ValuesOnly() {
  left_columns += turn * 131074ULL;
  top_diag9s += turn * 8589934592ULL;
  right_columns += turn * 562949953552384ULL;
  bottom_diag9s += turn * 8589935106ULL;
  bottom_diag7s += turn * 131586ULL;
  bottom_rows += turn * 708ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C8E8_G8() {
  Flip_C8E8_G8_ValuesOnly();
}

void Flip_C8E8_G8() {
  Flip_C8E8_G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8E8_G8;
  flipped = true;
}

void Flip_C8F8_ValuesOnly() {
  left_columns += turn * 131074ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543355904ULL;
  bottom_diag9s += turn * 131586ULL;
  bottom_diag7s += turn * 131586ULL;
  bottom_rows += turn * 720ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C8F8() {
  Flip_C8F8_ValuesOnly();
}

void Flip_C8F8() {
  Flip_C8F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8F8;
  flipped = true;
}

void Flip_C8G8_ValuesOnly() {
  left_columns += turn * 131074ULL;
  top_diag9s += turn * 8589934592ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543486976ULL;
  bottom_diag9s += turn * 8590066178ULL;
  bottom_diag7s += turn * 131586ULL;
  bottom_rows += turn * 726ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C8G8() {
  Flip_C8G8_ValuesOnly();
}

void Flip_C8G8() {
  Flip_C8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8G8;
  flipped = true;
}

void Flip_D8_ValuesOnly() {
  left_columns += turn * 2ULL;
  bottom_diag9s += turn * 2ULL;
  bottom_diag7s += turn * 512ULL;
  bottom_rows += turn * 162ULL;
  disk_difference += turn * 2;
}

void UndoFlip_D8() {
  Flip_D8_ValuesOnly();
}

void Flip_D8() {
  Flip_D8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8;
  flipped = true;
}

void Flip_D8_F8_ValuesOnly() {
  left_columns += turn * 2ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 8589934592ULL;
  bottom_diag9s += turn * 131074ULL;
  bottom_diag7s += turn * 512ULL;
  bottom_rows += turn * 180ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D8_F8() {
  Flip_D8_F8_ValuesOnly();
}

void Flip_D8_F8() {
  Flip_D8_F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8_F8;
  flipped = true;
}

void Flip_D8_F8G8_ValuesOnly() {
  left_columns += turn * 2ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 8590065664ULL;
  bottom_diag9s += turn * 8590065666ULL;
  bottom_diag7s += turn * 512ULL;
  bottom_rows += turn * 186ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D8_F8G8() {
  Flip_D8_F8G8_ValuesOnly();
}

void Flip_D8_F8G8() {
  Flip_D8_F8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8_F8G8;
  flipped = true;
}

void Flip_D8E8_ValuesOnly() {
  left_columns += turn * 2ULL;
  right_columns += turn * 562949953421312ULL;
  bottom_diag9s += turn * 514ULL;
  bottom_diag7s += turn * 514ULL;
  bottom_rows += turn * 216ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D8E8() {
  Flip_D8E8_ValuesOnly();
}

void Flip_D8E8() {
  Flip_D8E8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8E8;
  flipped = true;
}

void Flip_D8E8_G8_ValuesOnly() {
  left_columns += turn * 2ULL;
  right_columns += turn * 562949953552384ULL;
  bottom_diag9s += turn * 8589935106ULL;
  bottom_diag7s += turn * 514ULL;
  bottom_rows += turn * 222ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D8E8_G8() {
  Flip_D8E8_G8_ValuesOnly();
}

void Flip_D8E8_G8() {
  Flip_D8E8_G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8E8_G8;
  flipped = true;
}

void Flip_D8F8_ValuesOnly() {
  left_columns += turn * 2ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543355904ULL;
  bottom_diag9s += turn * 131586ULL;
  bottom_diag7s += turn * 514ULL;
  bottom_rows += turn * 234ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D8F8() {
  Flip_D8F8_ValuesOnly();
}

void Flip_D8F8() {
  Flip_D8F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8F8;
  flipped = true;
}

void Flip_D8G8_ValuesOnly() {
  left_columns += turn * 2ULL;
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543486976ULL;
  bottom_diag9s += turn * 8590066178ULL;
  bottom_diag7s += turn * 514ULL;
  bottom_rows += turn * 240ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D8G8() {
  Flip_D8G8_ValuesOnly();
}

void Flip_D8G8() {
  Flip_D8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8G8;
  flipped = true;
}

void Flip_E8_ValuesOnly() {
  right_columns += turn * 562949953421312ULL;
  bottom_diag9s += turn * 512ULL;
  bottom_diag7s += turn * 2ULL;
  bottom_rows += turn * 54ULL;
  disk_difference += turn * 2;
}

void UndoFlip_E8() {
  Flip_E8_ValuesOnly();
}

void Flip_E8() {
  Flip_E8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E8;
  flipped = true;
}

void Flip_E8_G8_ValuesOnly() {
  right_columns += turn * 562949953552384ULL;
  bottom_diag9s += turn * 8589935104ULL;
  bottom_diag7s += turn * 2ULL;
  bottom_rows += turn * 60ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E8_G8() {
  Flip_E8_G8_ValuesOnly();
}

void Flip_E8_G8() {
  Flip_E8_G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E8_G8;
  flipped = true;
}

void Flip_E8F8_ValuesOnly() {
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543355904ULL;
  bottom_diag9s += turn * 131584ULL;
  bottom_diag7s += turn * 2ULL;
  bottom_rows += turn * 72ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E8F8() {
  Flip_E8F8_ValuesOnly();
}

void Flip_E8F8() {
  Flip_E8F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E8F8;
  flipped = true;
}

void Flip_E8G8_ValuesOnly() {
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 562958543486976ULL;
  bottom_diag9s += turn * 8590066176ULL;
  bottom_diag7s += turn * 2ULL;
  bottom_rows += turn * 78ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E8G8() {
  Flip_E8G8_ValuesOnly();
}

void Flip_E8G8() {
  Flip_E8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E8G8;
  flipped = true;
}

void Flip_F8_ValuesOnly() {
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 8589934592ULL;
  bottom_diag9s += turn * 131072ULL;
  bottom_rows += turn * 18ULL;
  disk_difference += turn * 2;
}

void UndoFlip_F8() {
  Flip_F8_ValuesOnly();
}

void Flip_F8() {
  Flip_F8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F8;
  flipped = true;
}

void Flip_F8G8_ValuesOnly() {
  top_diag7s += turn * 8589934592ULL;
  right_columns += turn * 8590065664ULL;
  bottom_diag9s += turn * 8590065664ULL;
  bottom_rows += turn * 24ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F8G8() {
  Flip_F8G8_ValuesOnly();
}

void Flip_F8G8() {
  Flip_F8G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F8G8;
  flipped = true;
}

void Flip_G8_ValuesOnly() {
  right_columns += turn * 131072ULL;
  bottom_diag9s += turn * 8589934592ULL;
  bottom_rows += turn * 6ULL;
  disk_difference += turn * 2;
}

void UndoFlip_G8() {
  Flip_G8_ValuesOnly();
}

void Flip_G8() {
  Flip_G8_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G8;
  flipped = true;
}

void Flip_A2_ValuesOnly() {
  top_rows += turn * 18786186952704ULL;
  left_columns += turn * 410390516044136448ULL;
  bottom_diag9s += turn * 6262062317568ULL;
  disk_difference += turn * 2;
}

void UndoFlip_A2() {
  Flip_A2_ValuesOnly();
}

void Flip_A2() {
  Flip_A2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2;
  flipped = true;
}

void Flip_A2_A4_ValuesOnly() {
  top_rows += turn * 18786186957078ULL;
  left_columns += turn * 455989462271262720ULL;
  top_diag7s += turn * 562949953421312ULL;
  bottom_diag9s += turn * 6262062359040ULL;
  disk_difference += turn * 4;
}

void UndoFlip_A2_A4() {
  Flip_A2_A4_ValuesOnly();
}

void Flip_A2_A4() {
  Flip_A2_A4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2_A4;
  flipped = true;
}

void Flip_A2_A4A5_ValuesOnly() {
  top_rows += turn * 18786186957078ULL;
  left_columns += turn * 471189111013638144ULL;
  top_diag7s += turn * 565148976676864ULL;
  bottom_diag9s += turn * 6262062359094ULL;
  bottom_rows += turn * 1231171548132409344ULL;
  disk_difference += turn * 6;
}

void UndoFlip_A2_A4A5() {
  Flip_A2_A4A5_ValuesOnly();
}

void Flip_A2_A4A5() {
  Flip_A2_A4A5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2_A4A5;
  flipped = true;
}

void Flip_A2_A4A6_ValuesOnly() {
  top_rows += turn * 18786186957078ULL;
  left_columns += turn * 476255660594429952ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 565148976807936ULL;
  bottom_diag9s += turn * 6262062359094ULL;
  bottom_rows += turn * 1231190334319362048ULL;
  disk_difference += turn * 8;
}

void UndoFlip_A2_A4A6() {
  Flip_A2_A4A6_ValuesOnly();
}

void Flip_A2_A4A6() {
  Flip_A2_A4A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2_A4A6;
  flipped = true;
}

void Flip_A2_A4A7_ValuesOnly() {
  top_rows += turn * 18786186957078ULL;
  left_columns += turn * 477944510454693888ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 565148976807938ULL;
  bottom_diag9s += turn * 6262062359094ULL;
  bottom_rows += turn * 1231190334606016512ULL;
  disk_difference += turn * 10;
}

void UndoFlip_A2_A4A7() {
  Flip_A2_A4A7_ValuesOnly();
}

void Flip_A2_A4A7() {
  Flip_A2_A4A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2_A4A7;
  flipped = true;
}

void Flip_A2A3_ValuesOnly() {
  top_rows += turn * 18786473607168ULL;
  left_columns += turn * 547187354725515264ULL;
  top_diag7s += turn * 144115188075855872ULL;
  bottom_diag9s += turn * 6262094168064ULL;
  disk_difference += turn * 4;
}

void UndoFlip_A2A3() {
  Flip_A2A3_ValuesOnly();
}

void Flip_A2A3() {
  Flip_A2A3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A3;
  flipped = true;
}

void Flip_A2A3_A5_ValuesOnly() {
  top_rows += turn * 18786473607168ULL;
  left_columns += turn * 562387003467890688ULL;
  top_diag7s += turn * 144117387099111424ULL;
  bottom_diag9s += turn * 6262094168118ULL;
  bottom_rows += turn * 1231171548132409344ULL;
  disk_difference += turn * 6;
}

void UndoFlip_A2A3_A5() {
  Flip_A2A3_A5_ValuesOnly();
}

void Flip_A2A3_A5() {
  Flip_A2A3_A5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A3_A5;
  flipped = true;
}

void Flip_A2A3_A5A6_ValuesOnly() {
  top_rows += turn * 18786473607168ULL;
  left_columns += turn * 567453553048682496ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144117387099242496ULL;
  bottom_diag9s += turn * 6262094168118ULL;
  bottom_rows += turn * 1231190334319362048ULL;
  disk_difference += turn * 8;
}

void UndoFlip_A2A3_A5A6() {
  Flip_A2A3_A5A6_ValuesOnly();
}

void Flip_A2A3_A5A6() {
  Flip_A2A3_A5A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A3_A5A6;
  flipped = true;
}

void Flip_A2A3_A5A7_ValuesOnly() {
  top_rows += turn * 18786473607168ULL;
  left_columns += turn * 569142402908946432ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144117387099242498ULL;
  bottom_diag9s += turn * 6262094168118ULL;
  bottom_rows += turn * 1231190334606016512ULL;
  disk_difference += turn * 10;
}

void UndoFlip_A2A3_A5A7() {
  Flip_A2A3_A5A7_ValuesOnly();
}

void Flip_A2A3_A5A7() {
  Flip_A2A3_A5A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A3_A5A7;
  flipped = true;
}

void Flip_A2A4_ValuesOnly() {
  top_rows += turn * 18786473611542ULL;
  left_columns += turn * 592786300952641536ULL;
  top_diag7s += turn * 144678138029277184ULL;
  bottom_diag9s += turn * 6262094209536ULL;
  disk_difference += turn * 6;
}

void UndoFlip_A2A4() {
  Flip_A2A4_ValuesOnly();
}

void Flip_A2A4() {
  Flip_A2A4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A4;
  flipped = true;
}

void Flip_A2A4_A6_ValuesOnly() {
  top_rows += turn * 18786473611542ULL;
  left_columns += turn * 597852850533433344ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144678138029408256ULL;
  bottom_diag9s += turn * 6262094209536ULL;
  bottom_rows += turn * 18786186952704ULL;
  disk_difference += turn * 8;
}

void UndoFlip_A2A4_A6() {
  Flip_A2A4_A6_ValuesOnly();
}

void Flip_A2A4_A6() {
  Flip_A2A4_A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A4_A6;
  flipped = true;
}

void Flip_A2A4_A6A7_ValuesOnly() {
  top_rows += turn * 18786473611542ULL;
  left_columns += turn * 599541700393697280ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144678138029408258ULL;
  bottom_diag9s += turn * 6262094209536ULL;
  bottom_rows += turn * 18786473607168ULL;
  disk_difference += turn * 10;
}

void UndoFlip_A2A4_A6A7() {
  Flip_A2A4_A6A7_ValuesOnly();
}

void Flip_A2A4_A6A7() {
  Flip_A2A4_A6A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A4_A6A7;
  flipped = true;
}

void Flip_A2A5_ValuesOnly() {
  top_rows += turn * 18786473611542ULL;
  left_columns += turn * 607985949695016960ULL;
  top_diag7s += turn * 144680337052532736ULL;
  bottom_diag9s += turn * 6262094209590ULL;
  bottom_rows += turn * 1231171548132409344ULL;
  disk_difference += turn * 8;
}

void UndoFlip_A2A5() {
  Flip_A2A5_ValuesOnly();
}

void Flip_A2A5() {
  Flip_A2A5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A5;
  flipped = true;
}

void Flip_A2A5_A7_ValuesOnly() {
  top_rows += turn * 18786473611542ULL;
  left_columns += turn * 609674799555280896ULL;
  top_diag7s += turn * 144680337052532738ULL;
  bottom_diag9s += turn * 6262094209590ULL;
  bottom_rows += turn * 1231171548419063808ULL;
  disk_difference += turn * 10;
}

void UndoFlip_A2A5_A7() {
  Flip_A2A5_A7_ValuesOnly();
}

void Flip_A2A5_A7() {
  Flip_A2A5_A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A5_A7;
  flipped = true;
}

void Flip_A2A6_ValuesOnly() {
  top_rows += turn * 18786473611542ULL;
  left_columns += turn * 613052499275808768ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144680337052663808ULL;
  bottom_diag9s += turn * 6262094209590ULL;
  bottom_rows += turn * 1231190334319362048ULL;
  disk_difference += turn * 10;
}

void UndoFlip_A2A6() {
  Flip_A2A6_ValuesOnly();
}

void Flip_A2A6() {
  Flip_A2A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A6;
  flipped = true;
}

void Flip_A2A7_ValuesOnly() {
  top_rows += turn * 18786473611542ULL;
  left_columns += turn * 614741349136072704ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144680337052663810ULL;
  bottom_diag9s += turn * 6262094209590ULL;
  bottom_rows += turn * 1231190334606016512ULL;
  disk_difference += turn * 12;
}

void UndoFlip_A2A7() {
  Flip_A2A7_ValuesOnly();
}

void Flip_A2A7() {
  Flip_A2A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A7;
  flipped = true;
}

void Flip_A3_ValuesOnly() {
  top_rows += turn * 286654464ULL;
  left_columns += turn * 136796838681378816ULL;
  top_diag7s += turn * 144115188075855872ULL;
  bottom_diag9s += turn * 31850496ULL;
  disk_difference += turn * 2;
}

void UndoFlip_A3() {
  Flip_A3_ValuesOnly();
}

void Flip_A3() {
  Flip_A3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3;
  flipped = true;
}

void Flip_A3_A5_ValuesOnly() {
  top_rows += turn * 286654464ULL;
  left_columns += turn * 151996487423754240ULL;
  top_diag7s += turn * 144117387099111424ULL;
  bottom_diag9s += turn * 31850550ULL;
  bottom_rows += turn * 1231171548132409344ULL;
  disk_difference += turn * 4;
}

void UndoFlip_A3_A5() {
  Flip_A3_A5_ValuesOnly();
}

void Flip_A3_A5() {
  Flip_A3_A5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3_A5;
  flipped = true;
}

void Flip_A3_A5A6_ValuesOnly() {
  top_rows += turn * 286654464ULL;
  left_columns += turn * 157063037004546048ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144117387099242496ULL;
  bottom_diag9s += turn * 31850550ULL;
  bottom_rows += turn * 1231190334319362048ULL;
  disk_difference += turn * 6;
}

void UndoFlip_A3_A5A6() {
  Flip_A3_A5A6_ValuesOnly();
}

void Flip_A3_A5A6() {
  Flip_A3_A5A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3_A5A6;
  flipped = true;
}

void Flip_A3_A5A7_ValuesOnly() {
  top_rows += turn * 286654464ULL;
  left_columns += turn * 158751886864809984ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144117387099242498ULL;
  bottom_diag9s += turn * 31850550ULL;
  bottom_rows += turn * 1231190334606016512ULL;
  disk_difference += turn * 8;
}

void UndoFlip_A3_A5A7() {
  Flip_A3_A5A7_ValuesOnly();
}

void Flip_A3_A5A7() {
  Flip_A3_A5A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3_A5A7;
  flipped = true;
}

void Flip_A3A4_ValuesOnly() {
  top_rows += turn * 286658838ULL;
  left_columns += turn * 182395784908505088ULL;
  top_diag7s += turn * 144678138029277184ULL;
  bottom_diag9s += turn * 31891968ULL;
  disk_difference += turn * 4;
}

void UndoFlip_A3A4() {
  Flip_A3A4_ValuesOnly();
}

void Flip_A3A4() {
  Flip_A3A4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A4;
  flipped = true;
}

void Flip_A3A4_A6_ValuesOnly() {
  top_rows += turn * 286658838ULL;
  left_columns += turn * 187462334489296896ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144678138029408256ULL;
  bottom_diag9s += turn * 31891968ULL;
  bottom_rows += turn * 18786186952704ULL;
  disk_difference += turn * 6;
}

void UndoFlip_A3A4_A6() {
  Flip_A3A4_A6_ValuesOnly();
}

void Flip_A3A4_A6() {
  Flip_A3A4_A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A4_A6;
  flipped = true;
}

void Flip_A3A4_A6A7_ValuesOnly() {
  top_rows += turn * 286658838ULL;
  left_columns += turn * 189151184349560832ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144678138029408258ULL;
  bottom_diag9s += turn * 31891968ULL;
  bottom_rows += turn * 18786473607168ULL;
  disk_difference += turn * 8;
}

void UndoFlip_A3A4_A6A7() {
  Flip_A3A4_A6A7_ValuesOnly();
}

void Flip_A3A4_A6A7() {
  Flip_A3A4_A6A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A4_A6A7;
  flipped = true;
}

void Flip_A3A5_ValuesOnly() {
  top_rows += turn * 286658838ULL;
  left_columns += turn * 197595433650880512ULL;
  top_diag7s += turn * 144680337052532736ULL;
  bottom_diag9s += turn * 31892022ULL;
  bottom_rows += turn * 1231171548132409344ULL;
  disk_difference += turn * 6;
}

void UndoFlip_A3A5() {
  Flip_A3A5_ValuesOnly();
}

void Flip_A3A5() {
  Flip_A3A5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A5;
  flipped = true;
}

void Flip_A3A5_A7_ValuesOnly() {
  top_rows += turn * 286658838ULL;
  left_columns += turn * 199284283511144448ULL;
  top_diag7s += turn * 144680337052532738ULL;
  bottom_diag9s += turn * 31892022ULL;
  bottom_rows += turn * 1231171548419063808ULL;
  disk_difference += turn * 8;
}

void UndoFlip_A3A5_A7() {
  Flip_A3A5_A7_ValuesOnly();
}

void Flip_A3A5_A7() {
  Flip_A3A5_A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A5_A7;
  flipped = true;
}

void Flip_A3A6_ValuesOnly() {
  top_rows += turn * 286658838ULL;
  left_columns += turn * 202661983231672320ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144680337052663808ULL;
  bottom_diag9s += turn * 31892022ULL;
  bottom_rows += turn * 1231190334319362048ULL;
  disk_difference += turn * 8;
}

void UndoFlip_A3A6() {
  Flip_A3A6_ValuesOnly();
}

void Flip_A3A6() {
  Flip_A3A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A6;
  flipped = true;
}

void Flip_A3A7_ValuesOnly() {
  top_rows += turn * 286658838ULL;
  left_columns += turn * 204350833091936256ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 144680337052663810ULL;
  bottom_diag9s += turn * 31892022ULL;
  bottom_rows += turn * 1231190334606016512ULL;
  disk_difference += turn * 10;
}

void UndoFlip_A3A7() {
  Flip_A3A7_ValuesOnly();
}

void Flip_A3A7() {
  Flip_A3A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A7;
  flipped = true;
}

void Flip_A4_ValuesOnly() {
  top_rows += turn * 4374ULL;
  left_columns += turn * 45598946227126272ULL;
  top_diag7s += turn * 562949953421312ULL;
  bottom_diag9s += turn * 41472ULL;
  disk_difference += turn * 2;
}

void UndoFlip_A4() {
  Flip_A4_ValuesOnly();
}

void Flip_A4() {
  Flip_A4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4;
  flipped = true;
}

void Flip_A4_A6_ValuesOnly() {
  top_rows += turn * 4374ULL;
  left_columns += turn * 50665495807918080ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 562949953552384ULL;
  bottom_diag9s += turn * 41472ULL;
  bottom_rows += turn * 18786186952704ULL;
  disk_difference += turn * 4;
}

void UndoFlip_A4_A6() {
  Flip_A4_A6_ValuesOnly();
}

void Flip_A4_A6() {
  Flip_A4_A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4_A6;
  flipped = true;
}

void Flip_A4_A6A7_ValuesOnly() {
  top_rows += turn * 4374ULL;
  left_columns += turn * 52354345668182016ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 562949953552386ULL;
  bottom_diag9s += turn * 41472ULL;
  bottom_rows += turn * 18786473607168ULL;
  disk_difference += turn * 6;
}

void UndoFlip_A4_A6A7() {
  Flip_A4_A6A7_ValuesOnly();
}

void Flip_A4_A6A7() {
  Flip_A4_A6A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4_A6A7;
  flipped = true;
}

void Flip_A4A5_ValuesOnly() {
  top_rows += turn * 4374ULL;
  left_columns += turn * 60798594969501696ULL;
  top_diag7s += turn * 565148976676864ULL;
  bottom_diag9s += turn * 41526ULL;
  bottom_rows += turn * 1231171548132409344ULL;
  disk_difference += turn * 4;
}

void UndoFlip_A4A5() {
  Flip_A4A5_ValuesOnly();
}

void Flip_A4A5() {
  Flip_A4A5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4A5;
  flipped = true;
}

void Flip_A4A5_A7_ValuesOnly() {
  top_rows += turn * 4374ULL;
  left_columns += turn * 62487444829765632ULL;
  top_diag7s += turn * 565148976676866ULL;
  bottom_diag9s += turn * 41526ULL;
  bottom_rows += turn * 1231171548419063808ULL;
  disk_difference += turn * 6;
}

void UndoFlip_A4A5_A7() {
  Flip_A4A5_A7_ValuesOnly();
}

void Flip_A4A5_A7() {
  Flip_A4A5_A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4A5_A7;
  flipped = true;
}

void Flip_A4A6_ValuesOnly() {
  top_rows += turn * 4374ULL;
  left_columns += turn * 65865144550293504ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 565148976807936ULL;
  bottom_diag9s += turn * 41526ULL;
  bottom_rows += turn * 1231190334319362048ULL;
  disk_difference += turn * 6;
}

void UndoFlip_A4A6() {
  Flip_A4A6_ValuesOnly();
}

void Flip_A4A6() {
  Flip_A4A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4A6;
  flipped = true;
}

void Flip_A4A7_ValuesOnly() {
  top_rows += turn * 4374ULL;
  left_columns += turn * 67553994410557440ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 565148976807938ULL;
  bottom_diag9s += turn * 41526ULL;
  bottom_rows += turn * 1231190334606016512ULL;
  disk_difference += turn * 8;
}

void UndoFlip_A4A7() {
  Flip_A4A7_ValuesOnly();
}

void Flip_A4A7() {
  Flip_A4A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4A7;
  flipped = true;
}

void Flip_A5_ValuesOnly() {
  left_columns += turn * 15199648742375424ULL;
  top_diag7s += turn * 2199023255552ULL;
  bottom_diag9s += turn * 54ULL;
  bottom_rows += turn * 1231171548132409344ULL;
  disk_difference += turn * 2;
}

void UndoFlip_A5() {
  Flip_A5_ValuesOnly();
}

void Flip_A5() {
  Flip_A5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A5;
  flipped = true;
}

void Flip_A5_A7_ValuesOnly() {
  left_columns += turn * 16888498602639360ULL;
  top_diag7s += turn * 2199023255554ULL;
  bottom_diag9s += turn * 54ULL;
  bottom_rows += turn * 1231171548419063808ULL;
  disk_difference += turn * 4;
}

void UndoFlip_A5_A7() {
  Flip_A5_A7_ValuesOnly();
}

void Flip_A5_A7() {
  Flip_A5_A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A5_A7;
  flipped = true;
}

void Flip_A5A6_ValuesOnly() {
  left_columns += turn * 20266198323167232ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 2199023386624ULL;
  bottom_diag9s += turn * 54ULL;
  bottom_rows += turn * 1231190334319362048ULL;
  disk_difference += turn * 4;
}

void UndoFlip_A5A6() {
  Flip_A5A6_ValuesOnly();
}

void Flip_A5A6() {
  Flip_A5A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A5A6;
  flipped = true;
}

void Flip_A5A7_ValuesOnly() {
  left_columns += turn * 21955048183431168ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 2199023386626ULL;
  bottom_diag9s += turn * 54ULL;
  bottom_rows += turn * 1231190334606016512ULL;
  disk_difference += turn * 6;
}

void UndoFlip_A5A7() {
  Flip_A5A7_ValuesOnly();
}

void Flip_A5A7() {
  Flip_A5A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A5A7;
  flipped = true;
}

void Flip_A6_ValuesOnly() {
  left_columns += turn * 5066549580791808ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 131072ULL;
  bottom_rows += turn * 18786186952704ULL;
  disk_difference += turn * 2;
}

void UndoFlip_A6() {
  Flip_A6_ValuesOnly();
}

void Flip_A6() {
  Flip_A6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A6;
  flipped = true;
}

void Flip_A6A7_ValuesOnly() {
  left_columns += turn * 6755399441055744ULL;
  top_diag9s += turn * 77309411328ULL;
  top_diag7s += turn * 131074ULL;
  bottom_rows += turn * 18786473607168ULL;
  disk_difference += turn * 4;
}

void UndoFlip_A6A7() {
  Flip_A6A7_ValuesOnly();
}

void Flip_A6A7() {
  Flip_A6A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A6A7;
  flipped = true;
}

void Flip_A7_ValuesOnly() {
  left_columns += turn * 1688849860263936ULL;
  top_diag7s += turn * 2ULL;
  bottom_rows += turn * 286654464ULL;
  disk_difference += turn * 2;
}

void UndoFlip_A7() {
  Flip_A7_ValuesOnly();
}

void Flip_A7() {
  Flip_A7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A7;
  flipped = true;
}

void Flip_B2_B4_ValuesOnly() {
  top_rows += turn * 6262062319026ULL;
  left_columns += turn * 6957847019520ULL;
  top_diag7s += turn * 432352161297334272ULL;
  bottom_diag9s += turn * 410390516054753280ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B2_B4() {
  Flip_B2_B4_ValuesOnly();
}

void Flip_B2_B4() {
  Flip_B2_B4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_B4;
  flipped = true;
}

void Flip_B2_B4B5_ValuesOnly() {
  top_rows += turn * 6262062319026ULL;
  left_columns += turn * 7189775253504ULL;
  top_diag7s += turn * 432352161297727488ULL;
  bottom_diag9s += turn * 410390516054767104ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B2_B4B5() {
  Flip_B2_B4B5_ValuesOnly();
}

void Flip_B2_B4B5() {
  Flip_B2_B4B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_B4B5;
  flipped = true;
}

void Flip_B2_B4B6_ValuesOnly() {
  top_rows += turn * 6262062319026ULL;
  left_columns += turn * 7267084664832ULL;
  top_diag7s += turn * 432352161297727494ULL;
  bottom_diag9s += turn * 410390516054767122ULL;
  bottom_rows += turn * 410396778106454016ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2_B4B6() {
  Flip_B2_B4B6_ValuesOnly();
}

void Flip_B2_B4B6() {
  Flip_B2_B4B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_B4B6;
  flipped = true;
}

void Flip_B2_B4B7_ValuesOnly() {
  top_rows += turn * 6262062319026ULL;
  left_columns += turn * 7292854468608ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 432352161297727494ULL;
  bottom_diag9s += turn * 410390516054767122ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410396778202005504ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2_B4B7() {
  Flip_B2_B4B7_ValuesOnly();
}

void Flip_B2_B4B7() {
  Flip_B2_B4B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_B4B7;
  flipped = true;
}

void Flip_B2B3_ValuesOnly() {
  top_rows += turn * 6262157869056ULL;
  left_columns += turn * 8349416423424ULL;
  top_diag7s += turn * 434034414087831552ULL;
  bottom_diag9s += turn * 410392603398242304ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B2B3() {
  Flip_B2B3_ValuesOnly();
}

void Flip_B2B3() {
  Flip_B2B3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B3;
  flipped = true;
}

void Flip_B2B3_B5_ValuesOnly() {
  top_rows += turn * 6262157869056ULL;
  left_columns += turn * 8581344657408ULL;
  top_diag7s += turn * 434034414088224768ULL;
  bottom_diag9s += turn * 410392603398256128ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B2B3_B5() {
  Flip_B2B3_B5_ValuesOnly();
}

void Flip_B2B3_B5() {
  Flip_B2B3_B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B3_B5;
  flipped = true;
}

void Flip_B2B3_B5B6_ValuesOnly() {
  top_rows += turn * 6262157869056ULL;
  left_columns += turn * 8658654068736ULL;
  top_diag7s += turn * 434034414088224774ULL;
  bottom_diag9s += turn * 410392603398256146ULL;
  bottom_rows += turn * 410396778106454016ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2B3_B5B6() {
  Flip_B2B3_B5B6_ValuesOnly();
}

void Flip_B2B3_B5B6() {
  Flip_B2B3_B5B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B3_B5B6;
  flipped = true;
}

void Flip_B2B3_B5B7_ValuesOnly() {
  top_rows += turn * 6262157869056ULL;
  left_columns += turn * 8684423872512ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 434034414088224774ULL;
  bottom_diag9s += turn * 410392603398256146ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410396778202005504ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2B3_B5B7() {
  Flip_B2B3_B5B7_ValuesOnly();
}

void Flip_B2B3_B5B7() {
  Flip_B2B3_B5B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B3_B5B7;
  flipped = true;
}

void Flip_B2B4_ValuesOnly() {
  top_rows += turn * 6262157870514ULL;
  left_columns += turn * 9045201125376ULL;
  top_diag7s += turn * 434041011157598208ULL;
  bottom_diag9s += turn * 410392603408859136ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B2B4() {
  Flip_B2B4_ValuesOnly();
}

void Flip_B2B4() {
  Flip_B2B4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B4;
  flipped = true;
}

void Flip_B2B4_B6_ValuesOnly() {
  top_rows += turn * 6262157870514ULL;
  left_columns += turn * 9122510536704ULL;
  top_diag7s += turn * 434041011157598214ULL;
  bottom_diag9s += turn * 410392603408859154ULL;
  bottom_rows += turn * 6262062317568ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2B4_B6() {
  Flip_B2B4_B6_ValuesOnly();
}

void Flip_B2B4_B6() {
  Flip_B2B4_B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B4_B6;
  flipped = true;
}

void Flip_B2B4_B6B7_ValuesOnly() {
  top_rows += turn * 6262157870514ULL;
  left_columns += turn * 9148280340480ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 434041011157598214ULL;
  bottom_diag9s += turn * 410392603408859154ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 6262157869056ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2B4_B6B7() {
  Flip_B2B4_B6B7_ValuesOnly();
}

void Flip_B2B4_B6B7() {
  Flip_B2B4_B6B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B4_B6B7;
  flipped = true;
}

void Flip_B2B5_ValuesOnly() {
  top_rows += turn * 6262157870514ULL;
  left_columns += turn * 9277129359360ULL;
  top_diag7s += turn * 434041011157991424ULL;
  bottom_diag9s += turn * 410392603408872960ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2B5() {
  Flip_B2B5_ValuesOnly();
}

void Flip_B2B5() {
  Flip_B2B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B5;
  flipped = true;
}

void Flip_B2B5_B7_ValuesOnly() {
  top_rows += turn * 6262157870514ULL;
  left_columns += turn * 9302899163136ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 434041011157991424ULL;
  bottom_diag9s += turn * 410392603408872960ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410390516139687936ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2B5_B7() {
  Flip_B2B5_B7_ValuesOnly();
}

void Flip_B2B5_B7() {
  Flip_B2B5_B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B5_B7;
  flipped = true;
}

void Flip_B2B6_ValuesOnly() {
  top_rows += turn * 6262157870514ULL;
  left_columns += turn * 9354438770688ULL;
  top_diag7s += turn * 434041011157991430ULL;
  bottom_diag9s += turn * 410392603408872978ULL;
  bottom_rows += turn * 410396778106454016ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2B6() {
  Flip_B2B6_ValuesOnly();
}

void Flip_B2B6() {
  Flip_B2B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B6;
  flipped = true;
}

void Flip_B2B7_ValuesOnly() {
  top_rows += turn * 6262157870514ULL;
  left_columns += turn * 9380208574464ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 434041011157991430ULL;
  bottom_diag9s += turn * 410392603408872978ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410396778202005504ULL;
  disk_difference += turn * 12;
}

void UndoFlip_B2B7() {
  Flip_B2B7_ValuesOnly();
}

void Flip_B2B7() {
  Flip_B2B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B7;
  flipped = true;
}

void Flip_B3_B5_ValuesOnly() {
  top_rows += turn * 95551488ULL;
  left_columns += turn * 2319282339840ULL;
  top_diag7s += turn * 1688849860657152ULL;
  bottom_diag9s += turn * 2087354119680ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B3_B5() {
  Flip_B3_B5_ValuesOnly();
}

void Flip_B3_B5() {
  Flip_B3_B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_B5;
  flipped = true;
}

void Flip_B3_B5B6_ValuesOnly() {
  top_rows += turn * 95551488ULL;
  left_columns += turn * 2396591751168ULL;
  top_diag7s += turn * 1688849860657158ULL;
  bottom_diag9s += turn * 2087354119698ULL;
  bottom_rows += turn * 410396778106454016ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B3_B5B6() {
  Flip_B3_B5B6_ValuesOnly();
}

void Flip_B3_B5B6() {
  Flip_B3_B5B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_B5B6;
  flipped = true;
}

void Flip_B3_B5B7_ValuesOnly() {
  top_rows += turn * 95551488ULL;
  left_columns += turn * 2422361554944ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 1688849860657158ULL;
  bottom_diag9s += turn * 2087354119698ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410396778202005504ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3_B5B7() {
  Flip_B3_B5B7_ValuesOnly();
}

void Flip_B3_B5B7() {
  Flip_B3_B5B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_B5B7;
  flipped = true;
}

void Flip_B3B4_ValuesOnly() {
  top_rows += turn * 95552946ULL;
  left_columns += turn * 2783138807808ULL;
  top_diag7s += turn * 1695446930030592ULL;
  bottom_diag9s += turn * 2087364722688ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B3B4() {
  Flip_B3B4_ValuesOnly();
}

void Flip_B3B4() {
  Flip_B3B4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B4;
  flipped = true;
}

void Flip_B3B4_B6_ValuesOnly() {
  top_rows += turn * 95552946ULL;
  left_columns += turn * 2860448219136ULL;
  top_diag7s += turn * 1695446930030598ULL;
  bottom_diag9s += turn * 2087364722706ULL;
  bottom_rows += turn * 6262062317568ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B3B4_B6() {
  Flip_B3B4_B6_ValuesOnly();
}

void Flip_B3B4_B6() {
  Flip_B3B4_B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B4_B6;
  flipped = true;
}

void Flip_B3B4_B6B7_ValuesOnly() {
  top_rows += turn * 95552946ULL;
  left_columns += turn * 2886218022912ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 1695446930030598ULL;
  bottom_diag9s += turn * 2087364722706ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 6262157869056ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3B4_B6B7() {
  Flip_B3B4_B6B7_ValuesOnly();
}

void Flip_B3B4_B6B7() {
  Flip_B3B4_B6B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B4_B6B7;
  flipped = true;
}

void Flip_B3B5_ValuesOnly() {
  top_rows += turn * 95552946ULL;
  left_columns += turn * 3015067041792ULL;
  top_diag7s += turn * 1695446930423808ULL;
  bottom_diag9s += turn * 2087364736512ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B3B5() {
  Flip_B3B5_ValuesOnly();
}

void Flip_B3B5() {
  Flip_B3B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B5;
  flipped = true;
}

void Flip_B3B5_B7_ValuesOnly() {
  top_rows += turn * 95552946ULL;
  left_columns += turn * 3040836845568ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 1695446930423808ULL;
  bottom_diag9s += turn * 2087364736512ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410390516139687936ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3B5_B7() {
  Flip_B3B5_B7_ValuesOnly();
}

void Flip_B3B5_B7() {
  Flip_B3B5_B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B5_B7;
  flipped = true;
}

void Flip_B3B6_ValuesOnly() {
  top_rows += turn * 95552946ULL;
  left_columns += turn * 3092376453120ULL;
  top_diag7s += turn * 1695446930423814ULL;
  bottom_diag9s += turn * 2087364736530ULL;
  bottom_rows += turn * 410396778106454016ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3B6() {
  Flip_B3B6_ValuesOnly();
}

void Flip_B3B6() {
  Flip_B3B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B6;
  flipped = true;
}

void Flip_B3B7_ValuesOnly() {
  top_rows += turn * 95552946ULL;
  left_columns += turn * 3118146256896ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 1695446930423814ULL;
  bottom_diag9s += turn * 2087364736530ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410396778202005504ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B3B7() {
  Flip_B3B7_ValuesOnly();
}

void Flip_B3B7() {
  Flip_B3B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B7;
  flipped = true;
}

void Flip_B4_B6_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 773094113280ULL;
  top_diag7s += turn * 6597069766662ULL;
  bottom_diag9s += turn * 10616850ULL;
  bottom_rows += turn * 6262062317568ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B4_B6() {
  Flip_B4_B6_ValuesOnly();
}

void Flip_B4_B6() {
  Flip_B4_B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_B6;
  flipped = true;
}

void Flip_B4_B6B7_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 798863917056ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 6597069766662ULL;
  bottom_diag9s += turn * 10616850ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 6262157869056ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B4_B6B7() {
  Flip_B4_B6B7_ValuesOnly();
}

void Flip_B4_B6B7() {
  Flip_B4_B6B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_B6B7;
  flipped = true;
}

void Flip_B4B5_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 927712935936ULL;
  top_diag7s += turn * 6597070159872ULL;
  bottom_diag9s += turn * 10630656ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B4B5() {
  Flip_B4B5_ValuesOnly();
}

void Flip_B4B5() {
  Flip_B4B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4B5;
  flipped = true;
}

void Flip_B4B5_B7_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 953482739712ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 6597070159872ULL;
  bottom_diag9s += turn * 10630656ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410390516139687936ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B4B5_B7() {
  Flip_B4B5_B7_ValuesOnly();
}

void Flip_B4B5_B7() {
  Flip_B4B5_B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4B5_B7;
  flipped = true;
}

void Flip_B4B6_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 1005022347264ULL;
  top_diag7s += turn * 6597070159878ULL;
  bottom_diag9s += turn * 10630674ULL;
  bottom_rows += turn * 410396778106454016ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B4B6() {
  Flip_B4B6_ValuesOnly();
}

void Flip_B4B6() {
  Flip_B4B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4B6;
  flipped = true;
}

void Flip_B4B7_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 1030792151040ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 6597070159878ULL;
  bottom_diag9s += turn * 10630674ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410396778202005504ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B4B7() {
  Flip_B4B7_ValuesOnly();
}

void Flip_B4B7() {
  Flip_B4B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4B7;
  flipped = true;
}

void Flip_B5_B7_ValuesOnly() {
  left_columns += turn * 257698037760ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 393216ULL;
  bottom_diag9s += turn * 13824ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410390516139687936ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B5_B7() {
  Flip_B5_B7_ValuesOnly();
}

void Flip_B5_B7() {
  Flip_B5_B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_B7;
  flipped = true;
}

void Flip_B5B6_ValuesOnly() {
  left_columns += turn * 309237645312ULL;
  top_diag7s += turn * 393222ULL;
  bottom_diag9s += turn * 13842ULL;
  bottom_rows += turn * 410396778106454016ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B5B6() {
  Flip_B5B6_ValuesOnly();
}

void Flip_B5B6() {
  Flip_B5B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5B6;
  flipped = true;
}

void Flip_B5B7_ValuesOnly() {
  left_columns += turn * 335007449088ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 393222ULL;
  bottom_diag9s += turn * 13842ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 410396778202005504ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B5B7() {
  Flip_B5B7_ValuesOnly();
}

void Flip_B5B7() {
  Flip_B5B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5B7;
  flipped = true;
}

void Flip_B6B7_ValuesOnly() {
  left_columns += turn * 103079215104ULL;
  top_diag9s += turn * 25769803776ULL;
  top_diag7s += turn * 6ULL;
  bottom_diag9s += turn * 18ULL;
  bottom_diag7s += turn * 1688849860263936ULL;
  bottom_rows += turn * 6262157869056ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B6B7() {
  Flip_B6B7_ValuesOnly();
}

void Flip_B6B7() {
  Flip_B6B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6B7;
  flipped = true;
}

void Flip_C2_C4_ValuesOnly() {
  top_rows += turn * 2087354106342ULL;
  left_columns += turn * 106168320ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5066549581971456ULL;
  bottom_diag9s += turn * 695784701952ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C2_C4() {
  Flip_C2_C4_ValuesOnly();
}

void Flip_C2_C4() {
  Flip_C2_C4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_C4;
  flipped = true;
}

void Flip_C2_C4C5_ValuesOnly() {
  top_rows += turn * 2087354106342ULL;
  left_columns += turn * 109707264ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5066549581971474ULL;
  bottom_diag9s += turn * 695788240896ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C2_C4C5() {
  Flip_C2_C4C5_ValuesOnly();
}

void Flip_C2_C4C5() {
  Flip_C2_C4C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_C4C5;
  flipped = true;
}

void Flip_C2_C4C6_ValuesOnly() {
  top_rows += turn * 2087354106342ULL;
  left_columns += turn * 110886912ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5066549581971474ULL;
  bottom_diag9s += turn * 695788245504ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 136798926035484672ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2_C4C6() {
  Flip_C2_C4C6_ValuesOnly();
}

void Flip_C2_C4C6() {
  Flip_C2_C4C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_C4C6;
  flipped = true;
}

void Flip_C2_C4C7_ValuesOnly() {
  top_rows += turn * 2087354106342ULL;
  left_columns += turn * 111280128ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5066549581971474ULL;
  bottom_diag9s += turn * 695788245510ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 136798926067335168ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C2_C4C7() {
  Flip_C2_C4C7_ValuesOnly();
}

void Flip_C2_C4C7() {
  Flip_C2_C4C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_C4C7;
  flipped = true;
}

void Flip_C2C3_ValuesOnly() {
  top_rows += turn * 2087385956352ULL;
  left_columns += turn * 127401984ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340790091776ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C2C3() {
  Flip_C2C3_ValuesOnly();
}

void Flip_C2C3() {
  Flip_C2C3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C3;
  flipped = true;
}

void Flip_C2C3_C5_ValuesOnly() {
  top_rows += turn * 2087385956352ULL;
  left_columns += turn * 130940928ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340790091794ULL;
  bottom_diag9s += turn * 136796838684917760ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C2C3_C5() {
  Flip_C2C3_C5_ValuesOnly();
}

void Flip_C2C3_C5() {
  Flip_C2C3_C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C3_C5;
  flipped = true;
}

void Flip_C2C3_C5C6_ValuesOnly() {
  top_rows += turn * 2087385956352ULL;
  left_columns += turn * 132120576ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340790091794ULL;
  bottom_diag9s += turn * 136796838684922368ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 136798926035484672ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2C3_C5C6() {
  Flip_C2C3_C5C6_ValuesOnly();
}

void Flip_C2C3_C5C6() {
  Flip_C2C3_C5C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C3_C5C6;
  flipped = true;
}

void Flip_C2C3_C5C7_ValuesOnly() {
  top_rows += turn * 2087385956352ULL;
  left_columns += turn * 132513792ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340790091794ULL;
  bottom_diag9s += turn * 136796838684922374ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 136798926067335168ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C2C3_C5C7() {
  Flip_C2C3_C5C7_ValuesOnly();
}

void Flip_C2C3_C5C7() {
  Flip_C2C3_C5C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C3_C5C7;
  flipped = true;
}

void Flip_C2C4_ValuesOnly() {
  top_rows += turn * 2087385956838ULL;
  left_columns += turn * 138018816ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340791271424ULL;
  bottom_diag9s += turn * 136797534466080768ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C2C4() {
  Flip_C2C4_ValuesOnly();
}

void Flip_C2C4() {
  Flip_C2C4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C4;
  flipped = true;
}

void Flip_C2C4_C6_ValuesOnly() {
  top_rows += turn * 2087385956838ULL;
  left_columns += turn * 139198464ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340791271424ULL;
  bottom_diag9s += turn * 136797534466085376ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 2087354105856ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2C4_C6() {
  Flip_C2C4_C6_ValuesOnly();
}

void Flip_C2C4_C6() {
  Flip_C2C4_C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C4_C6;
  flipped = true;
}

void Flip_C2C4_C6C7_ValuesOnly() {
  top_rows += turn * 2087385956838ULL;
  left_columns += turn * 139591680ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340791271424ULL;
  bottom_diag9s += turn * 136797534466085382ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 2087385956352ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C2C4_C6C7() {
  Flip_C2C4_C6C7_ValuesOnly();
}

void Flip_C2C4_C6C7() {
  Flip_C2C4_C6C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C4_C6C7;
  flipped = true;
}

void Flip_C2C5_ValuesOnly() {
  top_rows += turn * 2087385956838ULL;
  left_columns += turn * 141557760ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340791271442ULL;
  bottom_diag9s += turn * 136797534469619712ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2C5() {
  Flip_C2C5_ValuesOnly();
}

void Flip_C2C5() {
  Flip_C2C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C5;
  flipped = true;
}

void Flip_C2C5_C7_ValuesOnly() {
  top_rows += turn * 2087385956838ULL;
  left_columns += turn * 141950976ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340791271442ULL;
  bottom_diag9s += turn * 136797534469619718ULL;
  bottom_diag7s += turn * 25769803776ULL;
  bottom_rows += turn * 136796838713229312ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C2C5_C7() {
  Flip_C2C5_C7_ValuesOnly();
}

void Flip_C2C5_C7() {
  Flip_C2C5_C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C5_C7;
  flipped = true;
}

void Flip_C2C6_ValuesOnly() {
  top_rows += turn * 2087385956838ULL;
  left_columns += turn * 142737408ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340791271442ULL;
  bottom_diag9s += turn * 136797534469624320ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 136798926035484672ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C2C6() {
  Flip_C2C6_ValuesOnly();
}

void Flip_C2C6() {
  Flip_C2C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C6;
  flipped = true;
}

void Flip_C2C7_ValuesOnly() {
  top_rows += turn * 2087385956838ULL;
  left_columns += turn * 143130624ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 5086340791271442ULL;
  bottom_diag9s += turn * 136797534469624326ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 136798926067335168ULL;
  disk_difference += turn * 12;
}

void UndoFlip_C2C7() {
  Flip_C2C7_ValuesOnly();
}

void Flip_C2C7() {
  Flip_C2C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C7;
  flipped = true;
}

void Flip_C3_C5_ValuesOnly() {
  top_rows += turn * 31850496ULL;
  left_columns += turn * 35389440ULL;
  top_diag7s += turn * 19791209299986ULL;
  bottom_diag9s += turn * 136796838684917760ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C3_C5() {
  Flip_C3_C5_ValuesOnly();
}

void Flip_C3_C5() {
  Flip_C3_C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_C5;
  flipped = true;
}

void Flip_C3_C5C6_ValuesOnly() {
  top_rows += turn * 31850496ULL;
  left_columns += turn * 36569088ULL;
  top_diag7s += turn * 19791209299986ULL;
  bottom_diag9s += turn * 136796838684922368ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 136798926035484672ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C3_C5C6() {
  Flip_C3_C5C6_ValuesOnly();
}

void Flip_C3_C5C6() {
  Flip_C3_C5C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_C5C6;
  flipped = true;
}

void Flip_C3_C5C7_ValuesOnly() {
  top_rows += turn * 31850496ULL;
  left_columns += turn * 36962304ULL;
  top_diag7s += turn * 19791209299986ULL;
  bottom_diag9s += turn * 136796838684922374ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 136798926067335168ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3_C5C7() {
  Flip_C3_C5C7_ValuesOnly();
}

void Flip_C3_C5C7() {
  Flip_C3_C5C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_C5C7;
  flipped = true;
}

void Flip_C3C4_ValuesOnly() {
  top_rows += turn * 31850982ULL;
  left_columns += turn * 42467328ULL;
  top_diag7s += turn * 19791210479616ULL;
  bottom_diag9s += turn * 136797534466080768ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C3C4() {
  Flip_C3C4_ValuesOnly();
}

void Flip_C3C4() {
  Flip_C3C4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C4;
  flipped = true;
}

void Flip_C3C4_C6_ValuesOnly() {
  top_rows += turn * 31850982ULL;
  left_columns += turn * 43646976ULL;
  top_diag7s += turn * 19791210479616ULL;
  bottom_diag9s += turn * 136797534466085376ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 2087354105856ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C3C4_C6() {
  Flip_C3C4_C6_ValuesOnly();
}

void Flip_C3C4_C6() {
  Flip_C3C4_C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C4_C6;
  flipped = true;
}

void Flip_C3C4_C6C7_ValuesOnly() {
  top_rows += turn * 31850982ULL;
  left_columns += turn * 44040192ULL;
  top_diag7s += turn * 19791210479616ULL;
  bottom_diag9s += turn * 136797534466085382ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 2087385956352ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3C4_C6C7() {
  Flip_C3C4_C6C7_ValuesOnly();
}

void Flip_C3C4_C6C7() {
  Flip_C3C4_C6C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C4_C6C7;
  flipped = true;
}

void Flip_C3C5_ValuesOnly() {
  top_rows += turn * 31850982ULL;
  left_columns += turn * 46006272ULL;
  top_diag7s += turn * 19791210479634ULL;
  bottom_diag9s += turn * 136797534469619712ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C3C5() {
  Flip_C3C5_ValuesOnly();
}

void Flip_C3C5() {
  Flip_C3C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C5;
  flipped = true;
}

void Flip_C3C5_C7_ValuesOnly() {
  top_rows += turn * 31850982ULL;
  left_columns += turn * 46399488ULL;
  top_diag7s += turn * 19791210479634ULL;
  bottom_diag9s += turn * 136797534469619718ULL;
  bottom_diag7s += turn * 25769803776ULL;
  bottom_rows += turn * 136796838713229312ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3C5_C7() {
  Flip_C3C5_C7_ValuesOnly();
}

void Flip_C3C5_C7() {
  Flip_C3C5_C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C5_C7;
  flipped = true;
}

void Flip_C3C6_ValuesOnly() {
  top_rows += turn * 31850982ULL;
  left_columns += turn * 47185920ULL;
  top_diag7s += turn * 19791210479634ULL;
  bottom_diag9s += turn * 136797534469624320ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 136798926035484672ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3C6() {
  Flip_C3C6_ValuesOnly();
}

void Flip_C3C6() {
  Flip_C3C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C6;
  flipped = true;
}

void Flip_C3C7_ValuesOnly() {
  top_rows += turn * 31850982ULL;
  left_columns += turn * 47579136ULL;
  top_diag7s += turn * 19791210479634ULL;
  bottom_diag9s += turn * 136797534469624326ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 136798926067335168ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C3C7() {
  Flip_C3C7_ValuesOnly();
}

void Flip_C3C7() {
  Flip_C3C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C7;
  flipped = true;
}

void Flip_C4_C6_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 11796480ULL;
  top_diag7s += turn * 1179648ULL;
  bottom_diag9s += turn * 695784706560ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 2087354105856ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C4_C6() {
  Flip_C4_C6_ValuesOnly();
}

void Flip_C4_C6() {
  Flip_C4_C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_C6;
  flipped = true;
}

void Flip_C4_C6C7_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 12189696ULL;
  top_diag7s += turn * 1179648ULL;
  bottom_diag9s += turn * 695784706566ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 2087385956352ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C4_C6C7() {
  Flip_C4_C6C7_ValuesOnly();
}

void Flip_C4_C6C7() {
  Flip_C4_C6C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_C6C7;
  flipped = true;
}

void Flip_C4C5_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 14155776ULL;
  top_diag7s += turn * 1179666ULL;
  bottom_diag9s += turn * 695788240896ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C4C5() {
  Flip_C4C5_ValuesOnly();
}

void Flip_C4C5() {
  Flip_C4C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4C5;
  flipped = true;
}

void Flip_C4C5_C7_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 14548992ULL;
  top_diag7s += turn * 1179666ULL;
  bottom_diag9s += turn * 695788240902ULL;
  bottom_diag7s += turn * 25769803776ULL;
  bottom_rows += turn * 136796838713229312ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C4C5_C7() {
  Flip_C4C5_C7_ValuesOnly();
}

void Flip_C4C5_C7() {
  Flip_C4C5_C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4C5_C7;
  flipped = true;
}

void Flip_C4C6_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 15335424ULL;
  top_diag7s += turn * 1179666ULL;
  bottom_diag9s += turn * 695788245504ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 136798926035484672ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C4C6() {
  Flip_C4C6_ValuesOnly();
}

void Flip_C4C6() {
  Flip_C4C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4C6;
  flipped = true;
}

void Flip_C4C7_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 15728640ULL;
  top_diag7s += turn * 1179666ULL;
  bottom_diag9s += turn * 695788245510ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 136798926067335168ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C4C7() {
  Flip_C4C7_ValuesOnly();
}

void Flip_C4C7() {
  Flip_C4C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4C7;
  flipped = true;
}

void Flip_C5_C7_ValuesOnly() {
  left_columns += turn * 3932160ULL;
  top_diag7s += turn * 18ULL;
  bottom_diag9s += turn * 3538950ULL;
  bottom_diag7s += turn * 25769803776ULL;
  bottom_rows += turn * 136796838713229312ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C5_C7() {
  Flip_C5_C7_ValuesOnly();
}

void Flip_C5_C7() {
  Flip_C5_C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5_C7;
  flipped = true;
}

void Flip_C5C6_ValuesOnly() {
  left_columns += turn * 4718592ULL;
  top_diag7s += turn * 18ULL;
  bottom_diag9s += turn * 3543552ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 136798926035484672ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C5C6() {
  Flip_C5C6_ValuesOnly();
}

void Flip_C5C6() {
  Flip_C5C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5C6;
  flipped = true;
}

void Flip_C5C7_ValuesOnly() {
  left_columns += turn * 5111808ULL;
  top_diag7s += turn * 18ULL;
  bottom_diag9s += turn * 3543558ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 136798926067335168ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C5C7() {
  Flip_C5C7_ValuesOnly();
}

void Flip_C5C7() {
  Flip_C5C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5C7;
  flipped = true;
}

void Flip_C6C7_ValuesOnly() {
  left_columns += turn * 1572864ULL;
  bottom_diag9s += turn * 4614ULL;
  bottom_diag7s += turn * 5066575350595584ULL;
  bottom_rows += turn * 2087385956352ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C6C7() {
  Flip_C6C7_ValuesOnly();
}

void Flip_C6C7() {
  Flip_C6C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6C7;
  flipped = true;
}

void Flip_D2_D4_ValuesOnly() {
  top_rows += turn * 695784702114ULL;
  left_columns += turn * 1620ULL;
  top_diag9s += turn * 10616832ULL;
  top_diag7s += turn * 59373627899958ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D2_D4() {
  Flip_D2_D4_ValuesOnly();
}

void Flip_D2_D4() {
  Flip_D2_D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_D4;
  flipped = true;
}

void Flip_D2_D4D5_ValuesOnly() {
  top_rows += turn * 695784702114ULL;
  left_columns += turn * 1674ULL;
  top_diag9s += turn * 10616832ULL;
  top_diag7s += turn * 59373627899958ULL;
  bottom_diag9s += turn * 45599178155360256ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D2_D4D5() {
  Flip_D2_D4D5_ValuesOnly();
}

void Flip_D2_D4D5() {
  Flip_D2_D4D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_D4D5;
  flipped = true;
}

void Flip_D2_D4D6_ValuesOnly() {
  top_rows += turn * 695784702114ULL;
  left_columns += turn * 1692ULL;
  top_diag9s += turn * 10616832ULL;
  top_diag7s += turn * 59373627899958ULL;
  bottom_diag9s += turn * 45599178156539904ULL;
  bottom_diag7s += turn * 15199726051786752ULL;
  bottom_rows += turn * 45599642011828224ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D2_D4D6() {
  Flip_D2_D4D6_ValuesOnly();
}

void Flip_D2_D4D6() {
  Flip_D2_D4D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_D4D6;
  flipped = true;
}

void Flip_D2_D4D7_ValuesOnly() {
  top_rows += turn * 695784702114ULL;
  left_columns += turn * 1698ULL;
  top_diag9s += turn * 10616832ULL;
  top_diag7s += turn * 59373627899958ULL;
  bottom_diag9s += turn * 45599178156541440ULL;
  bottom_diag7s += turn * 15199726052179968ULL;
  bottom_rows += turn * 45599642022445056ULL;
  disk_difference += turn * 10;
}

void UndoFlip_D2_D4D7() {
  Flip_D2_D4D7_ValuesOnly();
}

void Flip_D2_D4D7() {
  Flip_D2_D4D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_D4D7;
  flipped = true;
}

void Flip_D2D3_ValuesOnly() {
  top_rows += turn * 695795318784ULL;
  left_columns += turn * 1944ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438848ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D2D3() {
  Flip_D2D3_ValuesOnly();
}

void Flip_D2D3() {
  Flip_D2D3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D3;
  flipped = true;
}

void Flip_D2D3_D5_ValuesOnly() {
  top_rows += turn * 695795318784ULL;
  left_columns += turn * 1998ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438848ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D2D3_D5() {
  Flip_D2D3_D5_ValuesOnly();
}

void Flip_D2D3_D5() {
  Flip_D2D3_D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D3_D5;
  flipped = true;
}

void Flip_D2D3_D5D6_ValuesOnly() {
  top_rows += turn * 695795318784ULL;
  left_columns += turn * 2016ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438848ULL;
  bottom_diag9s += turn * 231929413632ULL;
  bottom_diag7s += turn * 15199726051786752ULL;
  bottom_rows += turn * 45599642011828224ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D2D3_D5D6() {
  Flip_D2D3_D5D6_ValuesOnly();
}

void Flip_D2D3_D5D6() {
  Flip_D2D3_D5D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D3_D5D6;
  flipped = true;
}

void Flip_D2D3_D5D7_ValuesOnly() {
  top_rows += turn * 695795318784ULL;
  left_columns += turn * 2022ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438848ULL;
  bottom_diag9s += turn * 231929415168ULL;
  bottom_diag7s += turn * 15199726052179968ULL;
  bottom_rows += turn * 45599642022445056ULL;
  disk_difference += turn * 10;
}

void UndoFlip_D2D3_D5D7() {
  Flip_D2D3_D5D7_ValuesOnly();
}

void Flip_D2D3_D5D7() {
  Flip_D2D3_D5D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D3_D5D7;
  flipped = true;
}

void Flip_D2D4_ValuesOnly() {
  top_rows += turn * 695795318946ULL;
  left_columns += turn * 2106ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438902ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D2D4() {
  Flip_D2D4_ValuesOnly();
}

void Flip_D2D4() {
  Flip_D2D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D4;
  flipped = true;
}

void Flip_D2D4_D6_ValuesOnly() {
  top_rows += turn * 695795318946ULL;
  left_columns += turn * 2124ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438902ULL;
  bottom_diag9s += turn * 45598946228305920ULL;
  bottom_diag7s += turn * 77309411328ULL;
  bottom_rows += turn * 695784701952ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D2D4_D6() {
  Flip_D2D4_D6_ValuesOnly();
}

void Flip_D2D4_D6() {
  Flip_D2D4_D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D4_D6;
  flipped = true;
}

void Flip_D2D4_D6D7_ValuesOnly() {
  top_rows += turn * 695795318946ULL;
  left_columns += turn * 2130ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438902ULL;
  bottom_diag9s += turn * 45598946228307456ULL;
  bottom_diag7s += turn * 77309804544ULL;
  bottom_rows += turn * 695795318784ULL;
  disk_difference += turn * 10;
}

void UndoFlip_D2D4_D6D7() {
  Flip_D2D4_D6D7_ValuesOnly();
}

void Flip_D2D4_D6D7() {
  Flip_D2D4_D6D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D4_D6D7;
  flipped = true;
}

void Flip_D2D5_ValuesOnly() {
  top_rows += turn * 695795318946ULL;
  left_columns += turn * 2160ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438902ULL;
  bottom_diag9s += turn * 45599178155360256ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D2D5() {
  Flip_D2D5_ValuesOnly();
}

void Flip_D2D5() {
  Flip_D2D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D5;
  flipped = true;
}

void Flip_D2D5_D7_ValuesOnly() {
  top_rows += turn * 695795318946ULL;
  left_columns += turn * 2166ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438902ULL;
  bottom_diag9s += turn * 45599178155361792ULL;
  bottom_diag7s += turn * 15199648742768640ULL;
  bottom_rows += turn * 45598946237743104ULL;
  disk_difference += turn * 10;
}

void UndoFlip_D2D5_D7() {
  Flip_D2D5_D7_ValuesOnly();
}

void Flip_D2D5_D7() {
  Flip_D2D5_D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D5_D7;
  flipped = true;
}

void Flip_D2D6_ValuesOnly() {
  top_rows += turn * 695795318946ULL;
  left_columns += turn * 2178ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438902ULL;
  bottom_diag9s += turn * 45599178156539904ULL;
  bottom_diag7s += turn * 15199726051786752ULL;
  bottom_rows += turn * 45599642011828224ULL;
  disk_difference += turn * 10;
}

void UndoFlip_D2D6() {
  Flip_D2D6_ValuesOnly();
}

void Flip_D2D6() {
  Flip_D2D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D6;
  flipped = true;
}

void Flip_D2D7_ValuesOnly() {
  top_rows += turn * 695795318946ULL;
  left_columns += turn * 2184ULL;
  top_diag9s += turn * 10616994ULL;
  top_diag7s += turn * 59373631438902ULL;
  bottom_diag9s += turn * 45599178156541440ULL;
  bottom_diag7s += turn * 15199726052179968ULL;
  bottom_rows += turn * 45599642022445056ULL;
  disk_difference += turn * 12;
}

void UndoFlip_D2D7() {
  Flip_D2D7_ValuesOnly();
}

void Flip_D2D7() {
  Flip_D2D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D7;
  flipped = true;
}

void Flip_D3_D5_ValuesOnly() {
  top_rows += turn * 10616832ULL;
  left_columns += turn * 540ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538944ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D3_D5() {
  Flip_D3_D5_ValuesOnly();
}

void Flip_D3_D5() {
  Flip_D3_D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_D5;
  flipped = true;
}

void Flip_D3_D5D6_ValuesOnly() {
  top_rows += turn * 10616832ULL;
  left_columns += turn * 558ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538944ULL;
  bottom_diag9s += turn * 231929413632ULL;
  bottom_diag7s += turn * 15199726051786752ULL;
  bottom_rows += turn * 45599642011828224ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D3_D5D6() {
  Flip_D3_D5D6_ValuesOnly();
}

void Flip_D3_D5D6() {
  Flip_D3_D5D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_D5D6;
  flipped = true;
}

void Flip_D3_D5D7_ValuesOnly() {
  top_rows += turn * 10616832ULL;
  left_columns += turn * 564ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538944ULL;
  bottom_diag9s += turn * 231929415168ULL;
  bottom_diag7s += turn * 15199726052179968ULL;
  bottom_rows += turn * 45599642022445056ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D3_D5D7() {
  Flip_D3_D5D7_ValuesOnly();
}

void Flip_D3_D5D7() {
  Flip_D3_D5D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_D5D7;
  flipped = true;
}

void Flip_D3D4_ValuesOnly() {
  top_rows += turn * 10616994ULL;
  left_columns += turn * 648ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538998ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D3D4() {
  Flip_D3D4_ValuesOnly();
}

void Flip_D3D4() {
  Flip_D3D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D4;
  flipped = true;
}

void Flip_D3D4_D6_ValuesOnly() {
  top_rows += turn * 10616994ULL;
  left_columns += turn * 666ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538998ULL;
  bottom_diag9s += turn * 45598946228305920ULL;
  bottom_diag7s += turn * 77309411328ULL;
  bottom_rows += turn * 695784701952ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D3D4_D6() {
  Flip_D3D4_D6_ValuesOnly();
}

void Flip_D3D4_D6() {
  Flip_D3D4_D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D4_D6;
  flipped = true;
}

void Flip_D3D4_D6D7_ValuesOnly() {
  top_rows += turn * 10616994ULL;
  left_columns += turn * 672ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538998ULL;
  bottom_diag9s += turn * 45598946228307456ULL;
  bottom_diag7s += turn * 77309804544ULL;
  bottom_rows += turn * 695795318784ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D3D4_D6D7() {
  Flip_D3D4_D6D7_ValuesOnly();
}

void Flip_D3D4_D6D7() {
  Flip_D3D4_D6D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D4_D6D7;
  flipped = true;
}

void Flip_D3D5_ValuesOnly() {
  top_rows += turn * 10616994ULL;
  left_columns += turn * 702ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538998ULL;
  bottom_diag9s += turn * 45599178155360256ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D3D5() {
  Flip_D3D5_ValuesOnly();
}

void Flip_D3D5() {
  Flip_D3D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D5;
  flipped = true;
}

void Flip_D3D5_D7_ValuesOnly() {
  top_rows += turn * 10616994ULL;
  left_columns += turn * 708ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538998ULL;
  bottom_diag9s += turn * 45599178155361792ULL;
  bottom_diag7s += turn * 15199648742768640ULL;
  bottom_rows += turn * 45598946237743104ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D3D5_D7() {
  Flip_D3D5_D7_ValuesOnly();
}

void Flip_D3D5_D7() {
  Flip_D3D5_D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D5_D7;
  flipped = true;
}

void Flip_D3D6_ValuesOnly() {
  top_rows += turn * 10616994ULL;
  left_columns += turn * 720ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538998ULL;
  bottom_diag9s += turn * 45599178156539904ULL;
  bottom_diag7s += turn * 15199726051786752ULL;
  bottom_rows += turn * 45599642011828224ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D3D6() {
  Flip_D3D6_ValuesOnly();
}

void Flip_D3D6() {
  Flip_D3D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D6;
  flipped = true;
}

void Flip_D3D7_ValuesOnly() {
  top_rows += turn * 10616994ULL;
  left_columns += turn * 726ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3538998ULL;
  bottom_diag9s += turn * 45599178156541440ULL;
  bottom_diag7s += turn * 15199726052179968ULL;
  bottom_rows += turn * 45599642022445056ULL;
  disk_difference += turn * 10;
}

void UndoFlip_D3D7() {
  Flip_D3D7_ValuesOnly();
}

void Flip_D3D7() {
  Flip_D3D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D7;
  flipped = true;
}

void Flip_D4_D6_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 180ULL;
  top_diag7s += turn * 54ULL;
  bottom_diag9s += turn * 45598946228305920ULL;
  bottom_diag7s += turn * 77309411328ULL;
  bottom_rows += turn * 695784701952ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D4_D6() {
  Flip_D4_D6_ValuesOnly();
}

void Flip_D4_D6() {
  Flip_D4_D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_D6;
  flipped = true;
}

void Flip_D4_D6D7_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 186ULL;
  top_diag7s += turn * 54ULL;
  bottom_diag9s += turn * 45598946228307456ULL;
  bottom_diag7s += turn * 77309804544ULL;
  bottom_rows += turn * 695795318784ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D4_D6D7() {
  Flip_D4_D6D7_ValuesOnly();
}

void Flip_D4_D6D7() {
  Flip_D4_D6D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_D6D7;
  flipped = true;
}

void Flip_D4D5_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 216ULL;
  top_diag7s += turn * 54ULL;
  bottom_diag9s += turn * 45599178155360256ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D4D5() {
  Flip_D4D5_ValuesOnly();
}

void Flip_D4D5() {
  Flip_D4D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4D5;
  flipped = true;
}

void Flip_D4D5_D7_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 222ULL;
  top_diag7s += turn * 54ULL;
  bottom_diag9s += turn * 45599178155361792ULL;
  bottom_diag7s += turn * 15199648742768640ULL;
  bottom_rows += turn * 45598946237743104ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D4D5_D7() {
  Flip_D4D5_D7_ValuesOnly();
}

void Flip_D4D5_D7() {
  Flip_D4D5_D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4D5_D7;
  flipped = true;
}

void Flip_D4D6_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 234ULL;
  top_diag7s += turn * 54ULL;
  bottom_diag9s += turn * 45599178156539904ULL;
  bottom_diag7s += turn * 15199726051786752ULL;
  bottom_rows += turn * 45599642011828224ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D4D6() {
  Flip_D4D6_ValuesOnly();
}

void Flip_D4D6() {
  Flip_D4D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4D6;
  flipped = true;
}

void Flip_D4D7_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 240ULL;
  top_diag7s += turn * 54ULL;
  bottom_diag9s += turn * 45599178156541440ULL;
  bottom_diag7s += turn * 15199726052179968ULL;
  bottom_rows += turn * 45599642022445056ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D4D7() {
  Flip_D4D7_ValuesOnly();
}

void Flip_D4D7() {
  Flip_D4D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4D7;
  flipped = true;
}

void Flip_D5_D7_ValuesOnly() {
  left_columns += turn * 60ULL;
  bottom_diag9s += turn * 231928235520ULL;
  bottom_diag7s += turn * 15199648742768640ULL;
  bottom_rows += turn * 45598946237743104ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D5_D7() {
  Flip_D5_D7_ValuesOnly();
}

void Flip_D5_D7() {
  Flip_D5_D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5_D7;
  flipped = true;
}

void Flip_D5D6_ValuesOnly() {
  left_columns += turn * 72ULL;
  bottom_diag9s += turn * 231929413632ULL;
  bottom_diag7s += turn * 15199726051786752ULL;
  bottom_rows += turn * 45599642011828224ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D5D6() {
  Flip_D5D6_ValuesOnly();
}

void Flip_D5D6() {
  Flip_D5D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5D6;
  flipped = true;
}

void Flip_D5D7_ValuesOnly() {
  left_columns += turn * 78ULL;
  bottom_diag9s += turn * 231929415168ULL;
  bottom_diag7s += turn * 15199726052179968ULL;
  bottom_rows += turn * 45599642022445056ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D5D7() {
  Flip_D5D7_ValuesOnly();
}

void Flip_D5D7() {
  Flip_D5D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5D7;
  flipped = true;
}

void Flip_D6D7_ValuesOnly() {
  left_columns += turn * 24ULL;
  bottom_diag9s += turn * 1181184ULL;
  bottom_diag7s += turn * 77309804544ULL;
  bottom_rows += turn * 695795318784ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D6D7() {
  Flip_D6D7_ValuesOnly();
}

void Flip_D6D7() {
  Flip_D6D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6D7;
  flipped = true;
}

void Flip_E2_E4_ValuesOnly() {
  top_rows += turn * 231928234038ULL;
  top_diag9s += turn * 59373627899958ULL;
  top_diag7s += turn * 10616832ULL;
  right_columns += turn * 455989462271262720ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E2_E4() {
  Flip_E2_E4_ValuesOnly();
}

void Flip_E2_E4() {
  Flip_E2_E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_E4;
  flipped = true;
}

void Flip_E2_E4E5_ValuesOnly() {
  top_rows += turn * 231928234038ULL;
  top_diag9s += turn * 59373627899958ULL;
  top_diag7s += turn * 10616832ULL;
  right_columns += turn * 471189111013638144ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 45599178155360256ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E2_E4E5() {
  Flip_E2_E4E5_ValuesOnly();
}

void Flip_E2_E4E5() {
  Flip_E2_E4E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_E4E5;
  flipped = true;
}

void Flip_E2_E4E6_ValuesOnly() {
  top_rows += turn * 231928234038ULL;
  top_diag9s += turn * 59373627899958ULL;
  top_diag7s += turn * 10616832ULL;
  right_columns += turn * 476255660594429952ULL;
  bottom_diag9s += turn * 15199726051786752ULL;
  bottom_diag7s += turn * 45599178156539904ULL;
  bottom_rows += turn * 15199880670609408ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E2_E4E6() {
  Flip_E2_E4E6_ValuesOnly();
}

void Flip_E2_E4E6() {
  Flip_E2_E4E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_E4E6;
  flipped = true;
}

void Flip_E2_E4E7_ValuesOnly() {
  top_rows += turn * 231928234038ULL;
  top_diag9s += turn * 59373627899958ULL;
  top_diag7s += turn * 10616832ULL;
  right_columns += turn * 477944510454693888ULL;
  bottom_diag9s += turn * 15199726052179968ULL;
  bottom_diag7s += turn * 45599178156541440ULL;
  bottom_rows += turn * 15199880674148352ULL;
  disk_difference += turn * 10;
}

void UndoFlip_E2_E4E7() {
  Flip_E2_E4E7_ValuesOnly();
}

void Flip_E2_E4E7() {
  Flip_E2_E4E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_E4E7;
  flipped = true;
}

void Flip_E2E3_ValuesOnly() {
  top_rows += turn * 231931772928ULL;
  top_diag9s += turn * 59373631438848ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 547187354725515264ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E2E3() {
  Flip_E2E3_ValuesOnly();
}

void Flip_E2E3() {
  Flip_E2E3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E3;
  flipped = true;
}

void Flip_E2E3_E5_ValuesOnly() {
  top_rows += turn * 231931772928ULL;
  top_diag9s += turn * 59373631438848ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 562387003467890688ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E2E3_E5() {
  Flip_E2E3_E5_ValuesOnly();
}

void Flip_E2E3_E5() {
  Flip_E2E3_E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E3_E5;
  flipped = true;
}

void Flip_E2E3_E5E6_ValuesOnly() {
  top_rows += turn * 231931772928ULL;
  top_diag9s += turn * 59373631438848ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 567453553048682496ULL;
  bottom_diag9s += turn * 15199726051786752ULL;
  bottom_diag7s += turn * 231929413632ULL;
  bottom_rows += turn * 15199880670609408ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E2E3_E5E6() {
  Flip_E2E3_E5E6_ValuesOnly();
}

void Flip_E2E3_E5E6() {
  Flip_E2E3_E5E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E3_E5E6;
  flipped = true;
}

void Flip_E2E3_E5E7_ValuesOnly() {
  top_rows += turn * 231931772928ULL;
  top_diag9s += turn * 59373631438848ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 569142402908946432ULL;
  bottom_diag9s += turn * 15199726052179968ULL;
  bottom_diag7s += turn * 231929415168ULL;
  bottom_rows += turn * 15199880674148352ULL;
  disk_difference += turn * 10;
}

void UndoFlip_E2E3_E5E7() {
  Flip_E2E3_E5E7_ValuesOnly();
}

void Flip_E2E3_E5E7() {
  Flip_E2E3_E5E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E3_E5E7;
  flipped = true;
}

void Flip_E2E4_ValuesOnly() {
  top_rows += turn * 231931772982ULL;
  top_diag9s += turn * 59373631438902ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 592786300952641536ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E2E4() {
  Flip_E2E4_ValuesOnly();
}

void Flip_E2E4() {
  Flip_E2E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E4;
  flipped = true;
}

void Flip_E2E4_E6_ValuesOnly() {
  top_rows += turn * 231931772982ULL;
  top_diag9s += turn * 59373631438902ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 597852850533433344ULL;
  bottom_diag9s += turn * 77309411328ULL;
  bottom_diag7s += turn * 45598946228305920ULL;
  bottom_rows += turn * 231928233984ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E2E4_E6() {
  Flip_E2E4_E6_ValuesOnly();
}

void Flip_E2E4_E6() {
  Flip_E2E4_E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E4_E6;
  flipped = true;
}

void Flip_E2E4_E6E7_ValuesOnly() {
  top_rows += turn * 231931772982ULL;
  top_diag9s += turn * 59373631438902ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 599541700393697280ULL;
  bottom_diag9s += turn * 77309804544ULL;
  bottom_diag7s += turn * 45598946228307456ULL;
  bottom_rows += turn * 231931772928ULL;
  disk_difference += turn * 10;
}

void UndoFlip_E2E4_E6E7() {
  Flip_E2E4_E6E7_ValuesOnly();
}

void Flip_E2E4_E6E7() {
  Flip_E2E4_E6E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E4_E6E7;
  flipped = true;
}

void Flip_E2E5_ValuesOnly() {
  top_rows += turn * 231931772982ULL;
  top_diag9s += turn * 59373631438902ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 607985949695016960ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 45599178155360256ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E2E5() {
  Flip_E2E5_ValuesOnly();
}

void Flip_E2E5() {
  Flip_E2E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E5;
  flipped = true;
}

void Flip_E2E5_E7_ValuesOnly() {
  top_rows += turn * 231931772982ULL;
  top_diag9s += turn * 59373631438902ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 609674799555280896ULL;
  bottom_diag9s += turn * 15199648742768640ULL;
  bottom_diag7s += turn * 45599178155361792ULL;
  bottom_rows += turn * 15199648745914368ULL;
  disk_difference += turn * 10;
}

void UndoFlip_E2E5_E7() {
  Flip_E2E5_E7_ValuesOnly();
}

void Flip_E2E5_E7() {
  Flip_E2E5_E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E5_E7;
  flipped = true;
}

void Flip_E2E6_ValuesOnly() {
  top_rows += turn * 231931772982ULL;
  top_diag9s += turn * 59373631438902ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 613052499275808768ULL;
  bottom_diag9s += turn * 15199726051786752ULL;
  bottom_diag7s += turn * 45599178156539904ULL;
  bottom_rows += turn * 15199880670609408ULL;
  disk_difference += turn * 10;
}

void UndoFlip_E2E6() {
  Flip_E2E6_ValuesOnly();
}

void Flip_E2E6() {
  Flip_E2E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E6;
  flipped = true;
}

void Flip_E2E7_ValuesOnly() {
  top_rows += turn * 231931772982ULL;
  top_diag9s += turn * 59373631438902ULL;
  top_diag7s += turn * 10616994ULL;
  right_columns += turn * 614741349136072704ULL;
  bottom_diag9s += turn * 15199726052179968ULL;
  bottom_diag7s += turn * 45599178156541440ULL;
  bottom_rows += turn * 15199880674148352ULL;
  disk_difference += turn * 12;
}

void UndoFlip_E2E7() {
  Flip_E2E7_ValuesOnly();
}

void Flip_E2E7() {
  Flip_E2E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E7;
  flipped = true;
}

void Flip_E3_E5_ValuesOnly() {
  top_rows += turn * 3538944ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 151996487423754240ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E3_E5() {
  Flip_E3_E5_ValuesOnly();
}

void Flip_E3_E5() {
  Flip_E3_E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_E5;
  flipped = true;
}

void Flip_E3_E5E6_ValuesOnly() {
  top_rows += turn * 3538944ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 157063037004546048ULL;
  bottom_diag9s += turn * 15199726051786752ULL;
  bottom_diag7s += turn * 231929413632ULL;
  bottom_rows += turn * 15199880670609408ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E3_E5E6() {
  Flip_E3_E5E6_ValuesOnly();
}

void Flip_E3_E5E6() {
  Flip_E3_E5E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_E5E6;
  flipped = true;
}

void Flip_E3_E5E7_ValuesOnly() {
  top_rows += turn * 3538944ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 158751886864809984ULL;
  bottom_diag9s += turn * 15199726052179968ULL;
  bottom_diag7s += turn * 231929415168ULL;
  bottom_rows += turn * 15199880674148352ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E3_E5E7() {
  Flip_E3_E5E7_ValuesOnly();
}

void Flip_E3_E5E7() {
  Flip_E3_E5E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_E5E7;
  flipped = true;
}

void Flip_E3E4_ValuesOnly() {
  top_rows += turn * 3538998ULL;
  top_diag9s += turn * 3538998ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 182395784908505088ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E3E4() {
  Flip_E3E4_ValuesOnly();
}

void Flip_E3E4() {
  Flip_E3E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E4;
  flipped = true;
}

void Flip_E3E4_E6_ValuesOnly() {
  top_rows += turn * 3538998ULL;
  top_diag9s += turn * 3538998ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 187462334489296896ULL;
  bottom_diag9s += turn * 77309411328ULL;
  bottom_diag7s += turn * 45598946228305920ULL;
  bottom_rows += turn * 231928233984ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E3E4_E6() {
  Flip_E3E4_E6_ValuesOnly();
}

void Flip_E3E4_E6() {
  Flip_E3E4_E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E4_E6;
  flipped = true;
}

void Flip_E3E4_E6E7_ValuesOnly() {
  top_rows += turn * 3538998ULL;
  top_diag9s += turn * 3538998ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 189151184349560832ULL;
  bottom_diag9s += turn * 77309804544ULL;
  bottom_diag7s += turn * 45598946228307456ULL;
  bottom_rows += turn * 231931772928ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E3E4_E6E7() {
  Flip_E3E4_E6E7_ValuesOnly();
}

void Flip_E3E4_E6E7() {
  Flip_E3E4_E6E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E4_E6E7;
  flipped = true;
}

void Flip_E3E5_ValuesOnly() {
  top_rows += turn * 3538998ULL;
  top_diag9s += turn * 3538998ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 197595433650880512ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 45599178155360256ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E3E5() {
  Flip_E3E5_ValuesOnly();
}

void Flip_E3E5() {
  Flip_E3E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E5;
  flipped = true;
}

void Flip_E3E5_E7_ValuesOnly() {
  top_rows += turn * 3538998ULL;
  top_diag9s += turn * 3538998ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 199284283511144448ULL;
  bottom_diag9s += turn * 15199648742768640ULL;
  bottom_diag7s += turn * 45599178155361792ULL;
  bottom_rows += turn * 15199648745914368ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E3E5_E7() {
  Flip_E3E5_E7_ValuesOnly();
}

void Flip_E3E5_E7() {
  Flip_E3E5_E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E5_E7;
  flipped = true;
}

void Flip_E3E6_ValuesOnly() {
  top_rows += turn * 3538998ULL;
  top_diag9s += turn * 3538998ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 202661983231672320ULL;
  bottom_diag9s += turn * 15199726051786752ULL;
  bottom_diag7s += turn * 45599178156539904ULL;
  bottom_rows += turn * 15199880670609408ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E3E6() {
  Flip_E3E6_ValuesOnly();
}

void Flip_E3E6() {
  Flip_E3E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E6;
  flipped = true;
}

void Flip_E3E7_ValuesOnly() {
  top_rows += turn * 3538998ULL;
  top_diag9s += turn * 3538998ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 204350833091936256ULL;
  bottom_diag9s += turn * 15199726052179968ULL;
  bottom_diag7s += turn * 45599178156541440ULL;
  bottom_rows += turn * 15199880674148352ULL;
  disk_difference += turn * 10;
}

void UndoFlip_E3E7() {
  Flip_E3E7_ValuesOnly();
}

void Flip_E3E7() {
  Flip_E3E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E7;
  flipped = true;
}

void Flip_E4_E6_ValuesOnly() {
  top_rows += turn * 54ULL;
  top_diag9s += turn * 54ULL;
  right_columns += turn * 50665495807918080ULL;
  bottom_diag9s += turn * 77309411328ULL;
  bottom_diag7s += turn * 45598946228305920ULL;
  bottom_rows += turn * 231928233984ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E4_E6() {
  Flip_E4_E6_ValuesOnly();
}

void Flip_E4_E6() {
  Flip_E4_E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_E6;
  flipped = true;
}

void Flip_E4_E6E7_ValuesOnly() {
  top_rows += turn * 54ULL;
  top_diag9s += turn * 54ULL;
  right_columns += turn * 52354345668182016ULL;
  bottom_diag9s += turn * 77309804544ULL;
  bottom_diag7s += turn * 45598946228307456ULL;
  bottom_rows += turn * 231931772928ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E4_E6E7() {
  Flip_E4_E6E7_ValuesOnly();
}

void Flip_E4_E6E7() {
  Flip_E4_E6E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_E6E7;
  flipped = true;
}

void Flip_E4E5_ValuesOnly() {
  top_rows += turn * 54ULL;
  top_diag9s += turn * 54ULL;
  right_columns += turn * 60798594969501696ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 45599178155360256ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E4E5() {
  Flip_E4E5_ValuesOnly();
}

void Flip_E4E5() {
  Flip_E4E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4E5;
  flipped = true;
}

void Flip_E4E5_E7_ValuesOnly() {
  top_rows += turn * 54ULL;
  top_diag9s += turn * 54ULL;
  right_columns += turn * 62487444829765632ULL;
  bottom_diag9s += turn * 15199648742768640ULL;
  bottom_diag7s += turn * 45599178155361792ULL;
  bottom_rows += turn * 15199648745914368ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E4E5_E7() {
  Flip_E4E5_E7_ValuesOnly();
}

void Flip_E4E5_E7() {
  Flip_E4E5_E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4E5_E7;
  flipped = true;
}

void Flip_E4E6_ValuesOnly() {
  top_rows += turn * 54ULL;
  top_diag9s += turn * 54ULL;
  right_columns += turn * 65865144550293504ULL;
  bottom_diag9s += turn * 15199726051786752ULL;
  bottom_diag7s += turn * 45599178156539904ULL;
  bottom_rows += turn * 15199880670609408ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E4E6() {
  Flip_E4E6_ValuesOnly();
}

void Flip_E4E6() {
  Flip_E4E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4E6;
  flipped = true;
}

void Flip_E4E7_ValuesOnly() {
  top_rows += turn * 54ULL;
  top_diag9s += turn * 54ULL;
  right_columns += turn * 67553994410557440ULL;
  bottom_diag9s += turn * 15199726052179968ULL;
  bottom_diag7s += turn * 45599178156541440ULL;
  bottom_rows += turn * 15199880674148352ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E4E7() {
  Flip_E4E7_ValuesOnly();
}

void Flip_E4E7() {
  Flip_E4E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4E7;
  flipped = true;
}

void Flip_E5_E7_ValuesOnly() {
  right_columns += turn * 16888498602639360ULL;
  bottom_diag9s += turn * 15199648742768640ULL;
  bottom_diag7s += turn * 231928235520ULL;
  bottom_rows += turn * 15199648745914368ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E5_E7() {
  Flip_E5_E7_ValuesOnly();
}

void Flip_E5_E7() {
  Flip_E5_E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5_E7;
  flipped = true;
}

void Flip_E5E6_ValuesOnly() {
  right_columns += turn * 20266198323167232ULL;
  bottom_diag9s += turn * 15199726051786752ULL;
  bottom_diag7s += turn * 231929413632ULL;
  bottom_rows += turn * 15199880670609408ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E5E6() {
  Flip_E5E6_ValuesOnly();
}

void Flip_E5E6() {
  Flip_E5E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5E6;
  flipped = true;
}

void Flip_E5E7_ValuesOnly() {
  right_columns += turn * 21955048183431168ULL;
  bottom_diag9s += turn * 15199726052179968ULL;
  bottom_diag7s += turn * 231929415168ULL;
  bottom_rows += turn * 15199880674148352ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E5E7() {
  Flip_E5E7_ValuesOnly();
}

void Flip_E5E7() {
  Flip_E5E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5E7;
  flipped = true;
}

void Flip_E6E7_ValuesOnly() {
  right_columns += turn * 6755399441055744ULL;
  bottom_diag9s += turn * 77309804544ULL;
  bottom_diag7s += turn * 1181184ULL;
  bottom_rows += turn * 231931772928ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E6E7() {
  Flip_E6E7_ValuesOnly();
}

void Flip_E6E7() {
  Flip_E6E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6E7;
  flipped = true;
}

void Flip_F2_F4_ValuesOnly() {
  top_rows += turn * 77309411346ULL;
  top_diag9s += turn * 5066549581971456ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 6957847019520ULL;
  bottom_diag7s += turn * 695784701952ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F2_F4() {
  Flip_F2_F4_ValuesOnly();
}

void Flip_F2_F4() {
  Flip_F2_F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_F4;
  flipped = true;
}

void Flip_F2_F4F5_ValuesOnly() {
  top_rows += turn * 77309411346ULL;
  top_diag9s += turn * 5066549581971474ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 7189775253504ULL;
  bottom_diag7s += turn * 695788240896ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F2_F4F5() {
  Flip_F2_F4F5_ValuesOnly();
}

void Flip_F2_F4F5() {
  Flip_F2_F4F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_F4F5;
  flipped = true;
}

void Flip_F2_F4F6_ValuesOnly() {
  top_rows += turn * 77309411346ULL;
  top_diag9s += turn * 5066549581971474ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 7267084664832ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 695788245504ULL;
  bottom_rows += turn * 5066626890203136ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F2_F4F6() {
  Flip_F2_F4F6_ValuesOnly();
}

void Flip_F2_F4F6() {
  Flip_F2_F4F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_F4F6;
  flipped = true;
}

void Flip_F2_F4F7_ValuesOnly() {
  top_rows += turn * 77309411346ULL;
  top_diag9s += turn * 5066549581971474ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 7292854468608ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 695788245510ULL;
  bottom_rows += turn * 5066626891382784ULL;
  disk_difference += turn * 10;
}

void UndoFlip_F2_F4F7() {
  Flip_F2_F4F7_ValuesOnly();
}

void Flip_F2_F4F7() {
  Flip_F2_F4F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_F4F7;
  flipped = true;
}

void Flip_F2F3_ValuesOnly() {
  top_rows += turn * 77310590976ULL;
  top_diag9s += turn * 5086340790091776ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 8349416423424ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F2F3() {
  Flip_F2F3_ValuesOnly();
}

void Flip_F2F3() {
  Flip_F2F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F3;
  flipped = true;
}

void Flip_F2F3_F5_ValuesOnly() {
  top_rows += turn * 77310590976ULL;
  top_diag9s += turn * 5086340790091794ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 8581344657408ULL;
  bottom_diag7s += turn * 136796838684917760ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F2F3_F5() {
  Flip_F2F3_F5_ValuesOnly();
}

void Flip_F2F3_F5() {
  Flip_F2F3_F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F3_F5;
  flipped = true;
}

void Flip_F2F3_F5F6_ValuesOnly() {
  top_rows += turn * 77310590976ULL;
  top_diag9s += turn * 5086340790091794ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 8658654068736ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 136796838684922368ULL;
  bottom_rows += turn * 5066626890203136ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F2F3_F5F6() {
  Flip_F2F3_F5F6_ValuesOnly();
}

void Flip_F2F3_F5F6() {
  Flip_F2F3_F5F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F3_F5F6;
  flipped = true;
}

void Flip_F2F3_F5F7_ValuesOnly() {
  top_rows += turn * 77310590976ULL;
  top_diag9s += turn * 5086340790091794ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 8684423872512ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 136796838684922374ULL;
  bottom_rows += turn * 5066626891382784ULL;
  disk_difference += turn * 10;
}

void UndoFlip_F2F3_F5F7() {
  Flip_F2F3_F5F7_ValuesOnly();
}

void Flip_F2F3_F5F7() {
  Flip_F2F3_F5F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F3_F5F7;
  flipped = true;
}

void Flip_F2F4_ValuesOnly() {
  top_rows += turn * 77310590994ULL;
  top_diag9s += turn * 5086340791271424ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 9045201125376ULL;
  bottom_diag7s += turn * 136797534466080768ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F2F4() {
  Flip_F2F4_ValuesOnly();
}

void Flip_F2F4() {
  Flip_F2F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F4;
  flipped = true;
}

void Flip_F2F4_F6_ValuesOnly() {
  top_rows += turn * 77310590994ULL;
  top_diag9s += turn * 5086340791271424ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 9122510536704ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 136797534466085376ULL;
  bottom_rows += turn * 77309411328ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F2F4_F6() {
  Flip_F2F4_F6_ValuesOnly();
}

void Flip_F2F4_F6() {
  Flip_F2F4_F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F4_F6;
  flipped = true;
}

void Flip_F2F4_F6F7_ValuesOnly() {
  top_rows += turn * 77310590994ULL;
  top_diag9s += turn * 5086340791271424ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 9148280340480ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 136797534466085382ULL;
  bottom_rows += turn * 77310590976ULL;
  disk_difference += turn * 10;
}

void UndoFlip_F2F4_F6F7() {
  Flip_F2F4_F6F7_ValuesOnly();
}

void Flip_F2F4_F6F7() {
  Flip_F2F4_F6F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F4_F6F7;
  flipped = true;
}

void Flip_F2F5_ValuesOnly() {
  top_rows += turn * 77310590994ULL;
  top_diag9s += turn * 5086340791271442ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 9277129359360ULL;
  bottom_diag7s += turn * 136797534469619712ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F2F5() {
  Flip_F2F5_ValuesOnly();
}

void Flip_F2F5() {
  Flip_F2F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F5;
  flipped = true;
}

void Flip_F2F5_F7_ValuesOnly() {
  top_rows += turn * 77310590994ULL;
  top_diag9s += turn * 5086340791271442ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 9302899163136ULL;
  bottom_diag9s += turn * 25769803776ULL;
  bottom_diag7s += turn * 136797534469619718ULL;
  bottom_rows += turn * 5066549581971456ULL;
  disk_difference += turn * 10;
}

void UndoFlip_F2F5_F7() {
  Flip_F2F5_F7_ValuesOnly();
}

void Flip_F2F5_F7() {
  Flip_F2F5_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F5_F7;
  flipped = true;
}

void Flip_F2F6_ValuesOnly() {
  top_rows += turn * 77310590994ULL;
  top_diag9s += turn * 5086340791271442ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 9354438770688ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 136797534469624320ULL;
  bottom_rows += turn * 5066626890203136ULL;
  disk_difference += turn * 10;
}

void UndoFlip_F2F6() {
  Flip_F2F6_ValuesOnly();
}

void Flip_F2F6() {
  Flip_F2F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F6;
  flipped = true;
}

void Flip_F2F7_ValuesOnly() {
  top_rows += turn * 77310590994ULL;
  top_diag9s += turn * 5086340791271442ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 9380208574464ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 136797534469624326ULL;
  bottom_rows += turn * 5066626891382784ULL;
  disk_difference += turn * 12;
}

void UndoFlip_F2F7() {
  Flip_F2F7_ValuesOnly();
}

void Flip_F2F7() {
  Flip_F2F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F7;
  flipped = true;
}

void Flip_F3_F5_ValuesOnly() {
  top_rows += turn * 1179648ULL;
  top_diag9s += turn * 19791209299986ULL;
  right_columns += turn * 2319282339840ULL;
  bottom_diag7s += turn * 136796838684917760ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F3_F5() {
  Flip_F3_F5_ValuesOnly();
}

void Flip_F3_F5() {
  Flip_F3_F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_F5;
  flipped = true;
}

void Flip_F3_F5F6_ValuesOnly() {
  top_rows += turn * 1179648ULL;
  top_diag9s += turn * 19791209299986ULL;
  right_columns += turn * 2396591751168ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 136796838684922368ULL;
  bottom_rows += turn * 5066626890203136ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F3_F5F6() {
  Flip_F3_F5F6_ValuesOnly();
}

void Flip_F3_F5F6() {
  Flip_F3_F5F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_F5F6;
  flipped = true;
}

void Flip_F3_F5F7_ValuesOnly() {
  top_rows += turn * 1179648ULL;
  top_diag9s += turn * 19791209299986ULL;
  right_columns += turn * 2422361554944ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 136796838684922374ULL;
  bottom_rows += turn * 5066626891382784ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F3_F5F7() {
  Flip_F3_F5F7_ValuesOnly();
}

void Flip_F3_F5F7() {
  Flip_F3_F5F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_F5F7;
  flipped = true;
}

void Flip_F3F4_ValuesOnly() {
  top_rows += turn * 1179666ULL;
  top_diag9s += turn * 19791210479616ULL;
  right_columns += turn * 2783138807808ULL;
  bottom_diag7s += turn * 136797534466080768ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F3F4() {
  Flip_F3F4_ValuesOnly();
}

void Flip_F3F4() {
  Flip_F3F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F4;
  flipped = true;
}

void Flip_F3F4_F6_ValuesOnly() {
  top_rows += turn * 1179666ULL;
  top_diag9s += turn * 19791210479616ULL;
  right_columns += turn * 2860448219136ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 136797534466085376ULL;
  bottom_rows += turn * 77309411328ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F3F4_F6() {
  Flip_F3F4_F6_ValuesOnly();
}

void Flip_F3F4_F6() {
  Flip_F3F4_F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F4_F6;
  flipped = true;
}

void Flip_F3F4_F6F7_ValuesOnly() {
  top_rows += turn * 1179666ULL;
  top_diag9s += turn * 19791210479616ULL;
  right_columns += turn * 2886218022912ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 136797534466085382ULL;
  bottom_rows += turn * 77310590976ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F3F4_F6F7() {
  Flip_F3F4_F6F7_ValuesOnly();
}

void Flip_F3F4_F6F7() {
  Flip_F3F4_F6F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F4_F6F7;
  flipped = true;
}

void Flip_F3F5_ValuesOnly() {
  top_rows += turn * 1179666ULL;
  top_diag9s += turn * 19791210479634ULL;
  right_columns += turn * 3015067041792ULL;
  bottom_diag7s += turn * 136797534469619712ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F3F5() {
  Flip_F3F5_ValuesOnly();
}

void Flip_F3F5() {
  Flip_F3F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F5;
  flipped = true;
}

void Flip_F3F5_F7_ValuesOnly() {
  top_rows += turn * 1179666ULL;
  top_diag9s += turn * 19791210479634ULL;
  right_columns += turn * 3040836845568ULL;
  bottom_diag9s += turn * 25769803776ULL;
  bottom_diag7s += turn * 136797534469619718ULL;
  bottom_rows += turn * 5066549581971456ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F3F5_F7() {
  Flip_F3F5_F7_ValuesOnly();
}

void Flip_F3F5_F7() {
  Flip_F3F5_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F5_F7;
  flipped = true;
}

void Flip_F3F6_ValuesOnly() {
  top_rows += turn * 1179666ULL;
  top_diag9s += turn * 19791210479634ULL;
  right_columns += turn * 3092376453120ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 136797534469624320ULL;
  bottom_rows += turn * 5066626890203136ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F3F6() {
  Flip_F3F6_ValuesOnly();
}

void Flip_F3F6() {
  Flip_F3F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F6;
  flipped = true;
}

void Flip_F3F7_ValuesOnly() {
  top_rows += turn * 1179666ULL;
  top_diag9s += turn * 19791210479634ULL;
  right_columns += turn * 3118146256896ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 136797534469624326ULL;
  bottom_rows += turn * 5066626891382784ULL;
  disk_difference += turn * 10;
}

void UndoFlip_F3F7() {
  Flip_F3F7_ValuesOnly();
}

void Flip_F3F7() {
  Flip_F3F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F7;
  flipped = true;
}

void Flip_F4_F6_ValuesOnly() {
  top_rows += turn * 18ULL;
  top_diag9s += turn * 1179648ULL;
  right_columns += turn * 773094113280ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 695784706560ULL;
  bottom_rows += turn * 77309411328ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F4_F6() {
  Flip_F4_F6_ValuesOnly();
}

void Flip_F4_F6() {
  Flip_F4_F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4_F6;
  flipped = true;
}

void Flip_F4_F6F7_ValuesOnly() {
  top_rows += turn * 18ULL;
  top_diag9s += turn * 1179648ULL;
  right_columns += turn * 798863917056ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 695784706566ULL;
  bottom_rows += turn * 77310590976ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F4_F6F7() {
  Flip_F4_F6F7_ValuesOnly();
}

void Flip_F4_F6F7() {
  Flip_F4_F6F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4_F6F7;
  flipped = true;
}

void Flip_F4F5_ValuesOnly() {
  top_rows += turn * 18ULL;
  top_diag9s += turn * 1179666ULL;
  right_columns += turn * 927712935936ULL;
  bottom_diag7s += turn * 695788240896ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F4F5() {
  Flip_F4F5_ValuesOnly();
}

void Flip_F4F5() {
  Flip_F4F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4F5;
  flipped = true;
}

void Flip_F4F5_F7_ValuesOnly() {
  top_rows += turn * 18ULL;
  top_diag9s += turn * 1179666ULL;
  right_columns += turn * 953482739712ULL;
  bottom_diag9s += turn * 25769803776ULL;
  bottom_diag7s += turn * 695788240902ULL;
  bottom_rows += turn * 5066549581971456ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F4F5_F7() {
  Flip_F4F5_F7_ValuesOnly();
}

void Flip_F4F5_F7() {
  Flip_F4F5_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4F5_F7;
  flipped = true;
}

void Flip_F4F6_ValuesOnly() {
  top_rows += turn * 18ULL;
  top_diag9s += turn * 1179666ULL;
  right_columns += turn * 1005022347264ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 695788245504ULL;
  bottom_rows += turn * 5066626890203136ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F4F6() {
  Flip_F4F6_ValuesOnly();
}

void Flip_F4F6() {
  Flip_F4F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4F6;
  flipped = true;
}

void Flip_F4F7_ValuesOnly() {
  top_rows += turn * 18ULL;
  top_diag9s += turn * 1179666ULL;
  right_columns += turn * 1030792151040ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 695788245510ULL;
  bottom_rows += turn * 5066626891382784ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F4F7() {
  Flip_F4F7_ValuesOnly();
}

void Flip_F4F7() {
  Flip_F4F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4F7;
  flipped = true;
}

void Flip_F5_F7_ValuesOnly() {
  top_diag9s += turn * 18ULL;
  right_columns += turn * 257698037760ULL;
  bottom_diag9s += turn * 25769803776ULL;
  bottom_diag7s += turn * 3538950ULL;
  bottom_rows += turn * 5066549581971456ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F5_F7() {
  Flip_F5_F7_ValuesOnly();
}

void Flip_F5_F7() {
  Flip_F5_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5_F7;
  flipped = true;
}

void Flip_F5F6_ValuesOnly() {
  top_diag9s += turn * 18ULL;
  right_columns += turn * 309237645312ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 3543552ULL;
  bottom_rows += turn * 5066626890203136ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F5F6() {
  Flip_F5F6_ValuesOnly();
}

void Flip_F5F6() {
  Flip_F5F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5F6;
  flipped = true;
}

void Flip_F5F7_ValuesOnly() {
  top_diag9s += turn * 18ULL;
  right_columns += turn * 335007449088ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 3543558ULL;
  bottom_rows += turn * 5066626891382784ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F5F7() {
  Flip_F5F7_ValuesOnly();
}

void Flip_F5F7() {
  Flip_F5F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5F7;
  flipped = true;
}

void Flip_F6F7_ValuesOnly() {
  right_columns += turn * 103079215104ULL;
  bottom_diag9s += turn * 5066575350595584ULL;
  bottom_diag7s += turn * 4614ULL;
  bottom_rows += turn * 77310590976ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F6F7() {
  Flip_F6F7_ValuesOnly();
}

void Flip_F6F7() {
  Flip_F6F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F6F7;
  flipped = true;
}

void Flip_G2_G4_ValuesOnly() {
  top_rows += turn * 25769803782ULL;
  top_diag9s += turn * 432352161297334272ULL;
  right_columns += turn * 106168320ULL;
  bottom_diag7s += turn * 410390516054753280ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G2_G4() {
  Flip_G2_G4_ValuesOnly();
}

void Flip_G2_G4() {
  Flip_G2_G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_G4;
  flipped = true;
}

void Flip_G2_G4G5_ValuesOnly() {
  top_rows += turn * 25769803782ULL;
  top_diag9s += turn * 432352161297727488ULL;
  right_columns += turn * 109707264ULL;
  bottom_diag7s += turn * 410390516054767104ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G2_G4G5() {
  Flip_G2_G4G5_ValuesOnly();
}

void Flip_G2_G4G5() {
  Flip_G2_G4G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_G4G5;
  flipped = true;
}

void Flip_G2_G4G6_ValuesOnly() {
  top_rows += turn * 25769803782ULL;
  top_diag9s += turn * 432352161297727494ULL;
  right_columns += turn * 110886912ULL;
  bottom_diag7s += turn * 410390516054767122ULL;
  bottom_rows += turn * 1688875630067712ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G2_G4G6() {
  Flip_G2_G4G6_ValuesOnly();
}

void Flip_G2_G4G6() {
  Flip_G2_G4G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_G4G6;
  flipped = true;
}

void Flip_G2_G4G7_ValuesOnly() {
  top_rows += turn * 25769803782ULL;
  top_diag9s += turn * 432352161297727494ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 111280128ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 410390516054767122ULL;
  bottom_rows += turn * 1688875630460928ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G2_G4G7() {
  Flip_G2_G4G7_ValuesOnly();
}

void Flip_G2_G4G7() {
  Flip_G2_G4G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_G4G7;
  flipped = true;
}

void Flip_G2G3_ValuesOnly() {
  top_rows += turn * 25770196992ULL;
  top_diag9s += turn * 434034414087831552ULL;
  right_columns += turn * 127401984ULL;
  bottom_diag7s += turn * 410392603398242304ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G2G3() {
  Flip_G2G3_ValuesOnly();
}

void Flip_G2G3() {
  Flip_G2G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G3;
  flipped = true;
}

void Flip_G2G3_G5_ValuesOnly() {
  top_rows += turn * 25770196992ULL;
  top_diag9s += turn * 434034414088224768ULL;
  right_columns += turn * 130940928ULL;
  bottom_diag7s += turn * 410392603398256128ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G2G3_G5() {
  Flip_G2G3_G5_ValuesOnly();
}

void Flip_G2G3_G5() {
  Flip_G2G3_G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G3_G5;
  flipped = true;
}

void Flip_G2G3_G5G6_ValuesOnly() {
  top_rows += turn * 25770196992ULL;
  top_diag9s += turn * 434034414088224774ULL;
  right_columns += turn * 132120576ULL;
  bottom_diag7s += turn * 410392603398256146ULL;
  bottom_rows += turn * 1688875630067712ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G2G3_G5G6() {
  Flip_G2G3_G5G6_ValuesOnly();
}

void Flip_G2G3_G5G6() {
  Flip_G2G3_G5G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G3_G5G6;
  flipped = true;
}

void Flip_G2G3_G5G7_ValuesOnly() {
  top_rows += turn * 25770196992ULL;
  top_diag9s += turn * 434034414088224774ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 132513792ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 410392603398256146ULL;
  bottom_rows += turn * 1688875630460928ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G2G3_G5G7() {
  Flip_G2G3_G5G7_ValuesOnly();
}

void Flip_G2G3_G5G7() {
  Flip_G2G3_G5G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G3_G5G7;
  flipped = true;
}

void Flip_G2G4_ValuesOnly() {
  top_rows += turn * 25770196998ULL;
  top_diag9s += turn * 434041011157598208ULL;
  right_columns += turn * 138018816ULL;
  bottom_diag7s += turn * 410392603408859136ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G2G4() {
  Flip_G2G4_ValuesOnly();
}

void Flip_G2G4() {
  Flip_G2G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G4;
  flipped = true;
}

void Flip_G2G4_G6_ValuesOnly() {
  top_rows += turn * 25770196998ULL;
  top_diag9s += turn * 434041011157598214ULL;
  right_columns += turn * 139198464ULL;
  bottom_diag7s += turn * 410392603408859154ULL;
  bottom_rows += turn * 25769803776ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G2G4_G6() {
  Flip_G2G4_G6_ValuesOnly();
}

void Flip_G2G4_G6() {
  Flip_G2G4_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G4_G6;
  flipped = true;
}

void Flip_G2G4_G6G7_ValuesOnly() {
  top_rows += turn * 25770196998ULL;
  top_diag9s += turn * 434041011157598214ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 139591680ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 410392603408859154ULL;
  bottom_rows += turn * 25770196992ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G2G4_G6G7() {
  Flip_G2G4_G6G7_ValuesOnly();
}

void Flip_G2G4_G6G7() {
  Flip_G2G4_G6G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G4_G6G7;
  flipped = true;
}

void Flip_G2G5_ValuesOnly() {
  top_rows += turn * 25770196998ULL;
  top_diag9s += turn * 434041011157991424ULL;
  right_columns += turn * 141557760ULL;
  bottom_diag7s += turn * 410392603408872960ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G2G5() {
  Flip_G2G5_ValuesOnly();
}

void Flip_G2G5() {
  Flip_G2G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G5;
  flipped = true;
}

void Flip_G2G5_G7_ValuesOnly() {
  top_rows += turn * 25770196998ULL;
  top_diag9s += turn * 434041011157991424ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 141950976ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 410392603408872960ULL;
  bottom_rows += turn * 1688849860657152ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G2G5_G7() {
  Flip_G2G5_G7_ValuesOnly();
}

void Flip_G2G5_G7() {
  Flip_G2G5_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G5_G7;
  flipped = true;
}

void Flip_G2G6_ValuesOnly() {
  top_rows += turn * 25770196998ULL;
  top_diag9s += turn * 434041011157991430ULL;
  right_columns += turn * 142737408ULL;
  bottom_diag7s += turn * 410392603408872978ULL;
  bottom_rows += turn * 1688875630067712ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G2G6() {
  Flip_G2G6_ValuesOnly();
}

void Flip_G2G6() {
  Flip_G2G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G6;
  flipped = true;
}

void Flip_G2G7_ValuesOnly() {
  top_rows += turn * 25770196998ULL;
  top_diag9s += turn * 434041011157991430ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 143130624ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 410392603408872978ULL;
  bottom_rows += turn * 1688875630460928ULL;
  disk_difference += turn * 12;
}

void UndoFlip_G2G7() {
  Flip_G2G7_ValuesOnly();
}

void Flip_G2G7() {
  Flip_G2G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G7;
  flipped = true;
}

void Flip_G3_G5_ValuesOnly() {
  top_rows += turn * 393216ULL;
  top_diag9s += turn * 1688849860657152ULL;
  right_columns += turn * 35389440ULL;
  bottom_diag7s += turn * 2087354119680ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G3_G5() {
  Flip_G3_G5_ValuesOnly();
}

void Flip_G3_G5() {
  Flip_G3_G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_G5;
  flipped = true;
}

void Flip_G3_G5G6_ValuesOnly() {
  top_rows += turn * 393216ULL;
  top_diag9s += turn * 1688849860657158ULL;
  right_columns += turn * 36569088ULL;
  bottom_diag7s += turn * 2087354119698ULL;
  bottom_rows += turn * 1688875630067712ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G3_G5G6() {
  Flip_G3_G5G6_ValuesOnly();
}

void Flip_G3_G5G6() {
  Flip_G3_G5G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_G5G6;
  flipped = true;
}

void Flip_G3_G5G7_ValuesOnly() {
  top_rows += turn * 393216ULL;
  top_diag9s += turn * 1688849860657158ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 36962304ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 2087354119698ULL;
  bottom_rows += turn * 1688875630460928ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G3_G5G7() {
  Flip_G3_G5G7_ValuesOnly();
}

void Flip_G3_G5G7() {
  Flip_G3_G5G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_G5G7;
  flipped = true;
}

void Flip_G3G4_ValuesOnly() {
  top_rows += turn * 393222ULL;
  top_diag9s += turn * 1695446930030592ULL;
  right_columns += turn * 42467328ULL;
  bottom_diag7s += turn * 2087364722688ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G3G4() {
  Flip_G3G4_ValuesOnly();
}

void Flip_G3G4() {
  Flip_G3G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G4;
  flipped = true;
}

void Flip_G3G4_G6_ValuesOnly() {
  top_rows += turn * 393222ULL;
  top_diag9s += turn * 1695446930030598ULL;
  right_columns += turn * 43646976ULL;
  bottom_diag7s += turn * 2087364722706ULL;
  bottom_rows += turn * 25769803776ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G3G4_G6() {
  Flip_G3G4_G6_ValuesOnly();
}

void Flip_G3G4_G6() {
  Flip_G3G4_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G4_G6;
  flipped = true;
}

void Flip_G3G4_G6G7_ValuesOnly() {
  top_rows += turn * 393222ULL;
  top_diag9s += turn * 1695446930030598ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 44040192ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 2087364722706ULL;
  bottom_rows += turn * 25770196992ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G3G4_G6G7() {
  Flip_G3G4_G6G7_ValuesOnly();
}

void Flip_G3G4_G6G7() {
  Flip_G3G4_G6G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G4_G6G7;
  flipped = true;
}

void Flip_G3G5_ValuesOnly() {
  top_rows += turn * 393222ULL;
  top_diag9s += turn * 1695446930423808ULL;
  right_columns += turn * 46006272ULL;
  bottom_diag7s += turn * 2087364736512ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G3G5() {
  Flip_G3G5_ValuesOnly();
}

void Flip_G3G5() {
  Flip_G3G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G5;
  flipped = true;
}

void Flip_G3G5_G7_ValuesOnly() {
  top_rows += turn * 393222ULL;
  top_diag9s += turn * 1695446930423808ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 46399488ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 2087364736512ULL;
  bottom_rows += turn * 1688849860657152ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G3G5_G7() {
  Flip_G3G5_G7_ValuesOnly();
}

void Flip_G3G5_G7() {
  Flip_G3G5_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G5_G7;
  flipped = true;
}

void Flip_G3G6_ValuesOnly() {
  top_rows += turn * 393222ULL;
  top_diag9s += turn * 1695446930423814ULL;
  right_columns += turn * 47185920ULL;
  bottom_diag7s += turn * 2087364736530ULL;
  bottom_rows += turn * 1688875630067712ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G3G6() {
  Flip_G3G6_ValuesOnly();
}

void Flip_G3G6() {
  Flip_G3G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G6;
  flipped = true;
}

void Flip_G3G7_ValuesOnly() {
  top_rows += turn * 393222ULL;
  top_diag9s += turn * 1695446930423814ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 47579136ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 2087364736530ULL;
  bottom_rows += turn * 1688875630460928ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G3G7() {
  Flip_G3G7_ValuesOnly();
}

void Flip_G3G7() {
  Flip_G3G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G7;
  flipped = true;
}

void Flip_G4_G6_ValuesOnly() {
  top_rows += turn * 6ULL;
  top_diag9s += turn * 6597069766662ULL;
  right_columns += turn * 11796480ULL;
  bottom_diag7s += turn * 10616850ULL;
  bottom_rows += turn * 25769803776ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G4_G6() {
  Flip_G4_G6_ValuesOnly();
}

void Flip_G4_G6() {
  Flip_G4_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4_G6;
  flipped = true;
}

void Flip_G4_G6G7_ValuesOnly() {
  top_rows += turn * 6ULL;
  top_diag9s += turn * 6597069766662ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 12189696ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 10616850ULL;
  bottom_rows += turn * 25770196992ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G4_G6G7() {
  Flip_G4_G6G7_ValuesOnly();
}

void Flip_G4_G6G7() {
  Flip_G4_G6G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4_G6G7;
  flipped = true;
}

void Flip_G4G5_ValuesOnly() {
  top_rows += turn * 6ULL;
  top_diag9s += turn * 6597070159872ULL;
  right_columns += turn * 14155776ULL;
  bottom_diag7s += turn * 10630656ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G4G5() {
  Flip_G4G5_ValuesOnly();
}

void Flip_G4G5() {
  Flip_G4G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4G5;
  flipped = true;
}

void Flip_G4G5_G7_ValuesOnly() {
  top_rows += turn * 6ULL;
  top_diag9s += turn * 6597070159872ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 14548992ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 10630656ULL;
  bottom_rows += turn * 1688849860657152ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G4G5_G7() {
  Flip_G4G5_G7_ValuesOnly();
}

void Flip_G4G5_G7() {
  Flip_G4G5_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4G5_G7;
  flipped = true;
}

void Flip_G4G6_ValuesOnly() {
  top_rows += turn * 6ULL;
  top_diag9s += turn * 6597070159878ULL;
  right_columns += turn * 15335424ULL;
  bottom_diag7s += turn * 10630674ULL;
  bottom_rows += turn * 1688875630067712ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G4G6() {
  Flip_G4G6_ValuesOnly();
}

void Flip_G4G6() {
  Flip_G4G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4G6;
  flipped = true;
}

void Flip_G4G7_ValuesOnly() {
  top_rows += turn * 6ULL;
  top_diag9s += turn * 6597070159878ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 15728640ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 10630674ULL;
  bottom_rows += turn * 1688875630460928ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G4G7() {
  Flip_G4G7_ValuesOnly();
}

void Flip_G4G7() {
  Flip_G4G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4G7;
  flipped = true;
}

void Flip_G5_G7_ValuesOnly() {
  top_diag9s += turn * 393216ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 3932160ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 13824ULL;
  bottom_rows += turn * 1688849860657152ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G5_G7() {
  Flip_G5_G7_ValuesOnly();
}

void Flip_G5_G7() {
  Flip_G5_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5_G7;
  flipped = true;
}

void Flip_G5G6_ValuesOnly() {
  top_diag9s += turn * 393222ULL;
  right_columns += turn * 4718592ULL;
  bottom_diag7s += turn * 13842ULL;
  bottom_rows += turn * 1688875630067712ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G5G6() {
  Flip_G5G6_ValuesOnly();
}

void Flip_G5G6() {
  Flip_G5G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5G6;
  flipped = true;
}

void Flip_G5G7_ValuesOnly() {
  top_diag9s += turn * 393222ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 5111808ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 13842ULL;
  bottom_rows += turn * 1688875630460928ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G5G7() {
  Flip_G5G7_ValuesOnly();
}

void Flip_G5G7() {
  Flip_G5G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5G7;
  flipped = true;
}

void Flip_G6G7_ValuesOnly() {
  top_diag9s += turn * 6ULL;
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 1572864ULL;
  bottom_diag9s += turn * 1688849860263936ULL;
  bottom_diag7s += turn * 18ULL;
  bottom_rows += turn * 25770196992ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G6G7() {
  Flip_G6G7_ValuesOnly();
}

void Flip_G6G7() {
  Flip_G6G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G6G7;
  flipped = true;
}

void Flip_H2_ValuesOnly() {
  top_rows += turn * 8589934592ULL;
  right_columns += turn * 1458ULL;
  bottom_diag7s += turn * 6262062317568ULL;
  disk_difference += turn * 2;
}

void UndoFlip_H2() {
  Flip_H2_ValuesOnly();
}

void Flip_H2() {
  Flip_H2_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2;
  flipped = true;
}

void Flip_H2_H4_ValuesOnly() {
  top_rows += turn * 8589934594ULL;
  top_diag9s += turn * 562949953421312ULL;
  right_columns += turn * 1620ULL;
  bottom_diag7s += turn * 6262062359040ULL;
  disk_difference += turn * 4;
}

void UndoFlip_H2_H4() {
  Flip_H2_H4_ValuesOnly();
}

void Flip_H2_H4() {
  Flip_H2_H4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2_H4;
  flipped = true;
}

void Flip_H2_H4H5_ValuesOnly() {
  top_rows += turn * 8589934594ULL;
  top_diag9s += turn * 565148976676864ULL;
  right_columns += turn * 1674ULL;
  bottom_diag7s += turn * 6262062359094ULL;
  bottom_rows += turn * 562949953421312ULL;
  disk_difference += turn * 6;
}

void UndoFlip_H2_H4H5() {
  Flip_H2_H4H5_ValuesOnly();
}

void Flip_H2_H4H5() {
  Flip_H2_H4H5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2_H4H5;
  flipped = true;
}

void Flip_H2_H4H6_ValuesOnly() {
  top_rows += turn * 8589934594ULL;
  top_diag9s += turn * 565148976807936ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 1692ULL;
  bottom_diag7s += turn * 6262062359094ULL;
  bottom_rows += turn * 562958543355904ULL;
  disk_difference += turn * 8;
}

void UndoFlip_H2_H4H6() {
  Flip_H2_H4H6_ValuesOnly();
}

void Flip_H2_H4H6() {
  Flip_H2_H4H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2_H4H6;
  flipped = true;
}

void Flip_H2_H4H7_ValuesOnly() {
  top_rows += turn * 8589934594ULL;
  top_diag9s += turn * 565148976807938ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 1698ULL;
  bottom_diag7s += turn * 6262062359094ULL;
  bottom_rows += turn * 562958543486976ULL;
  disk_difference += turn * 10;
}

void UndoFlip_H2_H4H7() {
  Flip_H2_H4H7_ValuesOnly();
}

void Flip_H2_H4H7() {
  Flip_H2_H4H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2_H4H7;
  flipped = true;
}

void Flip_H2H3_ValuesOnly() {
  top_rows += turn * 8590065664ULL;
  top_diag9s += turn * 144115188075855872ULL;
  right_columns += turn * 1944ULL;
  bottom_diag7s += turn * 6262094168064ULL;
  disk_difference += turn * 4;
}

void UndoFlip_H2H3() {
  Flip_H2H3_ValuesOnly();
}

void Flip_H2H3() {
  Flip_H2H3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H3;
  flipped = true;
}

void Flip_H2H3_H5_ValuesOnly() {
  top_rows += turn * 8590065664ULL;
  top_diag9s += turn * 144117387099111424ULL;
  right_columns += turn * 1998ULL;
  bottom_diag7s += turn * 6262094168118ULL;
  bottom_rows += turn * 562949953421312ULL;
  disk_difference += turn * 6;
}

void UndoFlip_H2H3_H5() {
  Flip_H2H3_H5_ValuesOnly();
}

void Flip_H2H3_H5() {
  Flip_H2H3_H5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H3_H5;
  flipped = true;
}

void Flip_H2H3_H5H6_ValuesOnly() {
  top_rows += turn * 8590065664ULL;
  top_diag9s += turn * 144117387099242496ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 2016ULL;
  bottom_diag7s += turn * 6262094168118ULL;
  bottom_rows += turn * 562958543355904ULL;
  disk_difference += turn * 8;
}

void UndoFlip_H2H3_H5H6() {
  Flip_H2H3_H5H6_ValuesOnly();
}

void Flip_H2H3_H5H6() {
  Flip_H2H3_H5H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H3_H5H6;
  flipped = true;
}

void Flip_H2H3_H5H7_ValuesOnly() {
  top_rows += turn * 8590065664ULL;
  top_diag9s += turn * 144117387099242498ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 2022ULL;
  bottom_diag7s += turn * 6262094168118ULL;
  bottom_rows += turn * 562958543486976ULL;
  disk_difference += turn * 10;
}

void UndoFlip_H2H3_H5H7() {
  Flip_H2H3_H5H7_ValuesOnly();
}

void Flip_H2H3_H5H7() {
  Flip_H2H3_H5H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H3_H5H7;
  flipped = true;
}

void Flip_H2H4_ValuesOnly() {
  top_rows += turn * 8590065666ULL;
  top_diag9s += turn * 144678138029277184ULL;
  right_columns += turn * 2106ULL;
  bottom_diag7s += turn * 6262094209536ULL;
  disk_difference += turn * 6;
}

void UndoFlip_H2H4() {
  Flip_H2H4_ValuesOnly();
}

void Flip_H2H4() {
  Flip_H2H4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H4;
  flipped = true;
}

void Flip_H2H4_H6_ValuesOnly() {
  top_rows += turn * 8590065666ULL;
  top_diag9s += turn * 144678138029408256ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 2124ULL;
  bottom_diag7s += turn * 6262094209536ULL;
  bottom_rows += turn * 8589934592ULL;
  disk_difference += turn * 8;
}

void UndoFlip_H2H4_H6() {
  Flip_H2H4_H6_ValuesOnly();
}

void Flip_H2H4_H6() {
  Flip_H2H4_H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H4_H6;
  flipped = true;
}

void Flip_H2H4_H6H7_ValuesOnly() {
  top_rows += turn * 8590065666ULL;
  top_diag9s += turn * 144678138029408258ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 2130ULL;
  bottom_diag7s += turn * 6262094209536ULL;
  bottom_rows += turn * 8590065664ULL;
  disk_difference += turn * 10;
}

void UndoFlip_H2H4_H6H7() {
  Flip_H2H4_H6H7_ValuesOnly();
}

void Flip_H2H4_H6H7() {
  Flip_H2H4_H6H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H4_H6H7;
  flipped = true;
}

void Flip_H2H5_ValuesOnly() {
  top_rows += turn * 8590065666ULL;
  top_diag9s += turn * 144680337052532736ULL;
  right_columns += turn * 2160ULL;
  bottom_diag7s += turn * 6262094209590ULL;
  bottom_rows += turn * 562949953421312ULL;
  disk_difference += turn * 8;
}

void UndoFlip_H2H5() {
  Flip_H2H5_ValuesOnly();
}

void Flip_H2H5() {
  Flip_H2H5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H5;
  flipped = true;
}

void Flip_H2H5_H7_ValuesOnly() {
  top_rows += turn * 8590065666ULL;
  top_diag9s += turn * 144680337052532738ULL;
  right_columns += turn * 2166ULL;
  bottom_diag7s += turn * 6262094209590ULL;
  bottom_rows += turn * 562949953552384ULL;
  disk_difference += turn * 10;
}

void UndoFlip_H2H5_H7() {
  Flip_H2H5_H7_ValuesOnly();
}

void Flip_H2H5_H7() {
  Flip_H2H5_H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H5_H7;
  flipped = true;
}

void Flip_H2H6_ValuesOnly() {
  top_rows += turn * 8590065666ULL;
  top_diag9s += turn * 144680337052663808ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 2178ULL;
  bottom_diag7s += turn * 6262094209590ULL;
  bottom_rows += turn * 562958543355904ULL;
  disk_difference += turn * 10;
}

void UndoFlip_H2H6() {
  Flip_H2H6_ValuesOnly();
}

void Flip_H2H6() {
  Flip_H2H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H6;
  flipped = true;
}

void Flip_H2H7_ValuesOnly() {
  top_rows += turn * 8590065666ULL;
  top_diag9s += turn * 144680337052663810ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 2184ULL;
  bottom_diag7s += turn * 6262094209590ULL;
  bottom_rows += turn * 562958543486976ULL;
  disk_difference += turn * 12;
}

void UndoFlip_H2H7() {
  Flip_H2H7_ValuesOnly();
}

void Flip_H2H7() {
  Flip_H2H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H7;
  flipped = true;
}

void Flip_H3_ValuesOnly() {
  top_rows += turn * 131072ULL;
  top_diag9s += turn * 144115188075855872ULL;
  right_columns += turn * 486ULL;
  bottom_diag7s += turn * 31850496ULL;
  disk_difference += turn * 2;
}

void UndoFlip_H3() {
  Flip_H3_ValuesOnly();
}

void Flip_H3() {
  Flip_H3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3;
  flipped = true;
}

void Flip_H3_H5_ValuesOnly() {
  top_rows += turn * 131072ULL;
  top_diag9s += turn * 144117387099111424ULL;
  right_columns += turn * 540ULL;
  bottom_diag7s += turn * 31850550ULL;
  bottom_rows += turn * 562949953421312ULL;
  disk_difference += turn * 4;
}

void UndoFlip_H3_H5() {
  Flip_H3_H5_ValuesOnly();
}

void Flip_H3_H5() {
  Flip_H3_H5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3_H5;
  flipped = true;
}

void Flip_H3_H5H6_ValuesOnly() {
  top_rows += turn * 131072ULL;
  top_diag9s += turn * 144117387099242496ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 558ULL;
  bottom_diag7s += turn * 31850550ULL;
  bottom_rows += turn * 562958543355904ULL;
  disk_difference += turn * 6;
}

void UndoFlip_H3_H5H6() {
  Flip_H3_H5H6_ValuesOnly();
}

void Flip_H3_H5H6() {
  Flip_H3_H5H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3_H5H6;
  flipped = true;
}

void Flip_H3_H5H7_ValuesOnly() {
  top_rows += turn * 131072ULL;
  top_diag9s += turn * 144117387099242498ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 564ULL;
  bottom_diag7s += turn * 31850550ULL;
  bottom_rows += turn * 562958543486976ULL;
  disk_difference += turn * 8;
}

void UndoFlip_H3_H5H7() {
  Flip_H3_H5H7_ValuesOnly();
}

void Flip_H3_H5H7() {
  Flip_H3_H5H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3_H5H7;
  flipped = true;
}

void Flip_H3H4_ValuesOnly() {
  top_rows += turn * 131074ULL;
  top_diag9s += turn * 144678138029277184ULL;
  right_columns += turn * 648ULL;
  bottom_diag7s += turn * 31891968ULL;
  disk_difference += turn * 4;
}

void UndoFlip_H3H4() {
  Flip_H3H4_ValuesOnly();
}

void Flip_H3H4() {
  Flip_H3H4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H4;
  flipped = true;
}

void Flip_H3H4_H6_ValuesOnly() {
  top_rows += turn * 131074ULL;
  top_diag9s += turn * 144678138029408256ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 666ULL;
  bottom_diag7s += turn * 31891968ULL;
  bottom_rows += turn * 8589934592ULL;
  disk_difference += turn * 6;
}

void UndoFlip_H3H4_H6() {
  Flip_H3H4_H6_ValuesOnly();
}

void Flip_H3H4_H6() {
  Flip_H3H4_H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H4_H6;
  flipped = true;
}

void Flip_H3H4_H6H7_ValuesOnly() {
  top_rows += turn * 131074ULL;
  top_diag9s += turn * 144678138029408258ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 672ULL;
  bottom_diag7s += turn * 31891968ULL;
  bottom_rows += turn * 8590065664ULL;
  disk_difference += turn * 8;
}

void UndoFlip_H3H4_H6H7() {
  Flip_H3H4_H6H7_ValuesOnly();
}

void Flip_H3H4_H6H7() {
  Flip_H3H4_H6H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H4_H6H7;
  flipped = true;
}

void Flip_H3H5_ValuesOnly() {
  top_rows += turn * 131074ULL;
  top_diag9s += turn * 144680337052532736ULL;
  right_columns += turn * 702ULL;
  bottom_diag7s += turn * 31892022ULL;
  bottom_rows += turn * 562949953421312ULL;
  disk_difference += turn * 6;
}

void UndoFlip_H3H5() {
  Flip_H3H5_ValuesOnly();
}

void Flip_H3H5() {
  Flip_H3H5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H5;
  flipped = true;
}

void Flip_H3H5_H7_ValuesOnly() {
  top_rows += turn * 131074ULL;
  top_diag9s += turn * 144680337052532738ULL;
  right_columns += turn * 708ULL;
  bottom_diag7s += turn * 31892022ULL;
  bottom_rows += turn * 562949953552384ULL;
  disk_difference += turn * 8;
}

void UndoFlip_H3H5_H7() {
  Flip_H3H5_H7_ValuesOnly();
}

void Flip_H3H5_H7() {
  Flip_H3H5_H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H5_H7;
  flipped = true;
}

void Flip_H3H6_ValuesOnly() {
  top_rows += turn * 131074ULL;
  top_diag9s += turn * 144680337052663808ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 720ULL;
  bottom_diag7s += turn * 31892022ULL;
  bottom_rows += turn * 562958543355904ULL;
  disk_difference += turn * 8;
}

void UndoFlip_H3H6() {
  Flip_H3H6_ValuesOnly();
}

void Flip_H3H6() {
  Flip_H3H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H6;
  flipped = true;
}

void Flip_H3H7_ValuesOnly() {
  top_rows += turn * 131074ULL;
  top_diag9s += turn * 144680337052663810ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 726ULL;
  bottom_diag7s += turn * 31892022ULL;
  bottom_rows += turn * 562958543486976ULL;
  disk_difference += turn * 10;
}

void UndoFlip_H3H7() {
  Flip_H3H7_ValuesOnly();
}

void Flip_H3H7() {
  Flip_H3H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H7;
  flipped = true;
}

void Flip_H4_ValuesOnly() {
  top_rows += turn * 2ULL;
  top_diag9s += turn * 562949953421312ULL;
  right_columns += turn * 162ULL;
  bottom_diag7s += turn * 41472ULL;
  disk_difference += turn * 2;
}

void UndoFlip_H4() {
  Flip_H4_ValuesOnly();
}

void Flip_H4() {
  Flip_H4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4;
  flipped = true;
}

void Flip_H4_H6_ValuesOnly() {
  top_rows += turn * 2ULL;
  top_diag9s += turn * 562949953552384ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 180ULL;
  bottom_diag7s += turn * 41472ULL;
  bottom_rows += turn * 8589934592ULL;
  disk_difference += turn * 4;
}

void UndoFlip_H4_H6() {
  Flip_H4_H6_ValuesOnly();
}

void Flip_H4_H6() {
  Flip_H4_H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4_H6;
  flipped = true;
}

void Flip_H4_H6H7_ValuesOnly() {
  top_rows += turn * 2ULL;
  top_diag9s += turn * 562949953552386ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 186ULL;
  bottom_diag7s += turn * 41472ULL;
  bottom_rows += turn * 8590065664ULL;
  disk_difference += turn * 6;
}

void UndoFlip_H4_H6H7() {
  Flip_H4_H6H7_ValuesOnly();
}

void Flip_H4_H6H7() {
  Flip_H4_H6H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4_H6H7;
  flipped = true;
}

void Flip_H4H5_ValuesOnly() {
  top_rows += turn * 2ULL;
  top_diag9s += turn * 565148976676864ULL;
  right_columns += turn * 216ULL;
  bottom_diag7s += turn * 41526ULL;
  bottom_rows += turn * 562949953421312ULL;
  disk_difference += turn * 4;
}

void UndoFlip_H4H5() {
  Flip_H4H5_ValuesOnly();
}

void Flip_H4H5() {
  Flip_H4H5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4H5;
  flipped = true;
}

void Flip_H4H5_H7_ValuesOnly() {
  top_rows += turn * 2ULL;
  top_diag9s += turn * 565148976676866ULL;
  right_columns += turn * 222ULL;
  bottom_diag7s += turn * 41526ULL;
  bottom_rows += turn * 562949953552384ULL;
  disk_difference += turn * 6;
}

void UndoFlip_H4H5_H7() {
  Flip_H4H5_H7_ValuesOnly();
}

void Flip_H4H5_H7() {
  Flip_H4H5_H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4H5_H7;
  flipped = true;
}

void Flip_H4H6_ValuesOnly() {
  top_rows += turn * 2ULL;
  top_diag9s += turn * 565148976807936ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 234ULL;
  bottom_diag7s += turn * 41526ULL;
  bottom_rows += turn * 562958543355904ULL;
  disk_difference += turn * 6;
}

void UndoFlip_H4H6() {
  Flip_H4H6_ValuesOnly();
}

void Flip_H4H6() {
  Flip_H4H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4H6;
  flipped = true;
}

void Flip_H4H7_ValuesOnly() {
  top_rows += turn * 2ULL;
  top_diag9s += turn * 565148976807938ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 240ULL;
  bottom_diag7s += turn * 41526ULL;
  bottom_rows += turn * 562958543486976ULL;
  disk_difference += turn * 8;
}

void UndoFlip_H4H7() {
  Flip_H4H7_ValuesOnly();
}

void Flip_H4H7() {
  Flip_H4H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4H7;
  flipped = true;
}

void Flip_H5_ValuesOnly() {
  top_diag9s += turn * 2199023255552ULL;
  right_columns += turn * 54ULL;
  bottom_diag7s += turn * 54ULL;
  bottom_rows += turn * 562949953421312ULL;
  disk_difference += turn * 2;
}

void UndoFlip_H5() {
  Flip_H5_ValuesOnly();
}

void Flip_H5() {
  Flip_H5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H5;
  flipped = true;
}

void Flip_H5_H7_ValuesOnly() {
  top_diag9s += turn * 2199023255554ULL;
  right_columns += turn * 60ULL;
  bottom_diag7s += turn * 54ULL;
  bottom_rows += turn * 562949953552384ULL;
  disk_difference += turn * 4;
}

void UndoFlip_H5_H7() {
  Flip_H5_H7_ValuesOnly();
}

void Flip_H5_H7() {
  Flip_H5_H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H5_H7;
  flipped = true;
}

void Flip_H5H6_ValuesOnly() {
  top_diag9s += turn * 2199023386624ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 72ULL;
  bottom_diag7s += turn * 54ULL;
  bottom_rows += turn * 562958543355904ULL;
  disk_difference += turn * 4;
}

void UndoFlip_H5H6() {
  Flip_H5H6_ValuesOnly();
}

void Flip_H5H6() {
  Flip_H5H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H5H6;
  flipped = true;
}

void Flip_H5H7_ValuesOnly() {
  top_diag9s += turn * 2199023386626ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 78ULL;
  bottom_diag7s += turn * 54ULL;
  bottom_rows += turn * 562958543486976ULL;
  disk_difference += turn * 6;
}

void UndoFlip_H5H7() {
  Flip_H5H7_ValuesOnly();
}

void Flip_H5H7() {
  Flip_H5H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H5H7;
  flipped = true;
}

void Flip_H6_ValuesOnly() {
  top_diag9s += turn * 131072ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 18ULL;
  bottom_rows += turn * 8589934592ULL;
  disk_difference += turn * 2;
}

void UndoFlip_H6() {
  Flip_H6_ValuesOnly();
}

void Flip_H6() {
  Flip_H6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H6;
  flipped = true;
}

void Flip_H6H7_ValuesOnly() {
  top_diag9s += turn * 131074ULL;
  top_diag7s += turn * 77309411328ULL;
  right_columns += turn * 24ULL;
  bottom_rows += turn * 8590065664ULL;
  disk_difference += turn * 4;
}

void UndoFlip_H6H7() {
  Flip_H6H7_ValuesOnly();
}

void Flip_H6H7() {
  Flip_H6H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H6H7;
  flipped = true;
}

void Flip_H7_ValuesOnly() {
  top_diag9s += turn * 2ULL;
  right_columns += turn * 6ULL;
  bottom_rows += turn * 131072ULL;
  disk_difference += turn * 2;
}

void UndoFlip_H7() {
  Flip_H7_ValuesOnly();
}

void Flip_H7() {
  Flip_H7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H7;
  flipped = true;
}

void Flip_C2B3_ValuesOnly() {
  top_rows += turn * 2087449657344ULL;
  left_columns += turn * 2087449657344ULL;
  top_diag9s += turn * 486ULL;
  top_diag7s += turn * 6755399441055744ULL;
  bottom_diag9s += turn * 2087354105856ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C2B3() {
  Flip_C2B3_ValuesOnly();
}

void Flip_C2B3() {
  Flip_C2B3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2B3;
  flipped = true;
}

void Flip_D2_B4_ValuesOnly() {
  top_rows += turn * 695784703410ULL;
  left_columns += turn * 695784703410ULL;
  top_diag9s += turn * 10616832ULL;
  top_diag7s += turn * 65970697666560ULL;
  bottom_diag9s += turn * 10616832ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D2_B4() {
  Flip_D2_B4_ValuesOnly();
}

void Flip_D2_B4() {
  Flip_D2_B4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_B4;
  flipped = true;
}

void Flip_D2C3_ValuesOnly() {
  top_rows += turn * 695816552448ULL;
  left_columns += turn * 31851954ULL;
  top_diag9s += turn * 10616832ULL;
  top_diag7s += turn * 79164837199872ULL;
  bottom_diag9s += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D2C3() {
  Flip_D2C3_ValuesOnly();
}

void Flip_D2C3() {
  Flip_D2C3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2C3;
  flipped = true;
}

void Flip_D2B4_ValuesOnly() {
  top_rows += turn * 695816553906ULL;
  left_columns += turn * 695816553906ULL;
  top_diag9s += turn * 10616832ULL;
  top_diag7s += turn * 85761906966528ULL;
  bottom_diag9s += turn * 136796838691995648ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D2B4() {
  Flip_D2B4_ValuesOnly();
}

void Flip_D2B4() {
  Flip_D2B4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2B4;
  flipped = true;
}

void Flip_C3B4_ValuesOnly() {
  top_rows += turn * 31851954ULL;
  left_columns += turn * 695816552448ULL;
  top_diag7s += turn * 26388279066624ULL;
  bottom_diag9s += turn * 136796838691995648ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C3B4() {
  Flip_C3B4_ValuesOnly();
}

void Flip_C3B4() {
  Flip_C3B4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3B4;
  flipped = true;
}

void Flip_E2_C4_ValuesOnly() {
  top_rows += turn * 231928234470ULL;
  left_columns += turn * 10616832ULL;
  top_diag9s += turn * 59373627899904ULL;
  top_diag7s += turn * 11796480ULL;
  right_columns += turn * 410390516044136448ULL;
  bottom_diag9s += turn * 695784701952ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E2_C4() {
  Flip_E2_C4_ValuesOnly();
}

void Flip_E2_C4() {
  Flip_E2_C4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_C4;
  flipped = true;
}

void Flip_E2_C4B5_ValuesOnly() {
  top_rows += turn * 231928234470ULL;
  left_columns += turn * 231938850816ULL;
  top_diag9s += turn * 59373627899904ULL;
  top_diag7s += turn * 12189696ULL;
  right_columns += turn * 410390516044136448ULL;
  bottom_diag9s += turn * 695784715776ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E2_C4B5() {
  Flip_E2_C4B5_ValuesOnly();
}

void Flip_E2_C4B5() {
  Flip_E2_C4B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_C4B5;
  flipped = true;
}

void Flip_E2D3_ValuesOnly() {
  top_rows += turn * 231938850816ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 59373627900066ULL;
  top_diag7s += turn * 14155776ULL;
  right_columns += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E2D3() {
  Flip_E2D3_ValuesOnly();
}

void Flip_E2D3() {
  Flip_E2D3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2D3;
  flipped = true;
}

void Flip_E2D3_B5_ValuesOnly() {
  top_rows += turn * 231938850816ULL;
  left_columns += turn * 231928234470ULL;
  top_diag9s += turn * 59373627900066ULL;
  top_diag7s += turn * 14548992ULL;
  right_columns += turn * 410390516044136448ULL;
  bottom_diag9s += turn * 13824ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E2D3_B5() {
  Flip_E2D3_B5_ValuesOnly();
}

void Flip_E2D3_B5() {
  Flip_E2D3_B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2D3_B5;
  flipped = true;
}

void Flip_E2C4_ValuesOnly() {
  top_rows += turn * 231938851302ULL;
  left_columns += turn * 10617318ULL;
  top_diag9s += turn * 59373627900066ULL;
  top_diag7s += turn * 15335424ULL;
  right_columns += turn * 410390516044136448ULL;
  bottom_diag9s += turn * 695784701952ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E2C4() {
  Flip_E2C4_ValuesOnly();
}

void Flip_E2C4() {
  Flip_E2C4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2C4;
  flipped = true;
}

void Flip_E2B5_ValuesOnly() {
  top_rows += turn * 231938851302ULL;
  left_columns += turn * 231938851302ULL;
  top_diag9s += turn * 59373627900066ULL;
  top_diag7s += turn * 15728640ULL;
  right_columns += turn * 410390516044136448ULL;
  bottom_diag9s += turn * 695784715776ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E2B5() {
  Flip_E2B5_ValuesOnly();
}

void Flip_E2B5() {
  Flip_E2B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2B5;
  flipped = true;
}

void Flip_D3_B5_ValuesOnly() {
  top_rows += turn * 10616832ULL;
  left_columns += turn * 231928234470ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 3932160ULL;
  bottom_diag9s += turn * 13824ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D3_B5() {
  Flip_D3_B5_ValuesOnly();
}

void Flip_D3_B5() {
  Flip_D3_B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_B5;
  flipped = true;
}

void Flip_D3C4_ValuesOnly() {
  top_rows += turn * 10617318ULL;
  left_columns += turn * 10617318ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 4718592ULL;
  bottom_diag9s += turn * 695784701952ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D3C4() {
  Flip_D3C4_ValuesOnly();
}

void Flip_D3C4() {
  Flip_D3C4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3C4;
  flipped = true;
}

void Flip_D3B5_ValuesOnly() {
  top_rows += turn * 10617318ULL;
  left_columns += turn * 231938851302ULL;
  top_diag9s += turn * 162ULL;
  top_diag7s += turn * 5111808ULL;
  bottom_diag9s += turn * 695784715776ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D3B5() {
  Flip_D3B5_ValuesOnly();
}

void Flip_D3B5() {
  Flip_D3B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3B5;
  flipped = true;
}

void Flip_C4B5_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 231938850816ULL;
  top_diag7s += turn * 1572864ULL;
  bottom_diag9s += turn * 695784715776ULL;
  bottom_rows += turn * 410390516044136448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C4B5() {
  Flip_C4B5_ValuesOnly();
}

void Flip_C4B5() {
  Flip_C4B5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4B5;
  flipped = true;
}

void Flip_F2_D4_ValuesOnly() {
  top_rows += turn * 77309411490ULL;
  left_columns += turn * 162ULL;
  top_diag9s += turn * 5066549580791808ULL;
  top_diag7s += turn * 540ULL;
  right_columns += turn * 6262062317568ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F2_D4() {
  Flip_F2_D4_ValuesOnly();
}

void Flip_F2_D4() {
  Flip_F2_D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_D4;
  flipped = true;
}

void Flip_F2_D4C5_ValuesOnly() {
  top_rows += turn * 77309411490ULL;
  left_columns += turn * 3539106ULL;
  top_diag9s += turn * 5066549580791808ULL;
  top_diag7s += turn * 558ULL;
  right_columns += turn * 6262062317568ULL;
  bottom_diag9s += turn * 45598946230665216ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F2_D4C5() {
  Flip_F2_D4C5_ValuesOnly();
}

void Flip_F2_D4C5() {
  Flip_F2_D4C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_D4C5;
  flipped = true;
}

void Flip_F2_D4B6_ValuesOnly() {
  top_rows += turn * 77309411490ULL;
  left_columns += turn * 77312950434ULL;
  top_diag9s += turn * 5066549580791808ULL;
  top_diag7s += turn * 564ULL;
  right_columns += turn * 6262062317568ULL;
  bottom_diag9s += turn * 45598946230665234ULL;
  bottom_rows += turn * 136803100743696384ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F2_D4B6() {
  Flip_F2_D4B6_ValuesOnly();
}

void Flip_F2_D4B6() {
  Flip_F2_D4B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_D4B6;
  flipped = true;
}

void Flip_F2E3_ValuesOnly() {
  top_rows += turn * 77312950272ULL;
  top_diag9s += turn * 5066549584330752ULL;
  top_diag7s += turn * 648ULL;
  right_columns += turn * 136803100743696384ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F2E3() {
  Flip_F2E3_ValuesOnly();
}

void Flip_F2E3() {
  Flip_F2E3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2E3;
  flipped = true;
}

void Flip_F2E3_C5_ValuesOnly() {
  top_rows += turn * 77312950272ULL;
  left_columns += turn * 3538944ULL;
  top_diag9s += turn * 5066549584330752ULL;
  top_diag7s += turn * 666ULL;
  right_columns += turn * 136803100743696384ULL;
  bottom_diag9s += turn * 3538944ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F2E3_C5() {
  Flip_F2E3_C5_ValuesOnly();
}

void Flip_F2E3_C5() {
  Flip_F2E3_C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2E3_C5;
  flipped = true;
}

void Flip_F2E3_C5B6_ValuesOnly() {
  top_rows += turn * 77312950272ULL;
  left_columns += turn * 77312950272ULL;
  top_diag9s += turn * 5066549584330752ULL;
  top_diag7s += turn * 672ULL;
  right_columns += turn * 136803100743696384ULL;
  bottom_diag9s += turn * 3538962ULL;
  bottom_rows += turn * 136803100743696384ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F2E3_C5B6() {
  Flip_F2E3_C5B6_ValuesOnly();
}

void Flip_F2E3_C5B6() {
  Flip_F2E3_C5B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2E3_C5B6;
  flipped = true;
}

void Flip_F2D4_ValuesOnly() {
  top_rows += turn * 77312950434ULL;
  left_columns += turn * 162ULL;
  top_diag9s += turn * 5066549584330752ULL;
  top_diag7s += turn * 702ULL;
  right_columns += turn * 136803100743696384ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F2D4() {
  Flip_F2D4_ValuesOnly();
}

void Flip_F2D4() {
  Flip_F2D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2D4;
  flipped = true;
}

void Flip_F2D4_B6_ValuesOnly() {
  top_rows += turn * 77312950434ULL;
  left_columns += turn * 77309411490ULL;
  top_diag9s += turn * 5066549584330752ULL;
  top_diag7s += turn * 708ULL;
  right_columns += turn * 136803100743696384ULL;
  bottom_diag9s += turn * 45598946227126290ULL;
  bottom_rows += turn * 6262062317568ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F2D4_B6() {
  Flip_F2D4_B6_ValuesOnly();
}

void Flip_F2D4_B6() {
  Flip_F2D4_B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2D4_B6;
  flipped = true;
}

void Flip_F2C5_ValuesOnly() {
  top_rows += turn * 77312950434ULL;
  left_columns += turn * 3539106ULL;
  top_diag9s += turn * 5066549584330752ULL;
  top_diag7s += turn * 720ULL;
  right_columns += turn * 136803100743696384ULL;
  bottom_diag9s += turn * 45598946230665216ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F2C5() {
  Flip_F2C5_ValuesOnly();
}

void Flip_F2C5() {
  Flip_F2C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2C5;
  flipped = true;
}

void Flip_F2B6_ValuesOnly() {
  top_rows += turn * 77312950434ULL;
  left_columns += turn * 77312950434ULL;
  top_diag9s += turn * 5066549584330752ULL;
  top_diag7s += turn * 726ULL;
  right_columns += turn * 136803100743696384ULL;
  bottom_diag9s += turn * 45598946230665234ULL;
  bottom_rows += turn * 136803100743696384ULL;
  disk_difference += turn * 10;
}

void UndoFlip_F2B6() {
  Flip_F2B6_ValuesOnly();
}

void Flip_F2B6() {
  Flip_F2B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2B6;
  flipped = true;
}

void Flip_E3_C5_ValuesOnly() {
  top_rows += turn * 3538944ULL;
  left_columns += turn * 3538944ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 180ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 3538944ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E3_C5() {
  Flip_E3_C5_ValuesOnly();
}

void Flip_E3_C5() {
  Flip_E3_C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_C5;
  flipped = true;
}

void Flip_E3_C5B6_ValuesOnly() {
  top_rows += turn * 3538944ULL;
  left_columns += turn * 77312950272ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 186ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 3538962ULL;
  bottom_rows += turn * 136803100743696384ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E3_C5B6() {
  Flip_E3_C5B6_ValuesOnly();
}

void Flip_E3_C5B6() {
  Flip_E3_C5B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_C5B6;
  flipped = true;
}

void Flip_E3D4_ValuesOnly() {
  top_rows += turn * 3539106ULL;
  left_columns += turn * 162ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 216ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E3D4() {
  Flip_E3D4_ValuesOnly();
}

void Flip_E3D4() {
  Flip_E3D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3D4;
  flipped = true;
}

void Flip_E3D4_B6_ValuesOnly() {
  top_rows += turn * 3539106ULL;
  left_columns += turn * 77309411490ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 222ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 45598946227126290ULL;
  bottom_rows += turn * 6262062317568ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E3D4_B6() {
  Flip_E3D4_B6_ValuesOnly();
}

void Flip_E3D4_B6() {
  Flip_E3D4_B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3D4_B6;
  flipped = true;
}

void Flip_E3C5_ValuesOnly() {
  top_rows += turn * 3539106ULL;
  left_columns += turn * 3539106ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 234ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 45598946230665216ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E3C5() {
  Flip_E3C5_ValuesOnly();
}

void Flip_E3C5() {
  Flip_E3C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3C5;
  flipped = true;
}

void Flip_E3B6_ValuesOnly() {
  top_rows += turn * 3539106ULL;
  left_columns += turn * 77312950434ULL;
  top_diag9s += turn * 3538944ULL;
  top_diag7s += turn * 240ULL;
  right_columns += turn * 136796838681378816ULL;
  bottom_diag9s += turn * 45598946230665234ULL;
  bottom_rows += turn * 136803100743696384ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E3B6() {
  Flip_E3B6_ValuesOnly();
}

void Flip_E3B6() {
  Flip_E3B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3B6;
  flipped = true;
}

void Flip_D4_B6_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 77309411490ULL;
  top_diag7s += turn * 60ULL;
  bottom_diag9s += turn * 45598946227126290ULL;
  bottom_rows += turn * 6262062317568ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D4_B6() {
  Flip_D4_B6_ValuesOnly();
}

void Flip_D4_B6() {
  Flip_D4_B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_B6;
  flipped = true;
}

void Flip_D4C5_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 3539106ULL;
  top_diag7s += turn * 72ULL;
  bottom_diag9s += turn * 45598946230665216ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D4C5() {
  Flip_D4C5_ValuesOnly();
}

void Flip_D4C5() {
  Flip_D4C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4C5;
  flipped = true;
}

void Flip_D4B6_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 77312950434ULL;
  top_diag7s += turn * 78ULL;
  bottom_diag9s += turn * 45598946230665234ULL;
  bottom_rows += turn * 136803100743696384ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D4B6() {
  Flip_D4B6_ValuesOnly();
}

void Flip_D4B6() {
  Flip_D4B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4B6;
  flipped = true;
}

void Flip_C5B6_ValuesOnly() {
  left_columns += turn * 77312950272ULL;
  top_diag7s += turn * 24ULL;
  bottom_diag9s += turn * 3538962ULL;
  bottom_rows += turn * 136803100743696384ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C5B6() {
  Flip_C5B6_ValuesOnly();
}

void Flip_C5B6() {
  Flip_C5B6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5B6;
  flipped = true;
}

void Flip_G2_E4_ValuesOnly() {
  top_rows += turn * 25769803830ULL;
  top_diag9s += turn * 432345564227567670ULL;
  right_columns += turn * 45598946322677760ULL;
  bottom_diag7s += turn * 455989462271262720ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G2_E4() {
  Flip_G2_E4_ValuesOnly();
}

void Flip_G2_E4() {
  Flip_G2_E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_E4;
  flipped = true;
}

void Flip_G2_E4D5_ValuesOnly() {
  top_rows += turn * 25769803830ULL;
  left_columns += turn * 54ULL;
  top_diag9s += turn * 432345564227567670ULL;
  right_columns += turn * 45598946322677760ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 471189111013638144ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G2_E4D5() {
  Flip_G2_E4D5_ValuesOnly();
}

void Flip_G2_E4D5() {
  Flip_G2_E4D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_E4D5;
  flipped = true;
}

void Flip_G2_E4C6_ValuesOnly() {
  top_rows += turn * 25769803830ULL;
  left_columns += turn * 1179702ULL;
  top_diag9s += turn * 432345564227567670ULL;
  right_columns += turn * 45598946322677760ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 476255660594429952ULL;
  bottom_rows += turn * 45601033581232128ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G2_E4C6() {
  Flip_G2_E4C6_ValuesOnly();
}

void Flip_G2_E4C6() {
  Flip_G2_E4C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_E4C6;
  flipped = true;
}

void Flip_G2_E4B7_ValuesOnly() {
  top_rows += turn * 25769803830ULL;
  left_columns += turn * 25770983478ULL;
  top_diag9s += turn * 432345589997371446ULL;
  right_columns += turn * 45598946322677760ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 477944510454693888ULL;
  bottom_rows += turn * 45601033676783616ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G2_E4B7() {
  Flip_G2_E4B7_ValuesOnly();
}

void Flip_G2_E4B7() {
  Flip_G2_E4B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_E4B7;
  flipped = true;
}

void Flip_G2F3_ValuesOnly() {
  top_rows += turn * 25770983424ULL;
  top_diag9s += turn * 432365355436867584ULL;
  right_columns += turn * 2087449657344ULL;
  bottom_diag7s += turn * 547187354725515264ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G2F3() {
  Flip_G2F3_ValuesOnly();
}

void Flip_G2F3() {
  Flip_G2F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2F3;
  flipped = true;
}

void Flip_G2F3_D5_ValuesOnly() {
  top_rows += turn * 25770983424ULL;
  left_columns += turn * 54ULL;
  top_diag9s += turn * 432365355436867584ULL;
  right_columns += turn * 2087449657344ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 562387003467890688ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G2F3_D5() {
  Flip_G2F3_D5_ValuesOnly();
}

void Flip_G2F3_D5() {
  Flip_G2F3_D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2F3_D5;
  flipped = true;
}

void Flip_G2F3_D5C6_ValuesOnly() {
  top_rows += turn * 25770983424ULL;
  left_columns += turn * 1179702ULL;
  top_diag9s += turn * 432365355436867584ULL;
  right_columns += turn * 2087449657344ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 567453553048682496ULL;
  bottom_rows += turn * 45601033581232128ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G2F3_D5C6() {
  Flip_G2F3_D5C6_ValuesOnly();
}

void Flip_G2F3_D5C6() {
  Flip_G2F3_D5C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2F3_D5C6;
  flipped = true;
}

void Flip_G2F3_D5B7_ValuesOnly() {
  top_rows += turn * 25770983424ULL;
  left_columns += turn * 25770983478ULL;
  top_diag9s += turn * 432365381206671360ULL;
  right_columns += turn * 2087449657344ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 569142402908946432ULL;
  bottom_rows += turn * 45601033676783616ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G2F3_D5B7() {
  Flip_G2F3_D5B7_ValuesOnly();
}

void Flip_G2F3_D5B7() {
  Flip_G2F3_D5B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2F3_D5B7;
  flipped = true;
}

void Flip_G2E4_ValuesOnly() {
  top_rows += turn * 25770983478ULL;
  top_diag9s += turn * 432365355436867638ULL;
  right_columns += turn * 45601033676783616ULL;
  bottom_diag7s += turn * 592786300952641536ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G2E4() {
  Flip_G2E4_ValuesOnly();
}

void Flip_G2E4() {
  Flip_G2E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2E4;
  flipped = true;
}

void Flip_G2E4_C6_ValuesOnly() {
  top_rows += turn * 25770983478ULL;
  left_columns += turn * 1179648ULL;
  top_diag9s += turn * 432365355436867638ULL;
  right_columns += turn * 45601033676783616ULL;
  bottom_diag9s += turn * 4608ULL;
  bottom_diag7s += turn * 597852850533433344ULL;
  bottom_rows += turn * 2087354105856ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G2E4_C6() {
  Flip_G2E4_C6_ValuesOnly();
}

void Flip_G2E4_C6() {
  Flip_G2E4_C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2E4_C6;
  flipped = true;
}

void Flip_G2E4_C6B7_ValuesOnly() {
  top_rows += turn * 25770983478ULL;
  left_columns += turn * 25770983424ULL;
  top_diag9s += turn * 432365381206671414ULL;
  right_columns += turn * 45601033676783616ULL;
  bottom_diag9s += turn * 4608ULL;
  bottom_diag7s += turn * 599541700393697280ULL;
  bottom_rows += turn * 2087449657344ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G2E4_C6B7() {
  Flip_G2E4_C6B7_ValuesOnly();
}

void Flip_G2E4_C6B7() {
  Flip_G2E4_C6B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2E4_C6B7;
  flipped = true;
}

void Flip_G2D5_ValuesOnly() {
  top_rows += turn * 25770983478ULL;
  left_columns += turn * 54ULL;
  top_diag9s += turn * 432365355436867638ULL;
  right_columns += turn * 45601033676783616ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 607985949695016960ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G2D5() {
  Flip_G2D5_ValuesOnly();
}

void Flip_G2D5() {
  Flip_G2D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2D5;
  flipped = true;
}

void Flip_G2D5_B7_ValuesOnly() {
  top_rows += turn * 25770983478ULL;
  left_columns += turn * 25769803830ULL;
  top_diag9s += turn * 432365381206671414ULL;
  right_columns += turn * 45601033676783616ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 609674799555280896ULL;
  bottom_rows += turn * 45598946322677760ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G2D5_B7() {
  Flip_G2D5_B7_ValuesOnly();
}

void Flip_G2D5_B7() {
  Flip_G2D5_B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2D5_B7;
  flipped = true;
}

void Flip_G2C6_ValuesOnly() {
  top_rows += turn * 25770983478ULL;
  left_columns += turn * 1179702ULL;
  top_diag9s += turn * 432365355436867638ULL;
  right_columns += turn * 45601033676783616ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 613052499275808768ULL;
  bottom_rows += turn * 45601033581232128ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G2C6() {
  Flip_G2C6_ValuesOnly();
}

void Flip_G2C6() {
  Flip_G2C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2C6;
  flipped = true;
}

void Flip_G2B7_ValuesOnly() {
  top_rows += turn * 25770983478ULL;
  left_columns += turn * 25770983478ULL;
  top_diag9s += turn * 432365381206671414ULL;
  right_columns += turn * 45601033676783616ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 614741349136072704ULL;
  bottom_rows += turn * 45601033676783616ULL;
  disk_difference += turn * 12;
}

void UndoFlip_G2B7() {
  Flip_G2B7_ValuesOnly();
}

void Flip_G2B7() {
  Flip_G2B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2B7;
  flipped = true;
}

void Flip_F3_D5_ValuesOnly() {
  top_rows += turn * 1179648ULL;
  left_columns += turn * 54ULL;
  top_diag9s += turn * 19791209299968ULL;
  right_columns += turn * 2087354105856ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 151996487423754240ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F3_D5() {
  Flip_F3_D5_ValuesOnly();
}

void Flip_F3_D5() {
  Flip_F3_D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_D5;
  flipped = true;
}

void Flip_F3_D5C6_ValuesOnly() {
  top_rows += turn * 1179648ULL;
  left_columns += turn * 1179702ULL;
  top_diag9s += turn * 19791209299968ULL;
  right_columns += turn * 2087354105856ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 157063037004546048ULL;
  bottom_rows += turn * 45601033581232128ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F3_D5C6() {
  Flip_F3_D5C6_ValuesOnly();
}

void Flip_F3_D5C6() {
  Flip_F3_D5C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_D5C6;
  flipped = true;
}

void Flip_F3_D5B7_ValuesOnly() {
  top_rows += turn * 1179648ULL;
  left_columns += turn * 25770983478ULL;
  top_diag9s += turn * 19816979103744ULL;
  right_columns += turn * 2087354105856ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 158751886864809984ULL;
  bottom_rows += turn * 45601033676783616ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F3_D5B7() {
  Flip_F3_D5B7_ValuesOnly();
}

void Flip_F3_D5B7() {
  Flip_F3_D5B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_D5B7;
  flipped = true;
}

void Flip_F3E4_ValuesOnly() {
  top_rows += turn * 1179702ULL;
  top_diag9s += turn * 19791209300022ULL;
  right_columns += turn * 45601033581232128ULL;
  bottom_diag7s += turn * 182395784908505088ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F3E4() {
  Flip_F3E4_ValuesOnly();
}

void Flip_F3E4() {
  Flip_F3E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3E4;
  flipped = true;
}

void Flip_F3E4_C6_ValuesOnly() {
  top_rows += turn * 1179702ULL;
  left_columns += turn * 1179648ULL;
  top_diag9s += turn * 19791209300022ULL;
  right_columns += turn * 45601033581232128ULL;
  bottom_diag9s += turn * 4608ULL;
  bottom_diag7s += turn * 187462334489296896ULL;
  bottom_rows += turn * 2087354105856ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F3E4_C6() {
  Flip_F3E4_C6_ValuesOnly();
}

void Flip_F3E4_C6() {
  Flip_F3E4_C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3E4_C6;
  flipped = true;
}

void Flip_F3E4_C6B7_ValuesOnly() {
  top_rows += turn * 1179702ULL;
  left_columns += turn * 25770983424ULL;
  top_diag9s += turn * 19816979103798ULL;
  right_columns += turn * 45601033581232128ULL;
  bottom_diag9s += turn * 4608ULL;
  bottom_diag7s += turn * 189151184349560832ULL;
  bottom_rows += turn * 2087449657344ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F3E4_C6B7() {
  Flip_F3E4_C6B7_ValuesOnly();
}

void Flip_F3E4_C6B7() {
  Flip_F3E4_C6B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3E4_C6B7;
  flipped = true;
}

void Flip_F3D5_ValuesOnly() {
  top_rows += turn * 1179702ULL;
  left_columns += turn * 54ULL;
  top_diag9s += turn * 19791209300022ULL;
  right_columns += turn * 45601033581232128ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 197595433650880512ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F3D5() {
  Flip_F3D5_ValuesOnly();
}

void Flip_F3D5() {
  Flip_F3D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3D5;
  flipped = true;
}

void Flip_F3D5_B7_ValuesOnly() {
  top_rows += turn * 1179702ULL;
  left_columns += turn * 25769803830ULL;
  top_diag9s += turn * 19816979103798ULL;
  right_columns += turn * 45601033581232128ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 199284283511144448ULL;
  bottom_rows += turn * 45598946322677760ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F3D5_B7() {
  Flip_F3D5_B7_ValuesOnly();
}

void Flip_F3D5_B7() {
  Flip_F3D5_B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3D5_B7;
  flipped = true;
}

void Flip_F3C6_ValuesOnly() {
  top_rows += turn * 1179702ULL;
  left_columns += turn * 1179702ULL;
  top_diag9s += turn * 19791209300022ULL;
  right_columns += turn * 45601033581232128ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 202661983231672320ULL;
  bottom_rows += turn * 45601033581232128ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F3C6() {
  Flip_F3C6_ValuesOnly();
}

void Flip_F3C6() {
  Flip_F3C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3C6;
  flipped = true;
}

void Flip_F3B7_ValuesOnly() {
  top_rows += turn * 1179702ULL;
  left_columns += turn * 25770983478ULL;
  top_diag9s += turn * 19816979103798ULL;
  right_columns += turn * 45601033581232128ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 204350833091936256ULL;
  bottom_rows += turn * 45601033676783616ULL;
  disk_difference += turn * 10;
}

void UndoFlip_F3B7() {
  Flip_F3B7_ValuesOnly();
}

void Flip_F3B7() {
  Flip_F3B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3B7;
  flipped = true;
}

void Flip_E4_C6_ValuesOnly() {
  top_rows += turn * 54ULL;
  left_columns += turn * 1179648ULL;
  top_diag9s += turn * 54ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 4608ULL;
  bottom_diag7s += turn * 50665495807918080ULL;
  bottom_rows += turn * 2087354105856ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E4_C6() {
  Flip_E4_C6_ValuesOnly();
}

void Flip_E4_C6() {
  Flip_E4_C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_C6;
  flipped = true;
}

void Flip_E4_C6B7_ValuesOnly() {
  top_rows += turn * 54ULL;
  left_columns += turn * 25770983424ULL;
  top_diag9s += turn * 25769803830ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 4608ULL;
  bottom_diag7s += turn * 52354345668182016ULL;
  bottom_rows += turn * 2087449657344ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E4_C6B7() {
  Flip_E4_C6B7_ValuesOnly();
}

void Flip_E4_C6B7() {
  Flip_E4_C6B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_C6B7;
  flipped = true;
}

void Flip_E4D5_ValuesOnly() {
  top_rows += turn * 54ULL;
  left_columns += turn * 54ULL;
  top_diag9s += turn * 54ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 60798594969501696ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E4D5() {
  Flip_E4D5_ValuesOnly();
}

void Flip_E4D5() {
  Flip_E4D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4D5;
  flipped = true;
}

void Flip_E4D5_B7_ValuesOnly() {
  top_rows += turn * 54ULL;
  left_columns += turn * 25769803830ULL;
  top_diag9s += turn * 25769803830ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 62487444829765632ULL;
  bottom_rows += turn * 45598946322677760ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E4D5_B7() {
  Flip_E4D5_B7_ValuesOnly();
}

void Flip_E4D5_B7() {
  Flip_E4D5_B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4D5_B7;
  flipped = true;
}

void Flip_E4C6_ValuesOnly() {
  top_rows += turn * 54ULL;
  left_columns += turn * 1179702ULL;
  top_diag9s += turn * 54ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 65865144550293504ULL;
  bottom_rows += turn * 45601033581232128ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E4C6() {
  Flip_E4C6_ValuesOnly();
}

void Flip_E4C6() {
  Flip_E4C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4C6;
  flipped = true;
}

void Flip_E4B7_ValuesOnly() {
  top_rows += turn * 54ULL;
  left_columns += turn * 25770983478ULL;
  top_diag9s += turn * 25769803830ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 67553994410557440ULL;
  bottom_rows += turn * 45601033676783616ULL;
  disk_difference += turn * 8;
}

void UndoFlip_E4B7() {
  Flip_E4B7_ValuesOnly();
}

void Flip_E4B7() {
  Flip_E4B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4B7;
  flipped = true;
}

void Flip_D5_B7_ValuesOnly() {
  left_columns += turn * 25769803830ULL;
  top_diag9s += turn * 25769803776ULL;
  bottom_diag9s += turn * 231928233984ULL;
  bottom_diag7s += turn * 16888498602639360ULL;
  bottom_rows += turn * 45598946322677760ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D5_B7() {
  Flip_D5_B7_ValuesOnly();
}

void Flip_D5_B7() {
  Flip_D5_B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5_B7;
  flipped = true;
}

void Flip_D5C6_ValuesOnly() {
  left_columns += turn * 1179702ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 20266198323167232ULL;
  bottom_rows += turn * 45601033581232128ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D5C6() {
  Flip_D5C6_ValuesOnly();
}

void Flip_D5C6() {
  Flip_D5C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5C6;
  flipped = true;
}

void Flip_D5B7_ValuesOnly() {
  left_columns += turn * 25770983478ULL;
  top_diag9s += turn * 25769803776ULL;
  bottom_diag9s += turn * 231928238592ULL;
  bottom_diag7s += turn * 21955048183431168ULL;
  bottom_rows += turn * 45601033676783616ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D5B7() {
  Flip_D5B7_ValuesOnly();
}

void Flip_D5B7() {
  Flip_D5B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5B7;
  flipped = true;
}

void Flip_C6B7_ValuesOnly() {
  left_columns += turn * 25770983424ULL;
  top_diag9s += turn * 25769803776ULL;
  bottom_diag9s += turn * 4608ULL;
  bottom_diag7s += turn * 6755399441055744ULL;
  bottom_rows += turn * 2087449657344ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C6B7() {
  Flip_C6B7_ValuesOnly();
}

void Flip_C6B7() {
  Flip_C6B7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6B7;
  flipped = true;
}

void Flip_G3_E5_ValuesOnly() {
  top_rows += turn * 393216ULL;
  top_diag9s += turn * 1688849860263936ULL;
  right_columns += turn * 15199648774225920ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 2319282339840ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G3_E5() {
  Flip_G3_E5_ValuesOnly();
}

void Flip_G3_E5() {
  Flip_G3_E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_E5;
  flipped = true;
}

void Flip_G3_E5D6_ValuesOnly() {
  top_rows += turn * 393216ULL;
  left_columns += turn * 18ULL;
  top_diag9s += turn * 1688849860263936ULL;
  right_columns += turn * 15199648774225920ULL;
  bottom_diag9s += turn * 15199648743555072ULL;
  bottom_diag7s += turn * 2396591751168ULL;
  bottom_rows += turn * 15200344527077376ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G3_E5D6() {
  Flip_G3_E5D6_ValuesOnly();
}

void Flip_G3_E5D6() {
  Flip_G3_E5D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_E5D6;
  flipped = true;
}

void Flip_G3_E5C7_ValuesOnly() {
  top_rows += turn * 393216ULL;
  left_columns += turn * 393234ULL;
  top_diag9s += turn * 1688849860263936ULL;
  right_columns += turn * 15199648774225920ULL;
  bottom_diag9s += turn * 15199648743555078ULL;
  bottom_diag7s += turn * 2422361554944ULL;
  bottom_rows += turn * 15200344558927872ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G3_E5C7() {
  Flip_G3_E5C7_ValuesOnly();
}

void Flip_G3_E5C7() {
  Flip_G3_E5C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_E5C7;
  flipped = true;
}

void Flip_G3F4_ValuesOnly() {
  top_rows += turn * 393234ULL;
  top_diag9s += turn * 1688849861443584ULL;
  right_columns += turn * 695816552448ULL;
  bottom_diag7s += turn * 2783138807808ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G3F4() {
  Flip_G3F4_ValuesOnly();
}

void Flip_G3F4() {
  Flip_G3F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3F4;
  flipped = true;
}

void Flip_G3F4_D6_ValuesOnly() {
  top_rows += turn * 393234ULL;
  left_columns += turn * 18ULL;
  top_diag9s += turn * 1688849861443584ULL;
  right_columns += turn * 695816552448ULL;
  bottom_diag9s += turn * 1179648ULL;
  bottom_diag7s += turn * 2860448219136ULL;
  bottom_rows += turn * 695784701952ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G3F4_D6() {
  Flip_G3F4_D6_ValuesOnly();
}

void Flip_G3F4_D6() {
  Flip_G3F4_D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3F4_D6;
  flipped = true;
}

void Flip_G3F4_D6C7_ValuesOnly() {
  top_rows += turn * 393234ULL;
  left_columns += turn * 393234ULL;
  top_diag9s += turn * 1688849861443584ULL;
  right_columns += turn * 695816552448ULL;
  bottom_diag9s += turn * 1179654ULL;
  bottom_diag7s += turn * 2886218022912ULL;
  bottom_rows += turn * 695816552448ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G3F4_D6C7() {
  Flip_G3F4_D6C7_ValuesOnly();
}

void Flip_G3F4_D6C7() {
  Flip_G3F4_D6C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3F4_D6C7;
  flipped = true;
}

void Flip_G3E5_ValuesOnly() {
  top_rows += turn * 393234ULL;
  top_diag9s += turn * 1688849861443584ULL;
  right_columns += turn * 15200344558927872ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 3015067041792ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G3E5() {
  Flip_G3E5_ValuesOnly();
}

void Flip_G3E5() {
  Flip_G3E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3E5;
  flipped = true;
}

void Flip_G3E5_C7_ValuesOnly() {
  top_rows += turn * 393234ULL;
  left_columns += turn * 393216ULL;
  top_diag9s += turn * 1688849861443584ULL;
  right_columns += turn * 15200344558927872ULL;
  bottom_diag9s += turn * 15199648742375430ULL;
  bottom_diag7s += turn * 3040836845568ULL;
  bottom_rows += turn * 15199648774225920ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G3E5_C7() {
  Flip_G3E5_C7_ValuesOnly();
}

void Flip_G3E5_C7() {
  Flip_G3E5_C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3E5_C7;
  flipped = true;
}

void Flip_G3D6_ValuesOnly() {
  top_rows += turn * 393234ULL;
  left_columns += turn * 18ULL;
  top_diag9s += turn * 1688849861443584ULL;
  right_columns += turn * 15200344558927872ULL;
  bottom_diag9s += turn * 15199648743555072ULL;
  bottom_diag7s += turn * 3092376453120ULL;
  bottom_rows += turn * 15200344527077376ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G3D6() {
  Flip_G3D6_ValuesOnly();
}

void Flip_G3D6() {
  Flip_G3D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3D6;
  flipped = true;
}

void Flip_G3C7_ValuesOnly() {
  top_rows += turn * 393234ULL;
  left_columns += turn * 393234ULL;
  top_diag9s += turn * 1688849861443584ULL;
  right_columns += turn * 15200344558927872ULL;
  bottom_diag9s += turn * 15199648743555078ULL;
  bottom_diag7s += turn * 3118146256896ULL;
  bottom_rows += turn * 15200344558927872ULL;
  disk_difference += turn * 10;
}

void UndoFlip_G3C7() {
  Flip_G3C7_ValuesOnly();
}

void Flip_G3C7() {
  Flip_G3C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3C7;
  flipped = true;
}

void Flip_F4_D6_ValuesOnly() {
  top_rows += turn * 18ULL;
  left_columns += turn * 18ULL;
  top_diag9s += turn * 1179648ULL;
  right_columns += turn * 695784701952ULL;
  bottom_diag9s += turn * 1179648ULL;
  bottom_diag7s += turn * 773094113280ULL;
  bottom_rows += turn * 695784701952ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F4_D6() {
  Flip_F4_D6_ValuesOnly();
}

void Flip_F4_D6() {
  Flip_F4_D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4_D6;
  flipped = true;
}

void Flip_F4_D6C7_ValuesOnly() {
  top_rows += turn * 18ULL;
  left_columns += turn * 393234ULL;
  top_diag9s += turn * 1179648ULL;
  right_columns += turn * 695784701952ULL;
  bottom_diag9s += turn * 1179654ULL;
  bottom_diag7s += turn * 798863917056ULL;
  bottom_rows += turn * 695816552448ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F4_D6C7() {
  Flip_F4_D6C7_ValuesOnly();
}

void Flip_F4_D6C7() {
  Flip_F4_D6C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4_D6C7;
  flipped = true;
}

void Flip_F4E5_ValuesOnly() {
  top_rows += turn * 18ULL;
  top_diag9s += turn * 1179648ULL;
  right_columns += turn * 15200344527077376ULL;
  bottom_diag9s += turn * 15199648742375424ULL;
  bottom_diag7s += turn * 927712935936ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F4E5() {
  Flip_F4E5_ValuesOnly();
}

void Flip_F4E5() {
  Flip_F4E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4E5;
  flipped = true;
}

void Flip_F4E5_C7_ValuesOnly() {
  top_rows += turn * 18ULL;
  left_columns += turn * 393216ULL;
  top_diag9s += turn * 1179648ULL;
  right_columns += turn * 15200344527077376ULL;
  bottom_diag9s += turn * 15199648742375430ULL;
  bottom_diag7s += turn * 953482739712ULL;
  bottom_rows += turn * 15199648774225920ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F4E5_C7() {
  Flip_F4E5_C7_ValuesOnly();
}

void Flip_F4E5_C7() {
  Flip_F4E5_C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4E5_C7;
  flipped = true;
}

void Flip_F4D6_ValuesOnly() {
  top_rows += turn * 18ULL;
  left_columns += turn * 18ULL;
  top_diag9s += turn * 1179648ULL;
  right_columns += turn * 15200344527077376ULL;
  bottom_diag9s += turn * 15199648743555072ULL;
  bottom_diag7s += turn * 1005022347264ULL;
  bottom_rows += turn * 15200344527077376ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F4D6() {
  Flip_F4D6_ValuesOnly();
}

void Flip_F4D6() {
  Flip_F4D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4D6;
  flipped = true;
}

void Flip_F4C7_ValuesOnly() {
  top_rows += turn * 18ULL;
  left_columns += turn * 393234ULL;
  top_diag9s += turn * 1179648ULL;
  right_columns += turn * 15200344527077376ULL;
  bottom_diag9s += turn * 15199648743555078ULL;
  bottom_diag7s += turn * 1030792151040ULL;
  bottom_rows += turn * 15200344558927872ULL;
  disk_difference += turn * 8;
}

void UndoFlip_F4C7() {
  Flip_F4C7_ValuesOnly();
}

void Flip_F4C7() {
  Flip_F4C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4C7;
  flipped = true;
}

void Flip_E5_C7_ValuesOnly() {
  left_columns += turn * 393216ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 15199648742375430ULL;
  bottom_diag7s += turn * 257698037760ULL;
  bottom_rows += turn * 15199648774225920ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E5_C7() {
  Flip_E5_C7_ValuesOnly();
}

void Flip_E5_C7() {
  Flip_E5_C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5_C7;
  flipped = true;
}

void Flip_E5D6_ValuesOnly() {
  left_columns += turn * 18ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 15199648743555072ULL;
  bottom_diag7s += turn * 309237645312ULL;
  bottom_rows += turn * 15200344527077376ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E5D6() {
  Flip_E5D6_ValuesOnly();
}

void Flip_E5D6() {
  Flip_E5D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5D6;
  flipped = true;
}

void Flip_E5C7_ValuesOnly() {
  left_columns += turn * 393234ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 15199648743555078ULL;
  bottom_diag7s += turn * 335007449088ULL;
  bottom_rows += turn * 15200344558927872ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E5C7() {
  Flip_E5C7_ValuesOnly();
}

void Flip_E5C7() {
  Flip_E5C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5C7;
  flipped = true;
}

void Flip_D6C7_ValuesOnly() {
  left_columns += turn * 393234ULL;
  bottom_diag9s += turn * 1179654ULL;
  bottom_diag7s += turn * 103079215104ULL;
  bottom_rows += turn * 695816552448ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D6C7() {
  Flip_D6C7_ValuesOnly();
}

void Flip_D6C7() {
  Flip_D6C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6C7;
  flipped = true;
}

void Flip_G4_E6_ValuesOnly() {
  top_rows += turn * 6ULL;
  top_diag9s += turn * 6597069766656ULL;
  right_columns += turn * 5066549591408640ULL;
  bottom_diag9s += turn * 77309411328ULL;
  bottom_diag7s += turn * 11796480ULL;
  bottom_rows += turn * 231928233984ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G4_E6() {
  Flip_G4_E6_ValuesOnly();
}

void Flip_G4_E6() {
  Flip_G4_E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4_E6;
  flipped = true;
}

void Flip_G4_E6D7_ValuesOnly() {
  top_rows += turn * 6ULL;
  left_columns += turn * 6ULL;
  top_diag9s += turn * 6597069766656ULL;
  right_columns += turn * 5066549591408640ULL;
  bottom_diag9s += turn * 77309412864ULL;
  bottom_diag7s += turn * 12189696ULL;
  bottom_rows += turn * 231938850816ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G4_E6D7() {
  Flip_G4_E6D7_ValuesOnly();
}

void Flip_G4_E6D7() {
  Flip_G4_E6D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4_E6D7;
  flipped = true;
}

void Flip_G4F5_ValuesOnly() {
  top_rows += turn * 6ULL;
  top_diag9s += turn * 6597069766674ULL;
  right_columns += turn * 231938850816ULL;
  bottom_diag7s += turn * 14155776ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G4F5() {
  Flip_G4F5_ValuesOnly();
}

void Flip_G4F5() {
  Flip_G4F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4F5;
  flipped = true;
}

void Flip_G4F5_D7_ValuesOnly() {
  top_rows += turn * 6ULL;
  left_columns += turn * 6ULL;
  top_diag9s += turn * 6597069766674ULL;
  right_columns += turn * 231938850816ULL;
  bottom_diag9s += turn * 1536ULL;
  bottom_diag7s += turn * 14548992ULL;
  bottom_rows += turn * 5066549591408640ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G4F5_D7() {
  Flip_G4F5_D7_ValuesOnly();
}

void Flip_G4F5_D7() {
  Flip_G4F5_D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4F5_D7;
  flipped = true;
}

void Flip_G4E6_ValuesOnly() {
  top_rows += turn * 6ULL;
  top_diag9s += turn * 6597069766674ULL;
  right_columns += turn * 5066781519642624ULL;
  bottom_diag9s += turn * 77309411328ULL;
  bottom_diag7s += turn * 15335424ULL;
  bottom_rows += turn * 5066781509025792ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G4E6() {
  Flip_G4E6_ValuesOnly();
}

void Flip_G4E6() {
  Flip_G4E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4E6;
  flipped = true;
}

void Flip_G4D7_ValuesOnly() {
  top_rows += turn * 6ULL;
  left_columns += turn * 6ULL;
  top_diag9s += turn * 6597069766674ULL;
  right_columns += turn * 5066781519642624ULL;
  bottom_diag9s += turn * 77309412864ULL;
  bottom_diag7s += turn * 15728640ULL;
  bottom_rows += turn * 5066781519642624ULL;
  disk_difference += turn * 8;
}

void UndoFlip_G4D7() {
  Flip_G4D7_ValuesOnly();
}

void Flip_G4D7() {
  Flip_G4D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4D7;
  flipped = true;
}

void Flip_F5_D7_ValuesOnly() {
  left_columns += turn * 6ULL;
  top_diag9s += turn * 18ULL;
  right_columns += turn * 231928233984ULL;
  bottom_diag9s += turn * 1536ULL;
  bottom_diag7s += turn * 3932160ULL;
  bottom_rows += turn * 5066549591408640ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F5_D7() {
  Flip_F5_D7_ValuesOnly();
}

void Flip_F5_D7() {
  Flip_F5_D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5_D7;
  flipped = true;
}

void Flip_F5E6_ValuesOnly() {
  top_diag9s += turn * 18ULL;
  right_columns += turn * 5066781509025792ULL;
  bottom_diag9s += turn * 77309411328ULL;
  bottom_diag7s += turn * 4718592ULL;
  bottom_rows += turn * 5066781509025792ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F5E6() {
  Flip_F5E6_ValuesOnly();
}

void Flip_F5E6() {
  Flip_F5E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5E6;
  flipped = true;
}

void Flip_F5D7_ValuesOnly() {
  left_columns += turn * 6ULL;
  top_diag9s += turn * 18ULL;
  right_columns += turn * 5066781509025792ULL;
  bottom_diag9s += turn * 77309412864ULL;
  bottom_diag7s += turn * 5111808ULL;
  bottom_rows += turn * 5066781519642624ULL;
  disk_difference += turn * 6;
}

void UndoFlip_F5D7() {
  Flip_F5D7_ValuesOnly();
}

void Flip_F5D7() {
  Flip_F5D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5D7;
  flipped = true;
}

void Flip_E6D7_ValuesOnly() {
  left_columns += turn * 6ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 77309412864ULL;
  bottom_diag7s += turn * 1572864ULL;
  bottom_rows += turn * 231938850816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E6D7() {
  Flip_E6D7_ValuesOnly();
}

void Flip_E6D7() {
  Flip_E6D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6D7;
  flipped = true;
}

void Flip_G5_E7_ValuesOnly() {
  top_diag9s += turn * 393216ULL;
  right_columns += turn * 1688849863802880ULL;
  bottom_diag9s += turn * 393216ULL;
  bottom_diag7s += turn * 15360ULL;
  bottom_rows += turn * 1688849863802880ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G5_E7() {
  Flip_G5_E7_ValuesOnly();
}

void Flip_G5_E7() {
  Flip_G5_E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5_E7;
  flipped = true;
}

void Flip_G5F6_ValuesOnly() {
  top_diag9s += turn * 393216ULL;
  right_columns += turn * 77312950272ULL;
  bottom_diag9s += turn * 5066549580791808ULL;
  bottom_diag7s += turn * 18432ULL;
  bottom_rows += turn * 1688927169675264ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G5F6() {
  Flip_G5F6_ValuesOnly();
}

void Flip_G5F6() {
  Flip_G5F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5F6;
  flipped = true;
}

void Flip_G5E7_ValuesOnly() {
  top_diag9s += turn * 393216ULL;
  right_columns += turn * 1688927173214208ULL;
  bottom_diag9s += turn * 5066549581185024ULL;
  bottom_diag7s += turn * 19968ULL;
  bottom_rows += turn * 1688927173214208ULL;
  disk_difference += turn * 6;
}

void UndoFlip_G5E7() {
  Flip_G5E7_ValuesOnly();
}

void Flip_G5E7() {
  Flip_G5E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5E7;
  flipped = true;
}

void Flip_F6E7_ValuesOnly() {
  right_columns += turn * 1688927169675264ULL;
  bottom_diag9s += turn * 5066549581185024ULL;
  bottom_diag7s += turn * 6144ULL;
  bottom_rows += turn * 77312950272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F6E7() {
  Flip_F6E7_ValuesOnly();
}

void Flip_F6E7() {
  Flip_F6E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F6E7;
  flipped = true;
}

void Flip_G6F7_ValuesOnly() {
  top_diag9s += turn * 6ULL;
  right_columns += turn * 25770983424ULL;
  bottom_diag9s += turn * 25769803776ULL;
  bottom_diag7s += turn * 24ULL;
  bottom_rows += turn * 25770983424ULL;
  disk_difference += turn * 4;
}

void UndoFlip_G6F7() {
  Flip_G6F7_ValuesOnly();
}

void Flip_G6F7() {
  Flip_G6F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G6F7;
  flipped = true;
}

void Flip_F2G3_ValuesOnly() {
  top_rows += turn * 77309804544ULL;
  top_diag9s += turn * 6755399441055744ULL;
  top_diag7s += turn * 486ULL;
  right_columns += turn * 6262094168064ULL;
  bottom_diag7s += turn * 2087354105856ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F2G3() {
  Flip_F2G3_ValuesOnly();
}

void Flip_F2G3() {
  Flip_F2G3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2G3;
  flipped = true;
}

void Flip_E2_G4_ValuesOnly() {
  top_rows += turn * 231928233990ULL;
  top_diag9s += turn * 65970697666560ULL;
  top_diag7s += turn * 10616832ULL;
  right_columns += turn * 410390516054753280ULL;
  bottom_diag7s += turn * 10616832ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E2_G4() {
  Flip_E2_G4_ValuesOnly();
}

void Flip_E2_G4() {
  Flip_E2_G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_G4;
  flipped = true;
}

void Flip_E2F3_ValuesOnly() {
  top_rows += turn * 231929413632ULL;
  top_diag9s += turn * 79164837199872ULL;
  top_diag7s += turn * 10616832ULL;
  right_columns += turn * 410392603398242304ULL;
  bottom_diag7s += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E2F3() {
  Flip_E2F3_ValuesOnly();
}

void Flip_E2F3() {
  Flip_E2F3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2F3;
  flipped = true;
}

void Flip_E2G4_ValuesOnly() {
  top_rows += turn * 231929413638ULL;
  top_diag9s += turn * 85761906966528ULL;
  top_diag7s += turn * 10616832ULL;
  right_columns += turn * 410392603408859136ULL;
  bottom_diag7s += turn * 136796838691995648ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E2G4() {
  Flip_E2G4_ValuesOnly();
}

void Flip_E2G4() {
  Flip_E2G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2G4;
  flipped = true;
}

void Flip_F3G4_ValuesOnly() {
  top_rows += turn * 1179654ULL;
  top_diag9s += turn * 26388279066624ULL;
  right_columns += turn * 2087364722688ULL;
  bottom_diag7s += turn * 136796838691995648ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F3G4() {
  Flip_F3G4_ValuesOnly();
}

void Flip_F3G4() {
  Flip_F3G4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3G4;
  flipped = true;
}

void Flip_D2_F4_ValuesOnly() {
  top_rows += turn * 695784701970ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 11796480ULL;
  top_diag7s += turn * 59373627899904ULL;
  right_columns += turn * 695784701952ULL;
  bottom_diag7s += turn * 695784701952ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D2_F4() {
  Flip_D2_F4_ValuesOnly();
}

void Flip_D2_F4() {
  Flip_D2_F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_F4;
  flipped = true;
}

void Flip_D2_F4G5_ValuesOnly() {
  top_rows += turn * 695784701970ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 12189696ULL;
  top_diag7s += turn * 59373627899904ULL;
  right_columns += turn * 695788240896ULL;
  bottom_diag7s += turn * 695784715776ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D2_F4G5() {
  Flip_D2_F4G5_ValuesOnly();
}

void Flip_D2_F4G5() {
  Flip_D2_F4G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_F4G5;
  flipped = true;
}

void Flip_D2E3_ValuesOnly() {
  top_rows += turn * 695788240896ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 14155776ULL;
  top_diag7s += turn * 59373627900066ULL;
  right_columns += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D2E3() {
  Flip_D2E3_ValuesOnly();
}

void Flip_D2E3() {
  Flip_D2E3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2E3;
  flipped = true;
}

void Flip_D2E3_G5_ValuesOnly() {
  top_rows += turn * 695788240896ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 14548992ULL;
  top_diag7s += turn * 59373627900066ULL;
  right_columns += turn * 136796838684917760ULL;
  bottom_diag7s += turn * 13824ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D2E3_G5() {
  Flip_D2E3_G5_ValuesOnly();
}

void Flip_D2E3_G5() {
  Flip_D2E3_G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2E3_G5;
  flipped = true;
}

void Flip_D2F4_ValuesOnly() {
  top_rows += turn * 695788240914ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 15335424ULL;
  top_diag7s += turn * 59373627900066ULL;
  right_columns += turn * 136797534466080768ULL;
  bottom_diag7s += turn * 695784701952ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D2F4() {
  Flip_D2F4_ValuesOnly();
}

void Flip_D2F4() {
  Flip_D2F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2F4;
  flipped = true;
}

void Flip_D2G5_ValuesOnly() {
  top_rows += turn * 695788240914ULL;
  left_columns += turn * 1458ULL;
  top_diag9s += turn * 15728640ULL;
  top_diag7s += turn * 59373627900066ULL;
  right_columns += turn * 136797534469619712ULL;
  bottom_diag7s += turn * 695784715776ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D2G5() {
  Flip_D2G5_ValuesOnly();
}

void Flip_D2G5() {
  Flip_D2G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2G5;
  flipped = true;
}

void Flip_E3_G5_ValuesOnly() {
  top_rows += turn * 3538944ULL;
  top_diag9s += turn * 3932160ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 136796838684917760ULL;
  bottom_diag7s += turn * 13824ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E3_G5() {
  Flip_E3_G5_ValuesOnly();
}

void Flip_E3_G5() {
  Flip_E3_G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_G5;
  flipped = true;
}

void Flip_E3F4_ValuesOnly() {
  top_rows += turn * 3538962ULL;
  top_diag9s += turn * 4718592ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 136797534466080768ULL;
  bottom_diag7s += turn * 695784701952ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E3F4() {
  Flip_E3F4_ValuesOnly();
}

void Flip_E3F4() {
  Flip_E3F4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3F4;
  flipped = true;
}

void Flip_E3G5_ValuesOnly() {
  top_rows += turn * 3538962ULL;
  top_diag9s += turn * 5111808ULL;
  top_diag7s += turn * 162ULL;
  right_columns += turn * 136797534469619712ULL;
  bottom_diag7s += turn * 695784715776ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E3G5() {
  Flip_E3G5_ValuesOnly();
}

void Flip_E3G5() {
  Flip_E3G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3G5;
  flipped = true;
}

void Flip_F4G5_ValuesOnly() {
  top_rows += turn * 18ULL;
  top_diag9s += turn * 1572864ULL;
  right_columns += turn * 695788240896ULL;
  bottom_diag7s += turn * 695784715776ULL;
  bottom_rows += turn * 1688849860263936ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F4G5() {
  Flip_F4G5_ValuesOnly();
}

void Flip_F4G5() {
  Flip_F4G5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4G5;
  flipped = true;
}

void Flip_C2_E4_ValuesOnly() {
  top_rows += turn * 2087354105910ULL;
  left_columns += turn * 95551488ULL;
  top_diag9s += turn * 540ULL;
  top_diag7s += turn * 5066549580791808ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C2_E4() {
  Flip_C2_E4_ValuesOnly();
}

void Flip_C2_E4() {
  Flip_C2_E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E4;
  flipped = true;
}

void Flip_C2_E4F5_ValuesOnly() {
  top_rows += turn * 2087354105910ULL;
  left_columns += turn * 95551488ULL;
  top_diag9s += turn * 558ULL;
  top_diag7s += turn * 5066549580791808ULL;
  right_columns += turn * 45599178155360256ULL;
  bottom_diag7s += turn * 45598946230665216ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C2_E4F5() {
  Flip_C2_E4F5_ValuesOnly();
}

void Flip_C2_E4F5() {
  Flip_C2_E4F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E4F5;
  flipped = true;
}

void Flip_C2_E4G6_ValuesOnly() {
  top_rows += turn * 2087354105910ULL;
  left_columns += turn * 95551488ULL;
  top_diag9s += turn * 564ULL;
  top_diag7s += turn * 5066549580791808ULL;
  right_columns += turn * 45599178156539904ULL;
  bottom_diag7s += turn * 45598946230665234ULL;
  bottom_rows += turn * 5066575350595584ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2_E4G6() {
  Flip_C2_E4G6_ValuesOnly();
}

void Flip_C2_E4G6() {
  Flip_C2_E4G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E4G6;
  flipped = true;
}

void Flip_C2D3_ValuesOnly() {
  top_rows += turn * 2087364722688ULL;
  left_columns += turn * 95551974ULL;
  top_diag9s += turn * 648ULL;
  top_diag7s += turn * 5066549584330752ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C2D3() {
  Flip_C2D3_ValuesOnly();
}

void Flip_C2D3() {
  Flip_C2D3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D3;
  flipped = true;
}

void Flip_C2D3_F5_ValuesOnly() {
  top_rows += turn * 2087364722688ULL;
  left_columns += turn * 95551974ULL;
  top_diag9s += turn * 666ULL;
  top_diag7s += turn * 5066549584330752ULL;
  right_columns += turn * 231928233984ULL;
  bottom_diag7s += turn * 3538944ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C2D3_F5() {
  Flip_C2D3_F5_ValuesOnly();
}

void Flip_C2D3_F5() {
  Flip_C2D3_F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D3_F5;
  flipped = true;
}

void Flip_C2D3_F5G6_ValuesOnly() {
  top_rows += turn * 2087364722688ULL;
  left_columns += turn * 95551974ULL;
  top_diag9s += turn * 672ULL;
  top_diag7s += turn * 5066549584330752ULL;
  right_columns += turn * 231929413632ULL;
  bottom_diag7s += turn * 3538962ULL;
  bottom_rows += turn * 5066575350595584ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2D3_F5G6() {
  Flip_C2D3_F5G6_ValuesOnly();
}

void Flip_C2D3_F5G6() {
  Flip_C2D3_F5G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D3_F5G6;
  flipped = true;
}

void Flip_C2E4_ValuesOnly() {
  top_rows += turn * 2087364722742ULL;
  left_columns += turn * 95551974ULL;
  top_diag9s += turn * 702ULL;
  top_diag7s += turn * 5066549584330752ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C2E4() {
  Flip_C2E4_ValuesOnly();
}

void Flip_C2E4() {
  Flip_C2E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2E4;
  flipped = true;
}

void Flip_C2E4_G6_ValuesOnly() {
  top_rows += turn * 2087364722742ULL;
  left_columns += turn * 95551974ULL;
  top_diag9s += turn * 708ULL;
  top_diag7s += turn * 5066549584330752ULL;
  right_columns += turn * 45598946228305920ULL;
  bottom_diag7s += turn * 45598946227126290ULL;
  bottom_rows += turn * 25769803776ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2E4_G6() {
  Flip_C2E4_G6_ValuesOnly();
}

void Flip_C2E4_G6() {
  Flip_C2E4_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2E4_G6;
  flipped = true;
}

void Flip_C2F5_ValuesOnly() {
  top_rows += turn * 2087364722742ULL;
  left_columns += turn * 95551974ULL;
  top_diag9s += turn * 720ULL;
  top_diag7s += turn * 5066549584330752ULL;
  right_columns += turn * 45599178155360256ULL;
  bottom_diag7s += turn * 45598946230665216ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C2F5() {
  Flip_C2F5_ValuesOnly();
}

void Flip_C2F5() {
  Flip_C2F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2F5;
  flipped = true;
}

void Flip_C2G6_ValuesOnly() {
  top_rows += turn * 2087364722742ULL;
  left_columns += turn * 95551974ULL;
  top_diag9s += turn * 726ULL;
  top_diag7s += turn * 5066549584330752ULL;
  right_columns += turn * 45599178156539904ULL;
  bottom_diag7s += turn * 45598946230665234ULL;
  bottom_rows += turn * 5066575350595584ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C2G6() {
  Flip_C2G6_ValuesOnly();
}

void Flip_C2G6() {
  Flip_C2G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2G6;
  flipped = true;
}

void Flip_D3_F5_ValuesOnly() {
  top_rows += turn * 10616832ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 180ULL;
  top_diag7s += turn * 3538944ULL;
  right_columns += turn * 231928233984ULL;
  bottom_diag7s += turn * 3538944ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D3_F5() {
  Flip_D3_F5_ValuesOnly();
}

void Flip_D3_F5() {
  Flip_D3_F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_F5;
  flipped = true;
}

void Flip_D3_F5G6_ValuesOnly() {
  top_rows += turn * 10616832ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 186ULL;
  top_diag7s += turn * 3538944ULL;
  right_columns += turn * 231929413632ULL;
  bottom_diag7s += turn * 3538962ULL;
  bottom_rows += turn * 5066575350595584ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D3_F5G6() {
  Flip_D3_F5G6_ValuesOnly();
}

void Flip_D3_F5G6() {
  Flip_D3_F5G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_F5G6;
  flipped = true;
}

void Flip_D3E4_ValuesOnly() {
  top_rows += turn * 10616886ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 216ULL;
  top_diag7s += turn * 3538944ULL;
  right_columns += turn * 45598946227126272ULL;
  bottom_diag7s += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D3E4() {
  Flip_D3E4_ValuesOnly();
}

void Flip_D3E4() {
  Flip_D3E4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3E4;
  flipped = true;
}

void Flip_D3E4_G6_ValuesOnly() {
  top_rows += turn * 10616886ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 222ULL;
  top_diag7s += turn * 3538944ULL;
  right_columns += turn * 45598946228305920ULL;
  bottom_diag7s += turn * 45598946227126290ULL;
  bottom_rows += turn * 25769803776ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D3E4_G6() {
  Flip_D3E4_G6_ValuesOnly();
}

void Flip_D3E4_G6() {
  Flip_D3E4_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3E4_G6;
  flipped = true;
}

void Flip_D3F5_ValuesOnly() {
  top_rows += turn * 10616886ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 234ULL;
  top_diag7s += turn * 3538944ULL;
  right_columns += turn * 45599178155360256ULL;
  bottom_diag7s += turn * 45598946230665216ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D3F5() {
  Flip_D3F5_ValuesOnly();
}

void Flip_D3F5() {
  Flip_D3F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3F5;
  flipped = true;
}

void Flip_D3G6_ValuesOnly() {
  top_rows += turn * 10616886ULL;
  left_columns += turn * 486ULL;
  top_diag9s += turn * 240ULL;
  top_diag7s += turn * 3538944ULL;
  right_columns += turn * 45599178156539904ULL;
  bottom_diag7s += turn * 45598946230665234ULL;
  bottom_rows += turn * 5066575350595584ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D3G6() {
  Flip_D3G6_ValuesOnly();
}

void Flip_D3G6() {
  Flip_D3G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3G6;
  flipped = true;
}

void Flip_E4_G6_ValuesOnly() {
  top_rows += turn * 54ULL;
  top_diag9s += turn * 60ULL;
  right_columns += turn * 45598946228305920ULL;
  bottom_diag7s += turn * 45598946227126290ULL;
  bottom_rows += turn * 25769803776ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E4_G6() {
  Flip_E4_G6_ValuesOnly();
}

void Flip_E4_G6() {
  Flip_E4_G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_G6;
  flipped = true;
}

void Flip_E4F5_ValuesOnly() {
  top_rows += turn * 54ULL;
  top_diag9s += turn * 72ULL;
  right_columns += turn * 45599178155360256ULL;
  bottom_diag7s += turn * 45598946230665216ULL;
  bottom_rows += turn * 5066549580791808ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E4F5() {
  Flip_E4F5_ValuesOnly();
}

void Flip_E4F5() {
  Flip_E4F5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4F5;
  flipped = true;
}

void Flip_E4G6_ValuesOnly() {
  top_rows += turn * 54ULL;
  top_diag9s += turn * 78ULL;
  right_columns += turn * 45599178156539904ULL;
  bottom_diag7s += turn * 45598946230665234ULL;
  bottom_rows += turn * 5066575350595584ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E4G6() {
  Flip_E4G6_ValuesOnly();
}

void Flip_E4G6() {
  Flip_E4G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4G6;
  flipped = true;
}

void Flip_F5G6_ValuesOnly() {
  top_diag9s += turn * 24ULL;
  right_columns += turn * 231929413632ULL;
  bottom_diag7s += turn * 3538962ULL;
  bottom_rows += turn * 5066575350595584ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F5G6() {
  Flip_F5G6_ValuesOnly();
}

void Flip_F5G6() {
  Flip_F5G6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5G6;
  flipped = true;
}

void Flip_B2_D4_ValuesOnly() {
  top_rows += turn * 6262062317730ULL;
  left_columns += turn * 6262062317730ULL;
  top_diag7s += turn * 432345564227567670ULL;
  bottom_diag9s += turn * 455989462271262720ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B2_D4() {
  Flip_B2_D4_ValuesOnly();
}

void Flip_B2_D4() {
  Flip_B2_D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D4;
  flipped = true;
}

void Flip_B2_D4E5_ValuesOnly() {
  top_rows += turn * 6262062317730ULL;
  left_columns += turn * 6262062317730ULL;
  top_diag7s += turn * 432345564227567670ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 471189111013638144ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B2_D4E5() {
  Flip_B2_D4E5_ValuesOnly();
}

void Flip_B2_D4E5() {
  Flip_B2_D4E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D4E5;
  flipped = true;
}

void Flip_B2_D4F6_ValuesOnly() {
  top_rows += turn * 6262062317730ULL;
  left_columns += turn * 6262062317730ULL;
  top_diag7s += turn * 432345564227567670ULL;
  right_columns += turn * 15199726051786752ULL;
  bottom_diag9s += turn * 476255660594429952ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726051786752ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2_D4F6() {
  Flip_B2_D4F6_ValuesOnly();
}

void Flip_B2_D4F6() {
  Flip_B2_D4F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D4F6;
  flipped = true;
}

void Flip_B2_D4G7_ValuesOnly() {
  top_rows += turn * 6262062317730ULL;
  left_columns += turn * 6262062317730ULL;
  top_diag7s += turn * 432345589997371446ULL;
  right_columns += turn * 15199726052179968ULL;
  bottom_diag9s += turn * 477944510454693888ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726052179968ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2_D4G7() {
  Flip_B2_D4G7_ValuesOnly();
}

void Flip_B2_D4G7() {
  Flip_B2_D4G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D4G7;
  flipped = true;
}

void Flip_B2C3_ValuesOnly() {
  top_rows += turn * 6262094168064ULL;
  left_columns += turn * 6262094168064ULL;
  top_diag7s += turn * 432365355436867584ULL;
  bottom_diag9s += turn * 547187354725515264ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B2C3() {
  Flip_B2C3_ValuesOnly();
}

void Flip_B2C3() {
  Flip_B2C3_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C3;
  flipped = true;
}

void Flip_B2C3_E5_ValuesOnly() {
  top_rows += turn * 6262094168064ULL;
  left_columns += turn * 6262094168064ULL;
  top_diag7s += turn * 432365355436867584ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 562387003467890688ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B2C3_E5() {
  Flip_B2C3_E5_ValuesOnly();
}

void Flip_B2C3_E5() {
  Flip_B2C3_E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C3_E5;
  flipped = true;
}

void Flip_B2C3_E5F6_ValuesOnly() {
  top_rows += turn * 6262094168064ULL;
  left_columns += turn * 6262094168064ULL;
  top_diag7s += turn * 432365355436867584ULL;
  right_columns += turn * 15199726051786752ULL;
  bottom_diag9s += turn * 567453553048682496ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726051786752ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2C3_E5F6() {
  Flip_B2C3_E5F6_ValuesOnly();
}

void Flip_B2C3_E5F6() {
  Flip_B2C3_E5F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C3_E5F6;
  flipped = true;
}

void Flip_B2C3_E5G7_ValuesOnly() {
  top_rows += turn * 6262094168064ULL;
  left_columns += turn * 6262094168064ULL;
  top_diag7s += turn * 432365381206671360ULL;
  right_columns += turn * 15199726052179968ULL;
  bottom_diag9s += turn * 569142402908946432ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726052179968ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2C3_E5G7() {
  Flip_B2C3_E5G7_ValuesOnly();
}

void Flip_B2C3_E5G7() {
  Flip_B2C3_E5G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C3_E5G7;
  flipped = true;
}

void Flip_B2D4_ValuesOnly() {
  top_rows += turn * 6262094168226ULL;
  left_columns += turn * 6262094168226ULL;
  top_diag7s += turn * 432365355436867638ULL;
  bottom_diag9s += turn * 592786300952641536ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B2D4() {
  Flip_B2D4_ValuesOnly();
}

void Flip_B2D4() {
  Flip_B2D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D4;
  flipped = true;
}

void Flip_B2D4_F6_ValuesOnly() {
  top_rows += turn * 6262094168226ULL;
  left_columns += turn * 6262094168226ULL;
  top_diag7s += turn * 432365355436867638ULL;
  right_columns += turn * 77309411328ULL;
  bottom_diag9s += turn * 597852850533433344ULL;
  bottom_diag7s += turn * 4608ULL;
  bottom_rows += turn * 77309411328ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2D4_F6() {
  Flip_B2D4_F6_ValuesOnly();
}

void Flip_B2D4_F6() {
  Flip_B2D4_F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D4_F6;
  flipped = true;
}

void Flip_B2D4_F6G7_ValuesOnly() {
  top_rows += turn * 6262094168226ULL;
  left_columns += turn * 6262094168226ULL;
  top_diag7s += turn * 432365381206671414ULL;
  right_columns += turn * 77309804544ULL;
  bottom_diag9s += turn * 599541700393697280ULL;
  bottom_diag7s += turn * 4608ULL;
  bottom_rows += turn * 77309804544ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2D4_F6G7() {
  Flip_B2D4_F6G7_ValuesOnly();
}

void Flip_B2D4_F6G7() {
  Flip_B2D4_F6G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D4_F6G7;
  flipped = true;
}

void Flip_B2E5_ValuesOnly() {
  top_rows += turn * 6262094168226ULL;
  left_columns += turn * 6262094168226ULL;
  top_diag7s += turn * 432365355436867638ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 607985949695016960ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B2E5() {
  Flip_B2E5_ValuesOnly();
}

void Flip_B2E5() {
  Flip_B2E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2E5;
  flipped = true;
}

void Flip_B2E5_G7_ValuesOnly() {
  top_rows += turn * 6262094168226ULL;
  left_columns += turn * 6262094168226ULL;
  top_diag7s += turn * 432365381206671414ULL;
  right_columns += turn * 15199648742768640ULL;
  bottom_diag9s += turn * 609674799555280896ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742768640ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2E5_G7() {
  Flip_B2E5_G7_ValuesOnly();
}

void Flip_B2E5_G7() {
  Flip_B2E5_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2E5_G7;
  flipped = true;
}

void Flip_B2F6_ValuesOnly() {
  top_rows += turn * 6262094168226ULL;
  left_columns += turn * 6262094168226ULL;
  top_diag7s += turn * 432365355436867638ULL;
  right_columns += turn * 15199726051786752ULL;
  bottom_diag9s += turn * 613052499275808768ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726051786752ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B2F6() {
  Flip_B2F6_ValuesOnly();
}

void Flip_B2F6() {
  Flip_B2F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2F6;
  flipped = true;
}

void Flip_B2G7_ValuesOnly() {
  top_rows += turn * 6262094168226ULL;
  left_columns += turn * 6262094168226ULL;
  top_diag7s += turn * 432365381206671414ULL;
  right_columns += turn * 15199726052179968ULL;
  bottom_diag9s += turn * 614741349136072704ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726052179968ULL;
  disk_difference += turn * 12;
}

void UndoFlip_B2G7() {
  Flip_B2G7_ValuesOnly();
}

void Flip_B2G7() {
  Flip_B2G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2G7;
  flipped = true;
}

void Flip_C3_E5_ValuesOnly() {
  top_rows += turn * 31850496ULL;
  left_columns += turn * 31850496ULL;
  top_diag7s += turn * 19791209299968ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 151996487423754240ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C3_E5() {
  Flip_C3_E5_ValuesOnly();
}

void Flip_C3_E5() {
  Flip_C3_E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E5;
  flipped = true;
}

void Flip_C3_E5F6_ValuesOnly() {
  top_rows += turn * 31850496ULL;
  left_columns += turn * 31850496ULL;
  top_diag7s += turn * 19791209299968ULL;
  right_columns += turn * 15199726051786752ULL;
  bottom_diag9s += turn * 157063037004546048ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726051786752ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C3_E5F6() {
  Flip_C3_E5F6_ValuesOnly();
}

void Flip_C3_E5F6() {
  Flip_C3_E5F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E5F6;
  flipped = true;
}

void Flip_C3_E5G7_ValuesOnly() {
  top_rows += turn * 31850496ULL;
  left_columns += turn * 31850496ULL;
  top_diag7s += turn * 19816979103744ULL;
  right_columns += turn * 15199726052179968ULL;
  bottom_diag9s += turn * 158751886864809984ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726052179968ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3_E5G7() {
  Flip_C3_E5G7_ValuesOnly();
}

void Flip_C3_E5G7() {
  Flip_C3_E5G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E5G7;
  flipped = true;
}

void Flip_C3D4_ValuesOnly() {
  top_rows += turn * 31850658ULL;
  left_columns += turn * 31850658ULL;
  top_diag7s += turn * 19791209300022ULL;
  bottom_diag9s += turn * 182395784908505088ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C3D4() {
  Flip_C3D4_ValuesOnly();
}

void Flip_C3D4() {
  Flip_C3D4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D4;
  flipped = true;
}

void Flip_C3D4_F6_ValuesOnly() {
  top_rows += turn * 31850658ULL;
  left_columns += turn * 31850658ULL;
  top_diag7s += turn * 19791209300022ULL;
  right_columns += turn * 77309411328ULL;
  bottom_diag9s += turn * 187462334489296896ULL;
  bottom_diag7s += turn * 4608ULL;
  bottom_rows += turn * 77309411328ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C3D4_F6() {
  Flip_C3D4_F6_ValuesOnly();
}

void Flip_C3D4_F6() {
  Flip_C3D4_F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D4_F6;
  flipped = true;
}

void Flip_C3D4_F6G7_ValuesOnly() {
  top_rows += turn * 31850658ULL;
  left_columns += turn * 31850658ULL;
  top_diag7s += turn * 19816979103798ULL;
  right_columns += turn * 77309804544ULL;
  bottom_diag9s += turn * 189151184349560832ULL;
  bottom_diag7s += turn * 4608ULL;
  bottom_rows += turn * 77309804544ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3D4_F6G7() {
  Flip_C3D4_F6G7_ValuesOnly();
}

void Flip_C3D4_F6G7() {
  Flip_C3D4_F6G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D4_F6G7;
  flipped = true;
}

void Flip_C3E5_ValuesOnly() {
  top_rows += turn * 31850658ULL;
  left_columns += turn * 31850658ULL;
  top_diag7s += turn * 19791209300022ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 197595433650880512ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C3E5() {
  Flip_C3E5_ValuesOnly();
}

void Flip_C3E5() {
  Flip_C3E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3E5;
  flipped = true;
}

void Flip_C3E5_G7_ValuesOnly() {
  top_rows += turn * 31850658ULL;
  left_columns += turn * 31850658ULL;
  top_diag7s += turn * 19816979103798ULL;
  right_columns += turn * 15199648742768640ULL;
  bottom_diag9s += turn * 199284283511144448ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742768640ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3E5_G7() {
  Flip_C3E5_G7_ValuesOnly();
}

void Flip_C3E5_G7() {
  Flip_C3E5_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3E5_G7;
  flipped = true;
}

void Flip_C3F6_ValuesOnly() {
  top_rows += turn * 31850658ULL;
  left_columns += turn * 31850658ULL;
  top_diag7s += turn * 19791209300022ULL;
  right_columns += turn * 15199726051786752ULL;
  bottom_diag9s += turn * 202661983231672320ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726051786752ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C3F6() {
  Flip_C3F6_ValuesOnly();
}

void Flip_C3F6() {
  Flip_C3F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3F6;
  flipped = true;
}

void Flip_C3G7_ValuesOnly() {
  top_rows += turn * 31850658ULL;
  left_columns += turn * 31850658ULL;
  top_diag7s += turn * 19816979103798ULL;
  right_columns += turn * 15199726052179968ULL;
  bottom_diag9s += turn * 204350833091936256ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726052179968ULL;
  disk_difference += turn * 10;
}

void UndoFlip_C3G7() {
  Flip_C3G7_ValuesOnly();
}

void Flip_C3G7() {
  Flip_C3G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3G7;
  flipped = true;
}

void Flip_D4_F6_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 162ULL;
  top_diag7s += turn * 54ULL;
  right_columns += turn * 77309411328ULL;
  bottom_diag9s += turn * 50665495807918080ULL;
  bottom_diag7s += turn * 4608ULL;
  bottom_rows += turn * 77309411328ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D4_F6() {
  Flip_D4_F6_ValuesOnly();
}

void Flip_D4_F6() {
  Flip_D4_F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_F6;
  flipped = true;
}

void Flip_D4_F6G7_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 162ULL;
  top_diag7s += turn * 25769803830ULL;
  right_columns += turn * 77309804544ULL;
  bottom_diag9s += turn * 52354345668182016ULL;
  bottom_diag7s += turn * 4608ULL;
  bottom_rows += turn * 77309804544ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D4_F6G7() {
  Flip_D4_F6G7_ValuesOnly();
}

void Flip_D4_F6G7() {
  Flip_D4_F6G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_F6G7;
  flipped = true;
}

void Flip_D4E5_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 162ULL;
  top_diag7s += turn * 54ULL;
  right_columns += turn * 15199648742375424ULL;
  bottom_diag9s += turn * 60798594969501696ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742375424ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D4E5() {
  Flip_D4E5_ValuesOnly();
}

void Flip_D4E5() {
  Flip_D4E5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4E5;
  flipped = true;
}

void Flip_D4E5_G7_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 162ULL;
  top_diag7s += turn * 25769803830ULL;
  right_columns += turn * 15199648742768640ULL;
  bottom_diag9s += turn * 62487444829765632ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742768640ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D4E5_G7() {
  Flip_D4E5_G7_ValuesOnly();
}

void Flip_D4E5_G7() {
  Flip_D4E5_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4E5_G7;
  flipped = true;
}

void Flip_D4F6_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 162ULL;
  top_diag7s += turn * 54ULL;
  right_columns += turn * 15199726051786752ULL;
  bottom_diag9s += turn * 65865144550293504ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726051786752ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D4F6() {
  Flip_D4F6_ValuesOnly();
}

void Flip_D4F6() {
  Flip_D4F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4F6;
  flipped = true;
}

void Flip_D4G7_ValuesOnly() {
  top_rows += turn * 162ULL;
  left_columns += turn * 162ULL;
  top_diag7s += turn * 25769803830ULL;
  right_columns += turn * 15199726052179968ULL;
  bottom_diag9s += turn * 67553994410557440ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726052179968ULL;
  disk_difference += turn * 8;
}

void UndoFlip_D4G7() {
  Flip_D4G7_ValuesOnly();
}

void Flip_D4G7() {
  Flip_D4G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4G7;
  flipped = true;
}

void Flip_E5_G7_ValuesOnly() {
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 15199648742768640ULL;
  bottom_diag9s += turn * 16888498602639360ULL;
  bottom_diag7s += turn * 231928233984ULL;
  bottom_rows += turn * 15199648742768640ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E5_G7() {
  Flip_E5_G7_ValuesOnly();
}

void Flip_E5_G7() {
  Flip_E5_G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5_G7;
  flipped = true;
}

void Flip_E5F6_ValuesOnly() {
  right_columns += turn * 15199726051786752ULL;
  bottom_diag9s += turn * 20266198323167232ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726051786752ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E5F6() {
  Flip_E5F6_ValuesOnly();
}

void Flip_E5F6() {
  Flip_E5F6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5F6;
  flipped = true;
}

void Flip_E5G7_ValuesOnly() {
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 15199726052179968ULL;
  bottom_diag9s += turn * 21955048183431168ULL;
  bottom_diag7s += turn * 231928238592ULL;
  bottom_rows += turn * 15199726052179968ULL;
  disk_difference += turn * 6;
}

void UndoFlip_E5G7() {
  Flip_E5G7_ValuesOnly();
}

void Flip_E5G7() {
  Flip_E5G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5G7;
  flipped = true;
}

void Flip_F6G7_ValuesOnly() {
  top_diag7s += turn * 25769803776ULL;
  right_columns += turn * 77309804544ULL;
  bottom_diag9s += turn * 6755399441055744ULL;
  bottom_diag7s += turn * 4608ULL;
  bottom_rows += turn * 77309804544ULL;
  disk_difference += turn * 4;
}

void UndoFlip_F6G7() {
  Flip_F6G7_ValuesOnly();
}

void Flip_F6G7() {
  Flip_F6G7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F6G7;
  flipped = true;
}

void Flip_B3_D5_ValuesOnly() {
  top_rows += turn * 95551488ULL;
  left_columns += turn * 2087354105910ULL;
  top_diag7s += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 2319282339840ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B3_D5() {
  Flip_B3_D5_ValuesOnly();
}

void Flip_B3_D5() {
  Flip_B3_D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D5;
  flipped = true;
}

void Flip_B3_D5E6_ValuesOnly() {
  top_rows += turn * 95551488ULL;
  left_columns += turn * 2087354105910ULL;
  top_diag7s += turn * 1688849860263936ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 2396591751168ULL;
  bottom_diag7s += turn * 15199648743555072ULL;
  bottom_rows += turn * 45599178155360256ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B3_D5E6() {
  Flip_B3_D5E6_ValuesOnly();
}

void Flip_B3_D5E6() {
  Flip_B3_D5E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D5E6;
  flipped = true;
}

void Flip_B3_D5F7_ValuesOnly() {
  top_rows += turn * 95551488ULL;
  left_columns += turn * 2087354105910ULL;
  top_diag7s += turn * 1688849860263936ULL;
  right_columns += turn * 5066575350595584ULL;
  bottom_diag9s += turn * 2422361554944ULL;
  bottom_diag7s += turn * 15199648743555078ULL;
  bottom_rows += turn * 45599178156539904ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3_D5F7() {
  Flip_B3_D5F7_ValuesOnly();
}

void Flip_B3_D5F7() {
  Flip_B3_D5F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D5F7;
  flipped = true;
}

void Flip_B3C4_ValuesOnly() {
  top_rows += turn * 95551974ULL;
  left_columns += turn * 2087364722688ULL;
  top_diag7s += turn * 1688849861443584ULL;
  bottom_diag9s += turn * 2783138807808ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B3C4() {
  Flip_B3C4_ValuesOnly();
}

void Flip_B3C4() {
  Flip_B3C4_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C4;
  flipped = true;
}

void Flip_B3C4_E6_ValuesOnly() {
  top_rows += turn * 95551974ULL;
  left_columns += turn * 2087364722688ULL;
  top_diag7s += turn * 1688849861443584ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 2860448219136ULL;
  bottom_diag7s += turn * 1179648ULL;
  bottom_rows += turn * 231928233984ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B3C4_E6() {
  Flip_B3C4_E6_ValuesOnly();
}

void Flip_B3C4_E6() {
  Flip_B3C4_E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C4_E6;
  flipped = true;
}

void Flip_B3C4_E6F7_ValuesOnly() {
  top_rows += turn * 95551974ULL;
  left_columns += turn * 2087364722688ULL;
  top_diag7s += turn * 1688849861443584ULL;
  right_columns += turn * 5066575350595584ULL;
  bottom_diag9s += turn * 2886218022912ULL;
  bottom_diag7s += turn * 1179654ULL;
  bottom_rows += turn * 231929413632ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3C4_E6F7() {
  Flip_B3C4_E6F7_ValuesOnly();
}

void Flip_B3C4_E6F7() {
  Flip_B3C4_E6F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C4_E6F7;
  flipped = true;
}

void Flip_B3D5_ValuesOnly() {
  top_rows += turn * 95551974ULL;
  left_columns += turn * 2087364722742ULL;
  top_diag7s += turn * 1688849861443584ULL;
  bottom_diag9s += turn * 3015067041792ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B3D5() {
  Flip_B3D5_ValuesOnly();
}

void Flip_B3D5() {
  Flip_B3D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3D5;
  flipped = true;
}

void Flip_B3D5_F7_ValuesOnly() {
  top_rows += turn * 95551974ULL;
  left_columns += turn * 2087364722742ULL;
  top_diag7s += turn * 1688849861443584ULL;
  right_columns += turn * 25769803776ULL;
  bottom_diag9s += turn * 3040836845568ULL;
  bottom_diag7s += turn * 15199648742375430ULL;
  bottom_rows += turn * 45598946228305920ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3D5_F7() {
  Flip_B3D5_F7_ValuesOnly();
}

void Flip_B3D5_F7() {
  Flip_B3D5_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3D5_F7;
  flipped = true;
}

void Flip_B3E6_ValuesOnly() {
  top_rows += turn * 95551974ULL;
  left_columns += turn * 2087364722742ULL;
  top_diag7s += turn * 1688849861443584ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 3092376453120ULL;
  bottom_diag7s += turn * 15199648743555072ULL;
  bottom_rows += turn * 45599178155360256ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B3E6() {
  Flip_B3E6_ValuesOnly();
}

void Flip_B3E6() {
  Flip_B3E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3E6;
  flipped = true;
}

void Flip_B3F7_ValuesOnly() {
  top_rows += turn * 95551974ULL;
  left_columns += turn * 2087364722742ULL;
  top_diag7s += turn * 1688849861443584ULL;
  right_columns += turn * 5066575350595584ULL;
  bottom_diag9s += turn * 3118146256896ULL;
  bottom_diag7s += turn * 15199648743555078ULL;
  bottom_rows += turn * 45599178156539904ULL;
  disk_difference += turn * 10;
}

void UndoFlip_B3F7() {
  Flip_B3F7_ValuesOnly();
}

void Flip_B3F7() {
  Flip_B3F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3F7;
  flipped = true;
}

void Flip_C4_E6_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 10616832ULL;
  top_diag7s += turn * 1179648ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 773094113280ULL;
  bottom_diag7s += turn * 1179648ULL;
  bottom_rows += turn * 231928233984ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C4_E6() {
  Flip_C4_E6_ValuesOnly();
}

void Flip_C4_E6() {
  Flip_C4_E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_E6;
  flipped = true;
}

void Flip_C4_E6F7_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 10616832ULL;
  top_diag7s += turn * 1179648ULL;
  right_columns += turn * 5066575350595584ULL;
  bottom_diag9s += turn * 798863917056ULL;
  bottom_diag7s += turn * 1179654ULL;
  bottom_rows += turn * 231929413632ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C4_E6F7() {
  Flip_C4_E6F7_ValuesOnly();
}

void Flip_C4_E6F7() {
  Flip_C4_E6F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_E6F7;
  flipped = true;
}

void Flip_C4D5_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 10616886ULL;
  top_diag7s += turn * 1179648ULL;
  bottom_diag9s += turn * 927712935936ULL;
  bottom_diag7s += turn * 15199648742375424ULL;
  bottom_rows += turn * 45598946227126272ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C4D5() {
  Flip_C4D5_ValuesOnly();
}

void Flip_C4D5() {
  Flip_C4D5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4D5;
  flipped = true;
}

void Flip_C4D5_F7_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 10616886ULL;
  top_diag7s += turn * 1179648ULL;
  right_columns += turn * 25769803776ULL;
  bottom_diag9s += turn * 953482739712ULL;
  bottom_diag7s += turn * 15199648742375430ULL;
  bottom_rows += turn * 45598946228305920ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C4D5_F7() {
  Flip_C4D5_F7_ValuesOnly();
}

void Flip_C4D5_F7() {
  Flip_C4D5_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4D5_F7;
  flipped = true;
}

void Flip_C4E6_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 10616886ULL;
  top_diag7s += turn * 1179648ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 1005022347264ULL;
  bottom_diag7s += turn * 15199648743555072ULL;
  bottom_rows += turn * 45599178155360256ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C4E6() {
  Flip_C4E6_ValuesOnly();
}

void Flip_C4E6() {
  Flip_C4E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4E6;
  flipped = true;
}

void Flip_C4F7_ValuesOnly() {
  top_rows += turn * 486ULL;
  left_columns += turn * 10616886ULL;
  top_diag7s += turn * 1179648ULL;
  right_columns += turn * 5066575350595584ULL;
  bottom_diag9s += turn * 1030792151040ULL;
  bottom_diag7s += turn * 15199648743555078ULL;
  bottom_rows += turn * 45599178156539904ULL;
  disk_difference += turn * 8;
}

void UndoFlip_C4F7() {
  Flip_C4F7_ValuesOnly();
}

void Flip_C4F7() {
  Flip_C4F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4F7;
  flipped = true;
}

void Flip_D5_F7_ValuesOnly() {
  left_columns += turn * 54ULL;
  right_columns += turn * 25769803776ULL;
  bottom_diag9s += turn * 257698037760ULL;
  bottom_diag7s += turn * 15199648742375430ULL;
  bottom_rows += turn * 45598946228305920ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D5_F7() {
  Flip_D5_F7_ValuesOnly();
}

void Flip_D5_F7() {
  Flip_D5_F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5_F7;
  flipped = true;
}

void Flip_D5E6_ValuesOnly() {
  left_columns += turn * 54ULL;
  right_columns += turn * 5066549580791808ULL;
  bottom_diag9s += turn * 309237645312ULL;
  bottom_diag7s += turn * 15199648743555072ULL;
  bottom_rows += turn * 45599178155360256ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D5E6() {
  Flip_D5E6_ValuesOnly();
}

void Flip_D5E6() {
  Flip_D5E6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5E6;
  flipped = true;
}

void Flip_D5F7_ValuesOnly() {
  left_columns += turn * 54ULL;
  right_columns += turn * 5066575350595584ULL;
  bottom_diag9s += turn * 335007449088ULL;
  bottom_diag7s += turn * 15199648743555078ULL;
  bottom_rows += turn * 45599178156539904ULL;
  disk_difference += turn * 6;
}

void UndoFlip_D5F7() {
  Flip_D5F7_ValuesOnly();
}

void Flip_D5F7() {
  Flip_D5F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5F7;
  flipped = true;
}

void Flip_E6F7_ValuesOnly() {
  right_columns += turn * 5066575350595584ULL;
  bottom_diag9s += turn * 103079215104ULL;
  bottom_diag7s += turn * 1179654ULL;
  bottom_rows += turn * 231929413632ULL;
  disk_difference += turn * 4;
}

void UndoFlip_E6F7() {
  Flip_E6F7_ValuesOnly();
}

void Flip_E6F7() {
  Flip_E6F7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6F7;
  flipped = true;
}

void Flip_B4_D6_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 695784701970ULL;
  top_diag7s += turn * 6597069766656ULL;
  bottom_diag9s += turn * 11796480ULL;
  bottom_diag7s += turn * 77309411328ULL;
  bottom_rows += turn * 695784701952ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B4_D6() {
  Flip_B4_D6_ValuesOnly();
}

void Flip_B4_D6() {
  Flip_B4_D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D6;
  flipped = true;
}

void Flip_B4_D6E7_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 695784701970ULL;
  top_diag7s += turn * 6597069766656ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 12189696ULL;
  bottom_diag7s += turn * 77309412864ULL;
  bottom_rows += turn * 695788240896ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B4_D6E7() {
  Flip_B4_D6E7_ValuesOnly();
}

void Flip_B4_D6E7() {
  Flip_B4_D6E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D6E7;
  flipped = true;
}

void Flip_B4C5_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 695788240896ULL;
  top_diag7s += turn * 6597069766674ULL;
  bottom_diag9s += turn * 14155776ULL;
  bottom_rows += turn * 136796838681378816ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B4C5() {
  Flip_B4C5_ValuesOnly();
}

void Flip_B4C5() {
  Flip_B4C5_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C5;
  flipped = true;
}

void Flip_B4C5_E7_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 695788240896ULL;
  top_diag7s += turn * 6597069766674ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 14548992ULL;
  bottom_diag7s += turn * 1536ULL;
  bottom_rows += turn * 136796838684917760ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B4C5_E7() {
  Flip_B4C5_E7_ValuesOnly();
}

void Flip_B4C5_E7() {
  Flip_B4C5_E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C5_E7;
  flipped = true;
}

void Flip_B4D6_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 695788240914ULL;
  top_diag7s += turn * 6597069766674ULL;
  bottom_diag9s += turn * 15335424ULL;
  bottom_diag7s += turn * 77309411328ULL;
  bottom_rows += turn * 136797534466080768ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B4D6() {
  Flip_B4D6_ValuesOnly();
}

void Flip_B4D6() {
  Flip_B4D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4D6;
  flipped = true;
}

void Flip_B4E7_ValuesOnly() {
  top_rows += turn * 1458ULL;
  left_columns += turn * 695788240914ULL;
  top_diag7s += turn * 6597069766674ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 15728640ULL;
  bottom_diag7s += turn * 77309412864ULL;
  bottom_rows += turn * 136797534469619712ULL;
  disk_difference += turn * 8;
}

void UndoFlip_B4E7() {
  Flip_B4E7_ValuesOnly();
}

void Flip_B4E7() {
  Flip_B4E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4E7;
  flipped = true;
}

void Flip_C5_E7_ValuesOnly() {
  left_columns += turn * 3538944ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 3932160ULL;
  bottom_diag7s += turn * 1536ULL;
  bottom_rows += turn * 136796838684917760ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C5_E7() {
  Flip_C5_E7_ValuesOnly();
}

void Flip_C5_E7() {
  Flip_C5_E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5_E7;
  flipped = true;
}

void Flip_C5D6_ValuesOnly() {
  left_columns += turn * 3538962ULL;
  top_diag7s += turn * 18ULL;
  bottom_diag9s += turn * 4718592ULL;
  bottom_diag7s += turn * 77309411328ULL;
  bottom_rows += turn * 136797534466080768ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C5D6() {
  Flip_C5D6_ValuesOnly();
}

void Flip_C5D6() {
  Flip_C5D6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5D6;
  flipped = true;
}

void Flip_C5E7_ValuesOnly() {
  left_columns += turn * 3538962ULL;
  top_diag7s += turn * 18ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 5111808ULL;
  bottom_diag7s += turn * 77309412864ULL;
  bottom_rows += turn * 136797534469619712ULL;
  disk_difference += turn * 6;
}

void UndoFlip_C5E7() {
  Flip_C5E7_ValuesOnly();
}

void Flip_C5E7() {
  Flip_C5E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5E7;
  flipped = true;
}

void Flip_D6E7_ValuesOnly() {
  left_columns += turn * 18ULL;
  right_columns += turn * 1688849860263936ULL;
  bottom_diag9s += turn * 1572864ULL;
  bottom_diag7s += turn * 77309412864ULL;
  bottom_rows += turn * 695788240896ULL;
  disk_difference += turn * 4;
}

void UndoFlip_D6E7() {
  Flip_D6E7_ValuesOnly();
}

void Flip_D6E7() {
  Flip_D6E7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6E7;
  flipped = true;
}

void Flip_B5_D7_ValuesOnly() {
  left_columns += turn * 231928233990ULL;
  top_diag7s += turn * 393216ULL;
  bottom_diag9s += turn * 15360ULL;
  bottom_diag7s += turn * 393216ULL;
  bottom_rows += turn * 410390516054753280ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B5_D7() {
  Flip_B5_D7_ValuesOnly();
}

void Flip_B5_D7() {
  Flip_B5_D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_D7;
  flipped = true;
}

void Flip_B5C6_ValuesOnly() {
  left_columns += turn * 231929413632ULL;
  top_diag7s += turn * 393216ULL;
  bottom_diag9s += turn * 18432ULL;
  bottom_diag7s += turn * 5066549580791808ULL;
  bottom_rows += turn * 410392603398242304ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B5C6() {
  Flip_B5C6_ValuesOnly();
}

void Flip_B5C6() {
  Flip_B5C6_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5C6;
  flipped = true;
}

void Flip_B5D7_ValuesOnly() {
  left_columns += turn * 231929413638ULL;
  top_diag7s += turn * 393216ULL;
  bottom_diag9s += turn * 19968ULL;
  bottom_diag7s += turn * 5066549581185024ULL;
  bottom_rows += turn * 410392603408859136ULL;
  disk_difference += turn * 6;
}

void UndoFlip_B5D7() {
  Flip_B5D7_ValuesOnly();
}

void Flip_B5D7() {
  Flip_B5D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5D7;
  flipped = true;
}

void Flip_C6D7_ValuesOnly() {
  left_columns += turn * 1179654ULL;
  bottom_diag9s += turn * 6144ULL;
  bottom_diag7s += turn * 5066549581185024ULL;
  bottom_rows += turn * 2087364722688ULL;
  disk_difference += turn * 4;
}

void UndoFlip_C6D7() {
  Flip_C6D7_ValuesOnly();
}

void Flip_C6D7() {
  Flip_C6D7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6D7;
  flipped = true;
}

void Flip_B6C7_ValuesOnly() {
  left_columns += turn * 77309804544ULL;
  top_diag7s += turn * 6ULL;
  bottom_diag9s += turn * 24ULL;
  bottom_diag7s += turn * 25769803776ULL;
  bottom_rows += turn * 6262094168064ULL;
  disk_difference += turn * 4;
}

void UndoFlip_B6C7() {
  Flip_B6C7_ValuesOnly();
}

void Flip_B6C7() {
  Flip_B6C7_ValuesOnly();
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6C7;
  flipped = true;
}

void Set_A1() {
  top_rows += turn * 615585774066204672ULL;
  left_columns += turn * 615585774066204672ULL;
  bottom_diag9s += turn * 615585774066204672ULL;
  disk_difference += turn;
}

void Set_B1() {
  top_rows += turn * 205195258022068224ULL;
  left_columns += turn * 9393093476352ULL;
  top_diag9s += turn * 729ULL;
  disk_difference += turn;
}

void Set_C1() {
  top_rows += turn * 68398419340689408ULL;
  left_columns += turn * 143327232ULL;
  top_diag7s += turn * 648518346341351424ULL;
  top_diag9s += turn * 15925248ULL;
  disk_difference += turn;
}

void Set_D1() {
  top_rows += turn * 22799473113563136ULL;
  left_columns += turn * 2187ULL;
  top_diag7s += turn * 7599824371187712ULL;
  top_diag9s += turn * 89060441849856ULL;
  disk_difference += turn;
}

void Set_E1() {
  top_rows += turn * 7599824371187712ULL;
  right_columns += turn * 615585774066204672ULL;
  top_diag7s += turn * 89060441849856ULL;
  top_diag9s += turn * 7599824371187712ULL;
  disk_difference += turn;
}

void Set_F1() {
  top_rows += turn * 2533274790395904ULL;
  right_columns += turn * 9393093476352ULL;
  top_diag7s += turn * 15925248ULL;
  top_diag9s += turn * 648518346341351424ULL;
  disk_difference += turn;
}

void Set_G1() {
  top_rows += turn * 844424930131968ULL;
  right_columns += turn * 143327232ULL;
  top_diag7s += turn * 729ULL;
  disk_difference += turn;
}

void Set_H1() {
  top_rows += turn * 281474976710656ULL;
  right_columns += turn * 2187ULL;
  bottom_diag7s += turn * 615585774066204672ULL;
  disk_difference += turn;
}

void Set_A2() {
  top_rows += turn * 9393093476352ULL;
  left_columns += turn * 205195258022068224ULL;
  bottom_diag9s += turn * 3131031158784ULL;
  disk_difference += turn;
}

void Set_B2() {
  top_rows += turn * 3131031158784ULL;
  left_columns += turn * 3131031158784ULL;
  top_diag7s += turn * 216172782113783808ULL;
  bottom_diag9s += turn * 205195258022068224ULL;
  disk_difference += turn;
}

void Set_C2() {
  top_rows += turn * 1043677052928ULL;
  left_columns += turn * 47775744ULL;
  top_diag7s += turn * 2533274790395904ULL;
  top_diag9s += turn * 243ULL;
  disk_difference += turn;
}

void Set_D2() {
  top_rows += turn * 347892350976ULL;
  left_columns += turn * 729ULL;
  top_diag7s += turn * 29686813949952ULL;
  top_diag9s += turn * 5308416ULL;
  disk_difference += turn;
}

void Set_E2() {
  top_rows += turn * 115964116992ULL;
  right_columns += turn * 205195258022068224ULL;
  top_diag7s += turn * 5308416ULL;
  top_diag9s += turn * 29686813949952ULL;
  disk_difference += turn;
}

void Set_F2() {
  top_rows += turn * 38654705664ULL;
  right_columns += turn * 3131031158784ULL;
  top_diag7s += turn * 243ULL;
  top_diag9s += turn * 2533274790395904ULL;
  disk_difference += turn;
}

void Set_G2() {
  top_rows += turn * 12884901888ULL;
  right_columns += turn * 47775744ULL;
  bottom_diag7s += turn * 205195258022068224ULL;
  top_diag9s += turn * 216172782113783808ULL;
  disk_difference += turn;
}

void Set_H2() {
  top_rows += turn * 4294967296ULL;
  right_columns += turn * 729ULL;
  bottom_diag7s += turn * 3131031158784ULL;
  disk_difference += turn;
}

void Set_A3() {
  top_rows += turn * 143327232ULL;
  left_columns += turn * 68398419340689408ULL;
  top_diag7s += turn * 72057594037927936ULL;
  bottom_diag9s += turn * 15925248ULL;
  disk_difference += turn;
}

void Set_B3() {
  top_rows += turn * 47775744ULL;
  left_columns += turn * 1043677052928ULL;
  top_diag7s += turn * 844424930131968ULL;
  bottom_diag9s += turn * 1043677052928ULL;
  disk_difference += turn;
}

void Set_C3() {
  top_rows += turn * 15925248ULL;
  left_columns += turn * 15925248ULL;
  top_diag7s += turn * 9895604649984ULL;
  bottom_diag9s += turn * 68398419340689408ULL;
  disk_difference += turn;
}

void Set_D3() {
  top_rows += turn * 5308416ULL;
  left_columns += turn * 243ULL;
  top_diag7s += turn * 1769472ULL;
  top_diag9s += turn * 81ULL;
  disk_difference += turn;
}

void Set_E3() {
  top_rows += turn * 1769472ULL;
  right_columns += turn * 68398419340689408ULL;
  top_diag7s += turn * 81ULL;
  top_diag9s += turn * 1769472ULL;
  disk_difference += turn;
}

void Set_F3() {
  top_rows += turn * 589824ULL;
  right_columns += turn * 1043677052928ULL;
  bottom_diag7s += turn * 68398419340689408ULL;
  top_diag9s += turn * 9895604649984ULL;
  disk_difference += turn;
}

void Set_G3() {
  top_rows += turn * 196608ULL;
  right_columns += turn * 15925248ULL;
  bottom_diag7s += turn * 1043677052928ULL;
  top_diag9s += turn * 844424930131968ULL;
  disk_difference += turn;
}

void Set_H3() {
  top_rows += turn * 65536ULL;
  right_columns += turn * 243ULL;
  bottom_diag7s += turn * 15925248ULL;
  top_diag9s += turn * 72057594037927936ULL;
  disk_difference += turn;
}

void Set_A4() {
  top_rows += turn * 2187ULL;
  left_columns += turn * 22799473113563136ULL;
  top_diag7s += turn * 281474976710656ULL;
  bottom_diag9s += turn * 20736ULL;
  disk_difference += turn;
}

void Set_B4() {
  top_rows += turn * 729ULL;
  left_columns += turn * 347892350976ULL;
  top_diag7s += turn * 3298534883328ULL;
  bottom_diag9s += turn * 5308416ULL;
  disk_difference += turn;
}

void Set_C4() {
  top_rows += turn * 243ULL;
  left_columns += turn * 5308416ULL;
  top_diag7s += turn * 589824ULL;
  bottom_diag9s += turn * 347892350976ULL;
  disk_difference += turn;
}

void Set_D4() {
  top_rows += turn * 81ULL;
  left_columns += turn * 81ULL;
  top_diag7s += turn * 27ULL;
  bottom_diag9s += turn * 22799473113563136ULL;
  disk_difference += turn;
}

void Set_E4() {
  top_rows += turn * 27ULL;
  right_columns += turn * 22799473113563136ULL;
  bottom_diag7s += turn * 22799473113563136ULL;
  top_diag9s += turn * 27ULL;
  disk_difference += turn;
}

void Set_F4() {
  top_rows += turn * 9ULL;
  right_columns += turn * 347892350976ULL;
  bottom_diag7s += turn * 347892350976ULL;
  top_diag9s += turn * 589824ULL;
  disk_difference += turn;
}

void Set_G4() {
  top_rows += turn * 3ULL;
  right_columns += turn * 5308416ULL;
  bottom_diag7s += turn * 5308416ULL;
  top_diag9s += turn * 3298534883328ULL;
  disk_difference += turn;
}

void Set_H4() {
  top_rows += turn * 1ULL;
  right_columns += turn * 81ULL;
  bottom_diag7s += turn * 20736ULL;
  top_diag9s += turn * 281474976710656ULL;
  disk_difference += turn;
}

void Set_A5() {
  bottom_rows += turn * 615585774066204672ULL;
  left_columns += turn * 7599824371187712ULL;
  top_diag7s += turn * 1099511627776ULL;
  bottom_diag9s += turn * 27ULL;
  disk_difference += turn;
}

void Set_B5() {
  bottom_rows += turn * 205195258022068224ULL;
  left_columns += turn * 115964116992ULL;
  top_diag7s += turn * 196608ULL;
  bottom_diag9s += turn * 6912ULL;
  disk_difference += turn;
}

void Set_C5() {
  bottom_rows += turn * 68398419340689408ULL;
  left_columns += turn * 1769472ULL;
  top_diag7s += turn * 9ULL;
  bottom_diag9s += turn * 1769472ULL;
  disk_difference += turn;
}

void Set_D5() {
  bottom_rows += turn * 22799473113563136ULL;
  left_columns += turn * 27ULL;
  bottom_diag7s += turn * 7599824371187712ULL;
  bottom_diag9s += turn * 115964116992ULL;
  disk_difference += turn;
}

void Set_E5() {
  bottom_rows += turn * 7599824371187712ULL;
  right_columns += turn * 7599824371187712ULL;
  bottom_diag7s += turn * 115964116992ULL;
  bottom_diag9s += turn * 7599824371187712ULL;
  disk_difference += turn;
}

void Set_F5() {
  bottom_rows += turn * 2533274790395904ULL;
  right_columns += turn * 115964116992ULL;
  bottom_diag7s += turn * 1769472ULL;
  top_diag9s += turn * 9ULL;
  disk_difference += turn;
}

void Set_G5() {
  bottom_rows += turn * 844424930131968ULL;
  right_columns += turn * 1769472ULL;
  bottom_diag7s += turn * 6912ULL;
  top_diag9s += turn * 196608ULL;
  disk_difference += turn;
}

void Set_H5() {
  bottom_rows += turn * 281474976710656ULL;
  right_columns += turn * 27ULL;
  bottom_diag7s += turn * 27ULL;
  top_diag9s += turn * 1099511627776ULL;
  disk_difference += turn;
}

void Set_A6() {
  bottom_rows += turn * 9393093476352ULL;
  left_columns += turn * 2533274790395904ULL;
  top_diag7s += turn * 65536ULL;
  top_diag9s += turn * 38654705664ULL;
  disk_difference += turn;
}

void Set_B6() {
  bottom_rows += turn * 3131031158784ULL;
  left_columns += turn * 38654705664ULL;
  top_diag7s += turn * 3ULL;
  bottom_diag9s += turn * 9ULL;
  disk_difference += turn;
}

void Set_C6() {
  bottom_rows += turn * 1043677052928ULL;
  left_columns += turn * 589824ULL;
  bottom_diag7s += turn * 2533274790395904ULL;
  bottom_diag9s += turn * 2304ULL;
  disk_difference += turn;
}

void Set_D6() {
  bottom_rows += turn * 347892350976ULL;
  left_columns += turn * 9ULL;
  bottom_diag7s += turn * 38654705664ULL;
  bottom_diag9s += turn * 589824ULL;
  disk_difference += turn;
}

void Set_E6() {
  bottom_rows += turn * 115964116992ULL;
  right_columns += turn * 2533274790395904ULL;
  bottom_diag7s += turn * 589824ULL;
  bottom_diag9s += turn * 38654705664ULL;
  disk_difference += turn;
}

void Set_F6() {
  bottom_rows += turn * 38654705664ULL;
  right_columns += turn * 38654705664ULL;
  bottom_diag7s += turn * 2304ULL;
  bottom_diag9s += turn * 2533274790395904ULL;
  disk_difference += turn;
}

void Set_G6() {
  bottom_rows += turn * 12884901888ULL;
  right_columns += turn * 589824ULL;
  bottom_diag7s += turn * 9ULL;
  top_diag9s += turn * 3ULL;
  disk_difference += turn;
}

void Set_H6() {
  bottom_rows += turn * 4294967296ULL;
  right_columns += turn * 9ULL;
  top_diag7s += turn * 38654705664ULL;
  top_diag9s += turn * 65536ULL;
  disk_difference += turn;
}

void Set_A7() {
  bottom_rows += turn * 143327232ULL;
  left_columns += turn * 844424930131968ULL;
  top_diag7s += turn * 1ULL;
  disk_difference += turn;
}

void Set_B7() {
  bottom_rows += turn * 47775744ULL;
  left_columns += turn * 12884901888ULL;
  bottom_diag7s += turn * 844424930131968ULL;
  top_diag9s += turn * 12884901888ULL;
  disk_difference += turn;
}

void Set_C7() {
  bottom_rows += turn * 15925248ULL;
  left_columns += turn * 196608ULL;
  bottom_diag7s += turn * 12884901888ULL;
  bottom_diag9s += turn * 3ULL;
  disk_difference += turn;
}

void Set_D7() {
  bottom_rows += turn * 5308416ULL;
  left_columns += turn * 3ULL;
  bottom_diag7s += turn * 196608ULL;
  bottom_diag9s += turn * 768ULL;
  disk_difference += turn;
}

void Set_E7() {
  bottom_rows += turn * 1769472ULL;
  right_columns += turn * 844424930131968ULL;
  bottom_diag7s += turn * 768ULL;
  bottom_diag9s += turn * 196608ULL;
  disk_difference += turn;
}

void Set_F7() {
  bottom_rows += turn * 589824ULL;
  right_columns += turn * 12884901888ULL;
  bottom_diag7s += turn * 3ULL;
  bottom_diag9s += turn * 12884901888ULL;
  disk_difference += turn;
}

void Set_G7() {
  bottom_rows += turn * 196608ULL;
  right_columns += turn * 196608ULL;
  top_diag7s += turn * 12884901888ULL;
  bottom_diag9s += turn * 844424930131968ULL;
  disk_difference += turn;
}

void Set_H7() {
  bottom_rows += turn * 65536ULL;
  right_columns += turn * 3ULL;
  top_diag9s += turn * 1ULL;
  disk_difference += turn;
}

void Set_A8() {
  bottom_rows += turn * 2187ULL;
  left_columns += turn * 281474976710656ULL;
  bottom_diag7s += turn * 281474976710656ULL;
  disk_difference += turn;
}

void Set_B8() {
  bottom_rows += turn * 729ULL;
  left_columns += turn * 4294967296ULL;
  bottom_diag7s += turn * 4294967296ULL;
  disk_difference += turn;
}

void Set_C8() {
  bottom_rows += turn * 243ULL;
  left_columns += turn * 65536ULL;
  bottom_diag7s += turn * 65536ULL;
  top_diag9s += turn * 4294967296ULL;
  disk_difference += turn;
}

void Set_D8() {
  bottom_rows += turn * 81ULL;
  left_columns += turn * 1ULL;
  bottom_diag7s += turn * 256ULL;
  bottom_diag9s += turn * 1ULL;
  disk_difference += turn;
}

void Set_E8() {
  bottom_rows += turn * 27ULL;
  right_columns += turn * 281474976710656ULL;
  bottom_diag7s += turn * 1ULL;
  bottom_diag9s += turn * 256ULL;
  disk_difference += turn;
}

void Set_F8() {
  bottom_rows += turn * 9ULL;
  right_columns += turn * 4294967296ULL;
  top_diag7s += turn * 4294967296ULL;
  bottom_diag9s += turn * 65536ULL;
  disk_difference += turn;
}

void Set_G8() {
  bottom_rows += turn * 3ULL;
  right_columns += turn * 65536ULL;
  bottom_diag9s += turn * 4294967296ULL;
  disk_difference += turn;
}

void Set_H8() {
  bottom_rows += turn * 1ULL;
  right_columns += turn * 1ULL;
  bottom_diag9s += turn * 281474976710656ULL;
  disk_difference += turn;
}

#endif  // SENSEI_BOARD_OPTIMIZED_FLIP_OPTIMIZED_H
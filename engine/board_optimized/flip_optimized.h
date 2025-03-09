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


void Invert() {
  top_rows = 1846504022637287840ULL - top_rows;
  bottom_rows = 1846504022637287840ULL - bottom_rows;
  left_columns = 1846504022637287840ULL - left_columns;
  right_columns = 1846504022637287840ULL - right_columns;
  top_diag7s = 1896281636653762698ULL - top_diag7s;
  bottom_diag7s = 1846485236068184656ULL - bottom_diag7s;
  top_diag9s = 1896281636653762698ULL - top_diag9s;
  bottom_diag9s = 1846485236068184656ULL - bottom_diag9s;
  disk_difference = -disk_difference;
}
void UndoFlip_B1() {
  top_rows -= 410390516044136448ULL;
  left_columns -= 18786186952704ULL;
  top_diag9s -= 1458ULL;
  disk_difference -= 2;
}

void Flip_B1() {
  top_rows += 410390516044136448ULL;
  left_columns += 18786186952704ULL;
  top_diag9s += 1458ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1;
  flipped = true;
}

void UndoFlip_B1_D1() {
  top_rows -= 455989462271262720ULL;
  left_columns -= 18786186957078ULL;
  top_diag9s -= 178120883701170ULL;
  top_diag7s -= 15199648742375424ULL;
  disk_difference -= 4;
}

void Flip_B1_D1() {
  top_rows += 455989462271262720ULL;
  left_columns += 18786186957078ULL;
  top_diag9s += 178120883701170ULL;
  top_diag7s += 15199648742375424ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1_D1;
  flipped = true;
}

void UndoFlip_B1_D1E1() {
  top_rows -= 471189111013638144ULL;
  left_columns -= 18786186957078ULL;
  top_diag9s -= 15377769626076594ULL;
  top_diag7s -= 15377769626075136ULL;
  right_columns -= 1231171548132409344ULL;
  disk_difference -= 6;
}

void Flip_B1_D1E1() {
  top_rows += 471189111013638144ULL;
  left_columns += 18786186957078ULL;
  top_diag9s += 15377769626076594ULL;
  top_diag7s += 15377769626075136ULL;
  right_columns += 1231171548132409344ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1_D1E1;
  flipped = true;
}

void UndoFlip_B1_D1F1() {
  top_rows -= 476255660594429952ULL;
  left_columns -= 18786186957078ULL;
  top_diag9s -= 1312414462308779442ULL;
  top_diag7s -= 15377769657925632ULL;
  right_columns -= 1231190334319362048ULL;
  disk_difference -= 8;
}

void Flip_B1_D1F1() {
  top_rows += 476255660594429952ULL;
  left_columns += 18786186957078ULL;
  top_diag9s += 1312414462308779442ULL;
  top_diag7s += 15377769657925632ULL;
  right_columns += 1231190334319362048ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1_D1F1;
  flipped = true;
}

void UndoFlip_B1_D1G1() {
  top_rows -= 477944510454693888ULL;
  left_columns -= 18786186957078ULL;
  top_diag9s -= 1312414462308779442ULL;
  top_diag7s -= 15377769657927090ULL;
  right_columns -= 1231190334606016512ULL;
  disk_difference -= 10;
}

void Flip_B1_D1G1() {
  top_rows += 477944510454693888ULL;
  left_columns += 18786186957078ULL;
  top_diag9s += 1312414462308779442ULL;
  top_diag7s += 15377769657927090ULL;
  right_columns += 1231190334606016512ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1_D1G1;
  flipped = true;
}

void UndoFlip_B1C1() {
  top_rows -= 547187354725515264ULL;
  left_columns -= 18786473607168ULL;
  top_diag9s -= 31851954ULL;
  top_diag7s -= 1297036692682702848ULL;
  disk_difference -= 4;
}

void Flip_B1C1() {
  top_rows += 547187354725515264ULL;
  left_columns += 18786473607168ULL;
  top_diag9s += 31851954ULL;
  top_diag7s += 1297036692682702848ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1C1;
  flipped = true;
}

void UndoFlip_B1C1_E1() {
  top_rows -= 562387003467890688ULL;
  left_columns -= 18786473607168ULL;
  top_diag9s -= 15199648774227378ULL;
  top_diag7s -= 1297214813566402560ULL;
  right_columns -= 1231171548132409344ULL;
  disk_difference -= 6;
}

void Flip_B1C1_E1() {
  top_rows += 562387003467890688ULL;
  left_columns += 18786473607168ULL;
  top_diag9s += 15199648774227378ULL;
  top_diag7s += 1297214813566402560ULL;
  right_columns += 1231171548132409344ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1C1_E1;
  flipped = true;
}

void UndoFlip_B1C1_E1F1() {
  top_rows -= 567453553048682496ULL;
  left_columns -= 18786473607168ULL;
  top_diag9s -= 1312236341456930226ULL;
  top_diag7s -= 1297214813598253056ULL;
  right_columns -= 1231190334319362048ULL;
  disk_difference -= 8;
}

void Flip_B1C1_E1F1() {
  top_rows += 567453553048682496ULL;
  left_columns += 18786473607168ULL;
  top_diag9s += 1312236341456930226ULL;
  top_diag7s += 1297214813598253056ULL;
  right_columns += 1231190334319362048ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1C1_E1F1;
  flipped = true;
}

void UndoFlip_B1C1_E1G1() {
  top_rows -= 569142402908946432ULL;
  left_columns -= 18786473607168ULL;
  top_diag9s -= 1312236341456930226ULL;
  top_diag7s -= 1297214813598254514ULL;
  right_columns -= 1231190334606016512ULL;
  disk_difference -= 10;
}

void Flip_B1C1_E1G1() {
  top_rows += 569142402908946432ULL;
  left_columns += 18786473607168ULL;
  top_diag9s += 1312236341456930226ULL;
  top_diag7s += 1297214813598254514ULL;
  right_columns += 1231190334606016512ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1C1_E1G1;
  flipped = true;
}

void UndoFlip_B1D1() {
  top_rows -= 592786300952641536ULL;
  left_columns -= 18786473611542ULL;
  top_diag9s -= 178120915551666ULL;
  top_diag7s -= 1312236341425078272ULL;
  disk_difference -= 6;
}

void Flip_B1D1() {
  top_rows += 592786300952641536ULL;
  left_columns += 18786473611542ULL;
  top_diag9s += 178120915551666ULL;
  top_diag7s += 1312236341425078272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1D1;
  flipped = true;
}

void UndoFlip_B1D1_F1() {
  top_rows -= 597852850533433344ULL;
  left_columns -= 18786473611542ULL;
  top_diag9s -= 1297214813598254514ULL;
  top_diag7s -= 1312236341456928768ULL;
  right_columns -= 18786186952704ULL;
  disk_difference -= 8;
}

void Flip_B1D1_F1() {
  top_rows += 597852850533433344ULL;
  left_columns += 18786473611542ULL;
  top_diag9s += 1297214813598254514ULL;
  top_diag7s += 1312236341456928768ULL;
  right_columns += 18786186952704ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1D1_F1;
  flipped = true;
}

void UndoFlip_B1D1_F1G1() {
  top_rows -= 599541700393697280ULL;
  left_columns -= 18786473611542ULL;
  top_diag9s -= 1297214813598254514ULL;
  top_diag7s -= 1312236341456930226ULL;
  right_columns -= 18786473607168ULL;
  disk_difference -= 10;
}

void Flip_B1D1_F1G1() {
  top_rows += 599541700393697280ULL;
  left_columns += 18786473611542ULL;
  top_diag9s += 1297214813598254514ULL;
  top_diag7s += 1312236341456930226ULL;
  right_columns += 18786473607168ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1D1_F1G1;
  flipped = true;
}

void UndoFlip_B1E1() {
  top_rows -= 607985949695016960ULL;
  left_columns -= 18786473611542ULL;
  top_diag9s -= 15377769657927090ULL;
  top_diag7s -= 1312414462308777984ULL;
  right_columns -= 1231171548132409344ULL;
  disk_difference -= 8;
}

void Flip_B1E1() {
  top_rows += 607985949695016960ULL;
  left_columns += 18786473611542ULL;
  top_diag9s += 15377769657927090ULL;
  top_diag7s += 1312414462308777984ULL;
  right_columns += 1231171548132409344ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1E1;
  flipped = true;
}

void UndoFlip_B1E1_G1() {
  top_rows -= 609674799555280896ULL;
  left_columns -= 18786473611542ULL;
  top_diag9s -= 15377769657927090ULL;
  top_diag7s -= 1312414462308779442ULL;
  right_columns -= 1231171548419063808ULL;
  disk_difference -= 10;
}

void Flip_B1E1_G1() {
  top_rows += 609674799555280896ULL;
  left_columns += 18786473611542ULL;
  top_diag9s += 15377769657927090ULL;
  top_diag7s += 1312414462308779442ULL;
  right_columns += 1231171548419063808ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1E1_G1;
  flipped = true;
}

void UndoFlip_B1F1() {
  top_rows -= 613052499275808768ULL;
  left_columns -= 18786473611542ULL;
  top_diag9s -= 1312414462340629938ULL;
  top_diag7s -= 1312414462340628480ULL;
  right_columns -= 1231190334319362048ULL;
  disk_difference -= 10;
}

void Flip_B1F1() {
  top_rows += 613052499275808768ULL;
  left_columns += 18786473611542ULL;
  top_diag9s += 1312414462340629938ULL;
  top_diag7s += 1312414462340628480ULL;
  right_columns += 1231190334319362048ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1F1;
  flipped = true;
}

void UndoFlip_B1G1() {
  top_rows -= 614741349136072704ULL;
  left_columns -= 18786473611542ULL;
  top_diag9s -= 1312414462340629938ULL;
  top_diag7s -= 1312414462340629938ULL;
  right_columns -= 1231190334606016512ULL;
  disk_difference -= 12;
}

void Flip_B1G1() {
  top_rows += 614741349136072704ULL;
  left_columns += 18786473611542ULL;
  top_diag9s += 1312414462340629938ULL;
  top_diag7s += 1312414462340629938ULL;
  right_columns += 1231190334606016512ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B1G1;
  flipped = true;
}

void UndoFlip_C1() {
  top_rows -= 136796838681378816ULL;
  left_columns -= 286654464ULL;
  top_diag9s -= 31850496ULL;
  top_diag7s -= 1297036692682702848ULL;
  disk_difference -= 2;
}

void Flip_C1() {
  top_rows += 136796838681378816ULL;
  left_columns += 286654464ULL;
  top_diag9s += 31850496ULL;
  top_diag7s += 1297036692682702848ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1;
  flipped = true;
}

void UndoFlip_C1_E1() {
  top_rows -= 151996487423754240ULL;
  left_columns -= 286654464ULL;
  top_diag9s -= 15199648774225920ULL;
  top_diag7s -= 1297214813566402560ULL;
  right_columns -= 1231171548132409344ULL;
  disk_difference -= 4;
}

void Flip_C1_E1() {
  top_rows += 151996487423754240ULL;
  left_columns += 286654464ULL;
  top_diag9s += 15199648774225920ULL;
  top_diag7s += 1297214813566402560ULL;
  right_columns += 1231171548132409344ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1_E1;
  flipped = true;
}

void UndoFlip_C1_E1F1() {
  top_rows -= 157063037004546048ULL;
  left_columns -= 286654464ULL;
  top_diag9s -= 1312236341456928768ULL;
  top_diag7s -= 1297214813598253056ULL;
  right_columns -= 1231190334319362048ULL;
  disk_difference -= 6;
}

void Flip_C1_E1F1() {
  top_rows += 157063037004546048ULL;
  left_columns += 286654464ULL;
  top_diag9s += 1312236341456928768ULL;
  top_diag7s += 1297214813598253056ULL;
  right_columns += 1231190334319362048ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1_E1F1;
  flipped = true;
}

void UndoFlip_C1_E1G1() {
  top_rows -= 158751886864809984ULL;
  left_columns -= 286654464ULL;
  top_diag9s -= 1312236341456928768ULL;
  top_diag7s -= 1297214813598254514ULL;
  right_columns -= 1231190334606016512ULL;
  disk_difference -= 8;
}

void Flip_C1_E1G1() {
  top_rows += 158751886864809984ULL;
  left_columns += 286654464ULL;
  top_diag9s += 1312236341456928768ULL;
  top_diag7s += 1297214813598254514ULL;
  right_columns += 1231190334606016512ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1_E1G1;
  flipped = true;
}

void UndoFlip_C1D1() {
  top_rows -= 182395784908505088ULL;
  left_columns -= 286658838ULL;
  top_diag9s -= 178120915550208ULL;
  top_diag7s -= 1312236341425078272ULL;
  disk_difference -= 4;
}

void Flip_C1D1() {
  top_rows += 182395784908505088ULL;
  left_columns += 286658838ULL;
  top_diag9s += 178120915550208ULL;
  top_diag7s += 1312236341425078272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1D1;
  flipped = true;
}

void UndoFlip_C1D1_F1() {
  top_rows -= 187462334489296896ULL;
  left_columns -= 286658838ULL;
  top_diag9s -= 1297214813598253056ULL;
  top_diag7s -= 1312236341456928768ULL;
  right_columns -= 18786186952704ULL;
  disk_difference -= 6;
}

void Flip_C1D1_F1() {
  top_rows += 187462334489296896ULL;
  left_columns += 286658838ULL;
  top_diag9s += 1297214813598253056ULL;
  top_diag7s += 1312236341456928768ULL;
  right_columns += 18786186952704ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1D1_F1;
  flipped = true;
}

void UndoFlip_C1D1_F1G1() {
  top_rows -= 189151184349560832ULL;
  left_columns -= 286658838ULL;
  top_diag9s -= 1297214813598253056ULL;
  top_diag7s -= 1312236341456930226ULL;
  right_columns -= 18786473607168ULL;
  disk_difference -= 8;
}

void Flip_C1D1_F1G1() {
  top_rows += 189151184349560832ULL;
  left_columns += 286658838ULL;
  top_diag9s += 1297214813598253056ULL;
  top_diag7s += 1312236341456930226ULL;
  right_columns += 18786473607168ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1D1_F1G1;
  flipped = true;
}

void UndoFlip_C1E1() {
  top_rows -= 197595433650880512ULL;
  left_columns -= 286658838ULL;
  top_diag9s -= 15377769657925632ULL;
  top_diag7s -= 1312414462308777984ULL;
  right_columns -= 1231171548132409344ULL;
  disk_difference -= 6;
}

void Flip_C1E1() {
  top_rows += 197595433650880512ULL;
  left_columns += 286658838ULL;
  top_diag9s += 15377769657925632ULL;
  top_diag7s += 1312414462308777984ULL;
  right_columns += 1231171548132409344ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1E1;
  flipped = true;
}

void UndoFlip_C1E1_G1() {
  top_rows -= 199284283511144448ULL;
  left_columns -= 286658838ULL;
  top_diag9s -= 15377769657925632ULL;
  top_diag7s -= 1312414462308779442ULL;
  right_columns -= 1231171548419063808ULL;
  disk_difference -= 8;
}

void Flip_C1E1_G1() {
  top_rows += 199284283511144448ULL;
  left_columns += 286658838ULL;
  top_diag9s += 15377769657925632ULL;
  top_diag7s += 1312414462308779442ULL;
  right_columns += 1231171548419063808ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1E1_G1;
  flipped = true;
}

void UndoFlip_C1F1() {
  top_rows -= 202661983231672320ULL;
  left_columns -= 286658838ULL;
  top_diag9s -= 1312414462340628480ULL;
  top_diag7s -= 1312414462340628480ULL;
  right_columns -= 1231190334319362048ULL;
  disk_difference -= 8;
}

void Flip_C1F1() {
  top_rows += 202661983231672320ULL;
  left_columns += 286658838ULL;
  top_diag9s += 1312414462340628480ULL;
  top_diag7s += 1312414462340628480ULL;
  right_columns += 1231190334319362048ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1F1;
  flipped = true;
}

void UndoFlip_C1G1() {
  top_rows -= 204350833091936256ULL;
  left_columns -= 286658838ULL;
  top_diag9s -= 1312414462340628480ULL;
  top_diag7s -= 1312414462340629938ULL;
  right_columns -= 1231190334606016512ULL;
  disk_difference -= 10;
}

void Flip_C1G1() {
  top_rows += 204350833091936256ULL;
  left_columns += 286658838ULL;
  top_diag9s += 1312414462340628480ULL;
  top_diag7s += 1312414462340629938ULL;
  right_columns += 1231190334606016512ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C1G1;
  flipped = true;
}

void UndoFlip_D1() {
  top_rows -= 45598946227126272ULL;
  left_columns -= 4374ULL;
  top_diag9s -= 178120883699712ULL;
  top_diag7s -= 15199648742375424ULL;
  disk_difference -= 2;
}

void Flip_D1() {
  top_rows += 45598946227126272ULL;
  left_columns += 4374ULL;
  top_diag9s += 178120883699712ULL;
  top_diag7s += 15199648742375424ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1;
  flipped = true;
}

void UndoFlip_D1_F1() {
  top_rows -= 50665495807918080ULL;
  left_columns -= 4374ULL;
  top_diag9s -= 1297214813566402560ULL;
  top_diag7s -= 15199648774225920ULL;
  right_columns -= 18786186952704ULL;
  disk_difference -= 4;
}

void Flip_D1_F1() {
  top_rows += 50665495807918080ULL;
  left_columns += 4374ULL;
  top_diag9s += 1297214813566402560ULL;
  top_diag7s += 15199648774225920ULL;
  right_columns += 18786186952704ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1_F1;
  flipped = true;
}

void UndoFlip_D1_F1G1() {
  top_rows -= 52354345668182016ULL;
  left_columns -= 4374ULL;
  top_diag9s -= 1297214813566402560ULL;
  top_diag7s -= 15199648774227378ULL;
  right_columns -= 18786473607168ULL;
  disk_difference -= 6;
}

void Flip_D1_F1G1() {
  top_rows += 52354345668182016ULL;
  left_columns += 4374ULL;
  top_diag9s += 1297214813566402560ULL;
  top_diag7s += 15199648774227378ULL;
  right_columns += 18786473607168ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1_F1G1;
  flipped = true;
}

void UndoFlip_D1E1() {
  top_rows -= 60798594969501696ULL;
  left_columns -= 4374ULL;
  top_diag9s -= 15377769626075136ULL;
  top_diag7s -= 15377769626075136ULL;
  right_columns -= 1231171548132409344ULL;
  disk_difference -= 4;
}

void Flip_D1E1() {
  top_rows += 60798594969501696ULL;
  left_columns += 4374ULL;
  top_diag9s += 15377769626075136ULL;
  top_diag7s += 15377769626075136ULL;
  right_columns += 1231171548132409344ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1E1;
  flipped = true;
}

void UndoFlip_D1E1_G1() {
  top_rows -= 62487444829765632ULL;
  left_columns -= 4374ULL;
  top_diag9s -= 15377769626075136ULL;
  top_diag7s -= 15377769626076594ULL;
  right_columns -= 1231171548419063808ULL;
  disk_difference -= 6;
}

void Flip_D1E1_G1() {
  top_rows += 62487444829765632ULL;
  left_columns += 4374ULL;
  top_diag9s += 15377769626075136ULL;
  top_diag7s += 15377769626076594ULL;
  right_columns += 1231171548419063808ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1E1_G1;
  flipped = true;
}

void UndoFlip_D1F1() {
  top_rows -= 65865144550293504ULL;
  left_columns -= 4374ULL;
  top_diag9s -= 1312414462308777984ULL;
  top_diag7s -= 15377769657925632ULL;
  right_columns -= 1231190334319362048ULL;
  disk_difference -= 6;
}

void Flip_D1F1() {
  top_rows += 65865144550293504ULL;
  left_columns += 4374ULL;
  top_diag9s += 1312414462308777984ULL;
  top_diag7s += 15377769657925632ULL;
  right_columns += 1231190334319362048ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1F1;
  flipped = true;
}

void UndoFlip_D1G1() {
  top_rows -= 67553994410557440ULL;
  left_columns -= 4374ULL;
  top_diag9s -= 1312414462308777984ULL;
  top_diag7s -= 15377769657927090ULL;
  right_columns -= 1231190334606016512ULL;
  disk_difference -= 8;
}

void Flip_D1G1() {
  top_rows += 67553994410557440ULL;
  left_columns += 4374ULL;
  top_diag9s += 1312414462308777984ULL;
  top_diag7s += 15377769657927090ULL;
  right_columns += 1231190334606016512ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D1G1;
  flipped = true;
}

void UndoFlip_E1() {
  top_rows -= 15199648742375424ULL;
  top_diag9s -= 15199648742375424ULL;
  top_diag7s -= 178120883699712ULL;
  right_columns -= 1231171548132409344ULL;
  disk_difference -= 2;
}

void Flip_E1() {
  top_rows += 15199648742375424ULL;
  top_diag9s += 15199648742375424ULL;
  top_diag7s += 178120883699712ULL;
  right_columns += 1231171548132409344ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E1;
  flipped = true;
}

void UndoFlip_E1_G1() {
  top_rows -= 16888498602639360ULL;
  top_diag9s -= 15199648742375424ULL;
  top_diag7s -= 178120883701170ULL;
  right_columns -= 1231171548419063808ULL;
  disk_difference -= 4;
}

void Flip_E1_G1() {
  top_rows += 16888498602639360ULL;
  top_diag9s += 15199648742375424ULL;
  top_diag7s += 178120883701170ULL;
  right_columns += 1231171548419063808ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E1_G1;
  flipped = true;
}

void UndoFlip_E1F1() {
  top_rows -= 20266198323167232ULL;
  top_diag9s -= 1312236341425078272ULL;
  top_diag7s -= 178120915550208ULL;
  right_columns -= 1231190334319362048ULL;
  disk_difference -= 4;
}

void Flip_E1F1() {
  top_rows += 20266198323167232ULL;
  top_diag9s += 1312236341425078272ULL;
  top_diag7s += 178120915550208ULL;
  right_columns += 1231190334319362048ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E1F1;
  flipped = true;
}

void UndoFlip_E1G1() {
  top_rows -= 21955048183431168ULL;
  top_diag9s -= 1312236341425078272ULL;
  top_diag7s -= 178120915551666ULL;
  right_columns -= 1231190334606016512ULL;
  disk_difference -= 6;
}

void Flip_E1G1() {
  top_rows += 21955048183431168ULL;
  top_diag9s += 1312236341425078272ULL;
  top_diag7s += 178120915551666ULL;
  right_columns += 1231190334606016512ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E1G1;
  flipped = true;
}

void UndoFlip_F1() {
  top_rows -= 5066549580791808ULL;
  top_diag9s -= 1297036692682702848ULL;
  top_diag7s -= 31850496ULL;
  right_columns -= 18786186952704ULL;
  disk_difference -= 2;
}

void Flip_F1() {
  top_rows += 5066549580791808ULL;
  top_diag9s += 1297036692682702848ULL;
  top_diag7s += 31850496ULL;
  right_columns += 18786186952704ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F1;
  flipped = true;
}

void UndoFlip_F1G1() {
  top_rows -= 6755399441055744ULL;
  top_diag9s -= 1297036692682702848ULL;
  top_diag7s -= 31851954ULL;
  right_columns -= 18786473607168ULL;
  disk_difference -= 4;
}

void Flip_F1G1() {
  top_rows += 6755399441055744ULL;
  top_diag9s += 1297036692682702848ULL;
  top_diag7s += 31851954ULL;
  right_columns += 18786473607168ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F1G1;
  flipped = true;
}

void UndoFlip_G1() {
  top_rows -= 1688849860263936ULL;
  top_diag7s -= 1458ULL;
  right_columns -= 286654464ULL;
  disk_difference -= 2;
}

void Flip_G1() {
  top_rows += 1688849860263936ULL;
  top_diag7s += 1458ULL;
  right_columns += 286654464ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G1;
  flipped = true;
}

void UndoFlip_B2() {
  top_rows -= 6262062317568ULL;
  left_columns -= 6262062317568ULL;
  top_diag7s -= 432345564227567616ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 2;
}

void Flip_B2() {
  top_rows += 6262062317568ULL;
  left_columns += 6262062317568ULL;
  top_diag7s += 432345564227567616ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2;
  flipped = true;
}

void UndoFlip_B2_D2() {
  top_rows -= 6957847019520ULL;
  left_columns -= 6262062319026ULL;
  top_diag9s -= 10616832ULL;
  top_diag7s -= 432404937855467520ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_B2_D2() {
  top_rows += 6957847019520ULL;
  left_columns += 6262062319026ULL;
  top_diag9s += 10616832ULL;
  top_diag7s += 432404937855467520ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D2;
  flipped = true;
}

void UndoFlip_B2_D2E2() {
  top_rows -= 7189775253504ULL;
  left_columns -= 6262062319026ULL;
  top_diag9s -= 59373638516736ULL;
  top_diag7s -= 432404937866084352ULL;
  right_columns -= 410390516044136448ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_B2_D2E2() {
  top_rows += 7189775253504ULL;
  left_columns += 6262062319026ULL;
  top_diag9s += 59373638516736ULL;
  top_diag7s += 432404937866084352ULL;
  right_columns += 410390516044136448ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D2E2;
  flipped = true;
}

void UndoFlip_B2_D2F2() {
  top_rows -= 7267084664832ULL;
  left_columns -= 6262062319026ULL;
  top_diag9s -= 5125923219308544ULL;
  top_diag7s -= 432404937866084838ULL;
  right_columns -= 410396778106454016ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 8;
}

void Flip_B2_D2F2() {
  top_rows += 7267084664832ULL;
  left_columns += 6262062319026ULL;
  top_diag9s += 5125923219308544ULL;
  top_diag7s += 432404937866084838ULL;
  right_columns += 410396778106454016ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D2F2;
  flipped = true;
}

void UndoFlip_B2_D2G2() {
  top_rows -= 7292854468608ULL;
  left_columns -= 6262062319026ULL;
  top_diag9s -= 437471487446876160ULL;
  top_diag7s -= 432404937866084838ULL;
  right_columns -= 410396778202005504ULL;
  bottom_diag9s -= 410390516044136448ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 10;
}

void Flip_B2_D2G2() {
  top_rows += 7292854468608ULL;
  left_columns += 6262062319026ULL;
  top_diag9s += 437471487446876160ULL;
  top_diag7s += 432404937866084838ULL;
  right_columns += 410396778202005504ULL;
  bottom_diag9s += 410390516044136448ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D2G2;
  flipped = true;
}

void UndoFlip_B2C2() {
  top_rows -= 8349416423424ULL;
  left_columns -= 6262157869056ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 437412113808359424ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_B2C2() {
  top_rows += 8349416423424ULL;
  left_columns += 6262157869056ULL;
  top_diag9s += 486ULL;
  top_diag7s += 437412113808359424ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C2;
  flipped = true;
}

void UndoFlip_B2C2_E2() {
  top_rows -= 8581344657408ULL;
  left_columns -= 6262157869056ULL;
  top_diag9s -= 59373627900390ULL;
  top_diag7s -= 437412113818976256ULL;
  right_columns -= 410390516044136448ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_B2C2_E2() {
  top_rows += 8581344657408ULL;
  left_columns += 6262157869056ULL;
  top_diag9s += 59373627900390ULL;
  top_diag7s += 437412113818976256ULL;
  right_columns += 410390516044136448ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C2_E2;
  flipped = true;
}

void UndoFlip_B2C2_E2F2() {
  top_rows -= 8658654068736ULL;
  left_columns -= 6262157869056ULL;
  top_diag9s -= 5125923208692198ULL;
  top_diag7s -= 437412113818976742ULL;
  right_columns -= 410396778106454016ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 8;
}

void Flip_B2C2_E2F2() {
  top_rows += 8658654068736ULL;
  left_columns += 6262157869056ULL;
  top_diag9s += 5125923208692198ULL;
  top_diag7s += 437412113818976742ULL;
  right_columns += 410396778106454016ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C2_E2F2;
  flipped = true;
}

void UndoFlip_B2C2_E2G2() {
  top_rows -= 8684423872512ULL;
  left_columns -= 6262157869056ULL;
  top_diag9s -= 437471487436259814ULL;
  top_diag7s -= 437412113818976742ULL;
  right_columns -= 410396778202005504ULL;
  bottom_diag9s -= 410390516044136448ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 10;
}

void Flip_B2C2_E2G2() {
  top_rows += 8684423872512ULL;
  left_columns += 6262157869056ULL;
  top_diag9s += 437471487436259814ULL;
  top_diag7s += 437412113818976742ULL;
  right_columns += 410396778202005504ULL;
  bottom_diag9s += 410390516044136448ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C2_E2G2;
  flipped = true;
}

void UndoFlip_B2D2() {
  top_rows -= 9045201125376ULL;
  left_columns -= 6262157870514ULL;
  top_diag9s -= 10617318ULL;
  top_diag7s -= 437471487436259328ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_B2D2() {
  top_rows += 9045201125376ULL;
  left_columns += 6262157870514ULL;
  top_diag9s += 10617318ULL;
  top_diag7s += 437471487436259328ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D2;
  flipped = true;
}

void UndoFlip_B2D2_F2() {
  top_rows -= 9122510536704ULL;
  left_columns -= 6262157870514ULL;
  top_diag9s -= 5066549591409126ULL;
  top_diag7s -= 437471487436259814ULL;
  right_columns -= 6262062317568ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 8;
}

void Flip_B2D2_F2() {
  top_rows += 9122510536704ULL;
  left_columns += 6262157870514ULL;
  top_diag9s += 5066549591409126ULL;
  top_diag7s += 437471487436259814ULL;
  right_columns += 6262062317568ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D2_F2;
  flipped = true;
}

void UndoFlip_B2D2_F2G2() {
  top_rows -= 9148280340480ULL;
  left_columns -= 6262157870514ULL;
  top_diag9s -= 437412113818976742ULL;
  top_diag7s -= 437471487436259814ULL;
  right_columns -= 6262157869056ULL;
  bottom_diag9s -= 410390516044136448ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 10;
}

void Flip_B2D2_F2G2() {
  top_rows += 9148280340480ULL;
  left_columns += 6262157870514ULL;
  top_diag9s += 437412113818976742ULL;
  top_diag7s += 437471487436259814ULL;
  right_columns += 6262157869056ULL;
  bottom_diag9s += 410390516044136448ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D2_F2G2;
  flipped = true;
}

void UndoFlip_B2E2() {
  top_rows -= 9277129359360ULL;
  left_columns -= 6262157870514ULL;
  top_diag9s -= 59373638517222ULL;
  top_diag7s -= 437471487446876160ULL;
  right_columns -= 410390516044136448ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 8;
}

void Flip_B2E2() {
  top_rows += 9277129359360ULL;
  left_columns += 6262157870514ULL;
  top_diag9s += 59373638517222ULL;
  top_diag7s += 437471487446876160ULL;
  right_columns += 410390516044136448ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2E2;
  flipped = true;
}

void UndoFlip_B2E2_G2() {
  top_rows -= 9302899163136ULL;
  left_columns -= 6262157870514ULL;
  top_diag9s -= 432404937866084838ULL;
  top_diag7s -= 437471487446876160ULL;
  right_columns -= 410390516139687936ULL;
  bottom_diag9s -= 410390516044136448ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 10;
}

void Flip_B2E2_G2() {
  top_rows += 9302899163136ULL;
  left_columns += 6262157870514ULL;
  top_diag9s += 432404937866084838ULL;
  top_diag7s += 437471487446876160ULL;
  right_columns += 410390516139687936ULL;
  bottom_diag9s += 410390516044136448ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2E2_G2;
  flipped = true;
}

void UndoFlip_B2F2() {
  top_rows -= 9354438770688ULL;
  left_columns -= 6262157870514ULL;
  top_diag9s -= 5125923219309030ULL;
  top_diag7s -= 437471487446876646ULL;
  right_columns -= 410396778106454016ULL;
  bottom_diag9s -= 410390516044136448ULL;
  disk_difference -= 10;
}

void Flip_B2F2() {
  top_rows += 9354438770688ULL;
  left_columns += 6262157870514ULL;
  top_diag9s += 5125923219309030ULL;
  top_diag7s += 437471487446876646ULL;
  right_columns += 410396778106454016ULL;
  bottom_diag9s += 410390516044136448ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2F2;
  flipped = true;
}

void UndoFlip_B2G2() {
  top_rows -= 9380208574464ULL;
  left_columns -= 6262157870514ULL;
  top_diag9s -= 437471487446876646ULL;
  top_diag7s -= 437471487446876646ULL;
  right_columns -= 410396778202005504ULL;
  bottom_diag9s -= 410390516044136448ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 12;
}

void Flip_B2G2() {
  top_rows += 9380208574464ULL;
  left_columns += 6262157870514ULL;
  top_diag9s += 437471487446876646ULL;
  top_diag7s += 437471487446876646ULL;
  right_columns += 410396778202005504ULL;
  bottom_diag9s += 410390516044136448ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2G2;
  flipped = true;
}

void UndoFlip_C2() {
  top_rows -= 2087354105856ULL;
  left_columns -= 95551488ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5066549580791808ULL;
  disk_difference -= 2;
}

void Flip_C2() {
  top_rows += 2087354105856ULL;
  left_columns += 95551488ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5066549580791808ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2;
  flipped = true;
}

void UndoFlip_C2_E2() {
  top_rows -= 2319282339840ULL;
  left_columns -= 95551488ULL;
  top_diag9s -= 59373627900390ULL;
  top_diag7s -= 5066549591408640ULL;
  right_columns -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_C2_E2() {
  top_rows += 2319282339840ULL;
  left_columns += 95551488ULL;
  top_diag9s += 59373627900390ULL;
  top_diag7s += 5066549591408640ULL;
  right_columns += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E2;
  flipped = true;
}

void UndoFlip_C2_E2F2() {
  top_rows -= 2396591751168ULL;
  left_columns -= 95551488ULL;
  top_diag9s -= 5125923208692198ULL;
  top_diag7s -= 5066549591409126ULL;
  right_columns -= 410396778106454016ULL;
  disk_difference -= 6;
}

void Flip_C2_E2F2() {
  top_rows += 2396591751168ULL;
  left_columns += 95551488ULL;
  top_diag9s += 5125923208692198ULL;
  top_diag7s += 5066549591409126ULL;
  right_columns += 410396778106454016ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E2F2;
  flipped = true;
}

void UndoFlip_C2_E2G2() {
  top_rows -= 2422361554944ULL;
  left_columns -= 95551488ULL;
  top_diag9s -= 437471487436259814ULL;
  top_diag7s -= 5066549591409126ULL;
  right_columns -= 410396778202005504ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 8;
}

void Flip_C2_E2G2() {
  top_rows += 2422361554944ULL;
  left_columns += 95551488ULL;
  top_diag9s += 437471487436259814ULL;
  top_diag7s += 5066549591409126ULL;
  right_columns += 410396778202005504ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E2G2;
  flipped = true;
}

void UndoFlip_C2D2() {
  top_rows -= 2783138807808ULL;
  left_columns -= 95552946ULL;
  top_diag9s -= 10617318ULL;
  top_diag7s -= 5125923208691712ULL;
  disk_difference -= 4;
}

void Flip_C2D2() {
  top_rows += 2783138807808ULL;
  left_columns += 95552946ULL;
  top_diag9s += 10617318ULL;
  top_diag7s += 5125923208691712ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D2;
  flipped = true;
}

void UndoFlip_C2D2_F2() {
  top_rows -= 2860448219136ULL;
  left_columns -= 95552946ULL;
  top_diag9s -= 5066549591409126ULL;
  top_diag7s -= 5125923208692198ULL;
  right_columns -= 6262062317568ULL;
  disk_difference -= 6;
}

void Flip_C2D2_F2() {
  top_rows += 2860448219136ULL;
  left_columns += 95552946ULL;
  top_diag9s += 5066549591409126ULL;
  top_diag7s += 5125923208692198ULL;
  right_columns += 6262062317568ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D2_F2;
  flipped = true;
}

void UndoFlip_C2D2_F2G2() {
  top_rows -= 2886218022912ULL;
  left_columns -= 95552946ULL;
  top_diag9s -= 437412113818976742ULL;
  top_diag7s -= 5125923208692198ULL;
  right_columns -= 6262157869056ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 8;
}

void Flip_C2D2_F2G2() {
  top_rows += 2886218022912ULL;
  left_columns += 95552946ULL;
  top_diag9s += 437412113818976742ULL;
  top_diag7s += 5125923208692198ULL;
  right_columns += 6262157869056ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D2_F2G2;
  flipped = true;
}

void UndoFlip_C2E2() {
  top_rows -= 3015067041792ULL;
  left_columns -= 95552946ULL;
  top_diag9s -= 59373638517222ULL;
  top_diag7s -= 5125923219308544ULL;
  right_columns -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_C2E2() {
  top_rows += 3015067041792ULL;
  left_columns += 95552946ULL;
  top_diag9s += 59373638517222ULL;
  top_diag7s += 5125923219308544ULL;
  right_columns += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2E2;
  flipped = true;
}

void UndoFlip_C2E2_G2() {
  top_rows -= 3040836845568ULL;
  left_columns -= 95552946ULL;
  top_diag9s -= 432404937866084838ULL;
  top_diag7s -= 5125923219308544ULL;
  right_columns -= 410390516139687936ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 8;
}

void Flip_C2E2_G2() {
  top_rows += 3040836845568ULL;
  left_columns += 95552946ULL;
  top_diag9s += 432404937866084838ULL;
  top_diag7s += 5125923219308544ULL;
  right_columns += 410390516139687936ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2E2_G2;
  flipped = true;
}

void UndoFlip_C2F2() {
  top_rows -= 3092376453120ULL;
  left_columns -= 95552946ULL;
  top_diag9s -= 5125923219309030ULL;
  top_diag7s -= 5125923219309030ULL;
  right_columns -= 410396778106454016ULL;
  disk_difference -= 8;
}

void Flip_C2F2() {
  top_rows += 3092376453120ULL;
  left_columns += 95552946ULL;
  top_diag9s += 5125923219309030ULL;
  top_diag7s += 5125923219309030ULL;
  right_columns += 410396778106454016ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2F2;
  flipped = true;
}

void UndoFlip_C2G2() {
  top_rows -= 3118146256896ULL;
  left_columns -= 95552946ULL;
  top_diag9s -= 437471487446876646ULL;
  top_diag7s -= 5125923219309030ULL;
  right_columns -= 410396778202005504ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 10;
}

void Flip_C2G2() {
  top_rows += 3118146256896ULL;
  left_columns += 95552946ULL;
  top_diag9s += 437471487446876646ULL;
  top_diag7s += 5125923219309030ULL;
  right_columns += 410396778202005504ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2G2;
  flipped = true;
}

void UndoFlip_D2() {
  top_rows -= 695784701952ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 10616832ULL;
  top_diag7s -= 59373627899904ULL;
  disk_difference -= 2;
}

void Flip_D2() {
  top_rows += 695784701952ULL;
  left_columns += 1458ULL;
  top_diag9s += 10616832ULL;
  top_diag7s += 59373627899904ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2;
  flipped = true;
}

void UndoFlip_D2_F2() {
  top_rows -= 773094113280ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 5066549591408640ULL;
  top_diag7s -= 59373627900390ULL;
  right_columns -= 6262062317568ULL;
  disk_difference -= 4;
}

void Flip_D2_F2() {
  top_rows += 773094113280ULL;
  left_columns += 1458ULL;
  top_diag9s += 5066549591408640ULL;
  top_diag7s += 59373627900390ULL;
  right_columns += 6262062317568ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_F2;
  flipped = true;
}

void UndoFlip_D2_F2G2() {
  top_rows -= 798863917056ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 437412113818976256ULL;
  top_diag7s -= 59373627900390ULL;
  right_columns -= 6262157869056ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_D2_F2G2() {
  top_rows += 798863917056ULL;
  left_columns += 1458ULL;
  top_diag9s += 437412113818976256ULL;
  top_diag7s += 59373627900390ULL;
  right_columns += 6262157869056ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_F2G2;
  flipped = true;
}

void UndoFlip_D2E2() {
  top_rows -= 927712935936ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 59373638516736ULL;
  top_diag7s -= 59373638516736ULL;
  right_columns -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_D2E2() {
  top_rows += 927712935936ULL;
  left_columns += 1458ULL;
  top_diag9s += 59373638516736ULL;
  top_diag7s += 59373638516736ULL;
  right_columns += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2E2;
  flipped = true;
}

void UndoFlip_D2E2_G2() {
  top_rows -= 953482739712ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 432404937866084352ULL;
  top_diag7s -= 59373638516736ULL;
  right_columns -= 410390516139687936ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_D2E2_G2() {
  top_rows += 953482739712ULL;
  left_columns += 1458ULL;
  top_diag9s += 432404937866084352ULL;
  top_diag7s += 59373638516736ULL;
  right_columns += 410390516139687936ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2E2_G2;
  flipped = true;
}

void UndoFlip_D2F2() {
  top_rows -= 1005022347264ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 5125923219308544ULL;
  top_diag7s -= 59373638517222ULL;
  right_columns -= 410396778106454016ULL;
  disk_difference -= 6;
}

void Flip_D2F2() {
  top_rows += 1005022347264ULL;
  left_columns += 1458ULL;
  top_diag9s += 5125923219308544ULL;
  top_diag7s += 59373638517222ULL;
  right_columns += 410396778106454016ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2F2;
  flipped = true;
}

void UndoFlip_D2G2() {
  top_rows -= 1030792151040ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 437471487446876160ULL;
  top_diag7s -= 59373638517222ULL;
  right_columns -= 410396778202005504ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 8;
}

void Flip_D2G2() {
  top_rows += 1030792151040ULL;
  left_columns += 1458ULL;
  top_diag9s += 437471487446876160ULL;
  top_diag7s += 59373638517222ULL;
  right_columns += 410396778202005504ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2G2;
  flipped = true;
}

void UndoFlip_E2() {
  top_rows -= 231928233984ULL;
  top_diag9s -= 59373627899904ULL;
  top_diag7s -= 10616832ULL;
  right_columns -= 410390516044136448ULL;
  disk_difference -= 2;
}

void Flip_E2() {
  top_rows += 231928233984ULL;
  top_diag9s += 59373627899904ULL;
  top_diag7s += 10616832ULL;
  right_columns += 410390516044136448ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2;
  flipped = true;
}

void UndoFlip_E2_G2() {
  top_rows -= 257698037760ULL;
  top_diag9s -= 432404937855467520ULL;
  top_diag7s -= 10616832ULL;
  right_columns -= 410390516139687936ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_E2_G2() {
  top_rows += 257698037760ULL;
  top_diag9s += 432404937855467520ULL;
  top_diag7s += 10616832ULL;
  right_columns += 410390516139687936ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_G2;
  flipped = true;
}

void UndoFlip_E2F2() {
  top_rows -= 309237645312ULL;
  top_diag9s -= 5125923208691712ULL;
  top_diag7s -= 10617318ULL;
  right_columns -= 410396778106454016ULL;
  disk_difference -= 4;
}

void Flip_E2F2() {
  top_rows += 309237645312ULL;
  top_diag9s += 5125923208691712ULL;
  top_diag7s += 10617318ULL;
  right_columns += 410396778106454016ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2F2;
  flipped = true;
}

void UndoFlip_E2G2() {
  top_rows -= 335007449088ULL;
  top_diag9s -= 437471487436259328ULL;
  top_diag7s -= 10617318ULL;
  right_columns -= 410396778202005504ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_E2G2() {
  top_rows += 335007449088ULL;
  top_diag9s += 437471487436259328ULL;
  top_diag7s += 10617318ULL;
  right_columns += 410396778202005504ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2G2;
  flipped = true;
}

void UndoFlip_F2() {
  top_rows -= 77309411328ULL;
  top_diag9s -= 5066549580791808ULL;
  top_diag7s -= 486ULL;
  right_columns -= 6262062317568ULL;
  disk_difference -= 2;
}

void Flip_F2() {
  top_rows += 77309411328ULL;
  top_diag9s += 5066549580791808ULL;
  top_diag7s += 486ULL;
  right_columns += 6262062317568ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2;
  flipped = true;
}

void UndoFlip_F2G2() {
  top_rows -= 103079215104ULL;
  top_diag9s -= 437412113808359424ULL;
  top_diag7s -= 486ULL;
  right_columns -= 6262157869056ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_F2G2() {
  top_rows += 103079215104ULL;
  top_diag9s += 437412113808359424ULL;
  top_diag7s += 486ULL;
  right_columns += 6262157869056ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2G2;
  flipped = true;
}

void UndoFlip_G2() {
  top_rows -= 25769803776ULL;
  top_diag9s -= 432345564227567616ULL;
  right_columns -= 95551488ULL;
  bottom_diag7s -= 410390516044136448ULL;
  disk_difference -= 2;
}

void Flip_G2() {
  top_rows += 25769803776ULL;
  top_diag9s += 432345564227567616ULL;
  right_columns += 95551488ULL;
  bottom_diag7s += 410390516044136448ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2;
  flipped = true;
}

void UndoFlip_B3() {
  top_rows -= 95551488ULL;
  left_columns -= 2087354105856ULL;
  top_diag7s -= 1688849860263936ULL;
  bottom_diag9s -= 2087354105856ULL;
  disk_difference -= 2;
}

void Flip_B3() {
  top_rows += 95551488ULL;
  left_columns += 2087354105856ULL;
  top_diag7s += 1688849860263936ULL;
  bottom_diag9s += 2087354105856ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3;
  flipped = true;
}

void UndoFlip_B3_D3() {
  top_rows -= 106168320ULL;
  left_columns -= 2087354106342ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 1688849863802880ULL;
  bottom_diag9s -= 2087354105856ULL;
  disk_difference -= 4;
}

void Flip_B3_D3() {
  top_rows += 106168320ULL;
  left_columns += 2087354106342ULL;
  top_diag9s += 162ULL;
  top_diag7s += 1688849863802880ULL;
  bottom_diag9s += 2087354105856ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D3;
  flipped = true;
}

void UndoFlip_B3_D3E3() {
  top_rows -= 109707264ULL;
  left_columns -= 2087354106342ULL;
  top_diag9s -= 3539106ULL;
  top_diag7s -= 1688849863803042ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 2087354105856ULL;
  disk_difference -= 6;
}

void Flip_B3_D3E3() {
  top_rows += 109707264ULL;
  left_columns += 2087354106342ULL;
  top_diag9s += 3539106ULL;
  top_diag7s += 1688849863803042ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 2087354105856ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D3E3;
  flipped = true;
}

void UndoFlip_B3_D3F3() {
  top_rows -= 110886912ULL;
  left_columns -= 2087354106342ULL;
  top_diag9s -= 19791212839074ULL;
  top_diag7s -= 1688849863803042ULL;
  right_columns -= 136798926035484672ULL;
  bottom_diag9s -= 2087354105856ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 8;
}

void Flip_B3_D3F3() {
  top_rows += 110886912ULL;
  left_columns += 2087354106342ULL;
  top_diag9s += 19791212839074ULL;
  top_diag7s += 1688849863803042ULL;
  right_columns += 136798926035484672ULL;
  bottom_diag9s += 2087354105856ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D3F3;
  flipped = true;
}

void UndoFlip_B3_D3G3() {
  top_rows -= 111280128ULL;
  left_columns -= 2087354106342ULL;
  top_diag9s -= 1708641073103010ULL;
  top_diag7s -= 1688849863803042ULL;
  right_columns -= 136798926067335168ULL;
  bottom_diag9s -= 2087354105856ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 10;
}

void Flip_B3_D3G3() {
  top_rows += 111280128ULL;
  left_columns += 2087354106342ULL;
  top_diag9s += 1708641073103010ULL;
  top_diag7s += 1688849863803042ULL;
  right_columns += 136798926067335168ULL;
  bottom_diag9s += 2087354105856ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D3G3;
  flipped = true;
}

void UndoFlip_B3C3() {
  top_rows -= 127401984ULL;
  left_columns -= 2087385956352ULL;
  top_diag7s -= 1708641069563904ULL;
  bottom_diag9s -= 136798926035484672ULL;
  disk_difference -= 4;
}

void Flip_B3C3() {
  top_rows += 127401984ULL;
  left_columns += 2087385956352ULL;
  top_diag7s += 1708641069563904ULL;
  bottom_diag9s += 136798926035484672ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C3;
  flipped = true;
}

void UndoFlip_B3C3_E3() {
  top_rows -= 130940928ULL;
  left_columns -= 2087385956352ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 1708641069564066ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 136798926035484672ULL;
  disk_difference -= 6;
}

void Flip_B3C3_E3() {
  top_rows += 130940928ULL;
  left_columns += 2087385956352ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 1708641069564066ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 136798926035484672ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C3_E3;
  flipped = true;
}

void UndoFlip_B3C3_E3F3() {
  top_rows -= 132120576ULL;
  left_columns -= 2087385956352ULL;
  top_diag9s -= 19791212838912ULL;
  top_diag7s -= 1708641069564066ULL;
  right_columns -= 136798926035484672ULL;
  bottom_diag9s -= 136798926035484672ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 8;
}

void Flip_B3C3_E3F3() {
  top_rows += 132120576ULL;
  left_columns += 2087385956352ULL;
  top_diag9s += 19791212838912ULL;
  top_diag7s += 1708641069564066ULL;
  right_columns += 136798926035484672ULL;
  bottom_diag9s += 136798926035484672ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C3_E3F3;
  flipped = true;
}

void UndoFlip_B3C3_E3G3() {
  top_rows -= 132513792ULL;
  left_columns -= 2087385956352ULL;
  top_diag9s -= 1708641073102848ULL;
  top_diag7s -= 1708641069564066ULL;
  right_columns -= 136798926067335168ULL;
  bottom_diag9s -= 136798926035484672ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 10;
}

void Flip_B3C3_E3G3() {
  top_rows += 132513792ULL;
  left_columns += 2087385956352ULL;
  top_diag9s += 1708641073102848ULL;
  top_diag7s += 1708641069564066ULL;
  right_columns += 136798926067335168ULL;
  bottom_diag9s += 136798926035484672ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C3_E3G3;
  flipped = true;
}

void UndoFlip_B3D3() {
  top_rows -= 138018816ULL;
  left_columns -= 2087385956838ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 1708641073102848ULL;
  bottom_diag9s -= 136798926035484672ULL;
  disk_difference -= 6;
}

void Flip_B3D3() {
  top_rows += 138018816ULL;
  left_columns += 2087385956838ULL;
  top_diag9s += 162ULL;
  top_diag7s += 1708641073102848ULL;
  bottom_diag9s += 136798926035484672ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3D3;
  flipped = true;
}

void UndoFlip_B3D3_F3() {
  top_rows -= 139198464ULL;
  left_columns -= 2087385956838ULL;
  top_diag9s -= 19791209300130ULL;
  top_diag7s -= 1708641073102848ULL;
  right_columns -= 2087354105856ULL;
  bottom_diag9s -= 136798926035484672ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 8;
}

void Flip_B3D3_F3() {
  top_rows += 139198464ULL;
  left_columns += 2087385956838ULL;
  top_diag9s += 19791209300130ULL;
  top_diag7s += 1708641073102848ULL;
  right_columns += 2087354105856ULL;
  bottom_diag9s += 136798926035484672ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3D3_F3;
  flipped = true;
}

void UndoFlip_B3D3_F3G3() {
  top_rows -= 139591680ULL;
  left_columns -= 2087385956838ULL;
  top_diag9s -= 1708641069564066ULL;
  top_diag7s -= 1708641073102848ULL;
  right_columns -= 2087385956352ULL;
  bottom_diag9s -= 136798926035484672ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 10;
}

void Flip_B3D3_F3G3() {
  top_rows += 139591680ULL;
  left_columns += 2087385956838ULL;
  top_diag9s += 1708641069564066ULL;
  top_diag7s += 1708641073102848ULL;
  right_columns += 2087385956352ULL;
  bottom_diag9s += 136798926035484672ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3D3_F3G3;
  flipped = true;
}

void UndoFlip_B3E3() {
  top_rows -= 141557760ULL;
  left_columns -= 2087385956838ULL;
  top_diag9s -= 3539106ULL;
  top_diag7s -= 1708641073103010ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 136798926035484672ULL;
  disk_difference -= 8;
}

void Flip_B3E3() {
  top_rows += 141557760ULL;
  left_columns += 2087385956838ULL;
  top_diag9s += 3539106ULL;
  top_diag7s += 1708641073103010ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 136798926035484672ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3E3;
  flipped = true;
}

void UndoFlip_B3E3_G3() {
  top_rows -= 141950976ULL;
  left_columns -= 2087385956838ULL;
  top_diag9s -= 1688849863803042ULL;
  top_diag7s -= 1708641073103010ULL;
  right_columns -= 136796838713229312ULL;
  bottom_diag9s -= 136798926035484672ULL;
  bottom_diag7s -= 2087354105856ULL;
  disk_difference -= 10;
}

void Flip_B3E3_G3() {
  top_rows += 141950976ULL;
  left_columns += 2087385956838ULL;
  top_diag9s += 1688849863803042ULL;
  top_diag7s += 1708641073103010ULL;
  right_columns += 136796838713229312ULL;
  bottom_diag9s += 136798926035484672ULL;
  bottom_diag7s += 2087354105856ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3E3_G3;
  flipped = true;
}

void UndoFlip_B3F3() {
  top_rows -= 142737408ULL;
  left_columns -= 2087385956838ULL;
  top_diag9s -= 19791212839074ULL;
  top_diag7s -= 1708641073103010ULL;
  right_columns -= 136798926035484672ULL;
  bottom_diag9s -= 136798926035484672ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 10;
}

void Flip_B3F3() {
  top_rows += 142737408ULL;
  left_columns += 2087385956838ULL;
  top_diag9s += 19791212839074ULL;
  top_diag7s += 1708641073103010ULL;
  right_columns += 136798926035484672ULL;
  bottom_diag9s += 136798926035484672ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3F3;
  flipped = true;
}

void UndoFlip_B3G3() {
  top_rows -= 143130624ULL;
  left_columns -= 2087385956838ULL;
  top_diag9s -= 1708641073103010ULL;
  top_diag7s -= 1708641073103010ULL;
  right_columns -= 136798926067335168ULL;
  bottom_diag9s -= 136798926035484672ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 12;
}

void Flip_B3G3() {
  top_rows += 143130624ULL;
  left_columns += 2087385956838ULL;
  top_diag9s += 1708641073103010ULL;
  top_diag7s += 1708641073103010ULL;
  right_columns += 136798926067335168ULL;
  bottom_diag9s += 136798926035484672ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3G3;
  flipped = true;
}

void UndoFlip_C3() {
  top_rows -= 31850496ULL;
  left_columns -= 31850496ULL;
  top_diag7s -= 19791209299968ULL;
  bottom_diag9s -= 136796838681378816ULL;
  disk_difference -= 2;
}

void Flip_C3() {
  top_rows += 31850496ULL;
  left_columns += 31850496ULL;
  top_diag7s += 19791209299968ULL;
  bottom_diag9s += 136796838681378816ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3;
  flipped = true;
}

void UndoFlip_C3_E3() {
  top_rows -= 35389440ULL;
  left_columns -= 31850496ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 19791209300130ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_C3_E3() {
  top_rows += 35389440ULL;
  left_columns += 31850496ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 19791209300130ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E3;
  flipped = true;
}

void UndoFlip_C3_E3F3() {
  top_rows -= 36569088ULL;
  left_columns -= 31850496ULL;
  top_diag9s -= 19791212838912ULL;
  top_diag7s -= 19791209300130ULL;
  right_columns -= 136798926035484672ULL;
  bottom_diag9s -= 136796838681378816ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 6;
}

void Flip_C3_E3F3() {
  top_rows += 36569088ULL;
  left_columns += 31850496ULL;
  top_diag9s += 19791212838912ULL;
  top_diag7s += 19791209300130ULL;
  right_columns += 136798926035484672ULL;
  bottom_diag9s += 136796838681378816ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E3F3;
  flipped = true;
}

void UndoFlip_C3_E3G3() {
  top_rows -= 36962304ULL;
  left_columns -= 31850496ULL;
  top_diag9s -= 1708641073102848ULL;
  top_diag7s -= 19791209300130ULL;
  right_columns -= 136798926067335168ULL;
  bottom_diag9s -= 136796838681378816ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 8;
}

void Flip_C3_E3G3() {
  top_rows += 36962304ULL;
  left_columns += 31850496ULL;
  top_diag9s += 1708641073102848ULL;
  top_diag7s += 19791209300130ULL;
  right_columns += 136798926067335168ULL;
  bottom_diag9s += 136796838681378816ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E3G3;
  flipped = true;
}

void UndoFlip_C3D3() {
  top_rows -= 42467328ULL;
  left_columns -= 31850982ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 19791212838912ULL;
  bottom_diag9s -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_C3D3() {
  top_rows += 42467328ULL;
  left_columns += 31850982ULL;
  top_diag9s += 162ULL;
  top_diag7s += 19791212838912ULL;
  bottom_diag9s += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D3;
  flipped = true;
}

void UndoFlip_C3D3_F3() {
  top_rows -= 43646976ULL;
  left_columns -= 31850982ULL;
  top_diag9s -= 19791209300130ULL;
  top_diag7s -= 19791212838912ULL;
  right_columns -= 2087354105856ULL;
  bottom_diag9s -= 136796838681378816ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 6;
}

void Flip_C3D3_F3() {
  top_rows += 43646976ULL;
  left_columns += 31850982ULL;
  top_diag9s += 19791209300130ULL;
  top_diag7s += 19791212838912ULL;
  right_columns += 2087354105856ULL;
  bottom_diag9s += 136796838681378816ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D3_F3;
  flipped = true;
}

void UndoFlip_C3D3_F3G3() {
  top_rows -= 44040192ULL;
  left_columns -= 31850982ULL;
  top_diag9s -= 1708641069564066ULL;
  top_diag7s -= 19791212838912ULL;
  right_columns -= 2087385956352ULL;
  bottom_diag9s -= 136796838681378816ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 8;
}

void Flip_C3D3_F3G3() {
  top_rows += 44040192ULL;
  left_columns += 31850982ULL;
  top_diag9s += 1708641069564066ULL;
  top_diag7s += 19791212838912ULL;
  right_columns += 2087385956352ULL;
  bottom_diag9s += 136796838681378816ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D3_F3G3;
  flipped = true;
}

void UndoFlip_C3E3() {
  top_rows -= 46006272ULL;
  left_columns -= 31850982ULL;
  top_diag9s -= 3539106ULL;
  top_diag7s -= 19791212839074ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 136796838681378816ULL;
  disk_difference -= 6;
}

void Flip_C3E3() {
  top_rows += 46006272ULL;
  left_columns += 31850982ULL;
  top_diag9s += 3539106ULL;
  top_diag7s += 19791212839074ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 136796838681378816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3E3;
  flipped = true;
}

void UndoFlip_C3E3_G3() {
  top_rows -= 46399488ULL;
  left_columns -= 31850982ULL;
  top_diag9s -= 1688849863803042ULL;
  top_diag7s -= 19791212839074ULL;
  right_columns -= 136796838713229312ULL;
  bottom_diag9s -= 136796838681378816ULL;
  bottom_diag7s -= 2087354105856ULL;
  disk_difference -= 8;
}

void Flip_C3E3_G3() {
  top_rows += 46399488ULL;
  left_columns += 31850982ULL;
  top_diag9s += 1688849863803042ULL;
  top_diag7s += 19791212839074ULL;
  right_columns += 136796838713229312ULL;
  bottom_diag9s += 136796838681378816ULL;
  bottom_diag7s += 2087354105856ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3E3_G3;
  flipped = true;
}

void UndoFlip_C3F3() {
  top_rows -= 47185920ULL;
  left_columns -= 31850982ULL;
  top_diag9s -= 19791212839074ULL;
  top_diag7s -= 19791212839074ULL;
  right_columns -= 136798926035484672ULL;
  bottom_diag9s -= 136796838681378816ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 8;
}

void Flip_C3F3() {
  top_rows += 47185920ULL;
  left_columns += 31850982ULL;
  top_diag9s += 19791212839074ULL;
  top_diag7s += 19791212839074ULL;
  right_columns += 136798926035484672ULL;
  bottom_diag9s += 136796838681378816ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3F3;
  flipped = true;
}

void UndoFlip_C3G3() {
  top_rows -= 47579136ULL;
  left_columns -= 31850982ULL;
  top_diag9s -= 1708641073103010ULL;
  top_diag7s -= 19791212839074ULL;
  right_columns -= 136798926067335168ULL;
  bottom_diag9s -= 136796838681378816ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 10;
}

void Flip_C3G3() {
  top_rows += 47579136ULL;
  left_columns += 31850982ULL;
  top_diag9s += 1708641073103010ULL;
  top_diag7s += 19791212839074ULL;
  right_columns += 136798926067335168ULL;
  bottom_diag9s += 136796838681378816ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3G3;
  flipped = true;
}

void UndoFlip_D3() {
  top_rows -= 10616832ULL;
  left_columns -= 486ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538944ULL;
  disk_difference -= 2;
}

void Flip_D3() {
  top_rows += 10616832ULL;
  left_columns += 486ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538944ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3;
  flipped = true;
}

void UndoFlip_D3_F3() {
  top_rows -= 11796480ULL;
  left_columns -= 486ULL;
  top_diag9s -= 19791209300130ULL;
  top_diag7s -= 3538944ULL;
  right_columns -= 2087354105856ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_D3_F3() {
  top_rows += 11796480ULL;
  left_columns += 486ULL;
  top_diag9s += 19791209300130ULL;
  top_diag7s += 3538944ULL;
  right_columns += 2087354105856ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_F3;
  flipped = true;
}

void UndoFlip_D3_F3G3() {
  top_rows -= 12189696ULL;
  left_columns -= 486ULL;
  top_diag9s -= 1708641069564066ULL;
  top_diag7s -= 3538944ULL;
  right_columns -= 2087385956352ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 6;
}

void Flip_D3_F3G3() {
  top_rows += 12189696ULL;
  left_columns += 486ULL;
  top_diag9s += 1708641069564066ULL;
  top_diag7s += 3538944ULL;
  right_columns += 2087385956352ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_F3G3;
  flipped = true;
}

void UndoFlip_D3E3() {
  top_rows -= 14155776ULL;
  left_columns -= 486ULL;
  top_diag9s -= 3539106ULL;
  top_diag7s -= 3539106ULL;
  right_columns -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_D3E3() {
  top_rows += 14155776ULL;
  left_columns += 486ULL;
  top_diag9s += 3539106ULL;
  top_diag7s += 3539106ULL;
  right_columns += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3E3;
  flipped = true;
}

void UndoFlip_D3E3_G3() {
  top_rows -= 14548992ULL;
  left_columns -= 486ULL;
  top_diag9s -= 1688849863803042ULL;
  top_diag7s -= 3539106ULL;
  right_columns -= 136796838713229312ULL;
  bottom_diag7s -= 2087354105856ULL;
  disk_difference -= 6;
}

void Flip_D3E3_G3() {
  top_rows += 14548992ULL;
  left_columns += 486ULL;
  top_diag9s += 1688849863803042ULL;
  top_diag7s += 3539106ULL;
  right_columns += 136796838713229312ULL;
  bottom_diag7s += 2087354105856ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3E3_G3;
  flipped = true;
}

void UndoFlip_D3F3() {
  top_rows -= 15335424ULL;
  left_columns -= 486ULL;
  top_diag9s -= 19791212839074ULL;
  top_diag7s -= 3539106ULL;
  right_columns -= 136798926035484672ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 6;
}

void Flip_D3F3() {
  top_rows += 15335424ULL;
  left_columns += 486ULL;
  top_diag9s += 19791212839074ULL;
  top_diag7s += 3539106ULL;
  right_columns += 136798926035484672ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3F3;
  flipped = true;
}

void UndoFlip_D3G3() {
  top_rows -= 15728640ULL;
  left_columns -= 486ULL;
  top_diag9s -= 1708641073103010ULL;
  top_diag7s -= 3539106ULL;
  right_columns -= 136798926067335168ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 8;
}

void Flip_D3G3() {
  top_rows += 15728640ULL;
  left_columns += 486ULL;
  top_diag9s += 1708641073103010ULL;
  top_diag7s += 3539106ULL;
  right_columns += 136798926067335168ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3G3;
  flipped = true;
}

void UndoFlip_E3() {
  top_rows -= 3538944ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 162ULL;
  right_columns -= 136796838681378816ULL;
  disk_difference -= 2;
}

void Flip_E3() {
  top_rows += 3538944ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 162ULL;
  right_columns += 136796838681378816ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3;
  flipped = true;
}

void UndoFlip_E3_G3() {
  top_rows -= 3932160ULL;
  top_diag9s -= 1688849863802880ULL;
  top_diag7s -= 162ULL;
  right_columns -= 136796838713229312ULL;
  bottom_diag7s -= 2087354105856ULL;
  disk_difference -= 4;
}

void Flip_E3_G3() {
  top_rows += 3932160ULL;
  top_diag9s += 1688849863802880ULL;
  top_diag7s += 162ULL;
  right_columns += 136796838713229312ULL;
  bottom_diag7s += 2087354105856ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_G3;
  flipped = true;
}

void UndoFlip_E3F3() {
  top_rows -= 4718592ULL;
  top_diag9s -= 19791212838912ULL;
  top_diag7s -= 162ULL;
  right_columns -= 136798926035484672ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_E3F3() {
  top_rows += 4718592ULL;
  top_diag9s += 19791212838912ULL;
  top_diag7s += 162ULL;
  right_columns += 136798926035484672ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3F3;
  flipped = true;
}

void UndoFlip_E3G3() {
  top_rows -= 5111808ULL;
  top_diag9s -= 1708641073102848ULL;
  top_diag7s -= 162ULL;
  right_columns -= 136798926067335168ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 6;
}

void Flip_E3G3() {
  top_rows += 5111808ULL;
  top_diag9s += 1708641073102848ULL;
  top_diag7s += 162ULL;
  right_columns += 136798926067335168ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3G3;
  flipped = true;
}

void UndoFlip_F3() {
  top_rows -= 1179648ULL;
  top_diag9s -= 19791209299968ULL;
  right_columns -= 2087354105856ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 2;
}

void Flip_F3() {
  top_rows += 1179648ULL;
  top_diag9s += 19791209299968ULL;
  right_columns += 2087354105856ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3;
  flipped = true;
}

void UndoFlip_F3G3() {
  top_rows -= 1572864ULL;
  top_diag9s -= 1708641069563904ULL;
  right_columns -= 2087385956352ULL;
  bottom_diag7s -= 136798926035484672ULL;
  disk_difference -= 4;
}

void Flip_F3G3() {
  top_rows += 1572864ULL;
  top_diag9s += 1708641069563904ULL;
  right_columns += 2087385956352ULL;
  bottom_diag7s += 136798926035484672ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3G3;
  flipped = true;
}

void UndoFlip_G3() {
  top_rows -= 393216ULL;
  top_diag9s -= 1688849860263936ULL;
  right_columns -= 31850496ULL;
  bottom_diag7s -= 2087354105856ULL;
  disk_difference -= 2;
}

void Flip_G3() {
  top_rows += 393216ULL;
  top_diag9s += 1688849860263936ULL;
  right_columns += 31850496ULL;
  bottom_diag7s += 2087354105856ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3;
  flipped = true;
}

void UndoFlip_B4() {
  top_rows -= 1458ULL;
  left_columns -= 695784701952ULL;
  top_diag7s -= 6597069766656ULL;
  bottom_diag9s -= 10616832ULL;
  disk_difference -= 2;
}

void Flip_B4() {
  top_rows += 1458ULL;
  left_columns += 695784701952ULL;
  top_diag7s += 6597069766656ULL;
  bottom_diag9s += 10616832ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4;
  flipped = true;
}

void UndoFlip_B4_D4() {
  top_rows -= 1620ULL;
  left_columns -= 695784702114ULL;
  top_diag7s -= 6597069766710ULL;
  bottom_diag9s -= 45598946237743104ULL;
  disk_difference -= 4;
}

void Flip_B4_D4() {
  top_rows += 1620ULL;
  left_columns += 695784702114ULL;
  top_diag7s += 6597069766710ULL;
  bottom_diag9s += 45598946237743104ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D4;
  flipped = true;
}

void UndoFlip_B4_D4E4() {
  top_rows -= 1674ULL;
  left_columns -= 695784702114ULL;
  top_diag9s -= 54ULL;
  top_diag7s -= 6597069766710ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 45598946237743104ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_B4_D4E4() {
  top_rows += 1674ULL;
  left_columns += 695784702114ULL;
  top_diag9s += 54ULL;
  top_diag7s += 6597069766710ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 45598946237743104ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D4E4;
  flipped = true;
}

void UndoFlip_B4_D4F4() {
  top_rows -= 1692ULL;
  left_columns -= 695784702114ULL;
  top_diag9s -= 1179702ULL;
  top_diag7s -= 6597069766710ULL;
  right_columns -= 45599642011828224ULL;
  bottom_diag9s -= 45598946237743104ULL;
  bottom_diag7s -= 45599642011828224ULL;
  disk_difference -= 8;
}

void Flip_B4_D4F4() {
  top_rows += 1692ULL;
  left_columns += 695784702114ULL;
  top_diag9s += 1179702ULL;
  top_diag7s += 6597069766710ULL;
  right_columns += 45599642011828224ULL;
  bottom_diag9s += 45598946237743104ULL;
  bottom_diag7s += 45599642011828224ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D4F4;
  flipped = true;
}

void UndoFlip_B4_D4G4() {
  top_rows -= 1698ULL;
  left_columns -= 695784702114ULL;
  top_diag9s -= 6597070946358ULL;
  top_diag7s -= 6597069766710ULL;
  right_columns -= 45599642022445056ULL;
  bottom_diag9s -= 45598946237743104ULL;
  bottom_diag7s -= 45599642022445056ULL;
  disk_difference -= 10;
}

void Flip_B4_D4G4() {
  top_rows += 1698ULL;
  left_columns += 695784702114ULL;
  top_diag9s += 6597070946358ULL;
  top_diag7s += 6597069766710ULL;
  right_columns += 45599642022445056ULL;
  bottom_diag9s += 45598946237743104ULL;
  bottom_diag7s += 45599642022445056ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D4G4;
  flipped = true;
}

void UndoFlip_B4C4() {
  top_rows -= 1944ULL;
  left_columns -= 695795318784ULL;
  top_diag7s -= 6597070946304ULL;
  bottom_diag9s -= 695795318784ULL;
  disk_difference -= 4;
}

void Flip_B4C4() {
  top_rows += 1944ULL;
  left_columns += 695795318784ULL;
  top_diag7s += 6597070946304ULL;
  bottom_diag9s += 695795318784ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C4;
  flipped = true;
}

void UndoFlip_B4C4_E4() {
  top_rows -= 1998ULL;
  left_columns -= 695795318784ULL;
  top_diag9s -= 54ULL;
  top_diag7s -= 6597070946304ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 695795318784ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_B4C4_E4() {
  top_rows += 1998ULL;
  left_columns += 695795318784ULL;
  top_diag9s += 54ULL;
  top_diag7s += 6597070946304ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 695795318784ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C4_E4;
  flipped = true;
}

void UndoFlip_B4C4_E4F4() {
  top_rows -= 2016ULL;
  left_columns -= 695795318784ULL;
  top_diag9s -= 1179702ULL;
  top_diag7s -= 6597070946304ULL;
  right_columns -= 45599642011828224ULL;
  bottom_diag9s -= 695795318784ULL;
  bottom_diag7s -= 45599642011828224ULL;
  disk_difference -= 8;
}

void Flip_B4C4_E4F4() {
  top_rows += 2016ULL;
  left_columns += 695795318784ULL;
  top_diag9s += 1179702ULL;
  top_diag7s += 6597070946304ULL;
  right_columns += 45599642011828224ULL;
  bottom_diag9s += 695795318784ULL;
  bottom_diag7s += 45599642011828224ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C4_E4F4;
  flipped = true;
}

void UndoFlip_B4C4_E4G4() {
  top_rows -= 2022ULL;
  left_columns -= 695795318784ULL;
  top_diag9s -= 6597070946358ULL;
  top_diag7s -= 6597070946304ULL;
  right_columns -= 45599642022445056ULL;
  bottom_diag9s -= 695795318784ULL;
  bottom_diag7s -= 45599642022445056ULL;
  disk_difference -= 10;
}

void Flip_B4C4_E4G4() {
  top_rows += 2022ULL;
  left_columns += 695795318784ULL;
  top_diag9s += 6597070946358ULL;
  top_diag7s += 6597070946304ULL;
  right_columns += 45599642022445056ULL;
  bottom_diag9s += 695795318784ULL;
  bottom_diag7s += 45599642022445056ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C4_E4G4;
  flipped = true;
}

void UndoFlip_B4D4() {
  top_rows -= 2106ULL;
  left_columns -= 695795318946ULL;
  top_diag7s -= 6597070946358ULL;
  bottom_diag9s -= 45599642022445056ULL;
  disk_difference -= 6;
}

void Flip_B4D4() {
  top_rows += 2106ULL;
  left_columns += 695795318946ULL;
  top_diag7s += 6597070946358ULL;
  bottom_diag9s += 45599642022445056ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4D4;
  flipped = true;
}

void UndoFlip_B4D4_F4() {
  top_rows -= 2124ULL;
  left_columns -= 695795318946ULL;
  top_diag9s -= 1179648ULL;
  top_diag7s -= 6597070946358ULL;
  right_columns -= 695784701952ULL;
  bottom_diag9s -= 45599642022445056ULL;
  bottom_diag7s -= 695784701952ULL;
  disk_difference -= 8;
}

void Flip_B4D4_F4() {
  top_rows += 2124ULL;
  left_columns += 695795318946ULL;
  top_diag9s += 1179648ULL;
  top_diag7s += 6597070946358ULL;
  right_columns += 695784701952ULL;
  bottom_diag9s += 45599642022445056ULL;
  bottom_diag7s += 695784701952ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4D4_F4;
  flipped = true;
}

void UndoFlip_B4D4_F4G4() {
  top_rows -= 2130ULL;
  left_columns -= 695795318946ULL;
  top_diag9s -= 6597070946304ULL;
  top_diag7s -= 6597070946358ULL;
  right_columns -= 695795318784ULL;
  bottom_diag9s -= 45599642022445056ULL;
  bottom_diag7s -= 695795318784ULL;
  disk_difference -= 10;
}

void Flip_B4D4_F4G4() {
  top_rows += 2130ULL;
  left_columns += 695795318946ULL;
  top_diag9s += 6597070946304ULL;
  top_diag7s += 6597070946358ULL;
  right_columns += 695795318784ULL;
  bottom_diag9s += 45599642022445056ULL;
  bottom_diag7s += 695795318784ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4D4_F4G4;
  flipped = true;
}

void UndoFlip_B4E4() {
  top_rows -= 2160ULL;
  left_columns -= 695795318946ULL;
  top_diag9s -= 54ULL;
  top_diag7s -= 6597070946358ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 45599642022445056ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 8;
}

void Flip_B4E4() {
  top_rows += 2160ULL;
  left_columns += 695795318946ULL;
  top_diag9s += 54ULL;
  top_diag7s += 6597070946358ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 45599642022445056ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4E4;
  flipped = true;
}

void UndoFlip_B4E4_G4() {
  top_rows -= 2166ULL;
  left_columns -= 695795318946ULL;
  top_diag9s -= 6597069766710ULL;
  top_diag7s -= 6597070946358ULL;
  right_columns -= 45598946237743104ULL;
  bottom_diag9s -= 45599642022445056ULL;
  bottom_diag7s -= 45598946237743104ULL;
  disk_difference -= 10;
}

void Flip_B4E4_G4() {
  top_rows += 2166ULL;
  left_columns += 695795318946ULL;
  top_diag9s += 6597069766710ULL;
  top_diag7s += 6597070946358ULL;
  right_columns += 45598946237743104ULL;
  bottom_diag9s += 45599642022445056ULL;
  bottom_diag7s += 45598946237743104ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4E4_G4;
  flipped = true;
}

void UndoFlip_B4F4() {
  top_rows -= 2178ULL;
  left_columns -= 695795318946ULL;
  top_diag9s -= 1179702ULL;
  top_diag7s -= 6597070946358ULL;
  right_columns -= 45599642011828224ULL;
  bottom_diag9s -= 45599642022445056ULL;
  bottom_diag7s -= 45599642011828224ULL;
  disk_difference -= 10;
}

void Flip_B4F4() {
  top_rows += 2178ULL;
  left_columns += 695795318946ULL;
  top_diag9s += 1179702ULL;
  top_diag7s += 6597070946358ULL;
  right_columns += 45599642011828224ULL;
  bottom_diag9s += 45599642022445056ULL;
  bottom_diag7s += 45599642011828224ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4F4;
  flipped = true;
}

void UndoFlip_B4G4() {
  top_rows -= 2184ULL;
  left_columns -= 695795318946ULL;
  top_diag9s -= 6597070946358ULL;
  top_diag7s -= 6597070946358ULL;
  right_columns -= 45599642022445056ULL;
  bottom_diag9s -= 45599642022445056ULL;
  bottom_diag7s -= 45599642022445056ULL;
  disk_difference -= 12;
}

void Flip_B4G4() {
  top_rows += 2184ULL;
  left_columns += 695795318946ULL;
  top_diag9s += 6597070946358ULL;
  top_diag7s += 6597070946358ULL;
  right_columns += 45599642022445056ULL;
  bottom_diag9s += 45599642022445056ULL;
  bottom_diag7s += 45599642022445056ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4G4;
  flipped = true;
}

void UndoFlip_C4() {
  top_rows -= 486ULL;
  left_columns -= 10616832ULL;
  top_diag7s -= 1179648ULL;
  bottom_diag9s -= 695784701952ULL;
  disk_difference -= 2;
}

void Flip_C4() {
  top_rows += 486ULL;
  left_columns += 10616832ULL;
  top_diag7s += 1179648ULL;
  bottom_diag9s += 695784701952ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4;
  flipped = true;
}

void UndoFlip_C4_E4() {
  top_rows -= 540ULL;
  left_columns -= 10616832ULL;
  top_diag9s -= 54ULL;
  top_diag7s -= 1179648ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 695784701952ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_C4_E4() {
  top_rows += 540ULL;
  left_columns += 10616832ULL;
  top_diag9s += 54ULL;
  top_diag7s += 1179648ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 695784701952ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_E4;
  flipped = true;
}

void UndoFlip_C4_E4F4() {
  top_rows -= 558ULL;
  left_columns -= 10616832ULL;
  top_diag9s -= 1179702ULL;
  top_diag7s -= 1179648ULL;
  right_columns -= 45599642011828224ULL;
  bottom_diag9s -= 695784701952ULL;
  bottom_diag7s -= 45599642011828224ULL;
  disk_difference -= 6;
}

void Flip_C4_E4F4() {
  top_rows += 558ULL;
  left_columns += 10616832ULL;
  top_diag9s += 1179702ULL;
  top_diag7s += 1179648ULL;
  right_columns += 45599642011828224ULL;
  bottom_diag9s += 695784701952ULL;
  bottom_diag7s += 45599642011828224ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_E4F4;
  flipped = true;
}

void UndoFlip_C4_E4G4() {
  top_rows -= 564ULL;
  left_columns -= 10616832ULL;
  top_diag9s -= 6597070946358ULL;
  top_diag7s -= 1179648ULL;
  right_columns -= 45599642022445056ULL;
  bottom_diag9s -= 695784701952ULL;
  bottom_diag7s -= 45599642022445056ULL;
  disk_difference -= 8;
}

void Flip_C4_E4G4() {
  top_rows += 564ULL;
  left_columns += 10616832ULL;
  top_diag9s += 6597070946358ULL;
  top_diag7s += 1179648ULL;
  right_columns += 45599642022445056ULL;
  bottom_diag9s += 695784701952ULL;
  bottom_diag7s += 45599642022445056ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_E4G4;
  flipped = true;
}

void UndoFlip_C4D4() {
  top_rows -= 648ULL;
  left_columns -= 10616994ULL;
  top_diag7s -= 1179702ULL;
  bottom_diag9s -= 45599642011828224ULL;
  disk_difference -= 4;
}

void Flip_C4D4() {
  top_rows += 648ULL;
  left_columns += 10616994ULL;
  top_diag7s += 1179702ULL;
  bottom_diag9s += 45599642011828224ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4D4;
  flipped = true;
}

void UndoFlip_C4D4_F4() {
  top_rows -= 666ULL;
  left_columns -= 10616994ULL;
  top_diag9s -= 1179648ULL;
  top_diag7s -= 1179702ULL;
  right_columns -= 695784701952ULL;
  bottom_diag9s -= 45599642011828224ULL;
  bottom_diag7s -= 695784701952ULL;
  disk_difference -= 6;
}

void Flip_C4D4_F4() {
  top_rows += 666ULL;
  left_columns += 10616994ULL;
  top_diag9s += 1179648ULL;
  top_diag7s += 1179702ULL;
  right_columns += 695784701952ULL;
  bottom_diag9s += 45599642011828224ULL;
  bottom_diag7s += 695784701952ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4D4_F4;
  flipped = true;
}

void UndoFlip_C4D4_F4G4() {
  top_rows -= 672ULL;
  left_columns -= 10616994ULL;
  top_diag9s -= 6597070946304ULL;
  top_diag7s -= 1179702ULL;
  right_columns -= 695795318784ULL;
  bottom_diag9s -= 45599642011828224ULL;
  bottom_diag7s -= 695795318784ULL;
  disk_difference -= 8;
}

void Flip_C4D4_F4G4() {
  top_rows += 672ULL;
  left_columns += 10616994ULL;
  top_diag9s += 6597070946304ULL;
  top_diag7s += 1179702ULL;
  right_columns += 695795318784ULL;
  bottom_diag9s += 45599642011828224ULL;
  bottom_diag7s += 695795318784ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4D4_F4G4;
  flipped = true;
}

void UndoFlip_C4E4() {
  top_rows -= 702ULL;
  left_columns -= 10616994ULL;
  top_diag9s -= 54ULL;
  top_diag7s -= 1179702ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 45599642011828224ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_C4E4() {
  top_rows += 702ULL;
  left_columns += 10616994ULL;
  top_diag9s += 54ULL;
  top_diag7s += 1179702ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 45599642011828224ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4E4;
  flipped = true;
}

void UndoFlip_C4E4_G4() {
  top_rows -= 708ULL;
  left_columns -= 10616994ULL;
  top_diag9s -= 6597069766710ULL;
  top_diag7s -= 1179702ULL;
  right_columns -= 45598946237743104ULL;
  bottom_diag9s -= 45599642011828224ULL;
  bottom_diag7s -= 45598946237743104ULL;
  disk_difference -= 8;
}

void Flip_C4E4_G4() {
  top_rows += 708ULL;
  left_columns += 10616994ULL;
  top_diag9s += 6597069766710ULL;
  top_diag7s += 1179702ULL;
  right_columns += 45598946237743104ULL;
  bottom_diag9s += 45599642011828224ULL;
  bottom_diag7s += 45598946237743104ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4E4_G4;
  flipped = true;
}

void UndoFlip_C4F4() {
  top_rows -= 720ULL;
  left_columns -= 10616994ULL;
  top_diag9s -= 1179702ULL;
  top_diag7s -= 1179702ULL;
  right_columns -= 45599642011828224ULL;
  bottom_diag9s -= 45599642011828224ULL;
  bottom_diag7s -= 45599642011828224ULL;
  disk_difference -= 8;
}

void Flip_C4F4() {
  top_rows += 720ULL;
  left_columns += 10616994ULL;
  top_diag9s += 1179702ULL;
  top_diag7s += 1179702ULL;
  right_columns += 45599642011828224ULL;
  bottom_diag9s += 45599642011828224ULL;
  bottom_diag7s += 45599642011828224ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4F4;
  flipped = true;
}

void UndoFlip_C4G4() {
  top_rows -= 726ULL;
  left_columns -= 10616994ULL;
  top_diag9s -= 6597070946358ULL;
  top_diag7s -= 1179702ULL;
  right_columns -= 45599642022445056ULL;
  bottom_diag9s -= 45599642011828224ULL;
  bottom_diag7s -= 45599642022445056ULL;
  disk_difference -= 10;
}

void Flip_C4G4() {
  top_rows += 726ULL;
  left_columns += 10616994ULL;
  top_diag9s += 6597070946358ULL;
  top_diag7s += 1179702ULL;
  right_columns += 45599642022445056ULL;
  bottom_diag9s += 45599642011828224ULL;
  bottom_diag7s += 45599642022445056ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4G4;
  flipped = true;
}

void UndoFlip_D4() {
  top_rows -= 162ULL;
  left_columns -= 162ULL;
  top_diag7s -= 54ULL;
  bottom_diag9s -= 45598946227126272ULL;
  disk_difference -= 2;
}

void Flip_D4() {
  top_rows += 162ULL;
  left_columns += 162ULL;
  top_diag7s += 54ULL;
  bottom_diag9s += 45598946227126272ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4;
  flipped = true;
}

void UndoFlip_D4_F4() {
  top_rows -= 180ULL;
  left_columns -= 162ULL;
  top_diag9s -= 1179648ULL;
  top_diag7s -= 54ULL;
  right_columns -= 695784701952ULL;
  bottom_diag9s -= 45598946227126272ULL;
  bottom_diag7s -= 695784701952ULL;
  disk_difference -= 4;
}

void Flip_D4_F4() {
  top_rows += 180ULL;
  left_columns += 162ULL;
  top_diag9s += 1179648ULL;
  top_diag7s += 54ULL;
  right_columns += 695784701952ULL;
  bottom_diag9s += 45598946227126272ULL;
  bottom_diag7s += 695784701952ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_F4;
  flipped = true;
}

void UndoFlip_D4_F4G4() {
  top_rows -= 186ULL;
  left_columns -= 162ULL;
  top_diag9s -= 6597070946304ULL;
  top_diag7s -= 54ULL;
  right_columns -= 695795318784ULL;
  bottom_diag9s -= 45598946227126272ULL;
  bottom_diag7s -= 695795318784ULL;
  disk_difference -= 6;
}

void Flip_D4_F4G4() {
  top_rows += 186ULL;
  left_columns += 162ULL;
  top_diag9s += 6597070946304ULL;
  top_diag7s += 54ULL;
  right_columns += 695795318784ULL;
  bottom_diag9s += 45598946227126272ULL;
  bottom_diag7s += 695795318784ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_F4G4;
  flipped = true;
}

void UndoFlip_D4E4() {
  top_rows -= 216ULL;
  left_columns -= 162ULL;
  top_diag9s -= 54ULL;
  top_diag7s -= 54ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 45598946227126272ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_D4E4() {
  top_rows += 216ULL;
  left_columns += 162ULL;
  top_diag9s += 54ULL;
  top_diag7s += 54ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 45598946227126272ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4E4;
  flipped = true;
}

void UndoFlip_D4E4_G4() {
  top_rows -= 222ULL;
  left_columns -= 162ULL;
  top_diag9s -= 6597069766710ULL;
  top_diag7s -= 54ULL;
  right_columns -= 45598946237743104ULL;
  bottom_diag9s -= 45598946227126272ULL;
  bottom_diag7s -= 45598946237743104ULL;
  disk_difference -= 6;
}

void Flip_D4E4_G4() {
  top_rows += 222ULL;
  left_columns += 162ULL;
  top_diag9s += 6597069766710ULL;
  top_diag7s += 54ULL;
  right_columns += 45598946237743104ULL;
  bottom_diag9s += 45598946227126272ULL;
  bottom_diag7s += 45598946237743104ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4E4_G4;
  flipped = true;
}

void UndoFlip_D4F4() {
  top_rows -= 234ULL;
  left_columns -= 162ULL;
  top_diag9s -= 1179702ULL;
  top_diag7s -= 54ULL;
  right_columns -= 45599642011828224ULL;
  bottom_diag9s -= 45598946227126272ULL;
  bottom_diag7s -= 45599642011828224ULL;
  disk_difference -= 6;
}

void Flip_D4F4() {
  top_rows += 234ULL;
  left_columns += 162ULL;
  top_diag9s += 1179702ULL;
  top_diag7s += 54ULL;
  right_columns += 45599642011828224ULL;
  bottom_diag9s += 45598946227126272ULL;
  bottom_diag7s += 45599642011828224ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4F4;
  flipped = true;
}

void UndoFlip_D4G4() {
  top_rows -= 240ULL;
  left_columns -= 162ULL;
  top_diag9s -= 6597070946358ULL;
  top_diag7s -= 54ULL;
  right_columns -= 45599642022445056ULL;
  bottom_diag9s -= 45598946227126272ULL;
  bottom_diag7s -= 45599642022445056ULL;
  disk_difference -= 8;
}

void Flip_D4G4() {
  top_rows += 240ULL;
  left_columns += 162ULL;
  top_diag9s += 6597070946358ULL;
  top_diag7s += 54ULL;
  right_columns += 45599642022445056ULL;
  bottom_diag9s += 45598946227126272ULL;
  bottom_diag7s += 45599642022445056ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4G4;
  flipped = true;
}

void UndoFlip_E4() {
  top_rows -= 54ULL;
  top_diag9s -= 54ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 2;
}

void Flip_E4() {
  top_rows += 54ULL;
  top_diag9s += 54ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4;
  flipped = true;
}

void UndoFlip_E4_G4() {
  top_rows -= 60ULL;
  top_diag9s -= 6597069766710ULL;
  right_columns -= 45598946237743104ULL;
  bottom_diag7s -= 45598946237743104ULL;
  disk_difference -= 4;
}

void Flip_E4_G4() {
  top_rows += 60ULL;
  top_diag9s += 6597069766710ULL;
  right_columns += 45598946237743104ULL;
  bottom_diag7s += 45598946237743104ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_G4;
  flipped = true;
}

void UndoFlip_E4F4() {
  top_rows -= 72ULL;
  top_diag9s -= 1179702ULL;
  right_columns -= 45599642011828224ULL;
  bottom_diag7s -= 45599642011828224ULL;
  disk_difference -= 4;
}

void Flip_E4F4() {
  top_rows += 72ULL;
  top_diag9s += 1179702ULL;
  right_columns += 45599642011828224ULL;
  bottom_diag7s += 45599642011828224ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4F4;
  flipped = true;
}

void UndoFlip_E4G4() {
  top_rows -= 78ULL;
  top_diag9s -= 6597070946358ULL;
  right_columns -= 45599642022445056ULL;
  bottom_diag7s -= 45599642022445056ULL;
  disk_difference -= 6;
}

void Flip_E4G4() {
  top_rows += 78ULL;
  top_diag9s += 6597070946358ULL;
  right_columns += 45599642022445056ULL;
  bottom_diag7s += 45599642022445056ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4G4;
  flipped = true;
}

void UndoFlip_F4() {
  top_rows -= 18ULL;
  top_diag9s -= 1179648ULL;
  right_columns -= 695784701952ULL;
  bottom_diag7s -= 695784701952ULL;
  disk_difference -= 2;
}

void Flip_F4() {
  top_rows += 18ULL;
  top_diag9s += 1179648ULL;
  right_columns += 695784701952ULL;
  bottom_diag7s += 695784701952ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4;
  flipped = true;
}

void UndoFlip_F4G4() {
  top_rows -= 24ULL;
  top_diag9s -= 6597070946304ULL;
  right_columns -= 695795318784ULL;
  bottom_diag7s -= 695795318784ULL;
  disk_difference -= 4;
}

void Flip_F4G4() {
  top_rows += 24ULL;
  top_diag9s += 6597070946304ULL;
  right_columns += 695795318784ULL;
  bottom_diag7s += 695795318784ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4G4;
  flipped = true;
}

void UndoFlip_G4() {
  top_rows -= 6ULL;
  top_diag9s -= 6597069766656ULL;
  right_columns -= 10616832ULL;
  bottom_diag7s -= 10616832ULL;
  disk_difference -= 2;
}

void Flip_G4() {
  top_rows += 6ULL;
  top_diag9s += 6597069766656ULL;
  right_columns += 10616832ULL;
  bottom_diag7s += 10616832ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4;
  flipped = true;
}

void UndoFlip_B5() {
  left_columns -= 231928233984ULL;
  top_diag7s -= 393216ULL;
  bottom_diag9s -= 13824ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 2;
}

void Flip_B5() {
  left_columns += 231928233984ULL;
  top_diag7s += 393216ULL;
  bottom_diag9s += 13824ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5;
  flipped = true;
}

void UndoFlip_B5_D5() {
  left_columns -= 231928234038ULL;
  top_diag7s -= 393216ULL;
  bottom_diag9s -= 231928247808ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 455989462271262720ULL;
  disk_difference -= 4;
}

void Flip_B5_D5() {
  left_columns += 231928234038ULL;
  top_diag7s += 393216ULL;
  bottom_diag9s += 231928247808ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 455989462271262720ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_D5;
  flipped = true;
}

void UndoFlip_B5_D5E5() {
  left_columns -= 231928234038ULL;
  top_diag7s -= 393216ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 15199880670623232ULL;
  bottom_diag7s -= 15199880670609408ULL;
  bottom_rows -= 471189111013638144ULL;
  disk_difference -= 6;
}

void Flip_B5_D5E5() {
  left_columns += 231928234038ULL;
  top_diag7s += 393216ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 15199880670623232ULL;
  bottom_diag7s += 15199880670609408ULL;
  bottom_rows += 471189111013638144ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_D5E5;
  flipped = true;
}

void UndoFlip_B5_D5F5() {
  left_columns -= 231928234038ULL;
  top_diag9s -= 18ULL;
  top_diag7s -= 393216ULL;
  right_columns -= 15199880670609408ULL;
  bottom_diag9s -= 15199880670623232ULL;
  bottom_diag7s -= 15199880674148352ULL;
  bottom_rows -= 476255660594429952ULL;
  disk_difference -= 8;
}

void Flip_B5_D5F5() {
  left_columns += 231928234038ULL;
  top_diag9s += 18ULL;
  top_diag7s += 393216ULL;
  right_columns += 15199880670609408ULL;
  bottom_diag9s += 15199880670623232ULL;
  bottom_diag7s += 15199880674148352ULL;
  bottom_rows += 476255660594429952ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_D5F5;
  flipped = true;
}

void UndoFlip_B5_D5G5() {
  left_columns -= 231928234038ULL;
  top_diag9s -= 393234ULL;
  top_diag7s -= 393216ULL;
  right_columns -= 15199880674148352ULL;
  bottom_diag9s -= 15199880670623232ULL;
  bottom_diag7s -= 15199880674162176ULL;
  bottom_rows -= 477944510454693888ULL;
  disk_difference -= 10;
}

void Flip_B5_D5G5() {
  left_columns += 231928234038ULL;
  top_diag9s += 393234ULL;
  top_diag7s += 393216ULL;
  right_columns += 15199880674148352ULL;
  bottom_diag9s += 15199880670623232ULL;
  bottom_diag7s += 15199880674162176ULL;
  bottom_rows += 477944510454693888ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_D5G5;
  flipped = true;
}

void UndoFlip_B5C5() {
  left_columns -= 231931772928ULL;
  top_diag7s -= 393234ULL;
  bottom_diag9s -= 3552768ULL;
  bottom_rows -= 547187354725515264ULL;
  disk_difference -= 4;
}

void Flip_B5C5() {
  left_columns += 231931772928ULL;
  top_diag7s += 393234ULL;
  bottom_diag9s += 3552768ULL;
  bottom_rows += 547187354725515264ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5C5;
  flipped = true;
}

void UndoFlip_B5C5_E5() {
  left_columns -= 231931772928ULL;
  top_diag7s -= 393234ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 15199648745928192ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 562387003467890688ULL;
  disk_difference -= 6;
}

void Flip_B5C5_E5() {
  left_columns += 231931772928ULL;
  top_diag7s += 393234ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 15199648745928192ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 562387003467890688ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5C5_E5;
  flipped = true;
}

void UndoFlip_B5C5_E5F5() {
  left_columns -= 231931772928ULL;
  top_diag9s -= 18ULL;
  top_diag7s -= 393234ULL;
  right_columns -= 15199880670609408ULL;
  bottom_diag9s -= 15199648745928192ULL;
  bottom_diag7s -= 231931772928ULL;
  bottom_rows -= 567453553048682496ULL;
  disk_difference -= 8;
}

void Flip_B5C5_E5F5() {
  left_columns += 231931772928ULL;
  top_diag9s += 18ULL;
  top_diag7s += 393234ULL;
  right_columns += 15199880670609408ULL;
  bottom_diag9s += 15199648745928192ULL;
  bottom_diag7s += 231931772928ULL;
  bottom_rows += 567453553048682496ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5C5_E5F5;
  flipped = true;
}

void UndoFlip_B5C5_E5G5() {
  left_columns -= 231931772928ULL;
  top_diag9s -= 393234ULL;
  top_diag7s -= 393234ULL;
  right_columns -= 15199880674148352ULL;
  bottom_diag9s -= 15199648745928192ULL;
  bottom_diag7s -= 231931786752ULL;
  bottom_rows -= 569142402908946432ULL;
  disk_difference -= 10;
}

void Flip_B5C5_E5G5() {
  left_columns += 231931772928ULL;
  top_diag9s += 393234ULL;
  top_diag7s += 393234ULL;
  right_columns += 15199880674148352ULL;
  bottom_diag9s += 15199648745928192ULL;
  bottom_diag7s += 231931786752ULL;
  bottom_rows += 569142402908946432ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5C5_E5G5;
  flipped = true;
}

void UndoFlip_B5D5() {
  left_columns -= 231931772982ULL;
  top_diag7s -= 393234ULL;
  bottom_diag9s -= 231931786752ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 592786300952641536ULL;
  disk_difference -= 6;
}

void Flip_B5D5() {
  left_columns += 231931772982ULL;
  top_diag7s += 393234ULL;
  bottom_diag9s += 231931786752ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 592786300952641536ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5D5;
  flipped = true;
}

void UndoFlip_B5D5_F5() {
  left_columns -= 231931772982ULL;
  top_diag9s -= 18ULL;
  top_diag7s -= 393234ULL;
  right_columns -= 231928233984ULL;
  bottom_diag9s -= 231931786752ULL;
  bottom_diag7s -= 15199648745914368ULL;
  bottom_rows -= 597852850533433344ULL;
  disk_difference -= 8;
}

void Flip_B5D5_F5() {
  left_columns += 231931772982ULL;
  top_diag9s += 18ULL;
  top_diag7s += 393234ULL;
  right_columns += 231928233984ULL;
  bottom_diag9s += 231931786752ULL;
  bottom_diag7s += 15199648745914368ULL;
  bottom_rows += 597852850533433344ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5D5_F5;
  flipped = true;
}

void UndoFlip_B5D5_F5G5() {
  left_columns -= 231931772982ULL;
  top_diag9s -= 393234ULL;
  top_diag7s -= 393234ULL;
  right_columns -= 231931772928ULL;
  bottom_diag9s -= 231931786752ULL;
  bottom_diag7s -= 15199648745928192ULL;
  bottom_rows -= 599541700393697280ULL;
  disk_difference -= 10;
}

void Flip_B5D5_F5G5() {
  left_columns += 231931772982ULL;
  top_diag9s += 393234ULL;
  top_diag7s += 393234ULL;
  right_columns += 231931772928ULL;
  bottom_diag9s += 231931786752ULL;
  bottom_diag7s += 15199648745928192ULL;
  bottom_rows += 599541700393697280ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5D5_F5G5;
  flipped = true;
}

void UndoFlip_B5E5() {
  left_columns -= 231931772982ULL;
  top_diag7s -= 393234ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 15199880674162176ULL;
  bottom_diag7s -= 15199880670609408ULL;
  bottom_rows -= 607985949695016960ULL;
  disk_difference -= 8;
}

void Flip_B5E5() {
  left_columns += 231931772982ULL;
  top_diag7s += 393234ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 15199880674162176ULL;
  bottom_diag7s += 15199880670609408ULL;
  bottom_rows += 607985949695016960ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5E5;
  flipped = true;
}

void UndoFlip_B5E5_G5() {
  left_columns -= 231931772982ULL;
  top_diag9s -= 393216ULL;
  top_diag7s -= 393234ULL;
  right_columns -= 15199648745914368ULL;
  bottom_diag9s -= 15199880674162176ULL;
  bottom_diag7s -= 15199880670623232ULL;
  bottom_rows -= 609674799555280896ULL;
  disk_difference -= 10;
}

void Flip_B5E5_G5() {
  left_columns += 231931772982ULL;
  top_diag9s += 393216ULL;
  top_diag7s += 393234ULL;
  right_columns += 15199648745914368ULL;
  bottom_diag9s += 15199880674162176ULL;
  bottom_diag7s += 15199880670623232ULL;
  bottom_rows += 609674799555280896ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5E5_G5;
  flipped = true;
}

void UndoFlip_B5F5() {
  left_columns -= 231931772982ULL;
  top_diag9s -= 18ULL;
  top_diag7s -= 393234ULL;
  right_columns -= 15199880670609408ULL;
  bottom_diag9s -= 15199880674162176ULL;
  bottom_diag7s -= 15199880674148352ULL;
  bottom_rows -= 613052499275808768ULL;
  disk_difference -= 10;
}

void Flip_B5F5() {
  left_columns += 231931772982ULL;
  top_diag9s += 18ULL;
  top_diag7s += 393234ULL;
  right_columns += 15199880670609408ULL;
  bottom_diag9s += 15199880674162176ULL;
  bottom_diag7s += 15199880674148352ULL;
  bottom_rows += 613052499275808768ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5F5;
  flipped = true;
}

void UndoFlip_B5G5() {
  left_columns -= 231931772982ULL;
  top_diag9s -= 393234ULL;
  top_diag7s -= 393234ULL;
  right_columns -= 15199880674148352ULL;
  bottom_diag9s -= 15199880674162176ULL;
  bottom_diag7s -= 15199880674162176ULL;
  bottom_rows -= 614741349136072704ULL;
  disk_difference -= 12;
}

void Flip_B5G5() {
  left_columns += 231931772982ULL;
  top_diag9s += 393234ULL;
  top_diag7s += 393234ULL;
  right_columns += 15199880674148352ULL;
  bottom_diag9s += 15199880674162176ULL;
  bottom_diag7s += 15199880674162176ULL;
  bottom_rows += 614741349136072704ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5G5;
  flipped = true;
}

void UndoFlip_C5() {
  left_columns -= 3538944ULL;
  top_diag7s -= 18ULL;
  bottom_diag9s -= 3538944ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 2;
}

void Flip_C5() {
  left_columns += 3538944ULL;
  top_diag7s += 18ULL;
  bottom_diag9s += 3538944ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5;
  flipped = true;
}

void UndoFlip_C5_E5() {
  left_columns -= 3538944ULL;
  top_diag7s -= 18ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 15199648745914368ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 151996487423754240ULL;
  disk_difference -= 4;
}

void Flip_C5_E5() {
  left_columns += 3538944ULL;
  top_diag7s += 18ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 15199648745914368ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 151996487423754240ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5_E5;
  flipped = true;
}

void UndoFlip_C5_E5F5() {
  left_columns -= 3538944ULL;
  top_diag9s -= 18ULL;
  top_diag7s -= 18ULL;
  right_columns -= 15199880670609408ULL;
  bottom_diag9s -= 15199648745914368ULL;
  bottom_diag7s -= 231931772928ULL;
  bottom_rows -= 157063037004546048ULL;
  disk_difference -= 6;
}

void Flip_C5_E5F5() {
  left_columns += 3538944ULL;
  top_diag9s += 18ULL;
  top_diag7s += 18ULL;
  right_columns += 15199880670609408ULL;
  bottom_diag9s += 15199648745914368ULL;
  bottom_diag7s += 231931772928ULL;
  bottom_rows += 157063037004546048ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5_E5F5;
  flipped = true;
}

void UndoFlip_C5_E5G5() {
  left_columns -= 3538944ULL;
  top_diag9s -= 393234ULL;
  top_diag7s -= 18ULL;
  right_columns -= 15199880674148352ULL;
  bottom_diag9s -= 15199648745914368ULL;
  bottom_diag7s -= 231931786752ULL;
  bottom_rows -= 158751886864809984ULL;
  disk_difference -= 8;
}

void Flip_C5_E5G5() {
  left_columns += 3538944ULL;
  top_diag9s += 393234ULL;
  top_diag7s += 18ULL;
  right_columns += 15199880674148352ULL;
  bottom_diag9s += 15199648745914368ULL;
  bottom_diag7s += 231931786752ULL;
  bottom_rows += 158751886864809984ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5_E5G5;
  flipped = true;
}

void UndoFlip_C5D5() {
  left_columns -= 3538998ULL;
  top_diag7s -= 18ULL;
  bottom_diag9s -= 231931772928ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 182395784908505088ULL;
  disk_difference -= 4;
}

void Flip_C5D5() {
  left_columns += 3538998ULL;
  top_diag7s += 18ULL;
  bottom_diag9s += 231931772928ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 182395784908505088ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5D5;
  flipped = true;
}

void UndoFlip_C5D5_F5() {
  left_columns -= 3538998ULL;
  top_diag9s -= 18ULL;
  top_diag7s -= 18ULL;
  right_columns -= 231928233984ULL;
  bottom_diag9s -= 231931772928ULL;
  bottom_diag7s -= 15199648745914368ULL;
  bottom_rows -= 187462334489296896ULL;
  disk_difference -= 6;
}

void Flip_C5D5_F5() {
  left_columns += 3538998ULL;
  top_diag9s += 18ULL;
  top_diag7s += 18ULL;
  right_columns += 231928233984ULL;
  bottom_diag9s += 231931772928ULL;
  bottom_diag7s += 15199648745914368ULL;
  bottom_rows += 187462334489296896ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5D5_F5;
  flipped = true;
}

void UndoFlip_C5D5_F5G5() {
  left_columns -= 3538998ULL;
  top_diag9s -= 393234ULL;
  top_diag7s -= 18ULL;
  right_columns -= 231931772928ULL;
  bottom_diag9s -= 231931772928ULL;
  bottom_diag7s -= 15199648745928192ULL;
  bottom_rows -= 189151184349560832ULL;
  disk_difference -= 8;
}

void Flip_C5D5_F5G5() {
  left_columns += 3538998ULL;
  top_diag9s += 393234ULL;
  top_diag7s += 18ULL;
  right_columns += 231931772928ULL;
  bottom_diag9s += 231931772928ULL;
  bottom_diag7s += 15199648745928192ULL;
  bottom_rows += 189151184349560832ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5D5_F5G5;
  flipped = true;
}

void UndoFlip_C5E5() {
  left_columns -= 3538998ULL;
  top_diag7s -= 18ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 15199880674148352ULL;
  bottom_diag7s -= 15199880670609408ULL;
  bottom_rows -= 197595433650880512ULL;
  disk_difference -= 6;
}

void Flip_C5E5() {
  left_columns += 3538998ULL;
  top_diag7s += 18ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 15199880674148352ULL;
  bottom_diag7s += 15199880670609408ULL;
  bottom_rows += 197595433650880512ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5E5;
  flipped = true;
}

void UndoFlip_C5E5_G5() {
  left_columns -= 3538998ULL;
  top_diag9s -= 393216ULL;
  top_diag7s -= 18ULL;
  right_columns -= 15199648745914368ULL;
  bottom_diag9s -= 15199880674148352ULL;
  bottom_diag7s -= 15199880670623232ULL;
  bottom_rows -= 199284283511144448ULL;
  disk_difference -= 8;
}

void Flip_C5E5_G5() {
  left_columns += 3538998ULL;
  top_diag9s += 393216ULL;
  top_diag7s += 18ULL;
  right_columns += 15199648745914368ULL;
  bottom_diag9s += 15199880674148352ULL;
  bottom_diag7s += 15199880670623232ULL;
  bottom_rows += 199284283511144448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5E5_G5;
  flipped = true;
}

void UndoFlip_C5F5() {
  left_columns -= 3538998ULL;
  top_diag9s -= 18ULL;
  top_diag7s -= 18ULL;
  right_columns -= 15199880670609408ULL;
  bottom_diag9s -= 15199880674148352ULL;
  bottom_diag7s -= 15199880674148352ULL;
  bottom_rows -= 202661983231672320ULL;
  disk_difference -= 8;
}

void Flip_C5F5() {
  left_columns += 3538998ULL;
  top_diag9s += 18ULL;
  top_diag7s += 18ULL;
  right_columns += 15199880670609408ULL;
  bottom_diag9s += 15199880674148352ULL;
  bottom_diag7s += 15199880674148352ULL;
  bottom_rows += 202661983231672320ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5F5;
  flipped = true;
}

void UndoFlip_C5G5() {
  left_columns -= 3538998ULL;
  top_diag9s -= 393234ULL;
  top_diag7s -= 18ULL;
  right_columns -= 15199880674148352ULL;
  bottom_diag9s -= 15199880674148352ULL;
  bottom_diag7s -= 15199880674162176ULL;
  bottom_rows -= 204350833091936256ULL;
  disk_difference -= 10;
}

void Flip_C5G5() {
  left_columns += 3538998ULL;
  top_diag9s += 393234ULL;
  top_diag7s += 18ULL;
  right_columns += 15199880674148352ULL;
  bottom_diag9s += 15199880674148352ULL;
  bottom_diag7s += 15199880674162176ULL;
  bottom_rows += 204350833091936256ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5G5;
  flipped = true;
}

void UndoFlip_D5() {
  left_columns -= 54ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 2;
}

void Flip_D5() {
  left_columns += 54ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5;
  flipped = true;
}

void UndoFlip_D5_F5() {
  left_columns -= 54ULL;
  top_diag9s -= 18ULL;
  right_columns -= 231928233984ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 15199648745914368ULL;
  bottom_rows -= 50665495807918080ULL;
  disk_difference -= 4;
}

void Flip_D5_F5() {
  left_columns += 54ULL;
  top_diag9s += 18ULL;
  right_columns += 231928233984ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 15199648745914368ULL;
  bottom_rows += 50665495807918080ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5_F5;
  flipped = true;
}

void UndoFlip_D5_F5G5() {
  left_columns -= 54ULL;
  top_diag9s -= 393234ULL;
  right_columns -= 231931772928ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 15199648745928192ULL;
  bottom_rows -= 52354345668182016ULL;
  disk_difference -= 6;
}

void Flip_D5_F5G5() {
  left_columns += 54ULL;
  top_diag9s += 393234ULL;
  right_columns += 231931772928ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 15199648745928192ULL;
  bottom_rows += 52354345668182016ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5_F5G5;
  flipped = true;
}

void UndoFlip_D5E5() {
  left_columns -= 54ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 15199880670609408ULL;
  bottom_diag7s -= 15199880670609408ULL;
  bottom_rows -= 60798594969501696ULL;
  disk_difference -= 4;
}

void Flip_D5E5() {
  left_columns += 54ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 15199880670609408ULL;
  bottom_diag7s += 15199880670609408ULL;
  bottom_rows += 60798594969501696ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5E5;
  flipped = true;
}

void UndoFlip_D5E5_G5() {
  left_columns -= 54ULL;
  top_diag9s -= 393216ULL;
  right_columns -= 15199648745914368ULL;
  bottom_diag9s -= 15199880670609408ULL;
  bottom_diag7s -= 15199880670623232ULL;
  bottom_rows -= 62487444829765632ULL;
  disk_difference -= 6;
}

void Flip_D5E5_G5() {
  left_columns += 54ULL;
  top_diag9s += 393216ULL;
  right_columns += 15199648745914368ULL;
  bottom_diag9s += 15199880670609408ULL;
  bottom_diag7s += 15199880670623232ULL;
  bottom_rows += 62487444829765632ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5E5_G5;
  flipped = true;
}

void UndoFlip_D5F5() {
  left_columns -= 54ULL;
  top_diag9s -= 18ULL;
  right_columns -= 15199880670609408ULL;
  bottom_diag9s -= 15199880670609408ULL;
  bottom_diag7s -= 15199880674148352ULL;
  bottom_rows -= 65865144550293504ULL;
  disk_difference -= 6;
}

void Flip_D5F5() {
  left_columns += 54ULL;
  top_diag9s += 18ULL;
  right_columns += 15199880670609408ULL;
  bottom_diag9s += 15199880670609408ULL;
  bottom_diag7s += 15199880674148352ULL;
  bottom_rows += 65865144550293504ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5F5;
  flipped = true;
}

void UndoFlip_D5G5() {
  left_columns -= 54ULL;
  top_diag9s -= 393234ULL;
  right_columns -= 15199880674148352ULL;
  bottom_diag9s -= 15199880670609408ULL;
  bottom_diag7s -= 15199880674162176ULL;
  bottom_rows -= 67553994410557440ULL;
  disk_difference -= 8;
}

void Flip_D5G5() {
  left_columns += 54ULL;
  top_diag9s += 393234ULL;
  right_columns += 15199880674148352ULL;
  bottom_diag9s += 15199880670609408ULL;
  bottom_diag7s += 15199880674162176ULL;
  bottom_rows += 67553994410557440ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5G5;
  flipped = true;
}

void UndoFlip_E5() {
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 2;
}

void Flip_E5() {
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5;
  flipped = true;
}

void UndoFlip_E5_G5() {
  top_diag9s -= 393216ULL;
  right_columns -= 15199648745914368ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 231928247808ULL;
  bottom_rows -= 16888498602639360ULL;
  disk_difference -= 4;
}

void Flip_E5_G5() {
  top_diag9s += 393216ULL;
  right_columns += 15199648745914368ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 231928247808ULL;
  bottom_rows += 16888498602639360ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5_G5;
  flipped = true;
}

void UndoFlip_E5F5() {
  top_diag9s -= 18ULL;
  right_columns -= 15199880670609408ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 231931772928ULL;
  bottom_rows -= 20266198323167232ULL;
  disk_difference -= 4;
}

void Flip_E5F5() {
  top_diag9s += 18ULL;
  right_columns += 15199880670609408ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 231931772928ULL;
  bottom_rows += 20266198323167232ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5F5;
  flipped = true;
}

void UndoFlip_E5G5() {
  top_diag9s -= 393234ULL;
  right_columns -= 15199880674148352ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 231931786752ULL;
  bottom_rows -= 21955048183431168ULL;
  disk_difference -= 6;
}

void Flip_E5G5() {
  top_diag9s += 393234ULL;
  right_columns += 15199880674148352ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 231931786752ULL;
  bottom_rows += 21955048183431168ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5G5;
  flipped = true;
}

void UndoFlip_F5() {
  top_diag9s -= 18ULL;
  right_columns -= 231928233984ULL;
  bottom_diag7s -= 3538944ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 2;
}

void Flip_F5() {
  top_diag9s += 18ULL;
  right_columns += 231928233984ULL;
  bottom_diag7s += 3538944ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5;
  flipped = true;
}

void UndoFlip_F5G5() {
  top_diag9s -= 393234ULL;
  right_columns -= 231931772928ULL;
  bottom_diag7s -= 3552768ULL;
  bottom_rows -= 6755399441055744ULL;
  disk_difference -= 4;
}

void Flip_F5G5() {
  top_diag9s += 393234ULL;
  right_columns += 231931772928ULL;
  bottom_diag7s += 3552768ULL;
  bottom_rows += 6755399441055744ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5G5;
  flipped = true;
}

void UndoFlip_G5() {
  top_diag9s -= 393216ULL;
  right_columns -= 3538944ULL;
  bottom_diag7s -= 13824ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 2;
}

void Flip_G5() {
  top_diag9s += 393216ULL;
  right_columns += 3538944ULL;
  bottom_diag7s += 13824ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5;
  flipped = true;
}

void UndoFlip_B6() {
  left_columns -= 77309411328ULL;
  top_diag7s -= 6ULL;
  bottom_diag9s -= 18ULL;
  bottom_rows -= 6262062317568ULL;
  disk_difference -= 2;
}

void Flip_B6() {
  left_columns += 77309411328ULL;
  top_diag7s += 6ULL;
  bottom_diag9s += 18ULL;
  bottom_rows += 6262062317568ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6;
  flipped = true;
}

void UndoFlip_B6_D6() {
  left_columns -= 77309411346ULL;
  top_diag7s -= 6ULL;
  bottom_diag9s -= 1179666ULL;
  bottom_diag7s -= 77309411328ULL;
  bottom_rows -= 6957847019520ULL;
  disk_difference -= 4;
}

void Flip_B6_D6() {
  left_columns += 77309411346ULL;
  top_diag7s += 6ULL;
  bottom_diag9s += 1179666ULL;
  bottom_diag7s += 77309411328ULL;
  bottom_rows += 6957847019520ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6_D6;
  flipped = true;
}

void UndoFlip_B6_D6E6() {
  left_columns -= 77309411346ULL;
  top_diag7s -= 6ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 77310590994ULL;
  bottom_diag7s -= 77310590976ULL;
  bottom_rows -= 7189775253504ULL;
  disk_difference -= 6;
}

void Flip_B6_D6E6() {
  left_columns += 77309411346ULL;
  top_diag7s += 6ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 77310590994ULL;
  bottom_diag7s += 77310590976ULL;
  bottom_rows += 7189775253504ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6_D6E6;
  flipped = true;
}

void UndoFlip_B6_D6F6() {
  left_columns -= 77309411346ULL;
  top_diag7s -= 6ULL;
  right_columns -= 5066626890203136ULL;
  bottom_diag9s -= 5066626891382802ULL;
  bottom_diag7s -= 77310595584ULL;
  bottom_rows -= 7267084664832ULL;
  disk_difference -= 8;
}

void Flip_B6_D6F6() {
  left_columns += 77309411346ULL;
  top_diag7s += 6ULL;
  right_columns += 5066626890203136ULL;
  bottom_diag9s += 5066626891382802ULL;
  bottom_diag7s += 77310595584ULL;
  bottom_rows += 7267084664832ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6_D6F6;
  flipped = true;
}

void UndoFlip_B6_D6G6() {
  left_columns -= 77309411346ULL;
  top_diag9s -= 6ULL;
  top_diag7s -= 6ULL;
  right_columns -= 5066626891382784ULL;
  bottom_diag9s -= 5066626891382802ULL;
  bottom_diag7s -= 77310595602ULL;
  bottom_rows -= 7292854468608ULL;
  disk_difference -= 10;
}

void Flip_B6_D6G6() {
  left_columns += 77309411346ULL;
  top_diag9s += 6ULL;
  top_diag7s += 6ULL;
  right_columns += 5066626891382784ULL;
  bottom_diag9s += 5066626891382802ULL;
  bottom_diag7s += 77310595602ULL;
  bottom_rows += 7292854468608ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6_D6G6;
  flipped = true;
}

void UndoFlip_B6C6() {
  left_columns -= 77310590976ULL;
  top_diag7s -= 6ULL;
  bottom_diag9s -= 4626ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 8349416423424ULL;
  disk_difference -= 4;
}

void Flip_B6C6() {
  left_columns += 77310590976ULL;
  top_diag7s += 6ULL;
  bottom_diag9s += 4626ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 8349416423424ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6C6;
  flipped = true;
}

void UndoFlip_B6C6_E6() {
  left_columns -= 77310590976ULL;
  top_diag7s -= 6ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 77309415954ULL;
  bottom_diag7s -= 5066549581971456ULL;
  bottom_rows -= 8581344657408ULL;
  disk_difference -= 6;
}

void Flip_B6C6_E6() {
  left_columns += 77310590976ULL;
  top_diag7s += 6ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 77309415954ULL;
  bottom_diag7s += 5066549581971456ULL;
  bottom_rows += 8581344657408ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6C6_E6;
  flipped = true;
}

void UndoFlip_B6C6_E6F6() {
  left_columns -= 77310590976ULL;
  top_diag7s -= 6ULL;
  right_columns -= 5066626890203136ULL;
  bottom_diag9s -= 5066626890207762ULL;
  bottom_diag7s -= 5066549581976064ULL;
  bottom_rows -= 8658654068736ULL;
  disk_difference -= 8;
}

void Flip_B6C6_E6F6() {
  left_columns += 77310590976ULL;
  top_diag7s += 6ULL;
  right_columns += 5066626890203136ULL;
  bottom_diag9s += 5066626890207762ULL;
  bottom_diag7s += 5066549581976064ULL;
  bottom_rows += 8658654068736ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6C6_E6F6;
  flipped = true;
}

void UndoFlip_B6C6_E6G6() {
  left_columns -= 77310590976ULL;
  top_diag9s -= 6ULL;
  top_diag7s -= 6ULL;
  right_columns -= 5066626891382784ULL;
  bottom_diag9s -= 5066626890207762ULL;
  bottom_diag7s -= 5066549581976082ULL;
  bottom_rows -= 8684423872512ULL;
  disk_difference -= 10;
}

void Flip_B6C6_E6G6() {
  left_columns += 77310590976ULL;
  top_diag9s += 6ULL;
  top_diag7s += 6ULL;
  right_columns += 5066626891382784ULL;
  bottom_diag9s += 5066626890207762ULL;
  bottom_diag7s += 5066549581976082ULL;
  bottom_rows += 8684423872512ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6C6_E6G6;
  flipped = true;
}

void UndoFlip_B6D6() {
  left_columns -= 77310590994ULL;
  top_diag7s -= 6ULL;
  bottom_diag9s -= 1184274ULL;
  bottom_diag7s -= 5066626890203136ULL;
  bottom_rows -= 9045201125376ULL;
  disk_difference -= 6;
}

void Flip_B6D6() {
  left_columns += 77310590994ULL;
  top_diag7s += 6ULL;
  bottom_diag9s += 1184274ULL;
  bottom_diag7s += 5066626890203136ULL;
  bottom_rows += 9045201125376ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6D6;
  flipped = true;
}

void UndoFlip_B6D6_F6() {
  left_columns -= 77310590994ULL;
  top_diag7s -= 6ULL;
  right_columns -= 77309411328ULL;
  bottom_diag9s -= 5066549581976082ULL;
  bottom_diag7s -= 5066626890207744ULL;
  bottom_rows -= 9122510536704ULL;
  disk_difference -= 8;
}

void Flip_B6D6_F6() {
  left_columns += 77310590994ULL;
  top_diag7s += 6ULL;
  right_columns += 77309411328ULL;
  bottom_diag9s += 5066549581976082ULL;
  bottom_diag7s += 5066626890207744ULL;
  bottom_rows += 9122510536704ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6D6_F6;
  flipped = true;
}

void UndoFlip_B6D6_F6G6() {
  left_columns -= 77310590994ULL;
  top_diag9s -= 6ULL;
  top_diag7s -= 6ULL;
  right_columns -= 77310590976ULL;
  bottom_diag9s -= 5066549581976082ULL;
  bottom_diag7s -= 5066626890207762ULL;
  bottom_rows -= 9148280340480ULL;
  disk_difference -= 10;
}

void Flip_B6D6_F6G6() {
  left_columns += 77310590994ULL;
  top_diag9s += 6ULL;
  top_diag7s += 6ULL;
  right_columns += 77310590976ULL;
  bottom_diag9s += 5066549581976082ULL;
  bottom_diag7s += 5066626890207762ULL;
  bottom_rows += 9148280340480ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6D6_F6G6;
  flipped = true;
}

void UndoFlip_B6E6() {
  left_columns -= 77310590994ULL;
  top_diag7s -= 6ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 77310595602ULL;
  bottom_diag7s -= 5066626891382784ULL;
  bottom_rows -= 9277129359360ULL;
  disk_difference -= 8;
}

void Flip_B6E6() {
  left_columns += 77310590994ULL;
  top_diag7s += 6ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 77310595602ULL;
  bottom_diag7s += 5066626891382784ULL;
  bottom_rows += 9277129359360ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6E6;
  flipped = true;
}

void UndoFlip_B6E6_G6() {
  left_columns -= 77310590994ULL;
  top_diag9s -= 6ULL;
  top_diag7s -= 6ULL;
  right_columns -= 5066549581971456ULL;
  bottom_diag9s -= 77310595602ULL;
  bottom_diag7s -= 5066626891382802ULL;
  bottom_rows -= 9302899163136ULL;
  disk_difference -= 10;
}

void Flip_B6E6_G6() {
  left_columns += 77310590994ULL;
  top_diag9s += 6ULL;
  top_diag7s += 6ULL;
  right_columns += 5066549581971456ULL;
  bottom_diag9s += 77310595602ULL;
  bottom_diag7s += 5066626891382802ULL;
  bottom_rows += 9302899163136ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6E6_G6;
  flipped = true;
}

void UndoFlip_B6F6() {
  left_columns -= 77310590994ULL;
  top_diag7s -= 6ULL;
  right_columns -= 5066626890203136ULL;
  bottom_diag9s -= 5066626891387410ULL;
  bottom_diag7s -= 5066626891387392ULL;
  bottom_rows -= 9354438770688ULL;
  disk_difference -= 10;
}

void Flip_B6F6() {
  left_columns += 77310590994ULL;
  top_diag7s += 6ULL;
  right_columns += 5066626890203136ULL;
  bottom_diag9s += 5066626891387410ULL;
  bottom_diag7s += 5066626891387392ULL;
  bottom_rows += 9354438770688ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6F6;
  flipped = true;
}

void UndoFlip_B6G6() {
  left_columns -= 77310590994ULL;
  top_diag9s -= 6ULL;
  top_diag7s -= 6ULL;
  right_columns -= 5066626891382784ULL;
  bottom_diag9s -= 5066626891387410ULL;
  bottom_diag7s -= 5066626891387410ULL;
  bottom_rows -= 9380208574464ULL;
  disk_difference -= 12;
}

void Flip_B6G6() {
  left_columns += 77310590994ULL;
  top_diag9s += 6ULL;
  top_diag7s += 6ULL;
  right_columns += 5066626891382784ULL;
  bottom_diag9s += 5066626891387410ULL;
  bottom_diag7s += 5066626891387410ULL;
  bottom_rows += 9380208574464ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6G6;
  flipped = true;
}

void UndoFlip_C6() {
  left_columns -= 1179648ULL;
  bottom_diag9s -= 4608ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 2087354105856ULL;
  disk_difference -= 2;
}

void Flip_C6() {
  left_columns += 1179648ULL;
  bottom_diag9s += 4608ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 2087354105856ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6;
  flipped = true;
}

void UndoFlip_C6_E6() {
  left_columns -= 1179648ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 77309415936ULL;
  bottom_diag7s -= 5066549581971456ULL;
  bottom_rows -= 2319282339840ULL;
  disk_difference -= 4;
}

void Flip_C6_E6() {
  left_columns += 1179648ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 77309415936ULL;
  bottom_diag7s += 5066549581971456ULL;
  bottom_rows += 2319282339840ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6_E6;
  flipped = true;
}

void UndoFlip_C6_E6F6() {
  left_columns -= 1179648ULL;
  right_columns -= 5066626890203136ULL;
  bottom_diag9s -= 5066626890207744ULL;
  bottom_diag7s -= 5066549581976064ULL;
  bottom_rows -= 2396591751168ULL;
  disk_difference -= 6;
}

void Flip_C6_E6F6() {
  left_columns += 1179648ULL;
  right_columns += 5066626890203136ULL;
  bottom_diag9s += 5066626890207744ULL;
  bottom_diag7s += 5066549581976064ULL;
  bottom_rows += 2396591751168ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6_E6F6;
  flipped = true;
}

void UndoFlip_C6_E6G6() {
  left_columns -= 1179648ULL;
  top_diag9s -= 6ULL;
  right_columns -= 5066626891382784ULL;
  bottom_diag9s -= 5066626890207744ULL;
  bottom_diag7s -= 5066549581976082ULL;
  bottom_rows -= 2422361554944ULL;
  disk_difference -= 8;
}

void Flip_C6_E6G6() {
  left_columns += 1179648ULL;
  top_diag9s += 6ULL;
  right_columns += 5066626891382784ULL;
  bottom_diag9s += 5066626890207744ULL;
  bottom_diag7s += 5066549581976082ULL;
  bottom_rows += 2422361554944ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6_E6G6;
  flipped = true;
}

void UndoFlip_C6D6() {
  left_columns -= 1179666ULL;
  bottom_diag9s -= 1184256ULL;
  bottom_diag7s -= 5066626890203136ULL;
  bottom_rows -= 2783138807808ULL;
  disk_difference -= 4;
}

void Flip_C6D6() {
  left_columns += 1179666ULL;
  bottom_diag9s += 1184256ULL;
  bottom_diag7s += 5066626890203136ULL;
  bottom_rows += 2783138807808ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6D6;
  flipped = true;
}

void UndoFlip_C6D6_F6() {
  left_columns -= 1179666ULL;
  right_columns -= 77309411328ULL;
  bottom_diag9s -= 5066549581976064ULL;
  bottom_diag7s -= 5066626890207744ULL;
  bottom_rows -= 2860448219136ULL;
  disk_difference -= 6;
}

void Flip_C6D6_F6() {
  left_columns += 1179666ULL;
  right_columns += 77309411328ULL;
  bottom_diag9s += 5066549581976064ULL;
  bottom_diag7s += 5066626890207744ULL;
  bottom_rows += 2860448219136ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6D6_F6;
  flipped = true;
}

void UndoFlip_C6D6_F6G6() {
  left_columns -= 1179666ULL;
  top_diag9s -= 6ULL;
  right_columns -= 77310590976ULL;
  bottom_diag9s -= 5066549581976064ULL;
  bottom_diag7s -= 5066626890207762ULL;
  bottom_rows -= 2886218022912ULL;
  disk_difference -= 8;
}

void Flip_C6D6_F6G6() {
  left_columns += 1179666ULL;
  top_diag9s += 6ULL;
  right_columns += 77310590976ULL;
  bottom_diag9s += 5066549581976064ULL;
  bottom_diag7s += 5066626890207762ULL;
  bottom_rows += 2886218022912ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6D6_F6G6;
  flipped = true;
}

void UndoFlip_C6E6() {
  left_columns -= 1179666ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 77310595584ULL;
  bottom_diag7s -= 5066626891382784ULL;
  bottom_rows -= 3015067041792ULL;
  disk_difference -= 6;
}

void Flip_C6E6() {
  left_columns += 1179666ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 77310595584ULL;
  bottom_diag7s += 5066626891382784ULL;
  bottom_rows += 3015067041792ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6E6;
  flipped = true;
}

void UndoFlip_C6E6_G6() {
  left_columns -= 1179666ULL;
  top_diag9s -= 6ULL;
  right_columns -= 5066549581971456ULL;
  bottom_diag9s -= 77310595584ULL;
  bottom_diag7s -= 5066626891382802ULL;
  bottom_rows -= 3040836845568ULL;
  disk_difference -= 8;
}

void Flip_C6E6_G6() {
  left_columns += 1179666ULL;
  top_diag9s += 6ULL;
  right_columns += 5066549581971456ULL;
  bottom_diag9s += 77310595584ULL;
  bottom_diag7s += 5066626891382802ULL;
  bottom_rows += 3040836845568ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6E6_G6;
  flipped = true;
}

void UndoFlip_C6F6() {
  left_columns -= 1179666ULL;
  right_columns -= 5066626890203136ULL;
  bottom_diag9s -= 5066626891387392ULL;
  bottom_diag7s -= 5066626891387392ULL;
  bottom_rows -= 3092376453120ULL;
  disk_difference -= 8;
}

void Flip_C6F6() {
  left_columns += 1179666ULL;
  right_columns += 5066626890203136ULL;
  bottom_diag9s += 5066626891387392ULL;
  bottom_diag7s += 5066626891387392ULL;
  bottom_rows += 3092376453120ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6F6;
  flipped = true;
}

void UndoFlip_C6G6() {
  left_columns -= 1179666ULL;
  top_diag9s -= 6ULL;
  right_columns -= 5066626891382784ULL;
  bottom_diag9s -= 5066626891387392ULL;
  bottom_diag7s -= 5066626891387410ULL;
  bottom_rows -= 3118146256896ULL;
  disk_difference -= 10;
}

void Flip_C6G6() {
  left_columns += 1179666ULL;
  top_diag9s += 6ULL;
  right_columns += 5066626891382784ULL;
  bottom_diag9s += 5066626891387392ULL;
  bottom_diag7s += 5066626891387410ULL;
  bottom_rows += 3118146256896ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6G6;
  flipped = true;
}

void UndoFlip_D6() {
  left_columns -= 18ULL;
  bottom_diag9s -= 1179648ULL;
  bottom_diag7s -= 77309411328ULL;
  bottom_rows -= 695784701952ULL;
  disk_difference -= 2;
}

void Flip_D6() {
  left_columns += 18ULL;
  bottom_diag9s += 1179648ULL;
  bottom_diag7s += 77309411328ULL;
  bottom_rows += 695784701952ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6;
  flipped = true;
}

void UndoFlip_D6_F6() {
  left_columns -= 18ULL;
  right_columns -= 77309411328ULL;
  bottom_diag9s -= 5066549581971456ULL;
  bottom_diag7s -= 77309415936ULL;
  bottom_rows -= 773094113280ULL;
  disk_difference -= 4;
}

void Flip_D6_F6() {
  left_columns += 18ULL;
  right_columns += 77309411328ULL;
  bottom_diag9s += 5066549581971456ULL;
  bottom_diag7s += 77309415936ULL;
  bottom_rows += 773094113280ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6_F6;
  flipped = true;
}

void UndoFlip_D6_F6G6() {
  left_columns -= 18ULL;
  top_diag9s -= 6ULL;
  right_columns -= 77310590976ULL;
  bottom_diag9s -= 5066549581971456ULL;
  bottom_diag7s -= 77309415954ULL;
  bottom_rows -= 798863917056ULL;
  disk_difference -= 6;
}

void Flip_D6_F6G6() {
  left_columns += 18ULL;
  top_diag9s += 6ULL;
  right_columns += 77310590976ULL;
  bottom_diag9s += 5066549581971456ULL;
  bottom_diag7s += 77309415954ULL;
  bottom_rows += 798863917056ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6_F6G6;
  flipped = true;
}

void UndoFlip_D6E6() {
  left_columns -= 18ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 77310590976ULL;
  bottom_diag7s -= 77310590976ULL;
  bottom_rows -= 927712935936ULL;
  disk_difference -= 4;
}

void Flip_D6E6() {
  left_columns += 18ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 77310590976ULL;
  bottom_diag7s += 77310590976ULL;
  bottom_rows += 927712935936ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6E6;
  flipped = true;
}

void UndoFlip_D6E6_G6() {
  left_columns -= 18ULL;
  top_diag9s -= 6ULL;
  right_columns -= 5066549581971456ULL;
  bottom_diag9s -= 77310590976ULL;
  bottom_diag7s -= 77310590994ULL;
  bottom_rows -= 953482739712ULL;
  disk_difference -= 6;
}

void Flip_D6E6_G6() {
  left_columns += 18ULL;
  top_diag9s += 6ULL;
  right_columns += 5066549581971456ULL;
  bottom_diag9s += 77310590976ULL;
  bottom_diag7s += 77310590994ULL;
  bottom_rows += 953482739712ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6E6_G6;
  flipped = true;
}

void UndoFlip_D6F6() {
  left_columns -= 18ULL;
  right_columns -= 5066626890203136ULL;
  bottom_diag9s -= 5066626891382784ULL;
  bottom_diag7s -= 77310595584ULL;
  bottom_rows -= 1005022347264ULL;
  disk_difference -= 6;
}

void Flip_D6F6() {
  left_columns += 18ULL;
  right_columns += 5066626890203136ULL;
  bottom_diag9s += 5066626891382784ULL;
  bottom_diag7s += 77310595584ULL;
  bottom_rows += 1005022347264ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6F6;
  flipped = true;
}

void UndoFlip_D6G6() {
  left_columns -= 18ULL;
  top_diag9s -= 6ULL;
  right_columns -= 5066626891382784ULL;
  bottom_diag9s -= 5066626891382784ULL;
  bottom_diag7s -= 77310595602ULL;
  bottom_rows -= 1030792151040ULL;
  disk_difference -= 8;
}

void Flip_D6G6() {
  left_columns += 18ULL;
  top_diag9s += 6ULL;
  right_columns += 5066626891382784ULL;
  bottom_diag9s += 5066626891382784ULL;
  bottom_diag7s += 77310595602ULL;
  bottom_rows += 1030792151040ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6G6;
  flipped = true;
}

void UndoFlip_E6() {
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 77309411328ULL;
  bottom_diag7s -= 1179648ULL;
  bottom_rows -= 231928233984ULL;
  disk_difference -= 2;
}

void Flip_E6() {
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 77309411328ULL;
  bottom_diag7s += 1179648ULL;
  bottom_rows += 231928233984ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6;
  flipped = true;
}

void UndoFlip_E6_G6() {
  top_diag9s -= 6ULL;
  right_columns -= 5066549581971456ULL;
  bottom_diag9s -= 77309411328ULL;
  bottom_diag7s -= 1179666ULL;
  bottom_rows -= 257698037760ULL;
  disk_difference -= 4;
}

void Flip_E6_G6() {
  top_diag9s += 6ULL;
  right_columns += 5066549581971456ULL;
  bottom_diag9s += 77309411328ULL;
  bottom_diag7s += 1179666ULL;
  bottom_rows += 257698037760ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6_G6;
  flipped = true;
}

void UndoFlip_E6F6() {
  right_columns -= 5066626890203136ULL;
  bottom_diag9s -= 5066626890203136ULL;
  bottom_diag7s -= 1184256ULL;
  bottom_rows -= 309237645312ULL;
  disk_difference -= 4;
}

void Flip_E6F6() {
  right_columns += 5066626890203136ULL;
  bottom_diag9s += 5066626890203136ULL;
  bottom_diag7s += 1184256ULL;
  bottom_rows += 309237645312ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6F6;
  flipped = true;
}

void UndoFlip_E6G6() {
  top_diag9s -= 6ULL;
  right_columns -= 5066626891382784ULL;
  bottom_diag9s -= 5066626890203136ULL;
  bottom_diag7s -= 1184274ULL;
  bottom_rows -= 335007449088ULL;
  disk_difference -= 6;
}

void Flip_E6G6() {
  top_diag9s += 6ULL;
  right_columns += 5066626891382784ULL;
  bottom_diag9s += 5066626890203136ULL;
  bottom_diag7s += 1184274ULL;
  bottom_rows += 335007449088ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6G6;
  flipped = true;
}

void UndoFlip_F6() {
  right_columns -= 77309411328ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 4608ULL;
  bottom_rows -= 77309411328ULL;
  disk_difference -= 2;
}

void Flip_F6() {
  right_columns += 77309411328ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 4608ULL;
  bottom_rows += 77309411328ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F6;
  flipped = true;
}

void UndoFlip_F6G6() {
  top_diag9s -= 6ULL;
  right_columns -= 77310590976ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 4626ULL;
  bottom_rows -= 103079215104ULL;
  disk_difference -= 4;
}

void Flip_F6G6() {
  top_diag9s += 6ULL;
  right_columns += 77310590976ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 4626ULL;
  bottom_rows += 103079215104ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F6G6;
  flipped = true;
}

void UndoFlip_G6() {
  top_diag9s -= 6ULL;
  right_columns -= 1179648ULL;
  bottom_diag7s -= 18ULL;
  bottom_rows -= 25769803776ULL;
  disk_difference -= 2;
}

void Flip_G6() {
  top_diag9s += 6ULL;
  right_columns += 1179648ULL;
  bottom_diag7s += 18ULL;
  bottom_rows += 25769803776ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G6;
  flipped = true;
}

void UndoFlip_B7() {
  left_columns -= 25769803776ULL;
  top_diag9s -= 25769803776ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 95551488ULL;
  disk_difference -= 2;
}

void Flip_B7() {
  left_columns += 25769803776ULL;
  top_diag9s += 25769803776ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 95551488ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7;
  flipped = true;
}

void UndoFlip_B7_D7() {
  left_columns -= 25769803782ULL;
  top_diag9s -= 25769803776ULL;
  bottom_diag9s -= 1536ULL;
  bottom_diag7s -= 1688849860657152ULL;
  bottom_rows -= 106168320ULL;
  disk_difference -= 4;
}

void Flip_B7_D7() {
  left_columns += 25769803782ULL;
  top_diag9s += 25769803776ULL;
  bottom_diag9s += 1536ULL;
  bottom_diag7s += 1688849860657152ULL;
  bottom_rows += 106168320ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7_D7;
  flipped = true;
}

void UndoFlip_B7_D7E7() {
  left_columns -= 25769803782ULL;
  top_diag9s -= 25769803776ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 394752ULL;
  bottom_diag7s -= 1688849860658688ULL;
  bottom_rows -= 109707264ULL;
  disk_difference -= 6;
}

void Flip_B7_D7E7() {
  left_columns += 25769803782ULL;
  top_diag9s += 25769803776ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 394752ULL;
  bottom_diag7s += 1688849860658688ULL;
  bottom_rows += 109707264ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7_D7E7;
  flipped = true;
}

void UndoFlip_B7_D7F7() {
  left_columns -= 25769803782ULL;
  top_diag9s -= 25769803776ULL;
  right_columns -= 1688875630067712ULL;
  bottom_diag9s -= 25770198528ULL;
  bottom_diag7s -= 1688849860658694ULL;
  bottom_rows -= 110886912ULL;
  disk_difference -= 8;
}

void Flip_B7_D7F7() {
  left_columns += 25769803782ULL;
  top_diag9s += 25769803776ULL;
  right_columns += 1688875630067712ULL;
  bottom_diag9s += 25770198528ULL;
  bottom_diag7s += 1688849860658694ULL;
  bottom_rows += 110886912ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7_D7F7;
  flipped = true;
}

void UndoFlip_B7_D7G7() {
  left_columns -= 25769803782ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688875630460928ULL;
  bottom_diag9s -= 1688875630462464ULL;
  bottom_diag7s -= 1688849860658694ULL;
  bottom_rows -= 111280128ULL;
  disk_difference -= 10;
}

void Flip_B7_D7G7() {
  left_columns += 25769803782ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 1688875630460928ULL;
  bottom_diag9s += 1688875630462464ULL;
  bottom_diag7s += 1688849860658694ULL;
  bottom_rows += 111280128ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7_D7G7;
  flipped = true;
}

void UndoFlip_B7C7() {
  left_columns -= 25770196992ULL;
  top_diag9s -= 25769803776ULL;
  bottom_diag9s -= 6ULL;
  bottom_diag7s -= 1688875630067712ULL;
  bottom_rows -= 127401984ULL;
  disk_difference -= 4;
}

void Flip_B7C7() {
  left_columns += 25770196992ULL;
  top_diag9s += 25769803776ULL;
  bottom_diag9s += 6ULL;
  bottom_diag7s += 1688875630067712ULL;
  bottom_rows += 127401984ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7C7;
  flipped = true;
}

void UndoFlip_B7C7_E7() {
  left_columns -= 25770196992ULL;
  top_diag9s -= 25769803776ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 393222ULL;
  bottom_diag7s -= 1688875630069248ULL;
  bottom_rows -= 130940928ULL;
  disk_difference -= 6;
}

void Flip_B7C7_E7() {
  left_columns += 25770196992ULL;
  top_diag9s += 25769803776ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 393222ULL;
  bottom_diag7s += 1688875630069248ULL;
  bottom_rows += 130940928ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7C7_E7;
  flipped = true;
}

void UndoFlip_B7C7_E7F7() {
  left_columns -= 25770196992ULL;
  top_diag9s -= 25769803776ULL;
  right_columns -= 1688875630067712ULL;
  bottom_diag9s -= 25770196998ULL;
  bottom_diag7s -= 1688875630069254ULL;
  bottom_rows -= 132120576ULL;
  disk_difference -= 8;
}

void Flip_B7C7_E7F7() {
  left_columns += 25770196992ULL;
  top_diag9s += 25769803776ULL;
  right_columns += 1688875630067712ULL;
  bottom_diag9s += 25770196998ULL;
  bottom_diag7s += 1688875630069254ULL;
  bottom_rows += 132120576ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7C7_E7F7;
  flipped = true;
}

void UndoFlip_B7C7_E7G7() {
  left_columns -= 25770196992ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688875630460928ULL;
  bottom_diag9s -= 1688875630460934ULL;
  bottom_diag7s -= 1688875630069254ULL;
  bottom_rows -= 132513792ULL;
  disk_difference -= 10;
}

void Flip_B7C7_E7G7() {
  left_columns += 25770196992ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 1688875630460928ULL;
  bottom_diag9s += 1688875630460934ULL;
  bottom_diag7s += 1688875630069254ULL;
  bottom_rows += 132513792ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7C7_E7G7;
  flipped = true;
}

void UndoFlip_B7D7() {
  left_columns -= 25770196998ULL;
  top_diag9s -= 25769803776ULL;
  bottom_diag9s -= 1542ULL;
  bottom_diag7s -= 1688875630460928ULL;
  bottom_rows -= 138018816ULL;
  disk_difference -= 6;
}

void Flip_B7D7() {
  left_columns += 25770196998ULL;
  top_diag9s += 25769803776ULL;
  bottom_diag9s += 1542ULL;
  bottom_diag7s += 1688875630460928ULL;
  bottom_rows += 138018816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7D7;
  flipped = true;
}

void UndoFlip_B7D7_F7() {
  left_columns -= 25770196998ULL;
  top_diag9s -= 25769803776ULL;
  right_columns -= 25769803776ULL;
  bottom_diag9s -= 25769805318ULL;
  bottom_diag7s -= 1688875630460934ULL;
  bottom_rows -= 139198464ULL;
  disk_difference -= 8;
}

void Flip_B7D7_F7() {
  left_columns += 25770196998ULL;
  top_diag9s += 25769803776ULL;
  right_columns += 25769803776ULL;
  bottom_diag9s += 25769805318ULL;
  bottom_diag7s += 1688875630460934ULL;
  bottom_rows += 139198464ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7D7_F7;
  flipped = true;
}

void UndoFlip_B7D7_F7G7() {
  left_columns -= 25770196998ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 25770196992ULL;
  bottom_diag9s -= 1688875630069254ULL;
  bottom_diag7s -= 1688875630460934ULL;
  bottom_rows -= 139591680ULL;
  disk_difference -= 10;
}

void Flip_B7D7_F7G7() {
  left_columns += 25770196998ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 25770196992ULL;
  bottom_diag9s += 1688875630069254ULL;
  bottom_diag7s += 1688875630460934ULL;
  bottom_rows += 139591680ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7D7_F7G7;
  flipped = true;
}

void UndoFlip_B7E7() {
  left_columns -= 25770196998ULL;
  top_diag9s -= 25769803776ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 394758ULL;
  bottom_diag7s -= 1688875630462464ULL;
  bottom_rows -= 141557760ULL;
  disk_difference -= 8;
}

void Flip_B7E7() {
  left_columns += 25770196998ULL;
  top_diag9s += 25769803776ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 394758ULL;
  bottom_diag7s += 1688875630462464ULL;
  bottom_rows += 141557760ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7E7;
  flipped = true;
}

void UndoFlip_B7E7_G7() {
  left_columns -= 25770196998ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688849860657152ULL;
  bottom_diag9s -= 1688849860658694ULL;
  bottom_diag7s -= 1688875630462464ULL;
  bottom_rows -= 141950976ULL;
  disk_difference -= 10;
}

void Flip_B7E7_G7() {
  left_columns += 25770196998ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 1688849860657152ULL;
  bottom_diag9s += 1688849860658694ULL;
  bottom_diag7s += 1688875630462464ULL;
  bottom_rows += 141950976ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7E7_G7;
  flipped = true;
}

void UndoFlip_B7F7() {
  left_columns -= 25770196998ULL;
  top_diag9s -= 25769803776ULL;
  right_columns -= 1688875630067712ULL;
  bottom_diag9s -= 25770198534ULL;
  bottom_diag7s -= 1688875630462470ULL;
  bottom_rows -= 142737408ULL;
  disk_difference -= 10;
}

void Flip_B7F7() {
  left_columns += 25770196998ULL;
  top_diag9s += 25769803776ULL;
  right_columns += 1688875630067712ULL;
  bottom_diag9s += 25770198534ULL;
  bottom_diag7s += 1688875630462470ULL;
  bottom_rows += 142737408ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7F7;
  flipped = true;
}

void UndoFlip_B7G7() {
  left_columns -= 25770196998ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688875630460928ULL;
  bottom_diag9s -= 1688875630462470ULL;
  bottom_diag7s -= 1688875630462470ULL;
  bottom_rows -= 143130624ULL;
  disk_difference -= 12;
}

void Flip_B7G7() {
  left_columns += 25770196998ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 1688875630460928ULL;
  bottom_diag9s += 1688875630462470ULL;
  bottom_diag7s += 1688875630462470ULL;
  bottom_rows += 143130624ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B7G7;
  flipped = true;
}

void UndoFlip_C7() {
  left_columns -= 393216ULL;
  bottom_diag9s -= 6ULL;
  bottom_diag7s -= 25769803776ULL;
  bottom_rows -= 31850496ULL;
  disk_difference -= 2;
}

void Flip_C7() {
  left_columns += 393216ULL;
  bottom_diag9s += 6ULL;
  bottom_diag7s += 25769803776ULL;
  bottom_rows += 31850496ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7;
  flipped = true;
}

void UndoFlip_C7_E7() {
  left_columns -= 393216ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 393222ULL;
  bottom_diag7s -= 25769805312ULL;
  bottom_rows -= 35389440ULL;
  disk_difference -= 4;
}

void Flip_C7_E7() {
  left_columns += 393216ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 393222ULL;
  bottom_diag7s += 25769805312ULL;
  bottom_rows += 35389440ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7_E7;
  flipped = true;
}

void UndoFlip_C7_E7F7() {
  left_columns -= 393216ULL;
  right_columns -= 1688875630067712ULL;
  bottom_diag9s -= 25770196998ULL;
  bottom_diag7s -= 25769805318ULL;
  bottom_rows -= 36569088ULL;
  disk_difference -= 6;
}

void Flip_C7_E7F7() {
  left_columns += 393216ULL;
  right_columns += 1688875630067712ULL;
  bottom_diag9s += 25770196998ULL;
  bottom_diag7s += 25769805318ULL;
  bottom_rows += 36569088ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7_E7F7;
  flipped = true;
}

void UndoFlip_C7_E7G7() {
  left_columns -= 393216ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688875630460928ULL;
  bottom_diag9s -= 1688875630460934ULL;
  bottom_diag7s -= 25769805318ULL;
  bottom_rows -= 36962304ULL;
  disk_difference -= 8;
}

void Flip_C7_E7G7() {
  left_columns += 393216ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 1688875630460928ULL;
  bottom_diag9s += 1688875630460934ULL;
  bottom_diag7s += 25769805318ULL;
  bottom_rows += 36962304ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7_E7G7;
  flipped = true;
}

void UndoFlip_C7D7() {
  left_columns -= 393222ULL;
  bottom_diag9s -= 1542ULL;
  bottom_diag7s -= 25770196992ULL;
  bottom_rows -= 42467328ULL;
  disk_difference -= 4;
}

void Flip_C7D7() {
  left_columns += 393222ULL;
  bottom_diag9s += 1542ULL;
  bottom_diag7s += 25770196992ULL;
  bottom_rows += 42467328ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7D7;
  flipped = true;
}

void UndoFlip_C7D7_F7() {
  left_columns -= 393222ULL;
  right_columns -= 25769803776ULL;
  bottom_diag9s -= 25769805318ULL;
  bottom_diag7s -= 25770196998ULL;
  bottom_rows -= 43646976ULL;
  disk_difference -= 6;
}

void Flip_C7D7_F7() {
  left_columns += 393222ULL;
  right_columns += 25769803776ULL;
  bottom_diag9s += 25769805318ULL;
  bottom_diag7s += 25770196998ULL;
  bottom_rows += 43646976ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7D7_F7;
  flipped = true;
}

void UndoFlip_C7D7_F7G7() {
  left_columns -= 393222ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 25770196992ULL;
  bottom_diag9s -= 1688875630069254ULL;
  bottom_diag7s -= 25770196998ULL;
  bottom_rows -= 44040192ULL;
  disk_difference -= 8;
}

void Flip_C7D7_F7G7() {
  left_columns += 393222ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 25770196992ULL;
  bottom_diag9s += 1688875630069254ULL;
  bottom_diag7s += 25770196998ULL;
  bottom_rows += 44040192ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7D7_F7G7;
  flipped = true;
}

void UndoFlip_C7E7() {
  left_columns -= 393222ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 394758ULL;
  bottom_diag7s -= 25770198528ULL;
  bottom_rows -= 46006272ULL;
  disk_difference -= 6;
}

void Flip_C7E7() {
  left_columns += 393222ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 394758ULL;
  bottom_diag7s += 25770198528ULL;
  bottom_rows += 46006272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7E7;
  flipped = true;
}

void UndoFlip_C7E7_G7() {
  left_columns -= 393222ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688849860657152ULL;
  bottom_diag9s -= 1688849860658694ULL;
  bottom_diag7s -= 25770198528ULL;
  bottom_rows -= 46399488ULL;
  disk_difference -= 8;
}

void Flip_C7E7_G7() {
  left_columns += 393222ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 1688849860657152ULL;
  bottom_diag9s += 1688849860658694ULL;
  bottom_diag7s += 25770198528ULL;
  bottom_rows += 46399488ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7E7_G7;
  flipped = true;
}

void UndoFlip_C7F7() {
  left_columns -= 393222ULL;
  right_columns -= 1688875630067712ULL;
  bottom_diag9s -= 25770198534ULL;
  bottom_diag7s -= 25770198534ULL;
  bottom_rows -= 47185920ULL;
  disk_difference -= 8;
}

void Flip_C7F7() {
  left_columns += 393222ULL;
  right_columns += 1688875630067712ULL;
  bottom_diag9s += 25770198534ULL;
  bottom_diag7s += 25770198534ULL;
  bottom_rows += 47185920ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7F7;
  flipped = true;
}

void UndoFlip_C7G7() {
  left_columns -= 393222ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688875630460928ULL;
  bottom_diag9s -= 1688875630462470ULL;
  bottom_diag7s -= 25770198534ULL;
  bottom_rows -= 47579136ULL;
  disk_difference -= 10;
}

void Flip_C7G7() {
  left_columns += 393222ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 1688875630460928ULL;
  bottom_diag9s += 1688875630462470ULL;
  bottom_diag7s += 25770198534ULL;
  bottom_rows += 47579136ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C7G7;
  flipped = true;
}

void UndoFlip_D7() {
  left_columns -= 6ULL;
  bottom_diag9s -= 1536ULL;
  bottom_diag7s -= 393216ULL;
  bottom_rows -= 10616832ULL;
  disk_difference -= 2;
}

void Flip_D7() {
  left_columns += 6ULL;
  bottom_diag9s += 1536ULL;
  bottom_diag7s += 393216ULL;
  bottom_rows += 10616832ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7;
  flipped = true;
}

void UndoFlip_D7_F7() {
  left_columns -= 6ULL;
  right_columns -= 25769803776ULL;
  bottom_diag9s -= 25769805312ULL;
  bottom_diag7s -= 393222ULL;
  bottom_rows -= 11796480ULL;
  disk_difference -= 4;
}

void Flip_D7_F7() {
  left_columns += 6ULL;
  right_columns += 25769803776ULL;
  bottom_diag9s += 25769805312ULL;
  bottom_diag7s += 393222ULL;
  bottom_rows += 11796480ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7_F7;
  flipped = true;
}

void UndoFlip_D7_F7G7() {
  left_columns -= 6ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 25770196992ULL;
  bottom_diag9s -= 1688875630069248ULL;
  bottom_diag7s -= 393222ULL;
  bottom_rows -= 12189696ULL;
  disk_difference -= 6;
}

void Flip_D7_F7G7() {
  left_columns += 6ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 25770196992ULL;
  bottom_diag9s += 1688875630069248ULL;
  bottom_diag7s += 393222ULL;
  bottom_rows += 12189696ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7_F7G7;
  flipped = true;
}

void UndoFlip_D7E7() {
  left_columns -= 6ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 394752ULL;
  bottom_diag7s -= 394752ULL;
  bottom_rows -= 14155776ULL;
  disk_difference -= 4;
}

void Flip_D7E7() {
  left_columns += 6ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 394752ULL;
  bottom_diag7s += 394752ULL;
  bottom_rows += 14155776ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7E7;
  flipped = true;
}

void UndoFlip_D7E7_G7() {
  left_columns -= 6ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688849860657152ULL;
  bottom_diag9s -= 1688849860658688ULL;
  bottom_diag7s -= 394752ULL;
  bottom_rows -= 14548992ULL;
  disk_difference -= 6;
}

void Flip_D7E7_G7() {
  left_columns += 6ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 1688849860657152ULL;
  bottom_diag9s += 1688849860658688ULL;
  bottom_diag7s += 394752ULL;
  bottom_rows += 14548992ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7E7_G7;
  flipped = true;
}

void UndoFlip_D7F7() {
  left_columns -= 6ULL;
  right_columns -= 1688875630067712ULL;
  bottom_diag9s -= 25770198528ULL;
  bottom_diag7s -= 394758ULL;
  bottom_rows -= 15335424ULL;
  disk_difference -= 6;
}

void Flip_D7F7() {
  left_columns += 6ULL;
  right_columns += 1688875630067712ULL;
  bottom_diag9s += 25770198528ULL;
  bottom_diag7s += 394758ULL;
  bottom_rows += 15335424ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7F7;
  flipped = true;
}

void UndoFlip_D7G7() {
  left_columns -= 6ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688875630460928ULL;
  bottom_diag9s -= 1688875630462464ULL;
  bottom_diag7s -= 394758ULL;
  bottom_rows -= 15728640ULL;
  disk_difference -= 8;
}

void Flip_D7G7() {
  left_columns += 6ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 1688875630460928ULL;
  bottom_diag9s += 1688875630462464ULL;
  bottom_diag7s += 394758ULL;
  bottom_rows += 15728640ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D7G7;
  flipped = true;
}

void UndoFlip_E7() {
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 393216ULL;
  bottom_diag7s -= 1536ULL;
  bottom_rows -= 3538944ULL;
  disk_difference -= 2;
}

void Flip_E7() {
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 393216ULL;
  bottom_diag7s += 1536ULL;
  bottom_rows += 3538944ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E7;
  flipped = true;
}

void UndoFlip_E7_G7() {
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688849860657152ULL;
  bottom_diag9s -= 1688849860657152ULL;
  bottom_diag7s -= 1536ULL;
  bottom_rows -= 3932160ULL;
  disk_difference -= 4;
}

void Flip_E7_G7() {
  top_diag7s += 25769803776ULL;
  right_columns += 1688849860657152ULL;
  bottom_diag9s += 1688849860657152ULL;
  bottom_diag7s += 1536ULL;
  bottom_rows += 3932160ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E7_G7;
  flipped = true;
}

void UndoFlip_E7F7() {
  right_columns -= 1688875630067712ULL;
  bottom_diag9s -= 25770196992ULL;
  bottom_diag7s -= 1542ULL;
  bottom_rows -= 4718592ULL;
  disk_difference -= 4;
}

void Flip_E7F7() {
  right_columns += 1688875630067712ULL;
  bottom_diag9s += 25770196992ULL;
  bottom_diag7s += 1542ULL;
  bottom_rows += 4718592ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E7F7;
  flipped = true;
}

void UndoFlip_E7G7() {
  top_diag7s -= 25769803776ULL;
  right_columns -= 1688875630460928ULL;
  bottom_diag9s -= 1688875630460928ULL;
  bottom_diag7s -= 1542ULL;
  bottom_rows -= 5111808ULL;
  disk_difference -= 6;
}

void Flip_E7G7() {
  top_diag7s += 25769803776ULL;
  right_columns += 1688875630460928ULL;
  bottom_diag9s += 1688875630460928ULL;
  bottom_diag7s += 1542ULL;
  bottom_rows += 5111808ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E7G7;
  flipped = true;
}

void UndoFlip_F7() {
  right_columns -= 25769803776ULL;
  bottom_diag9s -= 25769803776ULL;
  bottom_diag7s -= 6ULL;
  bottom_rows -= 1179648ULL;
  disk_difference -= 2;
}

void Flip_F7() {
  right_columns += 25769803776ULL;
  bottom_diag9s += 25769803776ULL;
  bottom_diag7s += 6ULL;
  bottom_rows += 1179648ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F7;
  flipped = true;
}

void UndoFlip_F7G7() {
  top_diag7s -= 25769803776ULL;
  right_columns -= 25770196992ULL;
  bottom_diag9s -= 1688875630067712ULL;
  bottom_diag7s -= 6ULL;
  bottom_rows -= 1572864ULL;
  disk_difference -= 4;
}

void Flip_F7G7() {
  top_diag7s += 25769803776ULL;
  right_columns += 25770196992ULL;
  bottom_diag9s += 1688875630067712ULL;
  bottom_diag7s += 6ULL;
  bottom_rows += 1572864ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F7G7;
  flipped = true;
}

void UndoFlip_G7() {
  top_diag7s -= 25769803776ULL;
  right_columns -= 393216ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_rows -= 393216ULL;
  disk_difference -= 2;
}

void Flip_G7() {
  top_diag7s += 25769803776ULL;
  right_columns += 393216ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_rows += 393216ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G7;
  flipped = true;
}

void UndoFlip_B8() {
  left_columns -= 8589934592ULL;
  bottom_diag7s -= 8589934592ULL;
  bottom_rows -= 1458ULL;
  disk_difference -= 2;
}

void Flip_B8() {
  left_columns += 8589934592ULL;
  bottom_diag7s += 8589934592ULL;
  bottom_rows += 1458ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8;
  flipped = true;
}

void UndoFlip_B8_D8() {
  left_columns -= 8589934594ULL;
  bottom_diag9s -= 2ULL;
  bottom_diag7s -= 8589935104ULL;
  bottom_rows -= 1620ULL;
  disk_difference -= 4;
}

void Flip_B8_D8() {
  left_columns += 8589934594ULL;
  bottom_diag9s += 2ULL;
  bottom_diag7s += 8589935104ULL;
  bottom_rows += 1620ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8_D8;
  flipped = true;
}

void UndoFlip_B8_D8E8() {
  left_columns -= 8589934594ULL;
  right_columns -= 562949953421312ULL;
  bottom_diag9s -= 514ULL;
  bottom_diag7s -= 8589935106ULL;
  bottom_rows -= 1674ULL;
  disk_difference -= 6;
}

void Flip_B8_D8E8() {
  left_columns += 8589934594ULL;
  right_columns += 562949953421312ULL;
  bottom_diag9s += 514ULL;
  bottom_diag7s += 8589935106ULL;
  bottom_rows += 1674ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8_D8E8;
  flipped = true;
}

void UndoFlip_B8_D8F8() {
  left_columns -= 8589934594ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543355904ULL;
  bottom_diag9s -= 131586ULL;
  bottom_diag7s -= 8589935106ULL;
  bottom_rows -= 1692ULL;
  disk_difference -= 8;
}

void Flip_B8_D8F8() {
  left_columns += 8589934594ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543355904ULL;
  bottom_diag9s += 131586ULL;
  bottom_diag7s += 8589935106ULL;
  bottom_rows += 1692ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8_D8F8;
  flipped = true;
}

void UndoFlip_B8_D8G8() {
  left_columns -= 8589934594ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543486976ULL;
  bottom_diag9s -= 8590066178ULL;
  bottom_diag7s -= 8589935106ULL;
  bottom_rows -= 1698ULL;
  disk_difference -= 10;
}

void Flip_B8_D8G8() {
  left_columns += 8589934594ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543486976ULL;
  bottom_diag9s += 8590066178ULL;
  bottom_diag7s += 8589935106ULL;
  bottom_rows += 1698ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8_D8G8;
  flipped = true;
}

void UndoFlip_B8C8() {
  left_columns -= 8590065664ULL;
  top_diag9s -= 8589934592ULL;
  bottom_diag7s -= 8590065664ULL;
  bottom_rows -= 1944ULL;
  disk_difference -= 4;
}

void Flip_B8C8() {
  left_columns += 8590065664ULL;
  top_diag9s += 8589934592ULL;
  bottom_diag7s += 8590065664ULL;
  bottom_rows += 1944ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8C8;
  flipped = true;
}

void UndoFlip_B8C8_E8() {
  left_columns -= 8590065664ULL;
  top_diag9s -= 8589934592ULL;
  right_columns -= 562949953421312ULL;
  bottom_diag9s -= 512ULL;
  bottom_diag7s -= 8590065666ULL;
  bottom_rows -= 1998ULL;
  disk_difference -= 6;
}

void Flip_B8C8_E8() {
  left_columns += 8590065664ULL;
  top_diag9s += 8589934592ULL;
  right_columns += 562949953421312ULL;
  bottom_diag9s += 512ULL;
  bottom_diag7s += 8590065666ULL;
  bottom_rows += 1998ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8C8_E8;
  flipped = true;
}

void UndoFlip_B8C8_E8F8() {
  left_columns -= 8590065664ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543355904ULL;
  bottom_diag9s -= 131584ULL;
  bottom_diag7s -= 8590065666ULL;
  bottom_rows -= 2016ULL;
  disk_difference -= 8;
}

void Flip_B8C8_E8F8() {
  left_columns += 8590065664ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543355904ULL;
  bottom_diag9s += 131584ULL;
  bottom_diag7s += 8590065666ULL;
  bottom_rows += 2016ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8C8_E8F8;
  flipped = true;
}

void UndoFlip_B8C8_E8G8() {
  left_columns -= 8590065664ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543486976ULL;
  bottom_diag9s -= 8590066176ULL;
  bottom_diag7s -= 8590065666ULL;
  bottom_rows -= 2022ULL;
  disk_difference -= 10;
}

void Flip_B8C8_E8G8() {
  left_columns += 8590065664ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543486976ULL;
  bottom_diag9s += 8590066176ULL;
  bottom_diag7s += 8590065666ULL;
  bottom_rows += 2022ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8C8_E8G8;
  flipped = true;
}

void UndoFlip_B8D8() {
  left_columns -= 8590065666ULL;
  top_diag9s -= 8589934592ULL;
  bottom_diag9s -= 2ULL;
  bottom_diag7s -= 8590066176ULL;
  bottom_rows -= 2106ULL;
  disk_difference -= 6;
}

void Flip_B8D8() {
  left_columns += 8590065666ULL;
  top_diag9s += 8589934592ULL;
  bottom_diag9s += 2ULL;
  bottom_diag7s += 8590066176ULL;
  bottom_rows += 2106ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8D8;
  flipped = true;
}

void UndoFlip_B8D8_F8() {
  left_columns -= 8590065666ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 8589934592ULL;
  bottom_diag9s -= 131074ULL;
  bottom_diag7s -= 8590066176ULL;
  bottom_rows -= 2124ULL;
  disk_difference -= 8;
}

void Flip_B8D8_F8() {
  left_columns += 8590065666ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 8589934592ULL;
  bottom_diag9s += 131074ULL;
  bottom_diag7s += 8590066176ULL;
  bottom_rows += 2124ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8D8_F8;
  flipped = true;
}

void UndoFlip_B8D8_F8G8() {
  left_columns -= 8590065666ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 8590065664ULL;
  bottom_diag9s -= 8590065666ULL;
  bottom_diag7s -= 8590066176ULL;
  bottom_rows -= 2130ULL;
  disk_difference -= 10;
}

void Flip_B8D8_F8G8() {
  left_columns += 8590065666ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 8590065664ULL;
  bottom_diag9s += 8590065666ULL;
  bottom_diag7s += 8590066176ULL;
  bottom_rows += 2130ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8D8_F8G8;
  flipped = true;
}

void UndoFlip_B8E8() {
  left_columns -= 8590065666ULL;
  top_diag9s -= 8589934592ULL;
  right_columns -= 562949953421312ULL;
  bottom_diag9s -= 514ULL;
  bottom_diag7s -= 8590066178ULL;
  bottom_rows -= 2160ULL;
  disk_difference -= 8;
}

void Flip_B8E8() {
  left_columns += 8590065666ULL;
  top_diag9s += 8589934592ULL;
  right_columns += 562949953421312ULL;
  bottom_diag9s += 514ULL;
  bottom_diag7s += 8590066178ULL;
  bottom_rows += 2160ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8E8;
  flipped = true;
}

void UndoFlip_B8E8_G8() {
  left_columns -= 8590065666ULL;
  top_diag9s -= 8589934592ULL;
  right_columns -= 562949953552384ULL;
  bottom_diag9s -= 8589935106ULL;
  bottom_diag7s -= 8590066178ULL;
  bottom_rows -= 2166ULL;
  disk_difference -= 10;
}

void Flip_B8E8_G8() {
  left_columns += 8590065666ULL;
  top_diag9s += 8589934592ULL;
  right_columns += 562949953552384ULL;
  bottom_diag9s += 8589935106ULL;
  bottom_diag7s += 8590066178ULL;
  bottom_rows += 2166ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8E8_G8;
  flipped = true;
}

void UndoFlip_B8F8() {
  left_columns -= 8590065666ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543355904ULL;
  bottom_diag9s -= 131586ULL;
  bottom_diag7s -= 8590066178ULL;
  bottom_rows -= 2178ULL;
  disk_difference -= 10;
}

void Flip_B8F8() {
  left_columns += 8590065666ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543355904ULL;
  bottom_diag9s += 131586ULL;
  bottom_diag7s += 8590066178ULL;
  bottom_rows += 2178ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8F8;
  flipped = true;
}

void UndoFlip_B8G8() {
  left_columns -= 8590065666ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543486976ULL;
  bottom_diag9s -= 8590066178ULL;
  bottom_diag7s -= 8590066178ULL;
  bottom_rows -= 2184ULL;
  disk_difference -= 12;
}

void Flip_B8G8() {
  left_columns += 8590065666ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543486976ULL;
  bottom_diag9s += 8590066178ULL;
  bottom_diag7s += 8590066178ULL;
  bottom_rows += 2184ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B8G8;
  flipped = true;
}

void UndoFlip_C8() {
  left_columns -= 131072ULL;
  top_diag9s -= 8589934592ULL;
  bottom_diag7s -= 131072ULL;
  bottom_rows -= 486ULL;
  disk_difference -= 2;
}

void Flip_C8() {
  left_columns += 131072ULL;
  top_diag9s += 8589934592ULL;
  bottom_diag7s += 131072ULL;
  bottom_rows += 486ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8;
  flipped = true;
}

void UndoFlip_C8_E8() {
  left_columns -= 131072ULL;
  top_diag9s -= 8589934592ULL;
  right_columns -= 562949953421312ULL;
  bottom_diag9s -= 512ULL;
  bottom_diag7s -= 131074ULL;
  bottom_rows -= 540ULL;
  disk_difference -= 4;
}

void Flip_C8_E8() {
  left_columns += 131072ULL;
  top_diag9s += 8589934592ULL;
  right_columns += 562949953421312ULL;
  bottom_diag9s += 512ULL;
  bottom_diag7s += 131074ULL;
  bottom_rows += 540ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8_E8;
  flipped = true;
}

void UndoFlip_C8_E8F8() {
  left_columns -= 131072ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543355904ULL;
  bottom_diag9s -= 131584ULL;
  bottom_diag7s -= 131074ULL;
  bottom_rows -= 558ULL;
  disk_difference -= 6;
}

void Flip_C8_E8F8() {
  left_columns += 131072ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543355904ULL;
  bottom_diag9s += 131584ULL;
  bottom_diag7s += 131074ULL;
  bottom_rows += 558ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8_E8F8;
  flipped = true;
}

void UndoFlip_C8_E8G8() {
  left_columns -= 131072ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543486976ULL;
  bottom_diag9s -= 8590066176ULL;
  bottom_diag7s -= 131074ULL;
  bottom_rows -= 564ULL;
  disk_difference -= 8;
}

void Flip_C8_E8G8() {
  left_columns += 131072ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543486976ULL;
  bottom_diag9s += 8590066176ULL;
  bottom_diag7s += 131074ULL;
  bottom_rows += 564ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8_E8G8;
  flipped = true;
}

void UndoFlip_C8D8() {
  left_columns -= 131074ULL;
  top_diag9s -= 8589934592ULL;
  bottom_diag9s -= 2ULL;
  bottom_diag7s -= 131584ULL;
  bottom_rows -= 648ULL;
  disk_difference -= 4;
}

void Flip_C8D8() {
  left_columns += 131074ULL;
  top_diag9s += 8589934592ULL;
  bottom_diag9s += 2ULL;
  bottom_diag7s += 131584ULL;
  bottom_rows += 648ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8D8;
  flipped = true;
}

void UndoFlip_C8D8_F8() {
  left_columns -= 131074ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 8589934592ULL;
  bottom_diag9s -= 131074ULL;
  bottom_diag7s -= 131584ULL;
  bottom_rows -= 666ULL;
  disk_difference -= 6;
}

void Flip_C8D8_F8() {
  left_columns += 131074ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 8589934592ULL;
  bottom_diag9s += 131074ULL;
  bottom_diag7s += 131584ULL;
  bottom_rows += 666ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8D8_F8;
  flipped = true;
}

void UndoFlip_C8D8_F8G8() {
  left_columns -= 131074ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 8590065664ULL;
  bottom_diag9s -= 8590065666ULL;
  bottom_diag7s -= 131584ULL;
  bottom_rows -= 672ULL;
  disk_difference -= 8;
}

void Flip_C8D8_F8G8() {
  left_columns += 131074ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 8590065664ULL;
  bottom_diag9s += 8590065666ULL;
  bottom_diag7s += 131584ULL;
  bottom_rows += 672ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8D8_F8G8;
  flipped = true;
}

void UndoFlip_C8E8() {
  left_columns -= 131074ULL;
  top_diag9s -= 8589934592ULL;
  right_columns -= 562949953421312ULL;
  bottom_diag9s -= 514ULL;
  bottom_diag7s -= 131586ULL;
  bottom_rows -= 702ULL;
  disk_difference -= 6;
}

void Flip_C8E8() {
  left_columns += 131074ULL;
  top_diag9s += 8589934592ULL;
  right_columns += 562949953421312ULL;
  bottom_diag9s += 514ULL;
  bottom_diag7s += 131586ULL;
  bottom_rows += 702ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8E8;
  flipped = true;
}

void UndoFlip_C8E8_G8() {
  left_columns -= 131074ULL;
  top_diag9s -= 8589934592ULL;
  right_columns -= 562949953552384ULL;
  bottom_diag9s -= 8589935106ULL;
  bottom_diag7s -= 131586ULL;
  bottom_rows -= 708ULL;
  disk_difference -= 8;
}

void Flip_C8E8_G8() {
  left_columns += 131074ULL;
  top_diag9s += 8589934592ULL;
  right_columns += 562949953552384ULL;
  bottom_diag9s += 8589935106ULL;
  bottom_diag7s += 131586ULL;
  bottom_rows += 708ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8E8_G8;
  flipped = true;
}

void UndoFlip_C8F8() {
  left_columns -= 131074ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543355904ULL;
  bottom_diag9s -= 131586ULL;
  bottom_diag7s -= 131586ULL;
  bottom_rows -= 720ULL;
  disk_difference -= 8;
}

void Flip_C8F8() {
  left_columns += 131074ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543355904ULL;
  bottom_diag9s += 131586ULL;
  bottom_diag7s += 131586ULL;
  bottom_rows += 720ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8F8;
  flipped = true;
}

void UndoFlip_C8G8() {
  left_columns -= 131074ULL;
  top_diag9s -= 8589934592ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543486976ULL;
  bottom_diag9s -= 8590066178ULL;
  bottom_diag7s -= 131586ULL;
  bottom_rows -= 726ULL;
  disk_difference -= 10;
}

void Flip_C8G8() {
  left_columns += 131074ULL;
  top_diag9s += 8589934592ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543486976ULL;
  bottom_diag9s += 8590066178ULL;
  bottom_diag7s += 131586ULL;
  bottom_rows += 726ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C8G8;
  flipped = true;
}

void UndoFlip_D8() {
  left_columns -= 2ULL;
  bottom_diag9s -= 2ULL;
  bottom_diag7s -= 512ULL;
  bottom_rows -= 162ULL;
  disk_difference -= 2;
}

void Flip_D8() {
  left_columns += 2ULL;
  bottom_diag9s += 2ULL;
  bottom_diag7s += 512ULL;
  bottom_rows += 162ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8;
  flipped = true;
}

void UndoFlip_D8_F8() {
  left_columns -= 2ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 8589934592ULL;
  bottom_diag9s -= 131074ULL;
  bottom_diag7s -= 512ULL;
  bottom_rows -= 180ULL;
  disk_difference -= 4;
}

void Flip_D8_F8() {
  left_columns += 2ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 8589934592ULL;
  bottom_diag9s += 131074ULL;
  bottom_diag7s += 512ULL;
  bottom_rows += 180ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8_F8;
  flipped = true;
}

void UndoFlip_D8_F8G8() {
  left_columns -= 2ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 8590065664ULL;
  bottom_diag9s -= 8590065666ULL;
  bottom_diag7s -= 512ULL;
  bottom_rows -= 186ULL;
  disk_difference -= 6;
}

void Flip_D8_F8G8() {
  left_columns += 2ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 8590065664ULL;
  bottom_diag9s += 8590065666ULL;
  bottom_diag7s += 512ULL;
  bottom_rows += 186ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8_F8G8;
  flipped = true;
}

void UndoFlip_D8E8() {
  left_columns -= 2ULL;
  right_columns -= 562949953421312ULL;
  bottom_diag9s -= 514ULL;
  bottom_diag7s -= 514ULL;
  bottom_rows -= 216ULL;
  disk_difference -= 4;
}

void Flip_D8E8() {
  left_columns += 2ULL;
  right_columns += 562949953421312ULL;
  bottom_diag9s += 514ULL;
  bottom_diag7s += 514ULL;
  bottom_rows += 216ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8E8;
  flipped = true;
}

void UndoFlip_D8E8_G8() {
  left_columns -= 2ULL;
  right_columns -= 562949953552384ULL;
  bottom_diag9s -= 8589935106ULL;
  bottom_diag7s -= 514ULL;
  bottom_rows -= 222ULL;
  disk_difference -= 6;
}

void Flip_D8E8_G8() {
  left_columns += 2ULL;
  right_columns += 562949953552384ULL;
  bottom_diag9s += 8589935106ULL;
  bottom_diag7s += 514ULL;
  bottom_rows += 222ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8E8_G8;
  flipped = true;
}

void UndoFlip_D8F8() {
  left_columns -= 2ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543355904ULL;
  bottom_diag9s -= 131586ULL;
  bottom_diag7s -= 514ULL;
  bottom_rows -= 234ULL;
  disk_difference -= 6;
}

void Flip_D8F8() {
  left_columns += 2ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543355904ULL;
  bottom_diag9s += 131586ULL;
  bottom_diag7s += 514ULL;
  bottom_rows += 234ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8F8;
  flipped = true;
}

void UndoFlip_D8G8() {
  left_columns -= 2ULL;
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543486976ULL;
  bottom_diag9s -= 8590066178ULL;
  bottom_diag7s -= 514ULL;
  bottom_rows -= 240ULL;
  disk_difference -= 8;
}

void Flip_D8G8() {
  left_columns += 2ULL;
  top_diag7s += 8589934592ULL;
  right_columns += 562958543486976ULL;
  bottom_diag9s += 8590066178ULL;
  bottom_diag7s += 514ULL;
  bottom_rows += 240ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D8G8;
  flipped = true;
}

void UndoFlip_E8() {
  right_columns -= 562949953421312ULL;
  bottom_diag9s -= 512ULL;
  bottom_diag7s -= 2ULL;
  bottom_rows -= 54ULL;
  disk_difference -= 2;
}

void Flip_E8() {
  right_columns += 562949953421312ULL;
  bottom_diag9s += 512ULL;
  bottom_diag7s += 2ULL;
  bottom_rows += 54ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E8;
  flipped = true;
}

void UndoFlip_E8_G8() {
  right_columns -= 562949953552384ULL;
  bottom_diag9s -= 8589935104ULL;
  bottom_diag7s -= 2ULL;
  bottom_rows -= 60ULL;
  disk_difference -= 4;
}

void Flip_E8_G8() {
  right_columns += 562949953552384ULL;
  bottom_diag9s += 8589935104ULL;
  bottom_diag7s += 2ULL;
  bottom_rows += 60ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E8_G8;
  flipped = true;
}

void UndoFlip_E8F8() {
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543355904ULL;
  bottom_diag9s -= 131584ULL;
  bottom_diag7s -= 2ULL;
  bottom_rows -= 72ULL;
  disk_difference -= 4;
}

void Flip_E8F8() {
  top_diag7s += 8589934592ULL;
  right_columns += 562958543355904ULL;
  bottom_diag9s += 131584ULL;
  bottom_diag7s += 2ULL;
  bottom_rows += 72ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E8F8;
  flipped = true;
}

void UndoFlip_E8G8() {
  top_diag7s -= 8589934592ULL;
  right_columns -= 562958543486976ULL;
  bottom_diag9s -= 8590066176ULL;
  bottom_diag7s -= 2ULL;
  bottom_rows -= 78ULL;
  disk_difference -= 6;
}

void Flip_E8G8() {
  top_diag7s += 8589934592ULL;
  right_columns += 562958543486976ULL;
  bottom_diag9s += 8590066176ULL;
  bottom_diag7s += 2ULL;
  bottom_rows += 78ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E8G8;
  flipped = true;
}

void UndoFlip_F8() {
  top_diag7s -= 8589934592ULL;
  right_columns -= 8589934592ULL;
  bottom_diag9s -= 131072ULL;
  bottom_rows -= 18ULL;
  disk_difference -= 2;
}

void Flip_F8() {
  top_diag7s += 8589934592ULL;
  right_columns += 8589934592ULL;
  bottom_diag9s += 131072ULL;
  bottom_rows += 18ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F8;
  flipped = true;
}

void UndoFlip_F8G8() {
  top_diag7s -= 8589934592ULL;
  right_columns -= 8590065664ULL;
  bottom_diag9s -= 8590065664ULL;
  bottom_rows -= 24ULL;
  disk_difference -= 4;
}

void Flip_F8G8() {
  top_diag7s += 8589934592ULL;
  right_columns += 8590065664ULL;
  bottom_diag9s += 8590065664ULL;
  bottom_rows += 24ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F8G8;
  flipped = true;
}

void UndoFlip_G8() {
  right_columns -= 131072ULL;
  bottom_diag9s -= 8589934592ULL;
  bottom_rows -= 6ULL;
  disk_difference -= 2;
}

void Flip_G8() {
  right_columns += 131072ULL;
  bottom_diag9s += 8589934592ULL;
  bottom_rows += 6ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G8;
  flipped = true;
}

void UndoFlip_A2() {
  top_rows -= 18786186952704ULL;
  left_columns -= 410390516044136448ULL;
  bottom_diag9s -= 6262062317568ULL;
  disk_difference -= 2;
}

void Flip_A2() {
  top_rows += 18786186952704ULL;
  left_columns += 410390516044136448ULL;
  bottom_diag9s += 6262062317568ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2;
  flipped = true;
}

void UndoFlip_A2_A4() {
  top_rows -= 18786186957078ULL;
  left_columns -= 455989462271262720ULL;
  top_diag7s -= 562949953421312ULL;
  bottom_diag9s -= 6262062359040ULL;
  disk_difference -= 4;
}

void Flip_A2_A4() {
  top_rows += 18786186957078ULL;
  left_columns += 455989462271262720ULL;
  top_diag7s += 562949953421312ULL;
  bottom_diag9s += 6262062359040ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2_A4;
  flipped = true;
}

void UndoFlip_A2_A4A5() {
  top_rows -= 18786186957078ULL;
  left_columns -= 471189111013638144ULL;
  top_diag7s -= 565148976676864ULL;
  bottom_diag9s -= 6262062359094ULL;
  bottom_rows -= 1231171548132409344ULL;
  disk_difference -= 6;
}

void Flip_A2_A4A5() {
  top_rows += 18786186957078ULL;
  left_columns += 471189111013638144ULL;
  top_diag7s += 565148976676864ULL;
  bottom_diag9s += 6262062359094ULL;
  bottom_rows += 1231171548132409344ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2_A4A5;
  flipped = true;
}

void UndoFlip_A2_A4A6() {
  top_rows -= 18786186957078ULL;
  left_columns -= 476255660594429952ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 565148976807936ULL;
  bottom_diag9s -= 6262062359094ULL;
  bottom_rows -= 1231190334319362048ULL;
  disk_difference -= 8;
}

void Flip_A2_A4A6() {
  top_rows += 18786186957078ULL;
  left_columns += 476255660594429952ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 565148976807936ULL;
  bottom_diag9s += 6262062359094ULL;
  bottom_rows += 1231190334319362048ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2_A4A6;
  flipped = true;
}

void UndoFlip_A2_A4A7() {
  top_rows -= 18786186957078ULL;
  left_columns -= 477944510454693888ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 565148976807938ULL;
  bottom_diag9s -= 6262062359094ULL;
  bottom_rows -= 1231190334606016512ULL;
  disk_difference -= 10;
}

void Flip_A2_A4A7() {
  top_rows += 18786186957078ULL;
  left_columns += 477944510454693888ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 565148976807938ULL;
  bottom_diag9s += 6262062359094ULL;
  bottom_rows += 1231190334606016512ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2_A4A7;
  flipped = true;
}

void UndoFlip_A2A3() {
  top_rows -= 18786473607168ULL;
  left_columns -= 547187354725515264ULL;
  top_diag7s -= 144115188075855872ULL;
  bottom_diag9s -= 6262094168064ULL;
  disk_difference -= 4;
}

void Flip_A2A3() {
  top_rows += 18786473607168ULL;
  left_columns += 547187354725515264ULL;
  top_diag7s += 144115188075855872ULL;
  bottom_diag9s += 6262094168064ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A3;
  flipped = true;
}

void UndoFlip_A2A3_A5() {
  top_rows -= 18786473607168ULL;
  left_columns -= 562387003467890688ULL;
  top_diag7s -= 144117387099111424ULL;
  bottom_diag9s -= 6262094168118ULL;
  bottom_rows -= 1231171548132409344ULL;
  disk_difference -= 6;
}

void Flip_A2A3_A5() {
  top_rows += 18786473607168ULL;
  left_columns += 562387003467890688ULL;
  top_diag7s += 144117387099111424ULL;
  bottom_diag9s += 6262094168118ULL;
  bottom_rows += 1231171548132409344ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A3_A5;
  flipped = true;
}

void UndoFlip_A2A3_A5A6() {
  top_rows -= 18786473607168ULL;
  left_columns -= 567453553048682496ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144117387099242496ULL;
  bottom_diag9s -= 6262094168118ULL;
  bottom_rows -= 1231190334319362048ULL;
  disk_difference -= 8;
}

void Flip_A2A3_A5A6() {
  top_rows += 18786473607168ULL;
  left_columns += 567453553048682496ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144117387099242496ULL;
  bottom_diag9s += 6262094168118ULL;
  bottom_rows += 1231190334319362048ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A3_A5A6;
  flipped = true;
}

void UndoFlip_A2A3_A5A7() {
  top_rows -= 18786473607168ULL;
  left_columns -= 569142402908946432ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144117387099242498ULL;
  bottom_diag9s -= 6262094168118ULL;
  bottom_rows -= 1231190334606016512ULL;
  disk_difference -= 10;
}

void Flip_A2A3_A5A7() {
  top_rows += 18786473607168ULL;
  left_columns += 569142402908946432ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144117387099242498ULL;
  bottom_diag9s += 6262094168118ULL;
  bottom_rows += 1231190334606016512ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A3_A5A7;
  flipped = true;
}

void UndoFlip_A2A4() {
  top_rows -= 18786473611542ULL;
  left_columns -= 592786300952641536ULL;
  top_diag7s -= 144678138029277184ULL;
  bottom_diag9s -= 6262094209536ULL;
  disk_difference -= 6;
}

void Flip_A2A4() {
  top_rows += 18786473611542ULL;
  left_columns += 592786300952641536ULL;
  top_diag7s += 144678138029277184ULL;
  bottom_diag9s += 6262094209536ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A4;
  flipped = true;
}

void UndoFlip_A2A4_A6() {
  top_rows -= 18786473611542ULL;
  left_columns -= 597852850533433344ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144678138029408256ULL;
  bottom_diag9s -= 6262094209536ULL;
  bottom_rows -= 18786186952704ULL;
  disk_difference -= 8;
}

void Flip_A2A4_A6() {
  top_rows += 18786473611542ULL;
  left_columns += 597852850533433344ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144678138029408256ULL;
  bottom_diag9s += 6262094209536ULL;
  bottom_rows += 18786186952704ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A4_A6;
  flipped = true;
}

void UndoFlip_A2A4_A6A7() {
  top_rows -= 18786473611542ULL;
  left_columns -= 599541700393697280ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144678138029408258ULL;
  bottom_diag9s -= 6262094209536ULL;
  bottom_rows -= 18786473607168ULL;
  disk_difference -= 10;
}

void Flip_A2A4_A6A7() {
  top_rows += 18786473611542ULL;
  left_columns += 599541700393697280ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144678138029408258ULL;
  bottom_diag9s += 6262094209536ULL;
  bottom_rows += 18786473607168ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A4_A6A7;
  flipped = true;
}

void UndoFlip_A2A5() {
  top_rows -= 18786473611542ULL;
  left_columns -= 607985949695016960ULL;
  top_diag7s -= 144680337052532736ULL;
  bottom_diag9s -= 6262094209590ULL;
  bottom_rows -= 1231171548132409344ULL;
  disk_difference -= 8;
}

void Flip_A2A5() {
  top_rows += 18786473611542ULL;
  left_columns += 607985949695016960ULL;
  top_diag7s += 144680337052532736ULL;
  bottom_diag9s += 6262094209590ULL;
  bottom_rows += 1231171548132409344ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A5;
  flipped = true;
}

void UndoFlip_A2A5_A7() {
  top_rows -= 18786473611542ULL;
  left_columns -= 609674799555280896ULL;
  top_diag7s -= 144680337052532738ULL;
  bottom_diag9s -= 6262094209590ULL;
  bottom_rows -= 1231171548419063808ULL;
  disk_difference -= 10;
}

void Flip_A2A5_A7() {
  top_rows += 18786473611542ULL;
  left_columns += 609674799555280896ULL;
  top_diag7s += 144680337052532738ULL;
  bottom_diag9s += 6262094209590ULL;
  bottom_rows += 1231171548419063808ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A5_A7;
  flipped = true;
}

void UndoFlip_A2A6() {
  top_rows -= 18786473611542ULL;
  left_columns -= 613052499275808768ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144680337052663808ULL;
  bottom_diag9s -= 6262094209590ULL;
  bottom_rows -= 1231190334319362048ULL;
  disk_difference -= 10;
}

void Flip_A2A6() {
  top_rows += 18786473611542ULL;
  left_columns += 613052499275808768ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144680337052663808ULL;
  bottom_diag9s += 6262094209590ULL;
  bottom_rows += 1231190334319362048ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A6;
  flipped = true;
}

void UndoFlip_A2A7() {
  top_rows -= 18786473611542ULL;
  left_columns -= 614741349136072704ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144680337052663810ULL;
  bottom_diag9s -= 6262094209590ULL;
  bottom_rows -= 1231190334606016512ULL;
  disk_difference -= 12;
}

void Flip_A2A7() {
  top_rows += 18786473611542ULL;
  left_columns += 614741349136072704ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144680337052663810ULL;
  bottom_diag9s += 6262094209590ULL;
  bottom_rows += 1231190334606016512ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A2A7;
  flipped = true;
}

void UndoFlip_A3() {
  top_rows -= 286654464ULL;
  left_columns -= 136796838681378816ULL;
  top_diag7s -= 144115188075855872ULL;
  bottom_diag9s -= 31850496ULL;
  disk_difference -= 2;
}

void Flip_A3() {
  top_rows += 286654464ULL;
  left_columns += 136796838681378816ULL;
  top_diag7s += 144115188075855872ULL;
  bottom_diag9s += 31850496ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3;
  flipped = true;
}

void UndoFlip_A3_A5() {
  top_rows -= 286654464ULL;
  left_columns -= 151996487423754240ULL;
  top_diag7s -= 144117387099111424ULL;
  bottom_diag9s -= 31850550ULL;
  bottom_rows -= 1231171548132409344ULL;
  disk_difference -= 4;
}

void Flip_A3_A5() {
  top_rows += 286654464ULL;
  left_columns += 151996487423754240ULL;
  top_diag7s += 144117387099111424ULL;
  bottom_diag9s += 31850550ULL;
  bottom_rows += 1231171548132409344ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3_A5;
  flipped = true;
}

void UndoFlip_A3_A5A6() {
  top_rows -= 286654464ULL;
  left_columns -= 157063037004546048ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144117387099242496ULL;
  bottom_diag9s -= 31850550ULL;
  bottom_rows -= 1231190334319362048ULL;
  disk_difference -= 6;
}

void Flip_A3_A5A6() {
  top_rows += 286654464ULL;
  left_columns += 157063037004546048ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144117387099242496ULL;
  bottom_diag9s += 31850550ULL;
  bottom_rows += 1231190334319362048ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3_A5A6;
  flipped = true;
}

void UndoFlip_A3_A5A7() {
  top_rows -= 286654464ULL;
  left_columns -= 158751886864809984ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144117387099242498ULL;
  bottom_diag9s -= 31850550ULL;
  bottom_rows -= 1231190334606016512ULL;
  disk_difference -= 8;
}

void Flip_A3_A5A7() {
  top_rows += 286654464ULL;
  left_columns += 158751886864809984ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144117387099242498ULL;
  bottom_diag9s += 31850550ULL;
  bottom_rows += 1231190334606016512ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3_A5A7;
  flipped = true;
}

void UndoFlip_A3A4() {
  top_rows -= 286658838ULL;
  left_columns -= 182395784908505088ULL;
  top_diag7s -= 144678138029277184ULL;
  bottom_diag9s -= 31891968ULL;
  disk_difference -= 4;
}

void Flip_A3A4() {
  top_rows += 286658838ULL;
  left_columns += 182395784908505088ULL;
  top_diag7s += 144678138029277184ULL;
  bottom_diag9s += 31891968ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A4;
  flipped = true;
}

void UndoFlip_A3A4_A6() {
  top_rows -= 286658838ULL;
  left_columns -= 187462334489296896ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144678138029408256ULL;
  bottom_diag9s -= 31891968ULL;
  bottom_rows -= 18786186952704ULL;
  disk_difference -= 6;
}

void Flip_A3A4_A6() {
  top_rows += 286658838ULL;
  left_columns += 187462334489296896ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144678138029408256ULL;
  bottom_diag9s += 31891968ULL;
  bottom_rows += 18786186952704ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A4_A6;
  flipped = true;
}

void UndoFlip_A3A4_A6A7() {
  top_rows -= 286658838ULL;
  left_columns -= 189151184349560832ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144678138029408258ULL;
  bottom_diag9s -= 31891968ULL;
  bottom_rows -= 18786473607168ULL;
  disk_difference -= 8;
}

void Flip_A3A4_A6A7() {
  top_rows += 286658838ULL;
  left_columns += 189151184349560832ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144678138029408258ULL;
  bottom_diag9s += 31891968ULL;
  bottom_rows += 18786473607168ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A4_A6A7;
  flipped = true;
}

void UndoFlip_A3A5() {
  top_rows -= 286658838ULL;
  left_columns -= 197595433650880512ULL;
  top_diag7s -= 144680337052532736ULL;
  bottom_diag9s -= 31892022ULL;
  bottom_rows -= 1231171548132409344ULL;
  disk_difference -= 6;
}

void Flip_A3A5() {
  top_rows += 286658838ULL;
  left_columns += 197595433650880512ULL;
  top_diag7s += 144680337052532736ULL;
  bottom_diag9s += 31892022ULL;
  bottom_rows += 1231171548132409344ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A5;
  flipped = true;
}

void UndoFlip_A3A5_A7() {
  top_rows -= 286658838ULL;
  left_columns -= 199284283511144448ULL;
  top_diag7s -= 144680337052532738ULL;
  bottom_diag9s -= 31892022ULL;
  bottom_rows -= 1231171548419063808ULL;
  disk_difference -= 8;
}

void Flip_A3A5_A7() {
  top_rows += 286658838ULL;
  left_columns += 199284283511144448ULL;
  top_diag7s += 144680337052532738ULL;
  bottom_diag9s += 31892022ULL;
  bottom_rows += 1231171548419063808ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A5_A7;
  flipped = true;
}

void UndoFlip_A3A6() {
  top_rows -= 286658838ULL;
  left_columns -= 202661983231672320ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144680337052663808ULL;
  bottom_diag9s -= 31892022ULL;
  bottom_rows -= 1231190334319362048ULL;
  disk_difference -= 8;
}

void Flip_A3A6() {
  top_rows += 286658838ULL;
  left_columns += 202661983231672320ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144680337052663808ULL;
  bottom_diag9s += 31892022ULL;
  bottom_rows += 1231190334319362048ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A6;
  flipped = true;
}

void UndoFlip_A3A7() {
  top_rows -= 286658838ULL;
  left_columns -= 204350833091936256ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 144680337052663810ULL;
  bottom_diag9s -= 31892022ULL;
  bottom_rows -= 1231190334606016512ULL;
  disk_difference -= 10;
}

void Flip_A3A7() {
  top_rows += 286658838ULL;
  left_columns += 204350833091936256ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 144680337052663810ULL;
  bottom_diag9s += 31892022ULL;
  bottom_rows += 1231190334606016512ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A3A7;
  flipped = true;
}

void UndoFlip_A4() {
  top_rows -= 4374ULL;
  left_columns -= 45598946227126272ULL;
  top_diag7s -= 562949953421312ULL;
  bottom_diag9s -= 41472ULL;
  disk_difference -= 2;
}

void Flip_A4() {
  top_rows += 4374ULL;
  left_columns += 45598946227126272ULL;
  top_diag7s += 562949953421312ULL;
  bottom_diag9s += 41472ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4;
  flipped = true;
}

void UndoFlip_A4_A6() {
  top_rows -= 4374ULL;
  left_columns -= 50665495807918080ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 562949953552384ULL;
  bottom_diag9s -= 41472ULL;
  bottom_rows -= 18786186952704ULL;
  disk_difference -= 4;
}

void Flip_A4_A6() {
  top_rows += 4374ULL;
  left_columns += 50665495807918080ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 562949953552384ULL;
  bottom_diag9s += 41472ULL;
  bottom_rows += 18786186952704ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4_A6;
  flipped = true;
}

void UndoFlip_A4_A6A7() {
  top_rows -= 4374ULL;
  left_columns -= 52354345668182016ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 562949953552386ULL;
  bottom_diag9s -= 41472ULL;
  bottom_rows -= 18786473607168ULL;
  disk_difference -= 6;
}

void Flip_A4_A6A7() {
  top_rows += 4374ULL;
  left_columns += 52354345668182016ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 562949953552386ULL;
  bottom_diag9s += 41472ULL;
  bottom_rows += 18786473607168ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4_A6A7;
  flipped = true;
}

void UndoFlip_A4A5() {
  top_rows -= 4374ULL;
  left_columns -= 60798594969501696ULL;
  top_diag7s -= 565148976676864ULL;
  bottom_diag9s -= 41526ULL;
  bottom_rows -= 1231171548132409344ULL;
  disk_difference -= 4;
}

void Flip_A4A5() {
  top_rows += 4374ULL;
  left_columns += 60798594969501696ULL;
  top_diag7s += 565148976676864ULL;
  bottom_diag9s += 41526ULL;
  bottom_rows += 1231171548132409344ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4A5;
  flipped = true;
}

void UndoFlip_A4A5_A7() {
  top_rows -= 4374ULL;
  left_columns -= 62487444829765632ULL;
  top_diag7s -= 565148976676866ULL;
  bottom_diag9s -= 41526ULL;
  bottom_rows -= 1231171548419063808ULL;
  disk_difference -= 6;
}

void Flip_A4A5_A7() {
  top_rows += 4374ULL;
  left_columns += 62487444829765632ULL;
  top_diag7s += 565148976676866ULL;
  bottom_diag9s += 41526ULL;
  bottom_rows += 1231171548419063808ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4A5_A7;
  flipped = true;
}

void UndoFlip_A4A6() {
  top_rows -= 4374ULL;
  left_columns -= 65865144550293504ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 565148976807936ULL;
  bottom_diag9s -= 41526ULL;
  bottom_rows -= 1231190334319362048ULL;
  disk_difference -= 6;
}

void Flip_A4A6() {
  top_rows += 4374ULL;
  left_columns += 65865144550293504ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 565148976807936ULL;
  bottom_diag9s += 41526ULL;
  bottom_rows += 1231190334319362048ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4A6;
  flipped = true;
}

void UndoFlip_A4A7() {
  top_rows -= 4374ULL;
  left_columns -= 67553994410557440ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 565148976807938ULL;
  bottom_diag9s -= 41526ULL;
  bottom_rows -= 1231190334606016512ULL;
  disk_difference -= 8;
}

void Flip_A4A7() {
  top_rows += 4374ULL;
  left_columns += 67553994410557440ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 565148976807938ULL;
  bottom_diag9s += 41526ULL;
  bottom_rows += 1231190334606016512ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A4A7;
  flipped = true;
}

void UndoFlip_A5() {
  left_columns -= 15199648742375424ULL;
  top_diag7s -= 2199023255552ULL;
  bottom_diag9s -= 54ULL;
  bottom_rows -= 1231171548132409344ULL;
  disk_difference -= 2;
}

void Flip_A5() {
  left_columns += 15199648742375424ULL;
  top_diag7s += 2199023255552ULL;
  bottom_diag9s += 54ULL;
  bottom_rows += 1231171548132409344ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A5;
  flipped = true;
}

void UndoFlip_A5_A7() {
  left_columns -= 16888498602639360ULL;
  top_diag7s -= 2199023255554ULL;
  bottom_diag9s -= 54ULL;
  bottom_rows -= 1231171548419063808ULL;
  disk_difference -= 4;
}

void Flip_A5_A7() {
  left_columns += 16888498602639360ULL;
  top_diag7s += 2199023255554ULL;
  bottom_diag9s += 54ULL;
  bottom_rows += 1231171548419063808ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A5_A7;
  flipped = true;
}

void UndoFlip_A5A6() {
  left_columns -= 20266198323167232ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 2199023386624ULL;
  bottom_diag9s -= 54ULL;
  bottom_rows -= 1231190334319362048ULL;
  disk_difference -= 4;
}

void Flip_A5A6() {
  left_columns += 20266198323167232ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 2199023386624ULL;
  bottom_diag9s += 54ULL;
  bottom_rows += 1231190334319362048ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A5A6;
  flipped = true;
}

void UndoFlip_A5A7() {
  left_columns -= 21955048183431168ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 2199023386626ULL;
  bottom_diag9s -= 54ULL;
  bottom_rows -= 1231190334606016512ULL;
  disk_difference -= 6;
}

void Flip_A5A7() {
  left_columns += 21955048183431168ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 2199023386626ULL;
  bottom_diag9s += 54ULL;
  bottom_rows += 1231190334606016512ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A5A7;
  flipped = true;
}

void UndoFlip_A6() {
  left_columns -= 5066549580791808ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 131072ULL;
  bottom_rows -= 18786186952704ULL;
  disk_difference -= 2;
}

void Flip_A6() {
  left_columns += 5066549580791808ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 131072ULL;
  bottom_rows += 18786186952704ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A6;
  flipped = true;
}

void UndoFlip_A6A7() {
  left_columns -= 6755399441055744ULL;
  top_diag9s -= 77309411328ULL;
  top_diag7s -= 131074ULL;
  bottom_rows -= 18786473607168ULL;
  disk_difference -= 4;
}

void Flip_A6A7() {
  left_columns += 6755399441055744ULL;
  top_diag9s += 77309411328ULL;
  top_diag7s += 131074ULL;
  bottom_rows += 18786473607168ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A6A7;
  flipped = true;
}

void UndoFlip_A7() {
  left_columns -= 1688849860263936ULL;
  top_diag7s -= 2ULL;
  bottom_rows -= 286654464ULL;
  disk_difference -= 2;
}

void Flip_A7() {
  left_columns += 1688849860263936ULL;
  top_diag7s += 2ULL;
  bottom_rows += 286654464ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_A7;
  flipped = true;
}

void UndoFlip_B2_B4() {
  top_rows -= 6262062319026ULL;
  left_columns -= 6957847019520ULL;
  top_diag7s -= 432352161297334272ULL;
  bottom_diag9s -= 410390516054753280ULL;
  disk_difference -= 4;
}

void Flip_B2_B4() {
  top_rows += 6262062319026ULL;
  left_columns += 6957847019520ULL;
  top_diag7s += 432352161297334272ULL;
  bottom_diag9s += 410390516054753280ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_B4;
  flipped = true;
}

void UndoFlip_B2_B4B5() {
  top_rows -= 6262062319026ULL;
  left_columns -= 7189775253504ULL;
  top_diag7s -= 432352161297727488ULL;
  bottom_diag9s -= 410390516054767104ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_B2_B4B5() {
  top_rows += 6262062319026ULL;
  left_columns += 7189775253504ULL;
  top_diag7s += 432352161297727488ULL;
  bottom_diag9s += 410390516054767104ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_B4B5;
  flipped = true;
}

void UndoFlip_B2_B4B6() {
  top_rows -= 6262062319026ULL;
  left_columns -= 7267084664832ULL;
  top_diag7s -= 432352161297727494ULL;
  bottom_diag9s -= 410390516054767122ULL;
  bottom_rows -= 410396778106454016ULL;
  disk_difference -= 8;
}

void Flip_B2_B4B6() {
  top_rows += 6262062319026ULL;
  left_columns += 7267084664832ULL;
  top_diag7s += 432352161297727494ULL;
  bottom_diag9s += 410390516054767122ULL;
  bottom_rows += 410396778106454016ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_B4B6;
  flipped = true;
}

void UndoFlip_B2_B4B7() {
  top_rows -= 6262062319026ULL;
  left_columns -= 7292854468608ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 432352161297727494ULL;
  bottom_diag9s -= 410390516054767122ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410396778202005504ULL;
  disk_difference -= 10;
}

void Flip_B2_B4B7() {
  top_rows += 6262062319026ULL;
  left_columns += 7292854468608ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 432352161297727494ULL;
  bottom_diag9s += 410390516054767122ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410396778202005504ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_B4B7;
  flipped = true;
}

void UndoFlip_B2B3() {
  top_rows -= 6262157869056ULL;
  left_columns -= 8349416423424ULL;
  top_diag7s -= 434034414087831552ULL;
  bottom_diag9s -= 410392603398242304ULL;
  disk_difference -= 4;
}

void Flip_B2B3() {
  top_rows += 6262157869056ULL;
  left_columns += 8349416423424ULL;
  top_diag7s += 434034414087831552ULL;
  bottom_diag9s += 410392603398242304ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B3;
  flipped = true;
}

void UndoFlip_B2B3_B5() {
  top_rows -= 6262157869056ULL;
  left_columns -= 8581344657408ULL;
  top_diag7s -= 434034414088224768ULL;
  bottom_diag9s -= 410392603398256128ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_B2B3_B5() {
  top_rows += 6262157869056ULL;
  left_columns += 8581344657408ULL;
  top_diag7s += 434034414088224768ULL;
  bottom_diag9s += 410392603398256128ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B3_B5;
  flipped = true;
}

void UndoFlip_B2B3_B5B6() {
  top_rows -= 6262157869056ULL;
  left_columns -= 8658654068736ULL;
  top_diag7s -= 434034414088224774ULL;
  bottom_diag9s -= 410392603398256146ULL;
  bottom_rows -= 410396778106454016ULL;
  disk_difference -= 8;
}

void Flip_B2B3_B5B6() {
  top_rows += 6262157869056ULL;
  left_columns += 8658654068736ULL;
  top_diag7s += 434034414088224774ULL;
  bottom_diag9s += 410392603398256146ULL;
  bottom_rows += 410396778106454016ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B3_B5B6;
  flipped = true;
}

void UndoFlip_B2B3_B5B7() {
  top_rows -= 6262157869056ULL;
  left_columns -= 8684423872512ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 434034414088224774ULL;
  bottom_diag9s -= 410392603398256146ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410396778202005504ULL;
  disk_difference -= 10;
}

void Flip_B2B3_B5B7() {
  top_rows += 6262157869056ULL;
  left_columns += 8684423872512ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 434034414088224774ULL;
  bottom_diag9s += 410392603398256146ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410396778202005504ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B3_B5B7;
  flipped = true;
}

void UndoFlip_B2B4() {
  top_rows -= 6262157870514ULL;
  left_columns -= 9045201125376ULL;
  top_diag7s -= 434041011157598208ULL;
  bottom_diag9s -= 410392603408859136ULL;
  disk_difference -= 6;
}

void Flip_B2B4() {
  top_rows += 6262157870514ULL;
  left_columns += 9045201125376ULL;
  top_diag7s += 434041011157598208ULL;
  bottom_diag9s += 410392603408859136ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B4;
  flipped = true;
}

void UndoFlip_B2B4_B6() {
  top_rows -= 6262157870514ULL;
  left_columns -= 9122510536704ULL;
  top_diag7s -= 434041011157598214ULL;
  bottom_diag9s -= 410392603408859154ULL;
  bottom_rows -= 6262062317568ULL;
  disk_difference -= 8;
}

void Flip_B2B4_B6() {
  top_rows += 6262157870514ULL;
  left_columns += 9122510536704ULL;
  top_diag7s += 434041011157598214ULL;
  bottom_diag9s += 410392603408859154ULL;
  bottom_rows += 6262062317568ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B4_B6;
  flipped = true;
}

void UndoFlip_B2B4_B6B7() {
  top_rows -= 6262157870514ULL;
  left_columns -= 9148280340480ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 434041011157598214ULL;
  bottom_diag9s -= 410392603408859154ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 6262157869056ULL;
  disk_difference -= 10;
}

void Flip_B2B4_B6B7() {
  top_rows += 6262157870514ULL;
  left_columns += 9148280340480ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 434041011157598214ULL;
  bottom_diag9s += 410392603408859154ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 6262157869056ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B4_B6B7;
  flipped = true;
}

void UndoFlip_B2B5() {
  top_rows -= 6262157870514ULL;
  left_columns -= 9277129359360ULL;
  top_diag7s -= 434041011157991424ULL;
  bottom_diag9s -= 410392603408872960ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 8;
}

void Flip_B2B5() {
  top_rows += 6262157870514ULL;
  left_columns += 9277129359360ULL;
  top_diag7s += 434041011157991424ULL;
  bottom_diag9s += 410392603408872960ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B5;
  flipped = true;
}

void UndoFlip_B2B5_B7() {
  top_rows -= 6262157870514ULL;
  left_columns -= 9302899163136ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 434041011157991424ULL;
  bottom_diag9s -= 410392603408872960ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410390516139687936ULL;
  disk_difference -= 10;
}

void Flip_B2B5_B7() {
  top_rows += 6262157870514ULL;
  left_columns += 9302899163136ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 434041011157991424ULL;
  bottom_diag9s += 410392603408872960ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410390516139687936ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B5_B7;
  flipped = true;
}

void UndoFlip_B2B6() {
  top_rows -= 6262157870514ULL;
  left_columns -= 9354438770688ULL;
  top_diag7s -= 434041011157991430ULL;
  bottom_diag9s -= 410392603408872978ULL;
  bottom_rows -= 410396778106454016ULL;
  disk_difference -= 10;
}

void Flip_B2B6() {
  top_rows += 6262157870514ULL;
  left_columns += 9354438770688ULL;
  top_diag7s += 434041011157991430ULL;
  bottom_diag9s += 410392603408872978ULL;
  bottom_rows += 410396778106454016ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B6;
  flipped = true;
}

void UndoFlip_B2B7() {
  top_rows -= 6262157870514ULL;
  left_columns -= 9380208574464ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 434041011157991430ULL;
  bottom_diag9s -= 410392603408872978ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410396778202005504ULL;
  disk_difference -= 12;
}

void Flip_B2B7() {
  top_rows += 6262157870514ULL;
  left_columns += 9380208574464ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 434041011157991430ULL;
  bottom_diag9s += 410392603408872978ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410396778202005504ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2B7;
  flipped = true;
}

void UndoFlip_B3_B5() {
  top_rows -= 95551488ULL;
  left_columns -= 2319282339840ULL;
  top_diag7s -= 1688849860657152ULL;
  bottom_diag9s -= 2087354119680ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_B3_B5() {
  top_rows += 95551488ULL;
  left_columns += 2319282339840ULL;
  top_diag7s += 1688849860657152ULL;
  bottom_diag9s += 2087354119680ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_B5;
  flipped = true;
}

void UndoFlip_B3_B5B6() {
  top_rows -= 95551488ULL;
  left_columns -= 2396591751168ULL;
  top_diag7s -= 1688849860657158ULL;
  bottom_diag9s -= 2087354119698ULL;
  bottom_rows -= 410396778106454016ULL;
  disk_difference -= 6;
}

void Flip_B3_B5B6() {
  top_rows += 95551488ULL;
  left_columns += 2396591751168ULL;
  top_diag7s += 1688849860657158ULL;
  bottom_diag9s += 2087354119698ULL;
  bottom_rows += 410396778106454016ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_B5B6;
  flipped = true;
}

void UndoFlip_B3_B5B7() {
  top_rows -= 95551488ULL;
  left_columns -= 2422361554944ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 1688849860657158ULL;
  bottom_diag9s -= 2087354119698ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410396778202005504ULL;
  disk_difference -= 8;
}

void Flip_B3_B5B7() {
  top_rows += 95551488ULL;
  left_columns += 2422361554944ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 1688849860657158ULL;
  bottom_diag9s += 2087354119698ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410396778202005504ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_B5B7;
  flipped = true;
}

void UndoFlip_B3B4() {
  top_rows -= 95552946ULL;
  left_columns -= 2783138807808ULL;
  top_diag7s -= 1695446930030592ULL;
  bottom_diag9s -= 2087364722688ULL;
  disk_difference -= 4;
}

void Flip_B3B4() {
  top_rows += 95552946ULL;
  left_columns += 2783138807808ULL;
  top_diag7s += 1695446930030592ULL;
  bottom_diag9s += 2087364722688ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B4;
  flipped = true;
}

void UndoFlip_B3B4_B6() {
  top_rows -= 95552946ULL;
  left_columns -= 2860448219136ULL;
  top_diag7s -= 1695446930030598ULL;
  bottom_diag9s -= 2087364722706ULL;
  bottom_rows -= 6262062317568ULL;
  disk_difference -= 6;
}

void Flip_B3B4_B6() {
  top_rows += 95552946ULL;
  left_columns += 2860448219136ULL;
  top_diag7s += 1695446930030598ULL;
  bottom_diag9s += 2087364722706ULL;
  bottom_rows += 6262062317568ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B4_B6;
  flipped = true;
}

void UndoFlip_B3B4_B6B7() {
  top_rows -= 95552946ULL;
  left_columns -= 2886218022912ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 1695446930030598ULL;
  bottom_diag9s -= 2087364722706ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 6262157869056ULL;
  disk_difference -= 8;
}

void Flip_B3B4_B6B7() {
  top_rows += 95552946ULL;
  left_columns += 2886218022912ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 1695446930030598ULL;
  bottom_diag9s += 2087364722706ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 6262157869056ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B4_B6B7;
  flipped = true;
}

void UndoFlip_B3B5() {
  top_rows -= 95552946ULL;
  left_columns -= 3015067041792ULL;
  top_diag7s -= 1695446930423808ULL;
  bottom_diag9s -= 2087364736512ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_B3B5() {
  top_rows += 95552946ULL;
  left_columns += 3015067041792ULL;
  top_diag7s += 1695446930423808ULL;
  bottom_diag9s += 2087364736512ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B5;
  flipped = true;
}

void UndoFlip_B3B5_B7() {
  top_rows -= 95552946ULL;
  left_columns -= 3040836845568ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 1695446930423808ULL;
  bottom_diag9s -= 2087364736512ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410390516139687936ULL;
  disk_difference -= 8;
}

void Flip_B3B5_B7() {
  top_rows += 95552946ULL;
  left_columns += 3040836845568ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 1695446930423808ULL;
  bottom_diag9s += 2087364736512ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410390516139687936ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B5_B7;
  flipped = true;
}

void UndoFlip_B3B6() {
  top_rows -= 95552946ULL;
  left_columns -= 3092376453120ULL;
  top_diag7s -= 1695446930423814ULL;
  bottom_diag9s -= 2087364736530ULL;
  bottom_rows -= 410396778106454016ULL;
  disk_difference -= 8;
}

void Flip_B3B6() {
  top_rows += 95552946ULL;
  left_columns += 3092376453120ULL;
  top_diag7s += 1695446930423814ULL;
  bottom_diag9s += 2087364736530ULL;
  bottom_rows += 410396778106454016ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B6;
  flipped = true;
}

void UndoFlip_B3B7() {
  top_rows -= 95552946ULL;
  left_columns -= 3118146256896ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 1695446930423814ULL;
  bottom_diag9s -= 2087364736530ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410396778202005504ULL;
  disk_difference -= 10;
}

void Flip_B3B7() {
  top_rows += 95552946ULL;
  left_columns += 3118146256896ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 1695446930423814ULL;
  bottom_diag9s += 2087364736530ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410396778202005504ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3B7;
  flipped = true;
}

void UndoFlip_B4_B6() {
  top_rows -= 1458ULL;
  left_columns -= 773094113280ULL;
  top_diag7s -= 6597069766662ULL;
  bottom_diag9s -= 10616850ULL;
  bottom_rows -= 6262062317568ULL;
  disk_difference -= 4;
}

void Flip_B4_B6() {
  top_rows += 1458ULL;
  left_columns += 773094113280ULL;
  top_diag7s += 6597069766662ULL;
  bottom_diag9s += 10616850ULL;
  bottom_rows += 6262062317568ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_B6;
  flipped = true;
}

void UndoFlip_B4_B6B7() {
  top_rows -= 1458ULL;
  left_columns -= 798863917056ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 6597069766662ULL;
  bottom_diag9s -= 10616850ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 6262157869056ULL;
  disk_difference -= 6;
}

void Flip_B4_B6B7() {
  top_rows += 1458ULL;
  left_columns += 798863917056ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 6597069766662ULL;
  bottom_diag9s += 10616850ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 6262157869056ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_B6B7;
  flipped = true;
}

void UndoFlip_B4B5() {
  top_rows -= 1458ULL;
  left_columns -= 927712935936ULL;
  top_diag7s -= 6597070159872ULL;
  bottom_diag9s -= 10630656ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_B4B5() {
  top_rows += 1458ULL;
  left_columns += 927712935936ULL;
  top_diag7s += 6597070159872ULL;
  bottom_diag9s += 10630656ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4B5;
  flipped = true;
}

void UndoFlip_B4B5_B7() {
  top_rows -= 1458ULL;
  left_columns -= 953482739712ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 6597070159872ULL;
  bottom_diag9s -= 10630656ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410390516139687936ULL;
  disk_difference -= 6;
}

void Flip_B4B5_B7() {
  top_rows += 1458ULL;
  left_columns += 953482739712ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 6597070159872ULL;
  bottom_diag9s += 10630656ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410390516139687936ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4B5_B7;
  flipped = true;
}

void UndoFlip_B4B6() {
  top_rows -= 1458ULL;
  left_columns -= 1005022347264ULL;
  top_diag7s -= 6597070159878ULL;
  bottom_diag9s -= 10630674ULL;
  bottom_rows -= 410396778106454016ULL;
  disk_difference -= 6;
}

void Flip_B4B6() {
  top_rows += 1458ULL;
  left_columns += 1005022347264ULL;
  top_diag7s += 6597070159878ULL;
  bottom_diag9s += 10630674ULL;
  bottom_rows += 410396778106454016ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4B6;
  flipped = true;
}

void UndoFlip_B4B7() {
  top_rows -= 1458ULL;
  left_columns -= 1030792151040ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 6597070159878ULL;
  bottom_diag9s -= 10630674ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410396778202005504ULL;
  disk_difference -= 8;
}

void Flip_B4B7() {
  top_rows += 1458ULL;
  left_columns += 1030792151040ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 6597070159878ULL;
  bottom_diag9s += 10630674ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410396778202005504ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4B7;
  flipped = true;
}

void UndoFlip_B5_B7() {
  left_columns -= 257698037760ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 393216ULL;
  bottom_diag9s -= 13824ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410390516139687936ULL;
  disk_difference -= 4;
}

void Flip_B5_B7() {
  left_columns += 257698037760ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 393216ULL;
  bottom_diag9s += 13824ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410390516139687936ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_B7;
  flipped = true;
}

void UndoFlip_B5B6() {
  left_columns -= 309237645312ULL;
  top_diag7s -= 393222ULL;
  bottom_diag9s -= 13842ULL;
  bottom_rows -= 410396778106454016ULL;
  disk_difference -= 4;
}

void Flip_B5B6() {
  left_columns += 309237645312ULL;
  top_diag7s += 393222ULL;
  bottom_diag9s += 13842ULL;
  bottom_rows += 410396778106454016ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5B6;
  flipped = true;
}

void UndoFlip_B5B7() {
  left_columns -= 335007449088ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 393222ULL;
  bottom_diag9s -= 13842ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 410396778202005504ULL;
  disk_difference -= 6;
}

void Flip_B5B7() {
  left_columns += 335007449088ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 393222ULL;
  bottom_diag9s += 13842ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 410396778202005504ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5B7;
  flipped = true;
}

void UndoFlip_B6B7() {
  left_columns -= 103079215104ULL;
  top_diag9s -= 25769803776ULL;
  top_diag7s -= 6ULL;
  bottom_diag9s -= 18ULL;
  bottom_diag7s -= 1688849860263936ULL;
  bottom_rows -= 6262157869056ULL;
  disk_difference -= 4;
}

void Flip_B6B7() {
  left_columns += 103079215104ULL;
  top_diag9s += 25769803776ULL;
  top_diag7s += 6ULL;
  bottom_diag9s += 18ULL;
  bottom_diag7s += 1688849860263936ULL;
  bottom_rows += 6262157869056ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6B7;
  flipped = true;
}

void UndoFlip_C2_C4() {
  top_rows -= 2087354106342ULL;
  left_columns -= 106168320ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5066549581971456ULL;
  bottom_diag9s -= 695784701952ULL;
  disk_difference -= 4;
}

void Flip_C2_C4() {
  top_rows += 2087354106342ULL;
  left_columns += 106168320ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5066549581971456ULL;
  bottom_diag9s += 695784701952ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_C4;
  flipped = true;
}

void UndoFlip_C2_C4C5() {
  top_rows -= 2087354106342ULL;
  left_columns -= 109707264ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5066549581971474ULL;
  bottom_diag9s -= 695788240896ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 6;
}

void Flip_C2_C4C5() {
  top_rows += 2087354106342ULL;
  left_columns += 109707264ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5066549581971474ULL;
  bottom_diag9s += 695788240896ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_C4C5;
  flipped = true;
}

void UndoFlip_C2_C4C6() {
  top_rows -= 2087354106342ULL;
  left_columns -= 110886912ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5066549581971474ULL;
  bottom_diag9s -= 695788245504ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 136798926035484672ULL;
  disk_difference -= 8;
}

void Flip_C2_C4C6() {
  top_rows += 2087354106342ULL;
  left_columns += 110886912ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5066549581971474ULL;
  bottom_diag9s += 695788245504ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 136798926035484672ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_C4C6;
  flipped = true;
}

void UndoFlip_C2_C4C7() {
  top_rows -= 2087354106342ULL;
  left_columns -= 111280128ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5066549581971474ULL;
  bottom_diag9s -= 695788245510ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 136798926067335168ULL;
  disk_difference -= 10;
}

void Flip_C2_C4C7() {
  top_rows += 2087354106342ULL;
  left_columns += 111280128ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5066549581971474ULL;
  bottom_diag9s += 695788245510ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 136798926067335168ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_C4C7;
  flipped = true;
}

void UndoFlip_C2C3() {
  top_rows -= 2087385956352ULL;
  left_columns -= 127401984ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340790091776ULL;
  bottom_diag9s -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_C2C3() {
  top_rows += 2087385956352ULL;
  left_columns += 127401984ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340790091776ULL;
  bottom_diag9s += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C3;
  flipped = true;
}

void UndoFlip_C2C3_C5() {
  top_rows -= 2087385956352ULL;
  left_columns -= 130940928ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340790091794ULL;
  bottom_diag9s -= 136796838684917760ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 6;
}

void Flip_C2C3_C5() {
  top_rows += 2087385956352ULL;
  left_columns += 130940928ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340790091794ULL;
  bottom_diag9s += 136796838684917760ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C3_C5;
  flipped = true;
}

void UndoFlip_C2C3_C5C6() {
  top_rows -= 2087385956352ULL;
  left_columns -= 132120576ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340790091794ULL;
  bottom_diag9s -= 136796838684922368ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 136798926035484672ULL;
  disk_difference -= 8;
}

void Flip_C2C3_C5C6() {
  top_rows += 2087385956352ULL;
  left_columns += 132120576ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340790091794ULL;
  bottom_diag9s += 136796838684922368ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 136798926035484672ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C3_C5C6;
  flipped = true;
}

void UndoFlip_C2C3_C5C7() {
  top_rows -= 2087385956352ULL;
  left_columns -= 132513792ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340790091794ULL;
  bottom_diag9s -= 136796838684922374ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 136798926067335168ULL;
  disk_difference -= 10;
}

void Flip_C2C3_C5C7() {
  top_rows += 2087385956352ULL;
  left_columns += 132513792ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340790091794ULL;
  bottom_diag9s += 136796838684922374ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 136798926067335168ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C3_C5C7;
  flipped = true;
}

void UndoFlip_C2C4() {
  top_rows -= 2087385956838ULL;
  left_columns -= 138018816ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340791271424ULL;
  bottom_diag9s -= 136797534466080768ULL;
  disk_difference -= 6;
}

void Flip_C2C4() {
  top_rows += 2087385956838ULL;
  left_columns += 138018816ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340791271424ULL;
  bottom_diag9s += 136797534466080768ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C4;
  flipped = true;
}

void UndoFlip_C2C4_C6() {
  top_rows -= 2087385956838ULL;
  left_columns -= 139198464ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340791271424ULL;
  bottom_diag9s -= 136797534466085376ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 2087354105856ULL;
  disk_difference -= 8;
}

void Flip_C2C4_C6() {
  top_rows += 2087385956838ULL;
  left_columns += 139198464ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340791271424ULL;
  bottom_diag9s += 136797534466085376ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 2087354105856ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C4_C6;
  flipped = true;
}

void UndoFlip_C2C4_C6C7() {
  top_rows -= 2087385956838ULL;
  left_columns -= 139591680ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340791271424ULL;
  bottom_diag9s -= 136797534466085382ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 2087385956352ULL;
  disk_difference -= 10;
}

void Flip_C2C4_C6C7() {
  top_rows += 2087385956838ULL;
  left_columns += 139591680ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340791271424ULL;
  bottom_diag9s += 136797534466085382ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 2087385956352ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C4_C6C7;
  flipped = true;
}

void UndoFlip_C2C5() {
  top_rows -= 2087385956838ULL;
  left_columns -= 141557760ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340791271442ULL;
  bottom_diag9s -= 136797534469619712ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 8;
}

void Flip_C2C5() {
  top_rows += 2087385956838ULL;
  left_columns += 141557760ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340791271442ULL;
  bottom_diag9s += 136797534469619712ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C5;
  flipped = true;
}

void UndoFlip_C2C5_C7() {
  top_rows -= 2087385956838ULL;
  left_columns -= 141950976ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340791271442ULL;
  bottom_diag9s -= 136797534469619718ULL;
  bottom_diag7s -= 25769803776ULL;
  bottom_rows -= 136796838713229312ULL;
  disk_difference -= 10;
}

void Flip_C2C5_C7() {
  top_rows += 2087385956838ULL;
  left_columns += 141950976ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340791271442ULL;
  bottom_diag9s += 136797534469619718ULL;
  bottom_diag7s += 25769803776ULL;
  bottom_rows += 136796838713229312ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C5_C7;
  flipped = true;
}

void UndoFlip_C2C6() {
  top_rows -= 2087385956838ULL;
  left_columns -= 142737408ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340791271442ULL;
  bottom_diag9s -= 136797534469624320ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 136798926035484672ULL;
  disk_difference -= 10;
}

void Flip_C2C6() {
  top_rows += 2087385956838ULL;
  left_columns += 142737408ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340791271442ULL;
  bottom_diag9s += 136797534469624320ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 136798926035484672ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C6;
  flipped = true;
}

void UndoFlip_C2C7() {
  top_rows -= 2087385956838ULL;
  left_columns -= 143130624ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 5086340791271442ULL;
  bottom_diag9s -= 136797534469624326ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 136798926067335168ULL;
  disk_difference -= 12;
}

void Flip_C2C7() {
  top_rows += 2087385956838ULL;
  left_columns += 143130624ULL;
  top_diag9s += 486ULL;
  top_diag7s += 5086340791271442ULL;
  bottom_diag9s += 136797534469624326ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 136798926067335168ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2C7;
  flipped = true;
}

void UndoFlip_C3_C5() {
  top_rows -= 31850496ULL;
  left_columns -= 35389440ULL;
  top_diag7s -= 19791209299986ULL;
  bottom_diag9s -= 136796838684917760ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_C3_C5() {
  top_rows += 31850496ULL;
  left_columns += 35389440ULL;
  top_diag7s += 19791209299986ULL;
  bottom_diag9s += 136796838684917760ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_C5;
  flipped = true;
}

void UndoFlip_C3_C5C6() {
  top_rows -= 31850496ULL;
  left_columns -= 36569088ULL;
  top_diag7s -= 19791209299986ULL;
  bottom_diag9s -= 136796838684922368ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 136798926035484672ULL;
  disk_difference -= 6;
}

void Flip_C3_C5C6() {
  top_rows += 31850496ULL;
  left_columns += 36569088ULL;
  top_diag7s += 19791209299986ULL;
  bottom_diag9s += 136796838684922368ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 136798926035484672ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_C5C6;
  flipped = true;
}

void UndoFlip_C3_C5C7() {
  top_rows -= 31850496ULL;
  left_columns -= 36962304ULL;
  top_diag7s -= 19791209299986ULL;
  bottom_diag9s -= 136796838684922374ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 136798926067335168ULL;
  disk_difference -= 8;
}

void Flip_C3_C5C7() {
  top_rows += 31850496ULL;
  left_columns += 36962304ULL;
  top_diag7s += 19791209299986ULL;
  bottom_diag9s += 136796838684922374ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 136798926067335168ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_C5C7;
  flipped = true;
}

void UndoFlip_C3C4() {
  top_rows -= 31850982ULL;
  left_columns -= 42467328ULL;
  top_diag7s -= 19791210479616ULL;
  bottom_diag9s -= 136797534466080768ULL;
  disk_difference -= 4;
}

void Flip_C3C4() {
  top_rows += 31850982ULL;
  left_columns += 42467328ULL;
  top_diag7s += 19791210479616ULL;
  bottom_diag9s += 136797534466080768ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C4;
  flipped = true;
}

void UndoFlip_C3C4_C6() {
  top_rows -= 31850982ULL;
  left_columns -= 43646976ULL;
  top_diag7s -= 19791210479616ULL;
  bottom_diag9s -= 136797534466085376ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 2087354105856ULL;
  disk_difference -= 6;
}

void Flip_C3C4_C6() {
  top_rows += 31850982ULL;
  left_columns += 43646976ULL;
  top_diag7s += 19791210479616ULL;
  bottom_diag9s += 136797534466085376ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 2087354105856ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C4_C6;
  flipped = true;
}

void UndoFlip_C3C4_C6C7() {
  top_rows -= 31850982ULL;
  left_columns -= 44040192ULL;
  top_diag7s -= 19791210479616ULL;
  bottom_diag9s -= 136797534466085382ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 2087385956352ULL;
  disk_difference -= 8;
}

void Flip_C3C4_C6C7() {
  top_rows += 31850982ULL;
  left_columns += 44040192ULL;
  top_diag7s += 19791210479616ULL;
  bottom_diag9s += 136797534466085382ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 2087385956352ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C4_C6C7;
  flipped = true;
}

void UndoFlip_C3C5() {
  top_rows -= 31850982ULL;
  left_columns -= 46006272ULL;
  top_diag7s -= 19791210479634ULL;
  bottom_diag9s -= 136797534469619712ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 6;
}

void Flip_C3C5() {
  top_rows += 31850982ULL;
  left_columns += 46006272ULL;
  top_diag7s += 19791210479634ULL;
  bottom_diag9s += 136797534469619712ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C5;
  flipped = true;
}

void UndoFlip_C3C5_C7() {
  top_rows -= 31850982ULL;
  left_columns -= 46399488ULL;
  top_diag7s -= 19791210479634ULL;
  bottom_diag9s -= 136797534469619718ULL;
  bottom_diag7s -= 25769803776ULL;
  bottom_rows -= 136796838713229312ULL;
  disk_difference -= 8;
}

void Flip_C3C5_C7() {
  top_rows += 31850982ULL;
  left_columns += 46399488ULL;
  top_diag7s += 19791210479634ULL;
  bottom_diag9s += 136797534469619718ULL;
  bottom_diag7s += 25769803776ULL;
  bottom_rows += 136796838713229312ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C5_C7;
  flipped = true;
}

void UndoFlip_C3C6() {
  top_rows -= 31850982ULL;
  left_columns -= 47185920ULL;
  top_diag7s -= 19791210479634ULL;
  bottom_diag9s -= 136797534469624320ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 136798926035484672ULL;
  disk_difference -= 8;
}

void Flip_C3C6() {
  top_rows += 31850982ULL;
  left_columns += 47185920ULL;
  top_diag7s += 19791210479634ULL;
  bottom_diag9s += 136797534469624320ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 136798926035484672ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C6;
  flipped = true;
}

void UndoFlip_C3C7() {
  top_rows -= 31850982ULL;
  left_columns -= 47579136ULL;
  top_diag7s -= 19791210479634ULL;
  bottom_diag9s -= 136797534469624326ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 136798926067335168ULL;
  disk_difference -= 10;
}

void Flip_C3C7() {
  top_rows += 31850982ULL;
  left_columns += 47579136ULL;
  top_diag7s += 19791210479634ULL;
  bottom_diag9s += 136797534469624326ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 136798926067335168ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3C7;
  flipped = true;
}

void UndoFlip_C4_C6() {
  top_rows -= 486ULL;
  left_columns -= 11796480ULL;
  top_diag7s -= 1179648ULL;
  bottom_diag9s -= 695784706560ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 2087354105856ULL;
  disk_difference -= 4;
}

void Flip_C4_C6() {
  top_rows += 486ULL;
  left_columns += 11796480ULL;
  top_diag7s += 1179648ULL;
  bottom_diag9s += 695784706560ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 2087354105856ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_C6;
  flipped = true;
}

void UndoFlip_C4_C6C7() {
  top_rows -= 486ULL;
  left_columns -= 12189696ULL;
  top_diag7s -= 1179648ULL;
  bottom_diag9s -= 695784706566ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 2087385956352ULL;
  disk_difference -= 6;
}

void Flip_C4_C6C7() {
  top_rows += 486ULL;
  left_columns += 12189696ULL;
  top_diag7s += 1179648ULL;
  bottom_diag9s += 695784706566ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 2087385956352ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_C6C7;
  flipped = true;
}

void UndoFlip_C4C5() {
  top_rows -= 486ULL;
  left_columns -= 14155776ULL;
  top_diag7s -= 1179666ULL;
  bottom_diag9s -= 695788240896ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_C4C5() {
  top_rows += 486ULL;
  left_columns += 14155776ULL;
  top_diag7s += 1179666ULL;
  bottom_diag9s += 695788240896ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4C5;
  flipped = true;
}

void UndoFlip_C4C5_C7() {
  top_rows -= 486ULL;
  left_columns -= 14548992ULL;
  top_diag7s -= 1179666ULL;
  bottom_diag9s -= 695788240902ULL;
  bottom_diag7s -= 25769803776ULL;
  bottom_rows -= 136796838713229312ULL;
  disk_difference -= 6;
}

void Flip_C4C5_C7() {
  top_rows += 486ULL;
  left_columns += 14548992ULL;
  top_diag7s += 1179666ULL;
  bottom_diag9s += 695788240902ULL;
  bottom_diag7s += 25769803776ULL;
  bottom_rows += 136796838713229312ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4C5_C7;
  flipped = true;
}

void UndoFlip_C4C6() {
  top_rows -= 486ULL;
  left_columns -= 15335424ULL;
  top_diag7s -= 1179666ULL;
  bottom_diag9s -= 695788245504ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 136798926035484672ULL;
  disk_difference -= 6;
}

void Flip_C4C6() {
  top_rows += 486ULL;
  left_columns += 15335424ULL;
  top_diag7s += 1179666ULL;
  bottom_diag9s += 695788245504ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 136798926035484672ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4C6;
  flipped = true;
}

void UndoFlip_C4C7() {
  top_rows -= 486ULL;
  left_columns -= 15728640ULL;
  top_diag7s -= 1179666ULL;
  bottom_diag9s -= 695788245510ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 136798926067335168ULL;
  disk_difference -= 8;
}

void Flip_C4C7() {
  top_rows += 486ULL;
  left_columns += 15728640ULL;
  top_diag7s += 1179666ULL;
  bottom_diag9s += 695788245510ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 136798926067335168ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4C7;
  flipped = true;
}

void UndoFlip_C5_C7() {
  left_columns -= 3932160ULL;
  top_diag7s -= 18ULL;
  bottom_diag9s -= 3538950ULL;
  bottom_diag7s -= 25769803776ULL;
  bottom_rows -= 136796838713229312ULL;
  disk_difference -= 4;
}

void Flip_C5_C7() {
  left_columns += 3932160ULL;
  top_diag7s += 18ULL;
  bottom_diag9s += 3538950ULL;
  bottom_diag7s += 25769803776ULL;
  bottom_rows += 136796838713229312ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5_C7;
  flipped = true;
}

void UndoFlip_C5C6() {
  left_columns -= 4718592ULL;
  top_diag7s -= 18ULL;
  bottom_diag9s -= 3543552ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 136798926035484672ULL;
  disk_difference -= 4;
}

void Flip_C5C6() {
  left_columns += 4718592ULL;
  top_diag7s += 18ULL;
  bottom_diag9s += 3543552ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 136798926035484672ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5C6;
  flipped = true;
}

void UndoFlip_C5C7() {
  left_columns -= 5111808ULL;
  top_diag7s -= 18ULL;
  bottom_diag9s -= 3543558ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 136798926067335168ULL;
  disk_difference -= 6;
}

void Flip_C5C7() {
  left_columns += 5111808ULL;
  top_diag7s += 18ULL;
  bottom_diag9s += 3543558ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 136798926067335168ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5C7;
  flipped = true;
}

void UndoFlip_C6C7() {
  left_columns -= 1572864ULL;
  bottom_diag9s -= 4614ULL;
  bottom_diag7s -= 5066575350595584ULL;
  bottom_rows -= 2087385956352ULL;
  disk_difference -= 4;
}

void Flip_C6C7() {
  left_columns += 1572864ULL;
  bottom_diag9s += 4614ULL;
  bottom_diag7s += 5066575350595584ULL;
  bottom_rows += 2087385956352ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6C7;
  flipped = true;
}

void UndoFlip_D2_D4() {
  top_rows -= 695784702114ULL;
  left_columns -= 1620ULL;
  top_diag9s -= 10616832ULL;
  top_diag7s -= 59373627899958ULL;
  bottom_diag9s -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_D2_D4() {
  top_rows += 695784702114ULL;
  left_columns += 1620ULL;
  top_diag9s += 10616832ULL;
  top_diag7s += 59373627899958ULL;
  bottom_diag9s += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_D4;
  flipped = true;
}

void UndoFlip_D2_D4D5() {
  top_rows -= 695784702114ULL;
  left_columns -= 1674ULL;
  top_diag9s -= 10616832ULL;
  top_diag7s -= 59373627899958ULL;
  bottom_diag9s -= 45599178155360256ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_D2_D4D5() {
  top_rows += 695784702114ULL;
  left_columns += 1674ULL;
  top_diag9s += 10616832ULL;
  top_diag7s += 59373627899958ULL;
  bottom_diag9s += 45599178155360256ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_D4D5;
  flipped = true;
}

void UndoFlip_D2_D4D6() {
  top_rows -= 695784702114ULL;
  left_columns -= 1692ULL;
  top_diag9s -= 10616832ULL;
  top_diag7s -= 59373627899958ULL;
  bottom_diag9s -= 45599178156539904ULL;
  bottom_diag7s -= 15199726051786752ULL;
  bottom_rows -= 45599642011828224ULL;
  disk_difference -= 8;
}

void Flip_D2_D4D6() {
  top_rows += 695784702114ULL;
  left_columns += 1692ULL;
  top_diag9s += 10616832ULL;
  top_diag7s += 59373627899958ULL;
  bottom_diag9s += 45599178156539904ULL;
  bottom_diag7s += 15199726051786752ULL;
  bottom_rows += 45599642011828224ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_D4D6;
  flipped = true;
}

void UndoFlip_D2_D4D7() {
  top_rows -= 695784702114ULL;
  left_columns -= 1698ULL;
  top_diag9s -= 10616832ULL;
  top_diag7s -= 59373627899958ULL;
  bottom_diag9s -= 45599178156541440ULL;
  bottom_diag7s -= 15199726052179968ULL;
  bottom_rows -= 45599642022445056ULL;
  disk_difference -= 10;
}

void Flip_D2_D4D7() {
  top_rows += 695784702114ULL;
  left_columns += 1698ULL;
  top_diag9s += 10616832ULL;
  top_diag7s += 59373627899958ULL;
  bottom_diag9s += 45599178156541440ULL;
  bottom_diag7s += 15199726052179968ULL;
  bottom_rows += 45599642022445056ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_D4D7;
  flipped = true;
}

void UndoFlip_D2D3() {
  top_rows -= 695795318784ULL;
  left_columns -= 1944ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438848ULL;
  disk_difference -= 4;
}

void Flip_D2D3() {
  top_rows += 695795318784ULL;
  left_columns += 1944ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438848ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D3;
  flipped = true;
}

void UndoFlip_D2D3_D5() {
  top_rows -= 695795318784ULL;
  left_columns -= 1998ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438848ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_D2D3_D5() {
  top_rows += 695795318784ULL;
  left_columns += 1998ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438848ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D3_D5;
  flipped = true;
}

void UndoFlip_D2D3_D5D6() {
  top_rows -= 695795318784ULL;
  left_columns -= 2016ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438848ULL;
  bottom_diag9s -= 231929413632ULL;
  bottom_diag7s -= 15199726051786752ULL;
  bottom_rows -= 45599642011828224ULL;
  disk_difference -= 8;
}

void Flip_D2D3_D5D6() {
  top_rows += 695795318784ULL;
  left_columns += 2016ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438848ULL;
  bottom_diag9s += 231929413632ULL;
  bottom_diag7s += 15199726051786752ULL;
  bottom_rows += 45599642011828224ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D3_D5D6;
  flipped = true;
}

void UndoFlip_D2D3_D5D7() {
  top_rows -= 695795318784ULL;
  left_columns -= 2022ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438848ULL;
  bottom_diag9s -= 231929415168ULL;
  bottom_diag7s -= 15199726052179968ULL;
  bottom_rows -= 45599642022445056ULL;
  disk_difference -= 10;
}

void Flip_D2D3_D5D7() {
  top_rows += 695795318784ULL;
  left_columns += 2022ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438848ULL;
  bottom_diag9s += 231929415168ULL;
  bottom_diag7s += 15199726052179968ULL;
  bottom_rows += 45599642022445056ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D3_D5D7;
  flipped = true;
}

void UndoFlip_D2D4() {
  top_rows -= 695795318946ULL;
  left_columns -= 2106ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438902ULL;
  bottom_diag9s -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_D2D4() {
  top_rows += 695795318946ULL;
  left_columns += 2106ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438902ULL;
  bottom_diag9s += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D4;
  flipped = true;
}

void UndoFlip_D2D4_D6() {
  top_rows -= 695795318946ULL;
  left_columns -= 2124ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438902ULL;
  bottom_diag9s -= 45598946228305920ULL;
  bottom_diag7s -= 77309411328ULL;
  bottom_rows -= 695784701952ULL;
  disk_difference -= 8;
}

void Flip_D2D4_D6() {
  top_rows += 695795318946ULL;
  left_columns += 2124ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438902ULL;
  bottom_diag9s += 45598946228305920ULL;
  bottom_diag7s += 77309411328ULL;
  bottom_rows += 695784701952ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D4_D6;
  flipped = true;
}

void UndoFlip_D2D4_D6D7() {
  top_rows -= 695795318946ULL;
  left_columns -= 2130ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438902ULL;
  bottom_diag9s -= 45598946228307456ULL;
  bottom_diag7s -= 77309804544ULL;
  bottom_rows -= 695795318784ULL;
  disk_difference -= 10;
}

void Flip_D2D4_D6D7() {
  top_rows += 695795318946ULL;
  left_columns += 2130ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438902ULL;
  bottom_diag9s += 45598946228307456ULL;
  bottom_diag7s += 77309804544ULL;
  bottom_rows += 695795318784ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D4_D6D7;
  flipped = true;
}

void UndoFlip_D2D5() {
  top_rows -= 695795318946ULL;
  left_columns -= 2160ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438902ULL;
  bottom_diag9s -= 45599178155360256ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 8;
}

void Flip_D2D5() {
  top_rows += 695795318946ULL;
  left_columns += 2160ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438902ULL;
  bottom_diag9s += 45599178155360256ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D5;
  flipped = true;
}

void UndoFlip_D2D5_D7() {
  top_rows -= 695795318946ULL;
  left_columns -= 2166ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438902ULL;
  bottom_diag9s -= 45599178155361792ULL;
  bottom_diag7s -= 15199648742768640ULL;
  bottom_rows -= 45598946237743104ULL;
  disk_difference -= 10;
}

void Flip_D2D5_D7() {
  top_rows += 695795318946ULL;
  left_columns += 2166ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438902ULL;
  bottom_diag9s += 45599178155361792ULL;
  bottom_diag7s += 15199648742768640ULL;
  bottom_rows += 45598946237743104ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D5_D7;
  flipped = true;
}

void UndoFlip_D2D6() {
  top_rows -= 695795318946ULL;
  left_columns -= 2178ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438902ULL;
  bottom_diag9s -= 45599178156539904ULL;
  bottom_diag7s -= 15199726051786752ULL;
  bottom_rows -= 45599642011828224ULL;
  disk_difference -= 10;
}

void Flip_D2D6() {
  top_rows += 695795318946ULL;
  left_columns += 2178ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438902ULL;
  bottom_diag9s += 45599178156539904ULL;
  bottom_diag7s += 15199726051786752ULL;
  bottom_rows += 45599642011828224ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D6;
  flipped = true;
}

void UndoFlip_D2D7() {
  top_rows -= 695795318946ULL;
  left_columns -= 2184ULL;
  top_diag9s -= 10616994ULL;
  top_diag7s -= 59373631438902ULL;
  bottom_diag9s -= 45599178156541440ULL;
  bottom_diag7s -= 15199726052179968ULL;
  bottom_rows -= 45599642022445056ULL;
  disk_difference -= 12;
}

void Flip_D2D7() {
  top_rows += 695795318946ULL;
  left_columns += 2184ULL;
  top_diag9s += 10616994ULL;
  top_diag7s += 59373631438902ULL;
  bottom_diag9s += 45599178156541440ULL;
  bottom_diag7s += 15199726052179968ULL;
  bottom_rows += 45599642022445056ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2D7;
  flipped = true;
}

void UndoFlip_D3_D5() {
  top_rows -= 10616832ULL;
  left_columns -= 540ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538944ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_D3_D5() {
  top_rows += 10616832ULL;
  left_columns += 540ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538944ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_D5;
  flipped = true;
}

void UndoFlip_D3_D5D6() {
  top_rows -= 10616832ULL;
  left_columns -= 558ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538944ULL;
  bottom_diag9s -= 231929413632ULL;
  bottom_diag7s -= 15199726051786752ULL;
  bottom_rows -= 45599642011828224ULL;
  disk_difference -= 6;
}

void Flip_D3_D5D6() {
  top_rows += 10616832ULL;
  left_columns += 558ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538944ULL;
  bottom_diag9s += 231929413632ULL;
  bottom_diag7s += 15199726051786752ULL;
  bottom_rows += 45599642011828224ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_D5D6;
  flipped = true;
}

void UndoFlip_D3_D5D7() {
  top_rows -= 10616832ULL;
  left_columns -= 564ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538944ULL;
  bottom_diag9s -= 231929415168ULL;
  bottom_diag7s -= 15199726052179968ULL;
  bottom_rows -= 45599642022445056ULL;
  disk_difference -= 8;
}

void Flip_D3_D5D7() {
  top_rows += 10616832ULL;
  left_columns += 564ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538944ULL;
  bottom_diag9s += 231929415168ULL;
  bottom_diag7s += 15199726052179968ULL;
  bottom_rows += 45599642022445056ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_D5D7;
  flipped = true;
}

void UndoFlip_D3D4() {
  top_rows -= 10616994ULL;
  left_columns -= 648ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538998ULL;
  bottom_diag9s -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_D3D4() {
  top_rows += 10616994ULL;
  left_columns += 648ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538998ULL;
  bottom_diag9s += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D4;
  flipped = true;
}

void UndoFlip_D3D4_D6() {
  top_rows -= 10616994ULL;
  left_columns -= 666ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538998ULL;
  bottom_diag9s -= 45598946228305920ULL;
  bottom_diag7s -= 77309411328ULL;
  bottom_rows -= 695784701952ULL;
  disk_difference -= 6;
}

void Flip_D3D4_D6() {
  top_rows += 10616994ULL;
  left_columns += 666ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538998ULL;
  bottom_diag9s += 45598946228305920ULL;
  bottom_diag7s += 77309411328ULL;
  bottom_rows += 695784701952ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D4_D6;
  flipped = true;
}

void UndoFlip_D3D4_D6D7() {
  top_rows -= 10616994ULL;
  left_columns -= 672ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538998ULL;
  bottom_diag9s -= 45598946228307456ULL;
  bottom_diag7s -= 77309804544ULL;
  bottom_rows -= 695795318784ULL;
  disk_difference -= 8;
}

void Flip_D3D4_D6D7() {
  top_rows += 10616994ULL;
  left_columns += 672ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538998ULL;
  bottom_diag9s += 45598946228307456ULL;
  bottom_diag7s += 77309804544ULL;
  bottom_rows += 695795318784ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D4_D6D7;
  flipped = true;
}

void UndoFlip_D3D5() {
  top_rows -= 10616994ULL;
  left_columns -= 702ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538998ULL;
  bottom_diag9s -= 45599178155360256ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_D3D5() {
  top_rows += 10616994ULL;
  left_columns += 702ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538998ULL;
  bottom_diag9s += 45599178155360256ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D5;
  flipped = true;
}

void UndoFlip_D3D5_D7() {
  top_rows -= 10616994ULL;
  left_columns -= 708ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538998ULL;
  bottom_diag9s -= 45599178155361792ULL;
  bottom_diag7s -= 15199648742768640ULL;
  bottom_rows -= 45598946237743104ULL;
  disk_difference -= 8;
}

void Flip_D3D5_D7() {
  top_rows += 10616994ULL;
  left_columns += 708ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538998ULL;
  bottom_diag9s += 45599178155361792ULL;
  bottom_diag7s += 15199648742768640ULL;
  bottom_rows += 45598946237743104ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D5_D7;
  flipped = true;
}

void UndoFlip_D3D6() {
  top_rows -= 10616994ULL;
  left_columns -= 720ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538998ULL;
  bottom_diag9s -= 45599178156539904ULL;
  bottom_diag7s -= 15199726051786752ULL;
  bottom_rows -= 45599642011828224ULL;
  disk_difference -= 8;
}

void Flip_D3D6() {
  top_rows += 10616994ULL;
  left_columns += 720ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538998ULL;
  bottom_diag9s += 45599178156539904ULL;
  bottom_diag7s += 15199726051786752ULL;
  bottom_rows += 45599642011828224ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D6;
  flipped = true;
}

void UndoFlip_D3D7() {
  top_rows -= 10616994ULL;
  left_columns -= 726ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3538998ULL;
  bottom_diag9s -= 45599178156541440ULL;
  bottom_diag7s -= 15199726052179968ULL;
  bottom_rows -= 45599642022445056ULL;
  disk_difference -= 10;
}

void Flip_D3D7() {
  top_rows += 10616994ULL;
  left_columns += 726ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3538998ULL;
  bottom_diag9s += 45599178156541440ULL;
  bottom_diag7s += 15199726052179968ULL;
  bottom_rows += 45599642022445056ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3D7;
  flipped = true;
}

void UndoFlip_D4_D6() {
  top_rows -= 162ULL;
  left_columns -= 180ULL;
  top_diag7s -= 54ULL;
  bottom_diag9s -= 45598946228305920ULL;
  bottom_diag7s -= 77309411328ULL;
  bottom_rows -= 695784701952ULL;
  disk_difference -= 4;
}

void Flip_D4_D6() {
  top_rows += 162ULL;
  left_columns += 180ULL;
  top_diag7s += 54ULL;
  bottom_diag9s += 45598946228305920ULL;
  bottom_diag7s += 77309411328ULL;
  bottom_rows += 695784701952ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_D6;
  flipped = true;
}

void UndoFlip_D4_D6D7() {
  top_rows -= 162ULL;
  left_columns -= 186ULL;
  top_diag7s -= 54ULL;
  bottom_diag9s -= 45598946228307456ULL;
  bottom_diag7s -= 77309804544ULL;
  bottom_rows -= 695795318784ULL;
  disk_difference -= 6;
}

void Flip_D4_D6D7() {
  top_rows += 162ULL;
  left_columns += 186ULL;
  top_diag7s += 54ULL;
  bottom_diag9s += 45598946228307456ULL;
  bottom_diag7s += 77309804544ULL;
  bottom_rows += 695795318784ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_D6D7;
  flipped = true;
}

void UndoFlip_D4D5() {
  top_rows -= 162ULL;
  left_columns -= 216ULL;
  top_diag7s -= 54ULL;
  bottom_diag9s -= 45599178155360256ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_D4D5() {
  top_rows += 162ULL;
  left_columns += 216ULL;
  top_diag7s += 54ULL;
  bottom_diag9s += 45599178155360256ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4D5;
  flipped = true;
}

void UndoFlip_D4D5_D7() {
  top_rows -= 162ULL;
  left_columns -= 222ULL;
  top_diag7s -= 54ULL;
  bottom_diag9s -= 45599178155361792ULL;
  bottom_diag7s -= 15199648742768640ULL;
  bottom_rows -= 45598946237743104ULL;
  disk_difference -= 6;
}

void Flip_D4D5_D7() {
  top_rows += 162ULL;
  left_columns += 222ULL;
  top_diag7s += 54ULL;
  bottom_diag9s += 45599178155361792ULL;
  bottom_diag7s += 15199648742768640ULL;
  bottom_rows += 45598946237743104ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4D5_D7;
  flipped = true;
}

void UndoFlip_D4D6() {
  top_rows -= 162ULL;
  left_columns -= 234ULL;
  top_diag7s -= 54ULL;
  bottom_diag9s -= 45599178156539904ULL;
  bottom_diag7s -= 15199726051786752ULL;
  bottom_rows -= 45599642011828224ULL;
  disk_difference -= 6;
}

void Flip_D4D6() {
  top_rows += 162ULL;
  left_columns += 234ULL;
  top_diag7s += 54ULL;
  bottom_diag9s += 45599178156539904ULL;
  bottom_diag7s += 15199726051786752ULL;
  bottom_rows += 45599642011828224ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4D6;
  flipped = true;
}

void UndoFlip_D4D7() {
  top_rows -= 162ULL;
  left_columns -= 240ULL;
  top_diag7s -= 54ULL;
  bottom_diag9s -= 45599178156541440ULL;
  bottom_diag7s -= 15199726052179968ULL;
  bottom_rows -= 45599642022445056ULL;
  disk_difference -= 8;
}

void Flip_D4D7() {
  top_rows += 162ULL;
  left_columns += 240ULL;
  top_diag7s += 54ULL;
  bottom_diag9s += 45599178156541440ULL;
  bottom_diag7s += 15199726052179968ULL;
  bottom_rows += 45599642022445056ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4D7;
  flipped = true;
}

void UndoFlip_D5_D7() {
  left_columns -= 60ULL;
  bottom_diag9s -= 231928235520ULL;
  bottom_diag7s -= 15199648742768640ULL;
  bottom_rows -= 45598946237743104ULL;
  disk_difference -= 4;
}

void Flip_D5_D7() {
  left_columns += 60ULL;
  bottom_diag9s += 231928235520ULL;
  bottom_diag7s += 15199648742768640ULL;
  bottom_rows += 45598946237743104ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5_D7;
  flipped = true;
}

void UndoFlip_D5D6() {
  left_columns -= 72ULL;
  bottom_diag9s -= 231929413632ULL;
  bottom_diag7s -= 15199726051786752ULL;
  bottom_rows -= 45599642011828224ULL;
  disk_difference -= 4;
}

void Flip_D5D6() {
  left_columns += 72ULL;
  bottom_diag9s += 231929413632ULL;
  bottom_diag7s += 15199726051786752ULL;
  bottom_rows += 45599642011828224ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5D6;
  flipped = true;
}

void UndoFlip_D5D7() {
  left_columns -= 78ULL;
  bottom_diag9s -= 231929415168ULL;
  bottom_diag7s -= 15199726052179968ULL;
  bottom_rows -= 45599642022445056ULL;
  disk_difference -= 6;
}

void Flip_D5D7() {
  left_columns += 78ULL;
  bottom_diag9s += 231929415168ULL;
  bottom_diag7s += 15199726052179968ULL;
  bottom_rows += 45599642022445056ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5D7;
  flipped = true;
}

void UndoFlip_D6D7() {
  left_columns -= 24ULL;
  bottom_diag9s -= 1181184ULL;
  bottom_diag7s -= 77309804544ULL;
  bottom_rows -= 695795318784ULL;
  disk_difference -= 4;
}

void Flip_D6D7() {
  left_columns += 24ULL;
  bottom_diag9s += 1181184ULL;
  bottom_diag7s += 77309804544ULL;
  bottom_rows += 695795318784ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6D7;
  flipped = true;
}

void UndoFlip_E2_E4() {
  top_rows -= 231928234038ULL;
  top_diag9s -= 59373627899958ULL;
  top_diag7s -= 10616832ULL;
  right_columns -= 455989462271262720ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_E2_E4() {
  top_rows += 231928234038ULL;
  top_diag9s += 59373627899958ULL;
  top_diag7s += 10616832ULL;
  right_columns += 455989462271262720ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_E4;
  flipped = true;
}

void UndoFlip_E2_E4E5() {
  top_rows -= 231928234038ULL;
  top_diag9s -= 59373627899958ULL;
  top_diag7s -= 10616832ULL;
  right_columns -= 471189111013638144ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 45599178155360256ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 6;
}

void Flip_E2_E4E5() {
  top_rows += 231928234038ULL;
  top_diag9s += 59373627899958ULL;
  top_diag7s += 10616832ULL;
  right_columns += 471189111013638144ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 45599178155360256ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_E4E5;
  flipped = true;
}

void UndoFlip_E2_E4E6() {
  top_rows -= 231928234038ULL;
  top_diag9s -= 59373627899958ULL;
  top_diag7s -= 10616832ULL;
  right_columns -= 476255660594429952ULL;
  bottom_diag9s -= 15199726051786752ULL;
  bottom_diag7s -= 45599178156539904ULL;
  bottom_rows -= 15199880670609408ULL;
  disk_difference -= 8;
}

void Flip_E2_E4E6() {
  top_rows += 231928234038ULL;
  top_diag9s += 59373627899958ULL;
  top_diag7s += 10616832ULL;
  right_columns += 476255660594429952ULL;
  bottom_diag9s += 15199726051786752ULL;
  bottom_diag7s += 45599178156539904ULL;
  bottom_rows += 15199880670609408ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_E4E6;
  flipped = true;
}

void UndoFlip_E2_E4E7() {
  top_rows -= 231928234038ULL;
  top_diag9s -= 59373627899958ULL;
  top_diag7s -= 10616832ULL;
  right_columns -= 477944510454693888ULL;
  bottom_diag9s -= 15199726052179968ULL;
  bottom_diag7s -= 45599178156541440ULL;
  bottom_rows -= 15199880674148352ULL;
  disk_difference -= 10;
}

void Flip_E2_E4E7() {
  top_rows += 231928234038ULL;
  top_diag9s += 59373627899958ULL;
  top_diag7s += 10616832ULL;
  right_columns += 477944510454693888ULL;
  bottom_diag9s += 15199726052179968ULL;
  bottom_diag7s += 45599178156541440ULL;
  bottom_rows += 15199880674148352ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_E4E7;
  flipped = true;
}

void UndoFlip_E2E3() {
  top_rows -= 231931772928ULL;
  top_diag9s -= 59373631438848ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 547187354725515264ULL;
  disk_difference -= 4;
}

void Flip_E2E3() {
  top_rows += 231931772928ULL;
  top_diag9s += 59373631438848ULL;
  top_diag7s += 10616994ULL;
  right_columns += 547187354725515264ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E3;
  flipped = true;
}

void UndoFlip_E2E3_E5() {
  top_rows -= 231931772928ULL;
  top_diag9s -= 59373631438848ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 562387003467890688ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 6;
}

void Flip_E2E3_E5() {
  top_rows += 231931772928ULL;
  top_diag9s += 59373631438848ULL;
  top_diag7s += 10616994ULL;
  right_columns += 562387003467890688ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E3_E5;
  flipped = true;
}

void UndoFlip_E2E3_E5E6() {
  top_rows -= 231931772928ULL;
  top_diag9s -= 59373631438848ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 567453553048682496ULL;
  bottom_diag9s -= 15199726051786752ULL;
  bottom_diag7s -= 231929413632ULL;
  bottom_rows -= 15199880670609408ULL;
  disk_difference -= 8;
}

void Flip_E2E3_E5E6() {
  top_rows += 231931772928ULL;
  top_diag9s += 59373631438848ULL;
  top_diag7s += 10616994ULL;
  right_columns += 567453553048682496ULL;
  bottom_diag9s += 15199726051786752ULL;
  bottom_diag7s += 231929413632ULL;
  bottom_rows += 15199880670609408ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E3_E5E6;
  flipped = true;
}

void UndoFlip_E2E3_E5E7() {
  top_rows -= 231931772928ULL;
  top_diag9s -= 59373631438848ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 569142402908946432ULL;
  bottom_diag9s -= 15199726052179968ULL;
  bottom_diag7s -= 231929415168ULL;
  bottom_rows -= 15199880674148352ULL;
  disk_difference -= 10;
}

void Flip_E2E3_E5E7() {
  top_rows += 231931772928ULL;
  top_diag9s += 59373631438848ULL;
  top_diag7s += 10616994ULL;
  right_columns += 569142402908946432ULL;
  bottom_diag9s += 15199726052179968ULL;
  bottom_diag7s += 231929415168ULL;
  bottom_rows += 15199880674148352ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E3_E5E7;
  flipped = true;
}

void UndoFlip_E2E4() {
  top_rows -= 231931772982ULL;
  top_diag9s -= 59373631438902ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 592786300952641536ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_E2E4() {
  top_rows += 231931772982ULL;
  top_diag9s += 59373631438902ULL;
  top_diag7s += 10616994ULL;
  right_columns += 592786300952641536ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E4;
  flipped = true;
}

void UndoFlip_E2E4_E6() {
  top_rows -= 231931772982ULL;
  top_diag9s -= 59373631438902ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 597852850533433344ULL;
  bottom_diag9s -= 77309411328ULL;
  bottom_diag7s -= 45598946228305920ULL;
  bottom_rows -= 231928233984ULL;
  disk_difference -= 8;
}

void Flip_E2E4_E6() {
  top_rows += 231931772982ULL;
  top_diag9s += 59373631438902ULL;
  top_diag7s += 10616994ULL;
  right_columns += 597852850533433344ULL;
  bottom_diag9s += 77309411328ULL;
  bottom_diag7s += 45598946228305920ULL;
  bottom_rows += 231928233984ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E4_E6;
  flipped = true;
}

void UndoFlip_E2E4_E6E7() {
  top_rows -= 231931772982ULL;
  top_diag9s -= 59373631438902ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 599541700393697280ULL;
  bottom_diag9s -= 77309804544ULL;
  bottom_diag7s -= 45598946228307456ULL;
  bottom_rows -= 231931772928ULL;
  disk_difference -= 10;
}

void Flip_E2E4_E6E7() {
  top_rows += 231931772982ULL;
  top_diag9s += 59373631438902ULL;
  top_diag7s += 10616994ULL;
  right_columns += 599541700393697280ULL;
  bottom_diag9s += 77309804544ULL;
  bottom_diag7s += 45598946228307456ULL;
  bottom_rows += 231931772928ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E4_E6E7;
  flipped = true;
}

void UndoFlip_E2E5() {
  top_rows -= 231931772982ULL;
  top_diag9s -= 59373631438902ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 607985949695016960ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 45599178155360256ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 8;
}

void Flip_E2E5() {
  top_rows += 231931772982ULL;
  top_diag9s += 59373631438902ULL;
  top_diag7s += 10616994ULL;
  right_columns += 607985949695016960ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 45599178155360256ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E5;
  flipped = true;
}

void UndoFlip_E2E5_E7() {
  top_rows -= 231931772982ULL;
  top_diag9s -= 59373631438902ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 609674799555280896ULL;
  bottom_diag9s -= 15199648742768640ULL;
  bottom_diag7s -= 45599178155361792ULL;
  bottom_rows -= 15199648745914368ULL;
  disk_difference -= 10;
}

void Flip_E2E5_E7() {
  top_rows += 231931772982ULL;
  top_diag9s += 59373631438902ULL;
  top_diag7s += 10616994ULL;
  right_columns += 609674799555280896ULL;
  bottom_diag9s += 15199648742768640ULL;
  bottom_diag7s += 45599178155361792ULL;
  bottom_rows += 15199648745914368ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E5_E7;
  flipped = true;
}

void UndoFlip_E2E6() {
  top_rows -= 231931772982ULL;
  top_diag9s -= 59373631438902ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 613052499275808768ULL;
  bottom_diag9s -= 15199726051786752ULL;
  bottom_diag7s -= 45599178156539904ULL;
  bottom_rows -= 15199880670609408ULL;
  disk_difference -= 10;
}

void Flip_E2E6() {
  top_rows += 231931772982ULL;
  top_diag9s += 59373631438902ULL;
  top_diag7s += 10616994ULL;
  right_columns += 613052499275808768ULL;
  bottom_diag9s += 15199726051786752ULL;
  bottom_diag7s += 45599178156539904ULL;
  bottom_rows += 15199880670609408ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E6;
  flipped = true;
}

void UndoFlip_E2E7() {
  top_rows -= 231931772982ULL;
  top_diag9s -= 59373631438902ULL;
  top_diag7s -= 10616994ULL;
  right_columns -= 614741349136072704ULL;
  bottom_diag9s -= 15199726052179968ULL;
  bottom_diag7s -= 45599178156541440ULL;
  bottom_rows -= 15199880674148352ULL;
  disk_difference -= 12;
}

void Flip_E2E7() {
  top_rows += 231931772982ULL;
  top_diag9s += 59373631438902ULL;
  top_diag7s += 10616994ULL;
  right_columns += 614741349136072704ULL;
  bottom_diag9s += 15199726052179968ULL;
  bottom_diag7s += 45599178156541440ULL;
  bottom_rows += 15199880674148352ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2E7;
  flipped = true;
}

void UndoFlip_E3_E5() {
  top_rows -= 3538944ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 162ULL;
  right_columns -= 151996487423754240ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 4;
}

void Flip_E3_E5() {
  top_rows += 3538944ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 162ULL;
  right_columns += 151996487423754240ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_E5;
  flipped = true;
}

void UndoFlip_E3_E5E6() {
  top_rows -= 3538944ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 162ULL;
  right_columns -= 157063037004546048ULL;
  bottom_diag9s -= 15199726051786752ULL;
  bottom_diag7s -= 231929413632ULL;
  bottom_rows -= 15199880670609408ULL;
  disk_difference -= 6;
}

void Flip_E3_E5E6() {
  top_rows += 3538944ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 162ULL;
  right_columns += 157063037004546048ULL;
  bottom_diag9s += 15199726051786752ULL;
  bottom_diag7s += 231929413632ULL;
  bottom_rows += 15199880670609408ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_E5E6;
  flipped = true;
}

void UndoFlip_E3_E5E7() {
  top_rows -= 3538944ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 162ULL;
  right_columns -= 158751886864809984ULL;
  bottom_diag9s -= 15199726052179968ULL;
  bottom_diag7s -= 231929415168ULL;
  bottom_rows -= 15199880674148352ULL;
  disk_difference -= 8;
}

void Flip_E3_E5E7() {
  top_rows += 3538944ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 162ULL;
  right_columns += 158751886864809984ULL;
  bottom_diag9s += 15199726052179968ULL;
  bottom_diag7s += 231929415168ULL;
  bottom_rows += 15199880674148352ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_E5E7;
  flipped = true;
}

void UndoFlip_E3E4() {
  top_rows -= 3538998ULL;
  top_diag9s -= 3538998ULL;
  top_diag7s -= 162ULL;
  right_columns -= 182395784908505088ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_E3E4() {
  top_rows += 3538998ULL;
  top_diag9s += 3538998ULL;
  top_diag7s += 162ULL;
  right_columns += 182395784908505088ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E4;
  flipped = true;
}

void UndoFlip_E3E4_E6() {
  top_rows -= 3538998ULL;
  top_diag9s -= 3538998ULL;
  top_diag7s -= 162ULL;
  right_columns -= 187462334489296896ULL;
  bottom_diag9s -= 77309411328ULL;
  bottom_diag7s -= 45598946228305920ULL;
  bottom_rows -= 231928233984ULL;
  disk_difference -= 6;
}

void Flip_E3E4_E6() {
  top_rows += 3538998ULL;
  top_diag9s += 3538998ULL;
  top_diag7s += 162ULL;
  right_columns += 187462334489296896ULL;
  bottom_diag9s += 77309411328ULL;
  bottom_diag7s += 45598946228305920ULL;
  bottom_rows += 231928233984ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E4_E6;
  flipped = true;
}

void UndoFlip_E3E4_E6E7() {
  top_rows -= 3538998ULL;
  top_diag9s -= 3538998ULL;
  top_diag7s -= 162ULL;
  right_columns -= 189151184349560832ULL;
  bottom_diag9s -= 77309804544ULL;
  bottom_diag7s -= 45598946228307456ULL;
  bottom_rows -= 231931772928ULL;
  disk_difference -= 8;
}

void Flip_E3E4_E6E7() {
  top_rows += 3538998ULL;
  top_diag9s += 3538998ULL;
  top_diag7s += 162ULL;
  right_columns += 189151184349560832ULL;
  bottom_diag9s += 77309804544ULL;
  bottom_diag7s += 45598946228307456ULL;
  bottom_rows += 231931772928ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E4_E6E7;
  flipped = true;
}

void UndoFlip_E3E5() {
  top_rows -= 3538998ULL;
  top_diag9s -= 3538998ULL;
  top_diag7s -= 162ULL;
  right_columns -= 197595433650880512ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 45599178155360256ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 6;
}

void Flip_E3E5() {
  top_rows += 3538998ULL;
  top_diag9s += 3538998ULL;
  top_diag7s += 162ULL;
  right_columns += 197595433650880512ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 45599178155360256ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E5;
  flipped = true;
}

void UndoFlip_E3E5_E7() {
  top_rows -= 3538998ULL;
  top_diag9s -= 3538998ULL;
  top_diag7s -= 162ULL;
  right_columns -= 199284283511144448ULL;
  bottom_diag9s -= 15199648742768640ULL;
  bottom_diag7s -= 45599178155361792ULL;
  bottom_rows -= 15199648745914368ULL;
  disk_difference -= 8;
}

void Flip_E3E5_E7() {
  top_rows += 3538998ULL;
  top_diag9s += 3538998ULL;
  top_diag7s += 162ULL;
  right_columns += 199284283511144448ULL;
  bottom_diag9s += 15199648742768640ULL;
  bottom_diag7s += 45599178155361792ULL;
  bottom_rows += 15199648745914368ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E5_E7;
  flipped = true;
}

void UndoFlip_E3E6() {
  top_rows -= 3538998ULL;
  top_diag9s -= 3538998ULL;
  top_diag7s -= 162ULL;
  right_columns -= 202661983231672320ULL;
  bottom_diag9s -= 15199726051786752ULL;
  bottom_diag7s -= 45599178156539904ULL;
  bottom_rows -= 15199880670609408ULL;
  disk_difference -= 8;
}

void Flip_E3E6() {
  top_rows += 3538998ULL;
  top_diag9s += 3538998ULL;
  top_diag7s += 162ULL;
  right_columns += 202661983231672320ULL;
  bottom_diag9s += 15199726051786752ULL;
  bottom_diag7s += 45599178156539904ULL;
  bottom_rows += 15199880670609408ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E6;
  flipped = true;
}

void UndoFlip_E3E7() {
  top_rows -= 3538998ULL;
  top_diag9s -= 3538998ULL;
  top_diag7s -= 162ULL;
  right_columns -= 204350833091936256ULL;
  bottom_diag9s -= 15199726052179968ULL;
  bottom_diag7s -= 45599178156541440ULL;
  bottom_rows -= 15199880674148352ULL;
  disk_difference -= 10;
}

void Flip_E3E7() {
  top_rows += 3538998ULL;
  top_diag9s += 3538998ULL;
  top_diag7s += 162ULL;
  right_columns += 204350833091936256ULL;
  bottom_diag9s += 15199726052179968ULL;
  bottom_diag7s += 45599178156541440ULL;
  bottom_rows += 15199880674148352ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3E7;
  flipped = true;
}

void UndoFlip_E4_E6() {
  top_rows -= 54ULL;
  top_diag9s -= 54ULL;
  right_columns -= 50665495807918080ULL;
  bottom_diag9s -= 77309411328ULL;
  bottom_diag7s -= 45598946228305920ULL;
  bottom_rows -= 231928233984ULL;
  disk_difference -= 4;
}

void Flip_E4_E6() {
  top_rows += 54ULL;
  top_diag9s += 54ULL;
  right_columns += 50665495807918080ULL;
  bottom_diag9s += 77309411328ULL;
  bottom_diag7s += 45598946228305920ULL;
  bottom_rows += 231928233984ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_E6;
  flipped = true;
}

void UndoFlip_E4_E6E7() {
  top_rows -= 54ULL;
  top_diag9s -= 54ULL;
  right_columns -= 52354345668182016ULL;
  bottom_diag9s -= 77309804544ULL;
  bottom_diag7s -= 45598946228307456ULL;
  bottom_rows -= 231931772928ULL;
  disk_difference -= 6;
}

void Flip_E4_E6E7() {
  top_rows += 54ULL;
  top_diag9s += 54ULL;
  right_columns += 52354345668182016ULL;
  bottom_diag9s += 77309804544ULL;
  bottom_diag7s += 45598946228307456ULL;
  bottom_rows += 231931772928ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_E6E7;
  flipped = true;
}

void UndoFlip_E4E5() {
  top_rows -= 54ULL;
  top_diag9s -= 54ULL;
  right_columns -= 60798594969501696ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 45599178155360256ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 4;
}

void Flip_E4E5() {
  top_rows += 54ULL;
  top_diag9s += 54ULL;
  right_columns += 60798594969501696ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 45599178155360256ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4E5;
  flipped = true;
}

void UndoFlip_E4E5_E7() {
  top_rows -= 54ULL;
  top_diag9s -= 54ULL;
  right_columns -= 62487444829765632ULL;
  bottom_diag9s -= 15199648742768640ULL;
  bottom_diag7s -= 45599178155361792ULL;
  bottom_rows -= 15199648745914368ULL;
  disk_difference -= 6;
}

void Flip_E4E5_E7() {
  top_rows += 54ULL;
  top_diag9s += 54ULL;
  right_columns += 62487444829765632ULL;
  bottom_diag9s += 15199648742768640ULL;
  bottom_diag7s += 45599178155361792ULL;
  bottom_rows += 15199648745914368ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4E5_E7;
  flipped = true;
}

void UndoFlip_E4E6() {
  top_rows -= 54ULL;
  top_diag9s -= 54ULL;
  right_columns -= 65865144550293504ULL;
  bottom_diag9s -= 15199726051786752ULL;
  bottom_diag7s -= 45599178156539904ULL;
  bottom_rows -= 15199880670609408ULL;
  disk_difference -= 6;
}

void Flip_E4E6() {
  top_rows += 54ULL;
  top_diag9s += 54ULL;
  right_columns += 65865144550293504ULL;
  bottom_diag9s += 15199726051786752ULL;
  bottom_diag7s += 45599178156539904ULL;
  bottom_rows += 15199880670609408ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4E6;
  flipped = true;
}

void UndoFlip_E4E7() {
  top_rows -= 54ULL;
  top_diag9s -= 54ULL;
  right_columns -= 67553994410557440ULL;
  bottom_diag9s -= 15199726052179968ULL;
  bottom_diag7s -= 45599178156541440ULL;
  bottom_rows -= 15199880674148352ULL;
  disk_difference -= 8;
}

void Flip_E4E7() {
  top_rows += 54ULL;
  top_diag9s += 54ULL;
  right_columns += 67553994410557440ULL;
  bottom_diag9s += 15199726052179968ULL;
  bottom_diag7s += 45599178156541440ULL;
  bottom_rows += 15199880674148352ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4E7;
  flipped = true;
}

void UndoFlip_E5_E7() {
  right_columns -= 16888498602639360ULL;
  bottom_diag9s -= 15199648742768640ULL;
  bottom_diag7s -= 231928235520ULL;
  bottom_rows -= 15199648745914368ULL;
  disk_difference -= 4;
}

void Flip_E5_E7() {
  right_columns += 16888498602639360ULL;
  bottom_diag9s += 15199648742768640ULL;
  bottom_diag7s += 231928235520ULL;
  bottom_rows += 15199648745914368ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5_E7;
  flipped = true;
}

void UndoFlip_E5E6() {
  right_columns -= 20266198323167232ULL;
  bottom_diag9s -= 15199726051786752ULL;
  bottom_diag7s -= 231929413632ULL;
  bottom_rows -= 15199880670609408ULL;
  disk_difference -= 4;
}

void Flip_E5E6() {
  right_columns += 20266198323167232ULL;
  bottom_diag9s += 15199726051786752ULL;
  bottom_diag7s += 231929413632ULL;
  bottom_rows += 15199880670609408ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5E6;
  flipped = true;
}

void UndoFlip_E5E7() {
  right_columns -= 21955048183431168ULL;
  bottom_diag9s -= 15199726052179968ULL;
  bottom_diag7s -= 231929415168ULL;
  bottom_rows -= 15199880674148352ULL;
  disk_difference -= 6;
}

void Flip_E5E7() {
  right_columns += 21955048183431168ULL;
  bottom_diag9s += 15199726052179968ULL;
  bottom_diag7s += 231929415168ULL;
  bottom_rows += 15199880674148352ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5E7;
  flipped = true;
}

void UndoFlip_E6E7() {
  right_columns -= 6755399441055744ULL;
  bottom_diag9s -= 77309804544ULL;
  bottom_diag7s -= 1181184ULL;
  bottom_rows -= 231931772928ULL;
  disk_difference -= 4;
}

void Flip_E6E7() {
  right_columns += 6755399441055744ULL;
  bottom_diag9s += 77309804544ULL;
  bottom_diag7s += 1181184ULL;
  bottom_rows += 231931772928ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6E7;
  flipped = true;
}

void UndoFlip_F2_F4() {
  top_rows -= 77309411346ULL;
  top_diag9s -= 5066549581971456ULL;
  top_diag7s -= 486ULL;
  right_columns -= 6957847019520ULL;
  bottom_diag7s -= 695784701952ULL;
  disk_difference -= 4;
}

void Flip_F2_F4() {
  top_rows += 77309411346ULL;
  top_diag9s += 5066549581971456ULL;
  top_diag7s += 486ULL;
  right_columns += 6957847019520ULL;
  bottom_diag7s += 695784701952ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_F4;
  flipped = true;
}

void UndoFlip_F2_F4F5() {
  top_rows -= 77309411346ULL;
  top_diag9s -= 5066549581971474ULL;
  top_diag7s -= 486ULL;
  right_columns -= 7189775253504ULL;
  bottom_diag7s -= 695788240896ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 6;
}

void Flip_F2_F4F5() {
  top_rows += 77309411346ULL;
  top_diag9s += 5066549581971474ULL;
  top_diag7s += 486ULL;
  right_columns += 7189775253504ULL;
  bottom_diag7s += 695788240896ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_F4F5;
  flipped = true;
}

void UndoFlip_F2_F4F6() {
  top_rows -= 77309411346ULL;
  top_diag9s -= 5066549581971474ULL;
  top_diag7s -= 486ULL;
  right_columns -= 7267084664832ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 695788245504ULL;
  bottom_rows -= 5066626890203136ULL;
  disk_difference -= 8;
}

void Flip_F2_F4F6() {
  top_rows += 77309411346ULL;
  top_diag9s += 5066549581971474ULL;
  top_diag7s += 486ULL;
  right_columns += 7267084664832ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 695788245504ULL;
  bottom_rows += 5066626890203136ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_F4F6;
  flipped = true;
}

void UndoFlip_F2_F4F7() {
  top_rows -= 77309411346ULL;
  top_diag9s -= 5066549581971474ULL;
  top_diag7s -= 486ULL;
  right_columns -= 7292854468608ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 695788245510ULL;
  bottom_rows -= 5066626891382784ULL;
  disk_difference -= 10;
}

void Flip_F2_F4F7() {
  top_rows += 77309411346ULL;
  top_diag9s += 5066549581971474ULL;
  top_diag7s += 486ULL;
  right_columns += 7292854468608ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 695788245510ULL;
  bottom_rows += 5066626891382784ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_F4F7;
  flipped = true;
}

void UndoFlip_F2F3() {
  top_rows -= 77310590976ULL;
  top_diag9s -= 5086340790091776ULL;
  top_diag7s -= 486ULL;
  right_columns -= 8349416423424ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_F2F3() {
  top_rows += 77310590976ULL;
  top_diag9s += 5086340790091776ULL;
  top_diag7s += 486ULL;
  right_columns += 8349416423424ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F3;
  flipped = true;
}

void UndoFlip_F2F3_F5() {
  top_rows -= 77310590976ULL;
  top_diag9s -= 5086340790091794ULL;
  top_diag7s -= 486ULL;
  right_columns -= 8581344657408ULL;
  bottom_diag7s -= 136796838684917760ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 6;
}

void Flip_F2F3_F5() {
  top_rows += 77310590976ULL;
  top_diag9s += 5086340790091794ULL;
  top_diag7s += 486ULL;
  right_columns += 8581344657408ULL;
  bottom_diag7s += 136796838684917760ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F3_F5;
  flipped = true;
}

void UndoFlip_F2F3_F5F6() {
  top_rows -= 77310590976ULL;
  top_diag9s -= 5086340790091794ULL;
  top_diag7s -= 486ULL;
  right_columns -= 8658654068736ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 136796838684922368ULL;
  bottom_rows -= 5066626890203136ULL;
  disk_difference -= 8;
}

void Flip_F2F3_F5F6() {
  top_rows += 77310590976ULL;
  top_diag9s += 5086340790091794ULL;
  top_diag7s += 486ULL;
  right_columns += 8658654068736ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 136796838684922368ULL;
  bottom_rows += 5066626890203136ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F3_F5F6;
  flipped = true;
}

void UndoFlip_F2F3_F5F7() {
  top_rows -= 77310590976ULL;
  top_diag9s -= 5086340790091794ULL;
  top_diag7s -= 486ULL;
  right_columns -= 8684423872512ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 136796838684922374ULL;
  bottom_rows -= 5066626891382784ULL;
  disk_difference -= 10;
}

void Flip_F2F3_F5F7() {
  top_rows += 77310590976ULL;
  top_diag9s += 5086340790091794ULL;
  top_diag7s += 486ULL;
  right_columns += 8684423872512ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 136796838684922374ULL;
  bottom_rows += 5066626891382784ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F3_F5F7;
  flipped = true;
}

void UndoFlip_F2F4() {
  top_rows -= 77310590994ULL;
  top_diag9s -= 5086340791271424ULL;
  top_diag7s -= 486ULL;
  right_columns -= 9045201125376ULL;
  bottom_diag7s -= 136797534466080768ULL;
  disk_difference -= 6;
}

void Flip_F2F4() {
  top_rows += 77310590994ULL;
  top_diag9s += 5086340791271424ULL;
  top_diag7s += 486ULL;
  right_columns += 9045201125376ULL;
  bottom_diag7s += 136797534466080768ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F4;
  flipped = true;
}

void UndoFlip_F2F4_F6() {
  top_rows -= 77310590994ULL;
  top_diag9s -= 5086340791271424ULL;
  top_diag7s -= 486ULL;
  right_columns -= 9122510536704ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 136797534466085376ULL;
  bottom_rows -= 77309411328ULL;
  disk_difference -= 8;
}

void Flip_F2F4_F6() {
  top_rows += 77310590994ULL;
  top_diag9s += 5086340791271424ULL;
  top_diag7s += 486ULL;
  right_columns += 9122510536704ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 136797534466085376ULL;
  bottom_rows += 77309411328ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F4_F6;
  flipped = true;
}

void UndoFlip_F2F4_F6F7() {
  top_rows -= 77310590994ULL;
  top_diag9s -= 5086340791271424ULL;
  top_diag7s -= 486ULL;
  right_columns -= 9148280340480ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 136797534466085382ULL;
  bottom_rows -= 77310590976ULL;
  disk_difference -= 10;
}

void Flip_F2F4_F6F7() {
  top_rows += 77310590994ULL;
  top_diag9s += 5086340791271424ULL;
  top_diag7s += 486ULL;
  right_columns += 9148280340480ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 136797534466085382ULL;
  bottom_rows += 77310590976ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F4_F6F7;
  flipped = true;
}

void UndoFlip_F2F5() {
  top_rows -= 77310590994ULL;
  top_diag9s -= 5086340791271442ULL;
  top_diag7s -= 486ULL;
  right_columns -= 9277129359360ULL;
  bottom_diag7s -= 136797534469619712ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 8;
}

void Flip_F2F5() {
  top_rows += 77310590994ULL;
  top_diag9s += 5086340791271442ULL;
  top_diag7s += 486ULL;
  right_columns += 9277129359360ULL;
  bottom_diag7s += 136797534469619712ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F5;
  flipped = true;
}

void UndoFlip_F2F5_F7() {
  top_rows -= 77310590994ULL;
  top_diag9s -= 5086340791271442ULL;
  top_diag7s -= 486ULL;
  right_columns -= 9302899163136ULL;
  bottom_diag9s -= 25769803776ULL;
  bottom_diag7s -= 136797534469619718ULL;
  bottom_rows -= 5066549581971456ULL;
  disk_difference -= 10;
}

void Flip_F2F5_F7() {
  top_rows += 77310590994ULL;
  top_diag9s += 5086340791271442ULL;
  top_diag7s += 486ULL;
  right_columns += 9302899163136ULL;
  bottom_diag9s += 25769803776ULL;
  bottom_diag7s += 136797534469619718ULL;
  bottom_rows += 5066549581971456ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F5_F7;
  flipped = true;
}

void UndoFlip_F2F6() {
  top_rows -= 77310590994ULL;
  top_diag9s -= 5086340791271442ULL;
  top_diag7s -= 486ULL;
  right_columns -= 9354438770688ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 136797534469624320ULL;
  bottom_rows -= 5066626890203136ULL;
  disk_difference -= 10;
}

void Flip_F2F6() {
  top_rows += 77310590994ULL;
  top_diag9s += 5086340791271442ULL;
  top_diag7s += 486ULL;
  right_columns += 9354438770688ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 136797534469624320ULL;
  bottom_rows += 5066626890203136ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F6;
  flipped = true;
}

void UndoFlip_F2F7() {
  top_rows -= 77310590994ULL;
  top_diag9s -= 5086340791271442ULL;
  top_diag7s -= 486ULL;
  right_columns -= 9380208574464ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 136797534469624326ULL;
  bottom_rows -= 5066626891382784ULL;
  disk_difference -= 12;
}

void Flip_F2F7() {
  top_rows += 77310590994ULL;
  top_diag9s += 5086340791271442ULL;
  top_diag7s += 486ULL;
  right_columns += 9380208574464ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 136797534469624326ULL;
  bottom_rows += 5066626891382784ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2F7;
  flipped = true;
}

void UndoFlip_F3_F5() {
  top_rows -= 1179648ULL;
  top_diag9s -= 19791209299986ULL;
  right_columns -= 2319282339840ULL;
  bottom_diag7s -= 136796838684917760ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 4;
}

void Flip_F3_F5() {
  top_rows += 1179648ULL;
  top_diag9s += 19791209299986ULL;
  right_columns += 2319282339840ULL;
  bottom_diag7s += 136796838684917760ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_F5;
  flipped = true;
}

void UndoFlip_F3_F5F6() {
  top_rows -= 1179648ULL;
  top_diag9s -= 19791209299986ULL;
  right_columns -= 2396591751168ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 136796838684922368ULL;
  bottom_rows -= 5066626890203136ULL;
  disk_difference -= 6;
}

void Flip_F3_F5F6() {
  top_rows += 1179648ULL;
  top_diag9s += 19791209299986ULL;
  right_columns += 2396591751168ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 136796838684922368ULL;
  bottom_rows += 5066626890203136ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_F5F6;
  flipped = true;
}

void UndoFlip_F3_F5F7() {
  top_rows -= 1179648ULL;
  top_diag9s -= 19791209299986ULL;
  right_columns -= 2422361554944ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 136796838684922374ULL;
  bottom_rows -= 5066626891382784ULL;
  disk_difference -= 8;
}

void Flip_F3_F5F7() {
  top_rows += 1179648ULL;
  top_diag9s += 19791209299986ULL;
  right_columns += 2422361554944ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 136796838684922374ULL;
  bottom_rows += 5066626891382784ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_F5F7;
  flipped = true;
}

void UndoFlip_F3F4() {
  top_rows -= 1179666ULL;
  top_diag9s -= 19791210479616ULL;
  right_columns -= 2783138807808ULL;
  bottom_diag7s -= 136797534466080768ULL;
  disk_difference -= 4;
}

void Flip_F3F4() {
  top_rows += 1179666ULL;
  top_diag9s += 19791210479616ULL;
  right_columns += 2783138807808ULL;
  bottom_diag7s += 136797534466080768ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F4;
  flipped = true;
}

void UndoFlip_F3F4_F6() {
  top_rows -= 1179666ULL;
  top_diag9s -= 19791210479616ULL;
  right_columns -= 2860448219136ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 136797534466085376ULL;
  bottom_rows -= 77309411328ULL;
  disk_difference -= 6;
}

void Flip_F3F4_F6() {
  top_rows += 1179666ULL;
  top_diag9s += 19791210479616ULL;
  right_columns += 2860448219136ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 136797534466085376ULL;
  bottom_rows += 77309411328ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F4_F6;
  flipped = true;
}

void UndoFlip_F3F4_F6F7() {
  top_rows -= 1179666ULL;
  top_diag9s -= 19791210479616ULL;
  right_columns -= 2886218022912ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 136797534466085382ULL;
  bottom_rows -= 77310590976ULL;
  disk_difference -= 8;
}

void Flip_F3F4_F6F7() {
  top_rows += 1179666ULL;
  top_diag9s += 19791210479616ULL;
  right_columns += 2886218022912ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 136797534466085382ULL;
  bottom_rows += 77310590976ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F4_F6F7;
  flipped = true;
}

void UndoFlip_F3F5() {
  top_rows -= 1179666ULL;
  top_diag9s -= 19791210479634ULL;
  right_columns -= 3015067041792ULL;
  bottom_diag7s -= 136797534469619712ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 6;
}

void Flip_F3F5() {
  top_rows += 1179666ULL;
  top_diag9s += 19791210479634ULL;
  right_columns += 3015067041792ULL;
  bottom_diag7s += 136797534469619712ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F5;
  flipped = true;
}

void UndoFlip_F3F5_F7() {
  top_rows -= 1179666ULL;
  top_diag9s -= 19791210479634ULL;
  right_columns -= 3040836845568ULL;
  bottom_diag9s -= 25769803776ULL;
  bottom_diag7s -= 136797534469619718ULL;
  bottom_rows -= 5066549581971456ULL;
  disk_difference -= 8;
}

void Flip_F3F5_F7() {
  top_rows += 1179666ULL;
  top_diag9s += 19791210479634ULL;
  right_columns += 3040836845568ULL;
  bottom_diag9s += 25769803776ULL;
  bottom_diag7s += 136797534469619718ULL;
  bottom_rows += 5066549581971456ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F5_F7;
  flipped = true;
}

void UndoFlip_F3F6() {
  top_rows -= 1179666ULL;
  top_diag9s -= 19791210479634ULL;
  right_columns -= 3092376453120ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 136797534469624320ULL;
  bottom_rows -= 5066626890203136ULL;
  disk_difference -= 8;
}

void Flip_F3F6() {
  top_rows += 1179666ULL;
  top_diag9s += 19791210479634ULL;
  right_columns += 3092376453120ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 136797534469624320ULL;
  bottom_rows += 5066626890203136ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F6;
  flipped = true;
}

void UndoFlip_F3F7() {
  top_rows -= 1179666ULL;
  top_diag9s -= 19791210479634ULL;
  right_columns -= 3118146256896ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 136797534469624326ULL;
  bottom_rows -= 5066626891382784ULL;
  disk_difference -= 10;
}

void Flip_F3F7() {
  top_rows += 1179666ULL;
  top_diag9s += 19791210479634ULL;
  right_columns += 3118146256896ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 136797534469624326ULL;
  bottom_rows += 5066626891382784ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3F7;
  flipped = true;
}

void UndoFlip_F4_F6() {
  top_rows -= 18ULL;
  top_diag9s -= 1179648ULL;
  right_columns -= 773094113280ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 695784706560ULL;
  bottom_rows -= 77309411328ULL;
  disk_difference -= 4;
}

void Flip_F4_F6() {
  top_rows += 18ULL;
  top_diag9s += 1179648ULL;
  right_columns += 773094113280ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 695784706560ULL;
  bottom_rows += 77309411328ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4_F6;
  flipped = true;
}

void UndoFlip_F4_F6F7() {
  top_rows -= 18ULL;
  top_diag9s -= 1179648ULL;
  right_columns -= 798863917056ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 695784706566ULL;
  bottom_rows -= 77310590976ULL;
  disk_difference -= 6;
}

void Flip_F4_F6F7() {
  top_rows += 18ULL;
  top_diag9s += 1179648ULL;
  right_columns += 798863917056ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 695784706566ULL;
  bottom_rows += 77310590976ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4_F6F7;
  flipped = true;
}

void UndoFlip_F4F5() {
  top_rows -= 18ULL;
  top_diag9s -= 1179666ULL;
  right_columns -= 927712935936ULL;
  bottom_diag7s -= 695788240896ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 4;
}

void Flip_F4F5() {
  top_rows += 18ULL;
  top_diag9s += 1179666ULL;
  right_columns += 927712935936ULL;
  bottom_diag7s += 695788240896ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4F5;
  flipped = true;
}

void UndoFlip_F4F5_F7() {
  top_rows -= 18ULL;
  top_diag9s -= 1179666ULL;
  right_columns -= 953482739712ULL;
  bottom_diag9s -= 25769803776ULL;
  bottom_diag7s -= 695788240902ULL;
  bottom_rows -= 5066549581971456ULL;
  disk_difference -= 6;
}

void Flip_F4F5_F7() {
  top_rows += 18ULL;
  top_diag9s += 1179666ULL;
  right_columns += 953482739712ULL;
  bottom_diag9s += 25769803776ULL;
  bottom_diag7s += 695788240902ULL;
  bottom_rows += 5066549581971456ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4F5_F7;
  flipped = true;
}

void UndoFlip_F4F6() {
  top_rows -= 18ULL;
  top_diag9s -= 1179666ULL;
  right_columns -= 1005022347264ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 695788245504ULL;
  bottom_rows -= 5066626890203136ULL;
  disk_difference -= 6;
}

void Flip_F4F6() {
  top_rows += 18ULL;
  top_diag9s += 1179666ULL;
  right_columns += 1005022347264ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 695788245504ULL;
  bottom_rows += 5066626890203136ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4F6;
  flipped = true;
}

void UndoFlip_F4F7() {
  top_rows -= 18ULL;
  top_diag9s -= 1179666ULL;
  right_columns -= 1030792151040ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 695788245510ULL;
  bottom_rows -= 5066626891382784ULL;
  disk_difference -= 8;
}

void Flip_F4F7() {
  top_rows += 18ULL;
  top_diag9s += 1179666ULL;
  right_columns += 1030792151040ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 695788245510ULL;
  bottom_rows += 5066626891382784ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4F7;
  flipped = true;
}

void UndoFlip_F5_F7() {
  top_diag9s -= 18ULL;
  right_columns -= 257698037760ULL;
  bottom_diag9s -= 25769803776ULL;
  bottom_diag7s -= 3538950ULL;
  bottom_rows -= 5066549581971456ULL;
  disk_difference -= 4;
}

void Flip_F5_F7() {
  top_diag9s += 18ULL;
  right_columns += 257698037760ULL;
  bottom_diag9s += 25769803776ULL;
  bottom_diag7s += 3538950ULL;
  bottom_rows += 5066549581971456ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5_F7;
  flipped = true;
}

void UndoFlip_F5F6() {
  top_diag9s -= 18ULL;
  right_columns -= 309237645312ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 3543552ULL;
  bottom_rows -= 5066626890203136ULL;
  disk_difference -= 4;
}

void Flip_F5F6() {
  top_diag9s += 18ULL;
  right_columns += 309237645312ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 3543552ULL;
  bottom_rows += 5066626890203136ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5F6;
  flipped = true;
}

void UndoFlip_F5F7() {
  top_diag9s -= 18ULL;
  right_columns -= 335007449088ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 3543558ULL;
  bottom_rows -= 5066626891382784ULL;
  disk_difference -= 6;
}

void Flip_F5F7() {
  top_diag9s += 18ULL;
  right_columns += 335007449088ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 3543558ULL;
  bottom_rows += 5066626891382784ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5F7;
  flipped = true;
}

void UndoFlip_F6F7() {
  right_columns -= 103079215104ULL;
  bottom_diag9s -= 5066575350595584ULL;
  bottom_diag7s -= 4614ULL;
  bottom_rows -= 77310590976ULL;
  disk_difference -= 4;
}

void Flip_F6F7() {
  right_columns += 103079215104ULL;
  bottom_diag9s += 5066575350595584ULL;
  bottom_diag7s += 4614ULL;
  bottom_rows += 77310590976ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F6F7;
  flipped = true;
}

void UndoFlip_G2_G4() {
  top_rows -= 25769803782ULL;
  top_diag9s -= 432352161297334272ULL;
  right_columns -= 106168320ULL;
  bottom_diag7s -= 410390516054753280ULL;
  disk_difference -= 4;
}

void Flip_G2_G4() {
  top_rows += 25769803782ULL;
  top_diag9s += 432352161297334272ULL;
  right_columns += 106168320ULL;
  bottom_diag7s += 410390516054753280ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_G4;
  flipped = true;
}

void UndoFlip_G2_G4G5() {
  top_rows -= 25769803782ULL;
  top_diag9s -= 432352161297727488ULL;
  right_columns -= 109707264ULL;
  bottom_diag7s -= 410390516054767104ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 6;
}

void Flip_G2_G4G5() {
  top_rows += 25769803782ULL;
  top_diag9s += 432352161297727488ULL;
  right_columns += 109707264ULL;
  bottom_diag7s += 410390516054767104ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_G4G5;
  flipped = true;
}

void UndoFlip_G2_G4G6() {
  top_rows -= 25769803782ULL;
  top_diag9s -= 432352161297727494ULL;
  right_columns -= 110886912ULL;
  bottom_diag7s -= 410390516054767122ULL;
  bottom_rows -= 1688875630067712ULL;
  disk_difference -= 8;
}

void Flip_G2_G4G6() {
  top_rows += 25769803782ULL;
  top_diag9s += 432352161297727494ULL;
  right_columns += 110886912ULL;
  bottom_diag7s += 410390516054767122ULL;
  bottom_rows += 1688875630067712ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_G4G6;
  flipped = true;
}

void UndoFlip_G2_G4G7() {
  top_rows -= 25769803782ULL;
  top_diag9s -= 432352161297727494ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 111280128ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 410390516054767122ULL;
  bottom_rows -= 1688875630460928ULL;
  disk_difference -= 10;
}

void Flip_G2_G4G7() {
  top_rows += 25769803782ULL;
  top_diag9s += 432352161297727494ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 111280128ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 410390516054767122ULL;
  bottom_rows += 1688875630460928ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_G4G7;
  flipped = true;
}

void UndoFlip_G2G3() {
  top_rows -= 25770196992ULL;
  top_diag9s -= 434034414087831552ULL;
  right_columns -= 127401984ULL;
  bottom_diag7s -= 410392603398242304ULL;
  disk_difference -= 4;
}

void Flip_G2G3() {
  top_rows += 25770196992ULL;
  top_diag9s += 434034414087831552ULL;
  right_columns += 127401984ULL;
  bottom_diag7s += 410392603398242304ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G3;
  flipped = true;
}

void UndoFlip_G2G3_G5() {
  top_rows -= 25770196992ULL;
  top_diag9s -= 434034414088224768ULL;
  right_columns -= 130940928ULL;
  bottom_diag7s -= 410392603398256128ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 6;
}

void Flip_G2G3_G5() {
  top_rows += 25770196992ULL;
  top_diag9s += 434034414088224768ULL;
  right_columns += 130940928ULL;
  bottom_diag7s += 410392603398256128ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G3_G5;
  flipped = true;
}

void UndoFlip_G2G3_G5G6() {
  top_rows -= 25770196992ULL;
  top_diag9s -= 434034414088224774ULL;
  right_columns -= 132120576ULL;
  bottom_diag7s -= 410392603398256146ULL;
  bottom_rows -= 1688875630067712ULL;
  disk_difference -= 8;
}

void Flip_G2G3_G5G6() {
  top_rows += 25770196992ULL;
  top_diag9s += 434034414088224774ULL;
  right_columns += 132120576ULL;
  bottom_diag7s += 410392603398256146ULL;
  bottom_rows += 1688875630067712ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G3_G5G6;
  flipped = true;
}

void UndoFlip_G2G3_G5G7() {
  top_rows -= 25770196992ULL;
  top_diag9s -= 434034414088224774ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 132513792ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 410392603398256146ULL;
  bottom_rows -= 1688875630460928ULL;
  disk_difference -= 10;
}

void Flip_G2G3_G5G7() {
  top_rows += 25770196992ULL;
  top_diag9s += 434034414088224774ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 132513792ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 410392603398256146ULL;
  bottom_rows += 1688875630460928ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G3_G5G7;
  flipped = true;
}

void UndoFlip_G2G4() {
  top_rows -= 25770196998ULL;
  top_diag9s -= 434041011157598208ULL;
  right_columns -= 138018816ULL;
  bottom_diag7s -= 410392603408859136ULL;
  disk_difference -= 6;
}

void Flip_G2G4() {
  top_rows += 25770196998ULL;
  top_diag9s += 434041011157598208ULL;
  right_columns += 138018816ULL;
  bottom_diag7s += 410392603408859136ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G4;
  flipped = true;
}

void UndoFlip_G2G4_G6() {
  top_rows -= 25770196998ULL;
  top_diag9s -= 434041011157598214ULL;
  right_columns -= 139198464ULL;
  bottom_diag7s -= 410392603408859154ULL;
  bottom_rows -= 25769803776ULL;
  disk_difference -= 8;
}

void Flip_G2G4_G6() {
  top_rows += 25770196998ULL;
  top_diag9s += 434041011157598214ULL;
  right_columns += 139198464ULL;
  bottom_diag7s += 410392603408859154ULL;
  bottom_rows += 25769803776ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G4_G6;
  flipped = true;
}

void UndoFlip_G2G4_G6G7() {
  top_rows -= 25770196998ULL;
  top_diag9s -= 434041011157598214ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 139591680ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 410392603408859154ULL;
  bottom_rows -= 25770196992ULL;
  disk_difference -= 10;
}

void Flip_G2G4_G6G7() {
  top_rows += 25770196998ULL;
  top_diag9s += 434041011157598214ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 139591680ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 410392603408859154ULL;
  bottom_rows += 25770196992ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G4_G6G7;
  flipped = true;
}

void UndoFlip_G2G5() {
  top_rows -= 25770196998ULL;
  top_diag9s -= 434041011157991424ULL;
  right_columns -= 141557760ULL;
  bottom_diag7s -= 410392603408872960ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 8;
}

void Flip_G2G5() {
  top_rows += 25770196998ULL;
  top_diag9s += 434041011157991424ULL;
  right_columns += 141557760ULL;
  bottom_diag7s += 410392603408872960ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G5;
  flipped = true;
}

void UndoFlip_G2G5_G7() {
  top_rows -= 25770196998ULL;
  top_diag9s -= 434041011157991424ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 141950976ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 410392603408872960ULL;
  bottom_rows -= 1688849860657152ULL;
  disk_difference -= 10;
}

void Flip_G2G5_G7() {
  top_rows += 25770196998ULL;
  top_diag9s += 434041011157991424ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 141950976ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 410392603408872960ULL;
  bottom_rows += 1688849860657152ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G5_G7;
  flipped = true;
}

void UndoFlip_G2G6() {
  top_rows -= 25770196998ULL;
  top_diag9s -= 434041011157991430ULL;
  right_columns -= 142737408ULL;
  bottom_diag7s -= 410392603408872978ULL;
  bottom_rows -= 1688875630067712ULL;
  disk_difference -= 10;
}

void Flip_G2G6() {
  top_rows += 25770196998ULL;
  top_diag9s += 434041011157991430ULL;
  right_columns += 142737408ULL;
  bottom_diag7s += 410392603408872978ULL;
  bottom_rows += 1688875630067712ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G6;
  flipped = true;
}

void UndoFlip_G2G7() {
  top_rows -= 25770196998ULL;
  top_diag9s -= 434041011157991430ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 143130624ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 410392603408872978ULL;
  bottom_rows -= 1688875630460928ULL;
  disk_difference -= 12;
}

void Flip_G2G7() {
  top_rows += 25770196998ULL;
  top_diag9s += 434041011157991430ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 143130624ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 410392603408872978ULL;
  bottom_rows += 1688875630460928ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2G7;
  flipped = true;
}

void UndoFlip_G3_G5() {
  top_rows -= 393216ULL;
  top_diag9s -= 1688849860657152ULL;
  right_columns -= 35389440ULL;
  bottom_diag7s -= 2087354119680ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 4;
}

void Flip_G3_G5() {
  top_rows += 393216ULL;
  top_diag9s += 1688849860657152ULL;
  right_columns += 35389440ULL;
  bottom_diag7s += 2087354119680ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_G5;
  flipped = true;
}

void UndoFlip_G3_G5G6() {
  top_rows -= 393216ULL;
  top_diag9s -= 1688849860657158ULL;
  right_columns -= 36569088ULL;
  bottom_diag7s -= 2087354119698ULL;
  bottom_rows -= 1688875630067712ULL;
  disk_difference -= 6;
}

void Flip_G3_G5G6() {
  top_rows += 393216ULL;
  top_diag9s += 1688849860657158ULL;
  right_columns += 36569088ULL;
  bottom_diag7s += 2087354119698ULL;
  bottom_rows += 1688875630067712ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_G5G6;
  flipped = true;
}

void UndoFlip_G3_G5G7() {
  top_rows -= 393216ULL;
  top_diag9s -= 1688849860657158ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 36962304ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 2087354119698ULL;
  bottom_rows -= 1688875630460928ULL;
  disk_difference -= 8;
}

void Flip_G3_G5G7() {
  top_rows += 393216ULL;
  top_diag9s += 1688849860657158ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 36962304ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 2087354119698ULL;
  bottom_rows += 1688875630460928ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_G5G7;
  flipped = true;
}

void UndoFlip_G3G4() {
  top_rows -= 393222ULL;
  top_diag9s -= 1695446930030592ULL;
  right_columns -= 42467328ULL;
  bottom_diag7s -= 2087364722688ULL;
  disk_difference -= 4;
}

void Flip_G3G4() {
  top_rows += 393222ULL;
  top_diag9s += 1695446930030592ULL;
  right_columns += 42467328ULL;
  bottom_diag7s += 2087364722688ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G4;
  flipped = true;
}

void UndoFlip_G3G4_G6() {
  top_rows -= 393222ULL;
  top_diag9s -= 1695446930030598ULL;
  right_columns -= 43646976ULL;
  bottom_diag7s -= 2087364722706ULL;
  bottom_rows -= 25769803776ULL;
  disk_difference -= 6;
}

void Flip_G3G4_G6() {
  top_rows += 393222ULL;
  top_diag9s += 1695446930030598ULL;
  right_columns += 43646976ULL;
  bottom_diag7s += 2087364722706ULL;
  bottom_rows += 25769803776ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G4_G6;
  flipped = true;
}

void UndoFlip_G3G4_G6G7() {
  top_rows -= 393222ULL;
  top_diag9s -= 1695446930030598ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 44040192ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 2087364722706ULL;
  bottom_rows -= 25770196992ULL;
  disk_difference -= 8;
}

void Flip_G3G4_G6G7() {
  top_rows += 393222ULL;
  top_diag9s += 1695446930030598ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 44040192ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 2087364722706ULL;
  bottom_rows += 25770196992ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G4_G6G7;
  flipped = true;
}

void UndoFlip_G3G5() {
  top_rows -= 393222ULL;
  top_diag9s -= 1695446930423808ULL;
  right_columns -= 46006272ULL;
  bottom_diag7s -= 2087364736512ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 6;
}

void Flip_G3G5() {
  top_rows += 393222ULL;
  top_diag9s += 1695446930423808ULL;
  right_columns += 46006272ULL;
  bottom_diag7s += 2087364736512ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G5;
  flipped = true;
}

void UndoFlip_G3G5_G7() {
  top_rows -= 393222ULL;
  top_diag9s -= 1695446930423808ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 46399488ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 2087364736512ULL;
  bottom_rows -= 1688849860657152ULL;
  disk_difference -= 8;
}

void Flip_G3G5_G7() {
  top_rows += 393222ULL;
  top_diag9s += 1695446930423808ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 46399488ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 2087364736512ULL;
  bottom_rows += 1688849860657152ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G5_G7;
  flipped = true;
}

void UndoFlip_G3G6() {
  top_rows -= 393222ULL;
  top_diag9s -= 1695446930423814ULL;
  right_columns -= 47185920ULL;
  bottom_diag7s -= 2087364736530ULL;
  bottom_rows -= 1688875630067712ULL;
  disk_difference -= 8;
}

void Flip_G3G6() {
  top_rows += 393222ULL;
  top_diag9s += 1695446930423814ULL;
  right_columns += 47185920ULL;
  bottom_diag7s += 2087364736530ULL;
  bottom_rows += 1688875630067712ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G6;
  flipped = true;
}

void UndoFlip_G3G7() {
  top_rows -= 393222ULL;
  top_diag9s -= 1695446930423814ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 47579136ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 2087364736530ULL;
  bottom_rows -= 1688875630460928ULL;
  disk_difference -= 10;
}

void Flip_G3G7() {
  top_rows += 393222ULL;
  top_diag9s += 1695446930423814ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 47579136ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 2087364736530ULL;
  bottom_rows += 1688875630460928ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3G7;
  flipped = true;
}

void UndoFlip_G4_G6() {
  top_rows -= 6ULL;
  top_diag9s -= 6597069766662ULL;
  right_columns -= 11796480ULL;
  bottom_diag7s -= 10616850ULL;
  bottom_rows -= 25769803776ULL;
  disk_difference -= 4;
}

void Flip_G4_G6() {
  top_rows += 6ULL;
  top_diag9s += 6597069766662ULL;
  right_columns += 11796480ULL;
  bottom_diag7s += 10616850ULL;
  bottom_rows += 25769803776ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4_G6;
  flipped = true;
}

void UndoFlip_G4_G6G7() {
  top_rows -= 6ULL;
  top_diag9s -= 6597069766662ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 12189696ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 10616850ULL;
  bottom_rows -= 25770196992ULL;
  disk_difference -= 6;
}

void Flip_G4_G6G7() {
  top_rows += 6ULL;
  top_diag9s += 6597069766662ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 12189696ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 10616850ULL;
  bottom_rows += 25770196992ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4_G6G7;
  flipped = true;
}

void UndoFlip_G4G5() {
  top_rows -= 6ULL;
  top_diag9s -= 6597070159872ULL;
  right_columns -= 14155776ULL;
  bottom_diag7s -= 10630656ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 4;
}

void Flip_G4G5() {
  top_rows += 6ULL;
  top_diag9s += 6597070159872ULL;
  right_columns += 14155776ULL;
  bottom_diag7s += 10630656ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4G5;
  flipped = true;
}

void UndoFlip_G4G5_G7() {
  top_rows -= 6ULL;
  top_diag9s -= 6597070159872ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 14548992ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 10630656ULL;
  bottom_rows -= 1688849860657152ULL;
  disk_difference -= 6;
}

void Flip_G4G5_G7() {
  top_rows += 6ULL;
  top_diag9s += 6597070159872ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 14548992ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 10630656ULL;
  bottom_rows += 1688849860657152ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4G5_G7;
  flipped = true;
}

void UndoFlip_G4G6() {
  top_rows -= 6ULL;
  top_diag9s -= 6597070159878ULL;
  right_columns -= 15335424ULL;
  bottom_diag7s -= 10630674ULL;
  bottom_rows -= 1688875630067712ULL;
  disk_difference -= 6;
}

void Flip_G4G6() {
  top_rows += 6ULL;
  top_diag9s += 6597070159878ULL;
  right_columns += 15335424ULL;
  bottom_diag7s += 10630674ULL;
  bottom_rows += 1688875630067712ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4G6;
  flipped = true;
}

void UndoFlip_G4G7() {
  top_rows -= 6ULL;
  top_diag9s -= 6597070159878ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 15728640ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 10630674ULL;
  bottom_rows -= 1688875630460928ULL;
  disk_difference -= 8;
}

void Flip_G4G7() {
  top_rows += 6ULL;
  top_diag9s += 6597070159878ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 15728640ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 10630674ULL;
  bottom_rows += 1688875630460928ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4G7;
  flipped = true;
}

void UndoFlip_G5_G7() {
  top_diag9s -= 393216ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 3932160ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 13824ULL;
  bottom_rows -= 1688849860657152ULL;
  disk_difference -= 4;
}

void Flip_G5_G7() {
  top_diag9s += 393216ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 3932160ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 13824ULL;
  bottom_rows += 1688849860657152ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5_G7;
  flipped = true;
}

void UndoFlip_G5G6() {
  top_diag9s -= 393222ULL;
  right_columns -= 4718592ULL;
  bottom_diag7s -= 13842ULL;
  bottom_rows -= 1688875630067712ULL;
  disk_difference -= 4;
}

void Flip_G5G6() {
  top_diag9s += 393222ULL;
  right_columns += 4718592ULL;
  bottom_diag7s += 13842ULL;
  bottom_rows += 1688875630067712ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5G6;
  flipped = true;
}

void UndoFlip_G5G7() {
  top_diag9s -= 393222ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 5111808ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 13842ULL;
  bottom_rows -= 1688875630460928ULL;
  disk_difference -= 6;
}

void Flip_G5G7() {
  top_diag9s += 393222ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 5111808ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 13842ULL;
  bottom_rows += 1688875630460928ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5G7;
  flipped = true;
}

void UndoFlip_G6G7() {
  top_diag9s -= 6ULL;
  top_diag7s -= 25769803776ULL;
  right_columns -= 1572864ULL;
  bottom_diag9s -= 1688849860263936ULL;
  bottom_diag7s -= 18ULL;
  bottom_rows -= 25770196992ULL;
  disk_difference -= 4;
}

void Flip_G6G7() {
  top_diag9s += 6ULL;
  top_diag7s += 25769803776ULL;
  right_columns += 1572864ULL;
  bottom_diag9s += 1688849860263936ULL;
  bottom_diag7s += 18ULL;
  bottom_rows += 25770196992ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G6G7;
  flipped = true;
}

void UndoFlip_H2() {
  top_rows -= 8589934592ULL;
  right_columns -= 1458ULL;
  bottom_diag7s -= 6262062317568ULL;
  disk_difference -= 2;
}

void Flip_H2() {
  top_rows += 8589934592ULL;
  right_columns += 1458ULL;
  bottom_diag7s += 6262062317568ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2;
  flipped = true;
}

void UndoFlip_H2_H4() {
  top_rows -= 8589934594ULL;
  top_diag9s -= 562949953421312ULL;
  right_columns -= 1620ULL;
  bottom_diag7s -= 6262062359040ULL;
  disk_difference -= 4;
}

void Flip_H2_H4() {
  top_rows += 8589934594ULL;
  top_diag9s += 562949953421312ULL;
  right_columns += 1620ULL;
  bottom_diag7s += 6262062359040ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2_H4;
  flipped = true;
}

void UndoFlip_H2_H4H5() {
  top_rows -= 8589934594ULL;
  top_diag9s -= 565148976676864ULL;
  right_columns -= 1674ULL;
  bottom_diag7s -= 6262062359094ULL;
  bottom_rows -= 562949953421312ULL;
  disk_difference -= 6;
}

void Flip_H2_H4H5() {
  top_rows += 8589934594ULL;
  top_diag9s += 565148976676864ULL;
  right_columns += 1674ULL;
  bottom_diag7s += 6262062359094ULL;
  bottom_rows += 562949953421312ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2_H4H5;
  flipped = true;
}

void UndoFlip_H2_H4H6() {
  top_rows -= 8589934594ULL;
  top_diag9s -= 565148976807936ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 1692ULL;
  bottom_diag7s -= 6262062359094ULL;
  bottom_rows -= 562958543355904ULL;
  disk_difference -= 8;
}

void Flip_H2_H4H6() {
  top_rows += 8589934594ULL;
  top_diag9s += 565148976807936ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 1692ULL;
  bottom_diag7s += 6262062359094ULL;
  bottom_rows += 562958543355904ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2_H4H6;
  flipped = true;
}

void UndoFlip_H2_H4H7() {
  top_rows -= 8589934594ULL;
  top_diag9s -= 565148976807938ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 1698ULL;
  bottom_diag7s -= 6262062359094ULL;
  bottom_rows -= 562958543486976ULL;
  disk_difference -= 10;
}

void Flip_H2_H4H7() {
  top_rows += 8589934594ULL;
  top_diag9s += 565148976807938ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 1698ULL;
  bottom_diag7s += 6262062359094ULL;
  bottom_rows += 562958543486976ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2_H4H7;
  flipped = true;
}

void UndoFlip_H2H3() {
  top_rows -= 8590065664ULL;
  top_diag9s -= 144115188075855872ULL;
  right_columns -= 1944ULL;
  bottom_diag7s -= 6262094168064ULL;
  disk_difference -= 4;
}

void Flip_H2H3() {
  top_rows += 8590065664ULL;
  top_diag9s += 144115188075855872ULL;
  right_columns += 1944ULL;
  bottom_diag7s += 6262094168064ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H3;
  flipped = true;
}

void UndoFlip_H2H3_H5() {
  top_rows -= 8590065664ULL;
  top_diag9s -= 144117387099111424ULL;
  right_columns -= 1998ULL;
  bottom_diag7s -= 6262094168118ULL;
  bottom_rows -= 562949953421312ULL;
  disk_difference -= 6;
}

void Flip_H2H3_H5() {
  top_rows += 8590065664ULL;
  top_diag9s += 144117387099111424ULL;
  right_columns += 1998ULL;
  bottom_diag7s += 6262094168118ULL;
  bottom_rows += 562949953421312ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H3_H5;
  flipped = true;
}

void UndoFlip_H2H3_H5H6() {
  top_rows -= 8590065664ULL;
  top_diag9s -= 144117387099242496ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 2016ULL;
  bottom_diag7s -= 6262094168118ULL;
  bottom_rows -= 562958543355904ULL;
  disk_difference -= 8;
}

void Flip_H2H3_H5H6() {
  top_rows += 8590065664ULL;
  top_diag9s += 144117387099242496ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 2016ULL;
  bottom_diag7s += 6262094168118ULL;
  bottom_rows += 562958543355904ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H3_H5H6;
  flipped = true;
}

void UndoFlip_H2H3_H5H7() {
  top_rows -= 8590065664ULL;
  top_diag9s -= 144117387099242498ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 2022ULL;
  bottom_diag7s -= 6262094168118ULL;
  bottom_rows -= 562958543486976ULL;
  disk_difference -= 10;
}

void Flip_H2H3_H5H7() {
  top_rows += 8590065664ULL;
  top_diag9s += 144117387099242498ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 2022ULL;
  bottom_diag7s += 6262094168118ULL;
  bottom_rows += 562958543486976ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H3_H5H7;
  flipped = true;
}

void UndoFlip_H2H4() {
  top_rows -= 8590065666ULL;
  top_diag9s -= 144678138029277184ULL;
  right_columns -= 2106ULL;
  bottom_diag7s -= 6262094209536ULL;
  disk_difference -= 6;
}

void Flip_H2H4() {
  top_rows += 8590065666ULL;
  top_diag9s += 144678138029277184ULL;
  right_columns += 2106ULL;
  bottom_diag7s += 6262094209536ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H4;
  flipped = true;
}

void UndoFlip_H2H4_H6() {
  top_rows -= 8590065666ULL;
  top_diag9s -= 144678138029408256ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 2124ULL;
  bottom_diag7s -= 6262094209536ULL;
  bottom_rows -= 8589934592ULL;
  disk_difference -= 8;
}

void Flip_H2H4_H6() {
  top_rows += 8590065666ULL;
  top_diag9s += 144678138029408256ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 2124ULL;
  bottom_diag7s += 6262094209536ULL;
  bottom_rows += 8589934592ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H4_H6;
  flipped = true;
}

void UndoFlip_H2H4_H6H7() {
  top_rows -= 8590065666ULL;
  top_diag9s -= 144678138029408258ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 2130ULL;
  bottom_diag7s -= 6262094209536ULL;
  bottom_rows -= 8590065664ULL;
  disk_difference -= 10;
}

void Flip_H2H4_H6H7() {
  top_rows += 8590065666ULL;
  top_diag9s += 144678138029408258ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 2130ULL;
  bottom_diag7s += 6262094209536ULL;
  bottom_rows += 8590065664ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H4_H6H7;
  flipped = true;
}

void UndoFlip_H2H5() {
  top_rows -= 8590065666ULL;
  top_diag9s -= 144680337052532736ULL;
  right_columns -= 2160ULL;
  bottom_diag7s -= 6262094209590ULL;
  bottom_rows -= 562949953421312ULL;
  disk_difference -= 8;
}

void Flip_H2H5() {
  top_rows += 8590065666ULL;
  top_diag9s += 144680337052532736ULL;
  right_columns += 2160ULL;
  bottom_diag7s += 6262094209590ULL;
  bottom_rows += 562949953421312ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H5;
  flipped = true;
}

void UndoFlip_H2H5_H7() {
  top_rows -= 8590065666ULL;
  top_diag9s -= 144680337052532738ULL;
  right_columns -= 2166ULL;
  bottom_diag7s -= 6262094209590ULL;
  bottom_rows -= 562949953552384ULL;
  disk_difference -= 10;
}

void Flip_H2H5_H7() {
  top_rows += 8590065666ULL;
  top_diag9s += 144680337052532738ULL;
  right_columns += 2166ULL;
  bottom_diag7s += 6262094209590ULL;
  bottom_rows += 562949953552384ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H5_H7;
  flipped = true;
}

void UndoFlip_H2H6() {
  top_rows -= 8590065666ULL;
  top_diag9s -= 144680337052663808ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 2178ULL;
  bottom_diag7s -= 6262094209590ULL;
  bottom_rows -= 562958543355904ULL;
  disk_difference -= 10;
}

void Flip_H2H6() {
  top_rows += 8590065666ULL;
  top_diag9s += 144680337052663808ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 2178ULL;
  bottom_diag7s += 6262094209590ULL;
  bottom_rows += 562958543355904ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H6;
  flipped = true;
}

void UndoFlip_H2H7() {
  top_rows -= 8590065666ULL;
  top_diag9s -= 144680337052663810ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 2184ULL;
  bottom_diag7s -= 6262094209590ULL;
  bottom_rows -= 562958543486976ULL;
  disk_difference -= 12;
}

void Flip_H2H7() {
  top_rows += 8590065666ULL;
  top_diag9s += 144680337052663810ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 2184ULL;
  bottom_diag7s += 6262094209590ULL;
  bottom_rows += 562958543486976ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H2H7;
  flipped = true;
}

void UndoFlip_H3() {
  top_rows -= 131072ULL;
  top_diag9s -= 144115188075855872ULL;
  right_columns -= 486ULL;
  bottom_diag7s -= 31850496ULL;
  disk_difference -= 2;
}

void Flip_H3() {
  top_rows += 131072ULL;
  top_diag9s += 144115188075855872ULL;
  right_columns += 486ULL;
  bottom_diag7s += 31850496ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3;
  flipped = true;
}

void UndoFlip_H3_H5() {
  top_rows -= 131072ULL;
  top_diag9s -= 144117387099111424ULL;
  right_columns -= 540ULL;
  bottom_diag7s -= 31850550ULL;
  bottom_rows -= 562949953421312ULL;
  disk_difference -= 4;
}

void Flip_H3_H5() {
  top_rows += 131072ULL;
  top_diag9s += 144117387099111424ULL;
  right_columns += 540ULL;
  bottom_diag7s += 31850550ULL;
  bottom_rows += 562949953421312ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3_H5;
  flipped = true;
}

void UndoFlip_H3_H5H6() {
  top_rows -= 131072ULL;
  top_diag9s -= 144117387099242496ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 558ULL;
  bottom_diag7s -= 31850550ULL;
  bottom_rows -= 562958543355904ULL;
  disk_difference -= 6;
}

void Flip_H3_H5H6() {
  top_rows += 131072ULL;
  top_diag9s += 144117387099242496ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 558ULL;
  bottom_diag7s += 31850550ULL;
  bottom_rows += 562958543355904ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3_H5H6;
  flipped = true;
}

void UndoFlip_H3_H5H7() {
  top_rows -= 131072ULL;
  top_diag9s -= 144117387099242498ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 564ULL;
  bottom_diag7s -= 31850550ULL;
  bottom_rows -= 562958543486976ULL;
  disk_difference -= 8;
}

void Flip_H3_H5H7() {
  top_rows += 131072ULL;
  top_diag9s += 144117387099242498ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 564ULL;
  bottom_diag7s += 31850550ULL;
  bottom_rows += 562958543486976ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3_H5H7;
  flipped = true;
}

void UndoFlip_H3H4() {
  top_rows -= 131074ULL;
  top_diag9s -= 144678138029277184ULL;
  right_columns -= 648ULL;
  bottom_diag7s -= 31891968ULL;
  disk_difference -= 4;
}

void Flip_H3H4() {
  top_rows += 131074ULL;
  top_diag9s += 144678138029277184ULL;
  right_columns += 648ULL;
  bottom_diag7s += 31891968ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H4;
  flipped = true;
}

void UndoFlip_H3H4_H6() {
  top_rows -= 131074ULL;
  top_diag9s -= 144678138029408256ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 666ULL;
  bottom_diag7s -= 31891968ULL;
  bottom_rows -= 8589934592ULL;
  disk_difference -= 6;
}

void Flip_H3H4_H6() {
  top_rows += 131074ULL;
  top_diag9s += 144678138029408256ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 666ULL;
  bottom_diag7s += 31891968ULL;
  bottom_rows += 8589934592ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H4_H6;
  flipped = true;
}

void UndoFlip_H3H4_H6H7() {
  top_rows -= 131074ULL;
  top_diag9s -= 144678138029408258ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 672ULL;
  bottom_diag7s -= 31891968ULL;
  bottom_rows -= 8590065664ULL;
  disk_difference -= 8;
}

void Flip_H3H4_H6H7() {
  top_rows += 131074ULL;
  top_diag9s += 144678138029408258ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 672ULL;
  bottom_diag7s += 31891968ULL;
  bottom_rows += 8590065664ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H4_H6H7;
  flipped = true;
}

void UndoFlip_H3H5() {
  top_rows -= 131074ULL;
  top_diag9s -= 144680337052532736ULL;
  right_columns -= 702ULL;
  bottom_diag7s -= 31892022ULL;
  bottom_rows -= 562949953421312ULL;
  disk_difference -= 6;
}

void Flip_H3H5() {
  top_rows += 131074ULL;
  top_diag9s += 144680337052532736ULL;
  right_columns += 702ULL;
  bottom_diag7s += 31892022ULL;
  bottom_rows += 562949953421312ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H5;
  flipped = true;
}

void UndoFlip_H3H5_H7() {
  top_rows -= 131074ULL;
  top_diag9s -= 144680337052532738ULL;
  right_columns -= 708ULL;
  bottom_diag7s -= 31892022ULL;
  bottom_rows -= 562949953552384ULL;
  disk_difference -= 8;
}

void Flip_H3H5_H7() {
  top_rows += 131074ULL;
  top_diag9s += 144680337052532738ULL;
  right_columns += 708ULL;
  bottom_diag7s += 31892022ULL;
  bottom_rows += 562949953552384ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H5_H7;
  flipped = true;
}

void UndoFlip_H3H6() {
  top_rows -= 131074ULL;
  top_diag9s -= 144680337052663808ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 720ULL;
  bottom_diag7s -= 31892022ULL;
  bottom_rows -= 562958543355904ULL;
  disk_difference -= 8;
}

void Flip_H3H6() {
  top_rows += 131074ULL;
  top_diag9s += 144680337052663808ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 720ULL;
  bottom_diag7s += 31892022ULL;
  bottom_rows += 562958543355904ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H6;
  flipped = true;
}

void UndoFlip_H3H7() {
  top_rows -= 131074ULL;
  top_diag9s -= 144680337052663810ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 726ULL;
  bottom_diag7s -= 31892022ULL;
  bottom_rows -= 562958543486976ULL;
  disk_difference -= 10;
}

void Flip_H3H7() {
  top_rows += 131074ULL;
  top_diag9s += 144680337052663810ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 726ULL;
  bottom_diag7s += 31892022ULL;
  bottom_rows += 562958543486976ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H3H7;
  flipped = true;
}

void UndoFlip_H4() {
  top_rows -= 2ULL;
  top_diag9s -= 562949953421312ULL;
  right_columns -= 162ULL;
  bottom_diag7s -= 41472ULL;
  disk_difference -= 2;
}

void Flip_H4() {
  top_rows += 2ULL;
  top_diag9s += 562949953421312ULL;
  right_columns += 162ULL;
  bottom_diag7s += 41472ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4;
  flipped = true;
}

void UndoFlip_H4_H6() {
  top_rows -= 2ULL;
  top_diag9s -= 562949953552384ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 180ULL;
  bottom_diag7s -= 41472ULL;
  bottom_rows -= 8589934592ULL;
  disk_difference -= 4;
}

void Flip_H4_H6() {
  top_rows += 2ULL;
  top_diag9s += 562949953552384ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 180ULL;
  bottom_diag7s += 41472ULL;
  bottom_rows += 8589934592ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4_H6;
  flipped = true;
}

void UndoFlip_H4_H6H7() {
  top_rows -= 2ULL;
  top_diag9s -= 562949953552386ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 186ULL;
  bottom_diag7s -= 41472ULL;
  bottom_rows -= 8590065664ULL;
  disk_difference -= 6;
}

void Flip_H4_H6H7() {
  top_rows += 2ULL;
  top_diag9s += 562949953552386ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 186ULL;
  bottom_diag7s += 41472ULL;
  bottom_rows += 8590065664ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4_H6H7;
  flipped = true;
}

void UndoFlip_H4H5() {
  top_rows -= 2ULL;
  top_diag9s -= 565148976676864ULL;
  right_columns -= 216ULL;
  bottom_diag7s -= 41526ULL;
  bottom_rows -= 562949953421312ULL;
  disk_difference -= 4;
}

void Flip_H4H5() {
  top_rows += 2ULL;
  top_diag9s += 565148976676864ULL;
  right_columns += 216ULL;
  bottom_diag7s += 41526ULL;
  bottom_rows += 562949953421312ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4H5;
  flipped = true;
}

void UndoFlip_H4H5_H7() {
  top_rows -= 2ULL;
  top_diag9s -= 565148976676866ULL;
  right_columns -= 222ULL;
  bottom_diag7s -= 41526ULL;
  bottom_rows -= 562949953552384ULL;
  disk_difference -= 6;
}

void Flip_H4H5_H7() {
  top_rows += 2ULL;
  top_diag9s += 565148976676866ULL;
  right_columns += 222ULL;
  bottom_diag7s += 41526ULL;
  bottom_rows += 562949953552384ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4H5_H7;
  flipped = true;
}

void UndoFlip_H4H6() {
  top_rows -= 2ULL;
  top_diag9s -= 565148976807936ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 234ULL;
  bottom_diag7s -= 41526ULL;
  bottom_rows -= 562958543355904ULL;
  disk_difference -= 6;
}

void Flip_H4H6() {
  top_rows += 2ULL;
  top_diag9s += 565148976807936ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 234ULL;
  bottom_diag7s += 41526ULL;
  bottom_rows += 562958543355904ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4H6;
  flipped = true;
}

void UndoFlip_H4H7() {
  top_rows -= 2ULL;
  top_diag9s -= 565148976807938ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 240ULL;
  bottom_diag7s -= 41526ULL;
  bottom_rows -= 562958543486976ULL;
  disk_difference -= 8;
}

void Flip_H4H7() {
  top_rows += 2ULL;
  top_diag9s += 565148976807938ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 240ULL;
  bottom_diag7s += 41526ULL;
  bottom_rows += 562958543486976ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H4H7;
  flipped = true;
}

void UndoFlip_H5() {
  top_diag9s -= 2199023255552ULL;
  right_columns -= 54ULL;
  bottom_diag7s -= 54ULL;
  bottom_rows -= 562949953421312ULL;
  disk_difference -= 2;
}

void Flip_H5() {
  top_diag9s += 2199023255552ULL;
  right_columns += 54ULL;
  bottom_diag7s += 54ULL;
  bottom_rows += 562949953421312ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H5;
  flipped = true;
}

void UndoFlip_H5_H7() {
  top_diag9s -= 2199023255554ULL;
  right_columns -= 60ULL;
  bottom_diag7s -= 54ULL;
  bottom_rows -= 562949953552384ULL;
  disk_difference -= 4;
}

void Flip_H5_H7() {
  top_diag9s += 2199023255554ULL;
  right_columns += 60ULL;
  bottom_diag7s += 54ULL;
  bottom_rows += 562949953552384ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H5_H7;
  flipped = true;
}

void UndoFlip_H5H6() {
  top_diag9s -= 2199023386624ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 72ULL;
  bottom_diag7s -= 54ULL;
  bottom_rows -= 562958543355904ULL;
  disk_difference -= 4;
}

void Flip_H5H6() {
  top_diag9s += 2199023386624ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 72ULL;
  bottom_diag7s += 54ULL;
  bottom_rows += 562958543355904ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H5H6;
  flipped = true;
}

void UndoFlip_H5H7() {
  top_diag9s -= 2199023386626ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 78ULL;
  bottom_diag7s -= 54ULL;
  bottom_rows -= 562958543486976ULL;
  disk_difference -= 6;
}

void Flip_H5H7() {
  top_diag9s += 2199023386626ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 78ULL;
  bottom_diag7s += 54ULL;
  bottom_rows += 562958543486976ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H5H7;
  flipped = true;
}

void UndoFlip_H6() {
  top_diag9s -= 131072ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 18ULL;
  bottom_rows -= 8589934592ULL;
  disk_difference -= 2;
}

void Flip_H6() {
  top_diag9s += 131072ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 18ULL;
  bottom_rows += 8589934592ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H6;
  flipped = true;
}

void UndoFlip_H6H7() {
  top_diag9s -= 131074ULL;
  top_diag7s -= 77309411328ULL;
  right_columns -= 24ULL;
  bottom_rows -= 8590065664ULL;
  disk_difference -= 4;
}

void Flip_H6H7() {
  top_diag9s += 131074ULL;
  top_diag7s += 77309411328ULL;
  right_columns += 24ULL;
  bottom_rows += 8590065664ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H6H7;
  flipped = true;
}

void UndoFlip_H7() {
  top_diag9s -= 2ULL;
  right_columns -= 6ULL;
  bottom_rows -= 131072ULL;
  disk_difference -= 2;
}

void Flip_H7() {
  top_diag9s += 2ULL;
  right_columns += 6ULL;
  bottom_rows += 131072ULL;
  disk_difference += 2;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_H7;
  flipped = true;
}

void UndoFlip_C2B3() {
  top_rows -= 2087449657344ULL;
  left_columns -= 2087449657344ULL;
  top_diag9s -= 486ULL;
  top_diag7s -= 6755399441055744ULL;
  bottom_diag9s -= 2087354105856ULL;
  disk_difference -= 4;
}

void Flip_C2B3() {
  top_rows += 2087449657344ULL;
  left_columns += 2087449657344ULL;
  top_diag9s += 486ULL;
  top_diag7s += 6755399441055744ULL;
  bottom_diag9s += 2087354105856ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2B3;
  flipped = true;
}

void UndoFlip_D2_B4() {
  top_rows -= 695784703410ULL;
  left_columns -= 695784703410ULL;
  top_diag9s -= 10616832ULL;
  top_diag7s -= 65970697666560ULL;
  bottom_diag9s -= 10616832ULL;
  disk_difference -= 4;
}

void Flip_D2_B4() {
  top_rows += 695784703410ULL;
  left_columns += 695784703410ULL;
  top_diag9s += 10616832ULL;
  top_diag7s += 65970697666560ULL;
  bottom_diag9s += 10616832ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_B4;
  flipped = true;
}

void UndoFlip_D2C3() {
  top_rows -= 695816552448ULL;
  left_columns -= 31851954ULL;
  top_diag9s -= 10616832ULL;
  top_diag7s -= 79164837199872ULL;
  bottom_diag9s -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_D2C3() {
  top_rows += 695816552448ULL;
  left_columns += 31851954ULL;
  top_diag9s += 10616832ULL;
  top_diag7s += 79164837199872ULL;
  bottom_diag9s += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2C3;
  flipped = true;
}

void UndoFlip_D2B4() {
  top_rows -= 695816553906ULL;
  left_columns -= 695816553906ULL;
  top_diag9s -= 10616832ULL;
  top_diag7s -= 85761906966528ULL;
  bottom_diag9s -= 136796838691995648ULL;
  disk_difference -= 6;
}

void Flip_D2B4() {
  top_rows += 695816553906ULL;
  left_columns += 695816553906ULL;
  top_diag9s += 10616832ULL;
  top_diag7s += 85761906966528ULL;
  bottom_diag9s += 136796838691995648ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2B4;
  flipped = true;
}

void UndoFlip_C3B4() {
  top_rows -= 31851954ULL;
  left_columns -= 695816552448ULL;
  top_diag7s -= 26388279066624ULL;
  bottom_diag9s -= 136796838691995648ULL;
  disk_difference -= 4;
}

void Flip_C3B4() {
  top_rows += 31851954ULL;
  left_columns += 695816552448ULL;
  top_diag7s += 26388279066624ULL;
  bottom_diag9s += 136796838691995648ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3B4;
  flipped = true;
}

void UndoFlip_E2_C4() {
  top_rows -= 231928234470ULL;
  left_columns -= 10616832ULL;
  top_diag9s -= 59373627899904ULL;
  top_diag7s -= 11796480ULL;
  right_columns -= 410390516044136448ULL;
  bottom_diag9s -= 695784701952ULL;
  disk_difference -= 4;
}

void Flip_E2_C4() {
  top_rows += 231928234470ULL;
  left_columns += 10616832ULL;
  top_diag9s += 59373627899904ULL;
  top_diag7s += 11796480ULL;
  right_columns += 410390516044136448ULL;
  bottom_diag9s += 695784701952ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_C4;
  flipped = true;
}

void UndoFlip_E2_C4B5() {
  top_rows -= 231928234470ULL;
  left_columns -= 231938850816ULL;
  top_diag9s -= 59373627899904ULL;
  top_diag7s -= 12189696ULL;
  right_columns -= 410390516044136448ULL;
  bottom_diag9s -= 695784715776ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_E2_C4B5() {
  top_rows += 231928234470ULL;
  left_columns += 231938850816ULL;
  top_diag9s += 59373627899904ULL;
  top_diag7s += 12189696ULL;
  right_columns += 410390516044136448ULL;
  bottom_diag9s += 695784715776ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_C4B5;
  flipped = true;
}

void UndoFlip_E2D3() {
  top_rows -= 231938850816ULL;
  left_columns -= 486ULL;
  top_diag9s -= 59373627900066ULL;
  top_diag7s -= 14155776ULL;
  right_columns -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_E2D3() {
  top_rows += 231938850816ULL;
  left_columns += 486ULL;
  top_diag9s += 59373627900066ULL;
  top_diag7s += 14155776ULL;
  right_columns += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2D3;
  flipped = true;
}

void UndoFlip_E2D3_B5() {
  top_rows -= 231938850816ULL;
  left_columns -= 231928234470ULL;
  top_diag9s -= 59373627900066ULL;
  top_diag7s -= 14548992ULL;
  right_columns -= 410390516044136448ULL;
  bottom_diag9s -= 13824ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_E2D3_B5() {
  top_rows += 231938850816ULL;
  left_columns += 231928234470ULL;
  top_diag9s += 59373627900066ULL;
  top_diag7s += 14548992ULL;
  right_columns += 410390516044136448ULL;
  bottom_diag9s += 13824ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2D3_B5;
  flipped = true;
}

void UndoFlip_E2C4() {
  top_rows -= 231938851302ULL;
  left_columns -= 10617318ULL;
  top_diag9s -= 59373627900066ULL;
  top_diag7s -= 15335424ULL;
  right_columns -= 410390516044136448ULL;
  bottom_diag9s -= 695784701952ULL;
  disk_difference -= 6;
}

void Flip_E2C4() {
  top_rows += 231938851302ULL;
  left_columns += 10617318ULL;
  top_diag9s += 59373627900066ULL;
  top_diag7s += 15335424ULL;
  right_columns += 410390516044136448ULL;
  bottom_diag9s += 695784701952ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2C4;
  flipped = true;
}

void UndoFlip_E2B5() {
  top_rows -= 231938851302ULL;
  left_columns -= 231938851302ULL;
  top_diag9s -= 59373627900066ULL;
  top_diag7s -= 15728640ULL;
  right_columns -= 410390516044136448ULL;
  bottom_diag9s -= 695784715776ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 8;
}

void Flip_E2B5() {
  top_rows += 231938851302ULL;
  left_columns += 231938851302ULL;
  top_diag9s += 59373627900066ULL;
  top_diag7s += 15728640ULL;
  right_columns += 410390516044136448ULL;
  bottom_diag9s += 695784715776ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2B5;
  flipped = true;
}

void UndoFlip_D3_B5() {
  top_rows -= 10616832ULL;
  left_columns -= 231928234470ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 3932160ULL;
  bottom_diag9s -= 13824ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_D3_B5() {
  top_rows += 10616832ULL;
  left_columns += 231928234470ULL;
  top_diag9s += 162ULL;
  top_diag7s += 3932160ULL;
  bottom_diag9s += 13824ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_B5;
  flipped = true;
}

void UndoFlip_D3C4() {
  top_rows -= 10617318ULL;
  left_columns -= 10617318ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 4718592ULL;
  bottom_diag9s -= 695784701952ULL;
  disk_difference -= 4;
}

void Flip_D3C4() {
  top_rows += 10617318ULL;
  left_columns += 10617318ULL;
  top_diag9s += 162ULL;
  top_diag7s += 4718592ULL;
  bottom_diag9s += 695784701952ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3C4;
  flipped = true;
}

void UndoFlip_D3B5() {
  top_rows -= 10617318ULL;
  left_columns -= 231938851302ULL;
  top_diag9s -= 162ULL;
  top_diag7s -= 5111808ULL;
  bottom_diag9s -= 695784715776ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 6;
}

void Flip_D3B5() {
  top_rows += 10617318ULL;
  left_columns += 231938851302ULL;
  top_diag9s += 162ULL;
  top_diag7s += 5111808ULL;
  bottom_diag9s += 695784715776ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3B5;
  flipped = true;
}

void UndoFlip_C4B5() {
  top_rows -= 486ULL;
  left_columns -= 231938850816ULL;
  top_diag7s -= 1572864ULL;
  bottom_diag9s -= 695784715776ULL;
  bottom_rows -= 410390516044136448ULL;
  disk_difference -= 4;
}

void Flip_C4B5() {
  top_rows += 486ULL;
  left_columns += 231938850816ULL;
  top_diag7s += 1572864ULL;
  bottom_diag9s += 695784715776ULL;
  bottom_rows += 410390516044136448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4B5;
  flipped = true;
}

void UndoFlip_F2_D4() {
  top_rows -= 77309411490ULL;
  left_columns -= 162ULL;
  top_diag9s -= 5066549580791808ULL;
  top_diag7s -= 540ULL;
  right_columns -= 6262062317568ULL;
  bottom_diag9s -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_F2_D4() {
  top_rows += 77309411490ULL;
  left_columns += 162ULL;
  top_diag9s += 5066549580791808ULL;
  top_diag7s += 540ULL;
  right_columns += 6262062317568ULL;
  bottom_diag9s += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_D4;
  flipped = true;
}

void UndoFlip_F2_D4C5() {
  top_rows -= 77309411490ULL;
  left_columns -= 3539106ULL;
  top_diag9s -= 5066549580791808ULL;
  top_diag7s -= 558ULL;
  right_columns -= 6262062317568ULL;
  bottom_diag9s -= 45598946230665216ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 6;
}

void Flip_F2_D4C5() {
  top_rows += 77309411490ULL;
  left_columns += 3539106ULL;
  top_diag9s += 5066549580791808ULL;
  top_diag7s += 558ULL;
  right_columns += 6262062317568ULL;
  bottom_diag9s += 45598946230665216ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_D4C5;
  flipped = true;
}

void UndoFlip_F2_D4B6() {
  top_rows -= 77309411490ULL;
  left_columns -= 77312950434ULL;
  top_diag9s -= 5066549580791808ULL;
  top_diag7s -= 564ULL;
  right_columns -= 6262062317568ULL;
  bottom_diag9s -= 45598946230665234ULL;
  bottom_rows -= 136803100743696384ULL;
  disk_difference -= 8;
}

void Flip_F2_D4B6() {
  top_rows += 77309411490ULL;
  left_columns += 77312950434ULL;
  top_diag9s += 5066549580791808ULL;
  top_diag7s += 564ULL;
  right_columns += 6262062317568ULL;
  bottom_diag9s += 45598946230665234ULL;
  bottom_rows += 136803100743696384ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2_D4B6;
  flipped = true;
}

void UndoFlip_F2E3() {
  top_rows -= 77312950272ULL;
  top_diag9s -= 5066549584330752ULL;
  top_diag7s -= 648ULL;
  right_columns -= 136803100743696384ULL;
  disk_difference -= 4;
}

void Flip_F2E3() {
  top_rows += 77312950272ULL;
  top_diag9s += 5066549584330752ULL;
  top_diag7s += 648ULL;
  right_columns += 136803100743696384ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2E3;
  flipped = true;
}

void UndoFlip_F2E3_C5() {
  top_rows -= 77312950272ULL;
  left_columns -= 3538944ULL;
  top_diag9s -= 5066549584330752ULL;
  top_diag7s -= 666ULL;
  right_columns -= 136803100743696384ULL;
  bottom_diag9s -= 3538944ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 6;
}

void Flip_F2E3_C5() {
  top_rows += 77312950272ULL;
  left_columns += 3538944ULL;
  top_diag9s += 5066549584330752ULL;
  top_diag7s += 666ULL;
  right_columns += 136803100743696384ULL;
  bottom_diag9s += 3538944ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2E3_C5;
  flipped = true;
}

void UndoFlip_F2E3_C5B6() {
  top_rows -= 77312950272ULL;
  left_columns -= 77312950272ULL;
  top_diag9s -= 5066549584330752ULL;
  top_diag7s -= 672ULL;
  right_columns -= 136803100743696384ULL;
  bottom_diag9s -= 3538962ULL;
  bottom_rows -= 136803100743696384ULL;
  disk_difference -= 8;
}

void Flip_F2E3_C5B6() {
  top_rows += 77312950272ULL;
  left_columns += 77312950272ULL;
  top_diag9s += 5066549584330752ULL;
  top_diag7s += 672ULL;
  right_columns += 136803100743696384ULL;
  bottom_diag9s += 3538962ULL;
  bottom_rows += 136803100743696384ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2E3_C5B6;
  flipped = true;
}

void UndoFlip_F2D4() {
  top_rows -= 77312950434ULL;
  left_columns -= 162ULL;
  top_diag9s -= 5066549584330752ULL;
  top_diag7s -= 702ULL;
  right_columns -= 136803100743696384ULL;
  bottom_diag9s -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_F2D4() {
  top_rows += 77312950434ULL;
  left_columns += 162ULL;
  top_diag9s += 5066549584330752ULL;
  top_diag7s += 702ULL;
  right_columns += 136803100743696384ULL;
  bottom_diag9s += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2D4;
  flipped = true;
}

void UndoFlip_F2D4_B6() {
  top_rows -= 77312950434ULL;
  left_columns -= 77309411490ULL;
  top_diag9s -= 5066549584330752ULL;
  top_diag7s -= 708ULL;
  right_columns -= 136803100743696384ULL;
  bottom_diag9s -= 45598946227126290ULL;
  bottom_rows -= 6262062317568ULL;
  disk_difference -= 8;
}

void Flip_F2D4_B6() {
  top_rows += 77312950434ULL;
  left_columns += 77309411490ULL;
  top_diag9s += 5066549584330752ULL;
  top_diag7s += 708ULL;
  right_columns += 136803100743696384ULL;
  bottom_diag9s += 45598946227126290ULL;
  bottom_rows += 6262062317568ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2D4_B6;
  flipped = true;
}

void UndoFlip_F2C5() {
  top_rows -= 77312950434ULL;
  left_columns -= 3539106ULL;
  top_diag9s -= 5066549584330752ULL;
  top_diag7s -= 720ULL;
  right_columns -= 136803100743696384ULL;
  bottom_diag9s -= 45598946230665216ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 8;
}

void Flip_F2C5() {
  top_rows += 77312950434ULL;
  left_columns += 3539106ULL;
  top_diag9s += 5066549584330752ULL;
  top_diag7s += 720ULL;
  right_columns += 136803100743696384ULL;
  bottom_diag9s += 45598946230665216ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2C5;
  flipped = true;
}

void UndoFlip_F2B6() {
  top_rows -= 77312950434ULL;
  left_columns -= 77312950434ULL;
  top_diag9s -= 5066549584330752ULL;
  top_diag7s -= 726ULL;
  right_columns -= 136803100743696384ULL;
  bottom_diag9s -= 45598946230665234ULL;
  bottom_rows -= 136803100743696384ULL;
  disk_difference -= 10;
}

void Flip_F2B6() {
  top_rows += 77312950434ULL;
  left_columns += 77312950434ULL;
  top_diag9s += 5066549584330752ULL;
  top_diag7s += 726ULL;
  right_columns += 136803100743696384ULL;
  bottom_diag9s += 45598946230665234ULL;
  bottom_rows += 136803100743696384ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2B6;
  flipped = true;
}

void UndoFlip_E3_C5() {
  top_rows -= 3538944ULL;
  left_columns -= 3538944ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 180ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 3538944ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_E3_C5() {
  top_rows += 3538944ULL;
  left_columns += 3538944ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 180ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 3538944ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_C5;
  flipped = true;
}

void UndoFlip_E3_C5B6() {
  top_rows -= 3538944ULL;
  left_columns -= 77312950272ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 186ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 3538962ULL;
  bottom_rows -= 136803100743696384ULL;
  disk_difference -= 6;
}

void Flip_E3_C5B6() {
  top_rows += 3538944ULL;
  left_columns += 77312950272ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 186ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 3538962ULL;
  bottom_rows += 136803100743696384ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_C5B6;
  flipped = true;
}

void UndoFlip_E3D4() {
  top_rows -= 3539106ULL;
  left_columns -= 162ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 216ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_E3D4() {
  top_rows += 3539106ULL;
  left_columns += 162ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 216ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3D4;
  flipped = true;
}

void UndoFlip_E3D4_B6() {
  top_rows -= 3539106ULL;
  left_columns -= 77309411490ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 222ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 45598946227126290ULL;
  bottom_rows -= 6262062317568ULL;
  disk_difference -= 6;
}

void Flip_E3D4_B6() {
  top_rows += 3539106ULL;
  left_columns += 77309411490ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 222ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 45598946227126290ULL;
  bottom_rows += 6262062317568ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3D4_B6;
  flipped = true;
}

void UndoFlip_E3C5() {
  top_rows -= 3539106ULL;
  left_columns -= 3539106ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 234ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 45598946230665216ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 6;
}

void Flip_E3C5() {
  top_rows += 3539106ULL;
  left_columns += 3539106ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 234ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 45598946230665216ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3C5;
  flipped = true;
}

void UndoFlip_E3B6() {
  top_rows -= 3539106ULL;
  left_columns -= 77312950434ULL;
  top_diag9s -= 3538944ULL;
  top_diag7s -= 240ULL;
  right_columns -= 136796838681378816ULL;
  bottom_diag9s -= 45598946230665234ULL;
  bottom_rows -= 136803100743696384ULL;
  disk_difference -= 8;
}

void Flip_E3B6() {
  top_rows += 3539106ULL;
  left_columns += 77312950434ULL;
  top_diag9s += 3538944ULL;
  top_diag7s += 240ULL;
  right_columns += 136796838681378816ULL;
  bottom_diag9s += 45598946230665234ULL;
  bottom_rows += 136803100743696384ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3B6;
  flipped = true;
}

void UndoFlip_D4_B6() {
  top_rows -= 162ULL;
  left_columns -= 77309411490ULL;
  top_diag7s -= 60ULL;
  bottom_diag9s -= 45598946227126290ULL;
  bottom_rows -= 6262062317568ULL;
  disk_difference -= 4;
}

void Flip_D4_B6() {
  top_rows += 162ULL;
  left_columns += 77309411490ULL;
  top_diag7s += 60ULL;
  bottom_diag9s += 45598946227126290ULL;
  bottom_rows += 6262062317568ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_B6;
  flipped = true;
}

void UndoFlip_D4C5() {
  top_rows -= 162ULL;
  left_columns -= 3539106ULL;
  top_diag7s -= 72ULL;
  bottom_diag9s -= 45598946230665216ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_D4C5() {
  top_rows += 162ULL;
  left_columns += 3539106ULL;
  top_diag7s += 72ULL;
  bottom_diag9s += 45598946230665216ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4C5;
  flipped = true;
}

void UndoFlip_D4B6() {
  top_rows -= 162ULL;
  left_columns -= 77312950434ULL;
  top_diag7s -= 78ULL;
  bottom_diag9s -= 45598946230665234ULL;
  bottom_rows -= 136803100743696384ULL;
  disk_difference -= 6;
}

void Flip_D4B6() {
  top_rows += 162ULL;
  left_columns += 77312950434ULL;
  top_diag7s += 78ULL;
  bottom_diag9s += 45598946230665234ULL;
  bottom_rows += 136803100743696384ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4B6;
  flipped = true;
}

void UndoFlip_C5B6() {
  left_columns -= 77312950272ULL;
  top_diag7s -= 24ULL;
  bottom_diag9s -= 3538962ULL;
  bottom_rows -= 136803100743696384ULL;
  disk_difference -= 4;
}

void Flip_C5B6() {
  left_columns += 77312950272ULL;
  top_diag7s += 24ULL;
  bottom_diag9s += 3538962ULL;
  bottom_rows += 136803100743696384ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5B6;
  flipped = true;
}

void UndoFlip_G2_E4() {
  top_rows -= 25769803830ULL;
  top_diag9s -= 432345564227567670ULL;
  right_columns -= 45598946322677760ULL;
  bottom_diag7s -= 455989462271262720ULL;
  disk_difference -= 4;
}

void Flip_G2_E4() {
  top_rows += 25769803830ULL;
  top_diag9s += 432345564227567670ULL;
  right_columns += 45598946322677760ULL;
  bottom_diag7s += 455989462271262720ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_E4;
  flipped = true;
}

void UndoFlip_G2_E4D5() {
  top_rows -= 25769803830ULL;
  left_columns -= 54ULL;
  top_diag9s -= 432345564227567670ULL;
  right_columns -= 45598946322677760ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 471189111013638144ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_G2_E4D5() {
  top_rows += 25769803830ULL;
  left_columns += 54ULL;
  top_diag9s += 432345564227567670ULL;
  right_columns += 45598946322677760ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 471189111013638144ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_E4D5;
  flipped = true;
}

void UndoFlip_G2_E4C6() {
  top_rows -= 25769803830ULL;
  left_columns -= 1179702ULL;
  top_diag9s -= 432345564227567670ULL;
  right_columns -= 45598946322677760ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 476255660594429952ULL;
  bottom_rows -= 45601033581232128ULL;
  disk_difference -= 8;
}

void Flip_G2_E4C6() {
  top_rows += 25769803830ULL;
  left_columns += 1179702ULL;
  top_diag9s += 432345564227567670ULL;
  right_columns += 45598946322677760ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 476255660594429952ULL;
  bottom_rows += 45601033581232128ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_E4C6;
  flipped = true;
}

void UndoFlip_G2_E4B7() {
  top_rows -= 25769803830ULL;
  left_columns -= 25770983478ULL;
  top_diag9s -= 432345589997371446ULL;
  right_columns -= 45598946322677760ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 477944510454693888ULL;
  bottom_rows -= 45601033676783616ULL;
  disk_difference -= 10;
}

void Flip_G2_E4B7() {
  top_rows += 25769803830ULL;
  left_columns += 25770983478ULL;
  top_diag9s += 432345589997371446ULL;
  right_columns += 45598946322677760ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 477944510454693888ULL;
  bottom_rows += 45601033676783616ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2_E4B7;
  flipped = true;
}

void UndoFlip_G2F3() {
  top_rows -= 25770983424ULL;
  top_diag9s -= 432365355436867584ULL;
  right_columns -= 2087449657344ULL;
  bottom_diag7s -= 547187354725515264ULL;
  disk_difference -= 4;
}

void Flip_G2F3() {
  top_rows += 25770983424ULL;
  top_diag9s += 432365355436867584ULL;
  right_columns += 2087449657344ULL;
  bottom_diag7s += 547187354725515264ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2F3;
  flipped = true;
}

void UndoFlip_G2F3_D5() {
  top_rows -= 25770983424ULL;
  left_columns -= 54ULL;
  top_diag9s -= 432365355436867584ULL;
  right_columns -= 2087449657344ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 562387003467890688ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_G2F3_D5() {
  top_rows += 25770983424ULL;
  left_columns += 54ULL;
  top_diag9s += 432365355436867584ULL;
  right_columns += 2087449657344ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 562387003467890688ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2F3_D5;
  flipped = true;
}

void UndoFlip_G2F3_D5C6() {
  top_rows -= 25770983424ULL;
  left_columns -= 1179702ULL;
  top_diag9s -= 432365355436867584ULL;
  right_columns -= 2087449657344ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 567453553048682496ULL;
  bottom_rows -= 45601033581232128ULL;
  disk_difference -= 8;
}

void Flip_G2F3_D5C6() {
  top_rows += 25770983424ULL;
  left_columns += 1179702ULL;
  top_diag9s += 432365355436867584ULL;
  right_columns += 2087449657344ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 567453553048682496ULL;
  bottom_rows += 45601033581232128ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2F3_D5C6;
  flipped = true;
}

void UndoFlip_G2F3_D5B7() {
  top_rows -= 25770983424ULL;
  left_columns -= 25770983478ULL;
  top_diag9s -= 432365381206671360ULL;
  right_columns -= 2087449657344ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 569142402908946432ULL;
  bottom_rows -= 45601033676783616ULL;
  disk_difference -= 10;
}

void Flip_G2F3_D5B7() {
  top_rows += 25770983424ULL;
  left_columns += 25770983478ULL;
  top_diag9s += 432365381206671360ULL;
  right_columns += 2087449657344ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 569142402908946432ULL;
  bottom_rows += 45601033676783616ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2F3_D5B7;
  flipped = true;
}

void UndoFlip_G2E4() {
  top_rows -= 25770983478ULL;
  top_diag9s -= 432365355436867638ULL;
  right_columns -= 45601033676783616ULL;
  bottom_diag7s -= 592786300952641536ULL;
  disk_difference -= 6;
}

void Flip_G2E4() {
  top_rows += 25770983478ULL;
  top_diag9s += 432365355436867638ULL;
  right_columns += 45601033676783616ULL;
  bottom_diag7s += 592786300952641536ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2E4;
  flipped = true;
}

void UndoFlip_G2E4_C6() {
  top_rows -= 25770983478ULL;
  left_columns -= 1179648ULL;
  top_diag9s -= 432365355436867638ULL;
  right_columns -= 45601033676783616ULL;
  bottom_diag9s -= 4608ULL;
  bottom_diag7s -= 597852850533433344ULL;
  bottom_rows -= 2087354105856ULL;
  disk_difference -= 8;
}

void Flip_G2E4_C6() {
  top_rows += 25770983478ULL;
  left_columns += 1179648ULL;
  top_diag9s += 432365355436867638ULL;
  right_columns += 45601033676783616ULL;
  bottom_diag9s += 4608ULL;
  bottom_diag7s += 597852850533433344ULL;
  bottom_rows += 2087354105856ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2E4_C6;
  flipped = true;
}

void UndoFlip_G2E4_C6B7() {
  top_rows -= 25770983478ULL;
  left_columns -= 25770983424ULL;
  top_diag9s -= 432365381206671414ULL;
  right_columns -= 45601033676783616ULL;
  bottom_diag9s -= 4608ULL;
  bottom_diag7s -= 599541700393697280ULL;
  bottom_rows -= 2087449657344ULL;
  disk_difference -= 10;
}

void Flip_G2E4_C6B7() {
  top_rows += 25770983478ULL;
  left_columns += 25770983424ULL;
  top_diag9s += 432365381206671414ULL;
  right_columns += 45601033676783616ULL;
  bottom_diag9s += 4608ULL;
  bottom_diag7s += 599541700393697280ULL;
  bottom_rows += 2087449657344ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2E4_C6B7;
  flipped = true;
}

void UndoFlip_G2D5() {
  top_rows -= 25770983478ULL;
  left_columns -= 54ULL;
  top_diag9s -= 432365355436867638ULL;
  right_columns -= 45601033676783616ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 607985949695016960ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 8;
}

void Flip_G2D5() {
  top_rows += 25770983478ULL;
  left_columns += 54ULL;
  top_diag9s += 432365355436867638ULL;
  right_columns += 45601033676783616ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 607985949695016960ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2D5;
  flipped = true;
}

void UndoFlip_G2D5_B7() {
  top_rows -= 25770983478ULL;
  left_columns -= 25769803830ULL;
  top_diag9s -= 432365381206671414ULL;
  right_columns -= 45601033676783616ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 609674799555280896ULL;
  bottom_rows -= 45598946322677760ULL;
  disk_difference -= 10;
}

void Flip_G2D5_B7() {
  top_rows += 25770983478ULL;
  left_columns += 25769803830ULL;
  top_diag9s += 432365381206671414ULL;
  right_columns += 45601033676783616ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 609674799555280896ULL;
  bottom_rows += 45598946322677760ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2D5_B7;
  flipped = true;
}

void UndoFlip_G2C6() {
  top_rows -= 25770983478ULL;
  left_columns -= 1179702ULL;
  top_diag9s -= 432365355436867638ULL;
  right_columns -= 45601033676783616ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 613052499275808768ULL;
  bottom_rows -= 45601033581232128ULL;
  disk_difference -= 10;
}

void Flip_G2C6() {
  top_rows += 25770983478ULL;
  left_columns += 1179702ULL;
  top_diag9s += 432365355436867638ULL;
  right_columns += 45601033676783616ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 613052499275808768ULL;
  bottom_rows += 45601033581232128ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2C6;
  flipped = true;
}

void UndoFlip_G2B7() {
  top_rows -= 25770983478ULL;
  left_columns -= 25770983478ULL;
  top_diag9s -= 432365381206671414ULL;
  right_columns -= 45601033676783616ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 614741349136072704ULL;
  bottom_rows -= 45601033676783616ULL;
  disk_difference -= 12;
}

void Flip_G2B7() {
  top_rows += 25770983478ULL;
  left_columns += 25770983478ULL;
  top_diag9s += 432365381206671414ULL;
  right_columns += 45601033676783616ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 614741349136072704ULL;
  bottom_rows += 45601033676783616ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G2B7;
  flipped = true;
}

void UndoFlip_F3_D5() {
  top_rows -= 1179648ULL;
  left_columns -= 54ULL;
  top_diag9s -= 19791209299968ULL;
  right_columns -= 2087354105856ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 151996487423754240ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_F3_D5() {
  top_rows += 1179648ULL;
  left_columns += 54ULL;
  top_diag9s += 19791209299968ULL;
  right_columns += 2087354105856ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 151996487423754240ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_D5;
  flipped = true;
}

void UndoFlip_F3_D5C6() {
  top_rows -= 1179648ULL;
  left_columns -= 1179702ULL;
  top_diag9s -= 19791209299968ULL;
  right_columns -= 2087354105856ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 157063037004546048ULL;
  bottom_rows -= 45601033581232128ULL;
  disk_difference -= 6;
}

void Flip_F3_D5C6() {
  top_rows += 1179648ULL;
  left_columns += 1179702ULL;
  top_diag9s += 19791209299968ULL;
  right_columns += 2087354105856ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 157063037004546048ULL;
  bottom_rows += 45601033581232128ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_D5C6;
  flipped = true;
}

void UndoFlip_F3_D5B7() {
  top_rows -= 1179648ULL;
  left_columns -= 25770983478ULL;
  top_diag9s -= 19816979103744ULL;
  right_columns -= 2087354105856ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 158751886864809984ULL;
  bottom_rows -= 45601033676783616ULL;
  disk_difference -= 8;
}

void Flip_F3_D5B7() {
  top_rows += 1179648ULL;
  left_columns += 25770983478ULL;
  top_diag9s += 19816979103744ULL;
  right_columns += 2087354105856ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 158751886864809984ULL;
  bottom_rows += 45601033676783616ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3_D5B7;
  flipped = true;
}

void UndoFlip_F3E4() {
  top_rows -= 1179702ULL;
  top_diag9s -= 19791209300022ULL;
  right_columns -= 45601033581232128ULL;
  bottom_diag7s -= 182395784908505088ULL;
  disk_difference -= 4;
}

void Flip_F3E4() {
  top_rows += 1179702ULL;
  top_diag9s += 19791209300022ULL;
  right_columns += 45601033581232128ULL;
  bottom_diag7s += 182395784908505088ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3E4;
  flipped = true;
}

void UndoFlip_F3E4_C6() {
  top_rows -= 1179702ULL;
  left_columns -= 1179648ULL;
  top_diag9s -= 19791209300022ULL;
  right_columns -= 45601033581232128ULL;
  bottom_diag9s -= 4608ULL;
  bottom_diag7s -= 187462334489296896ULL;
  bottom_rows -= 2087354105856ULL;
  disk_difference -= 6;
}

void Flip_F3E4_C6() {
  top_rows += 1179702ULL;
  left_columns += 1179648ULL;
  top_diag9s += 19791209300022ULL;
  right_columns += 45601033581232128ULL;
  bottom_diag9s += 4608ULL;
  bottom_diag7s += 187462334489296896ULL;
  bottom_rows += 2087354105856ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3E4_C6;
  flipped = true;
}

void UndoFlip_F3E4_C6B7() {
  top_rows -= 1179702ULL;
  left_columns -= 25770983424ULL;
  top_diag9s -= 19816979103798ULL;
  right_columns -= 45601033581232128ULL;
  bottom_diag9s -= 4608ULL;
  bottom_diag7s -= 189151184349560832ULL;
  bottom_rows -= 2087449657344ULL;
  disk_difference -= 8;
}

void Flip_F3E4_C6B7() {
  top_rows += 1179702ULL;
  left_columns += 25770983424ULL;
  top_diag9s += 19816979103798ULL;
  right_columns += 45601033581232128ULL;
  bottom_diag9s += 4608ULL;
  bottom_diag7s += 189151184349560832ULL;
  bottom_rows += 2087449657344ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3E4_C6B7;
  flipped = true;
}

void UndoFlip_F3D5() {
  top_rows -= 1179702ULL;
  left_columns -= 54ULL;
  top_diag9s -= 19791209300022ULL;
  right_columns -= 45601033581232128ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 197595433650880512ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_F3D5() {
  top_rows += 1179702ULL;
  left_columns += 54ULL;
  top_diag9s += 19791209300022ULL;
  right_columns += 45601033581232128ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 197595433650880512ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3D5;
  flipped = true;
}

void UndoFlip_F3D5_B7() {
  top_rows -= 1179702ULL;
  left_columns -= 25769803830ULL;
  top_diag9s -= 19816979103798ULL;
  right_columns -= 45601033581232128ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 199284283511144448ULL;
  bottom_rows -= 45598946322677760ULL;
  disk_difference -= 8;
}

void Flip_F3D5_B7() {
  top_rows += 1179702ULL;
  left_columns += 25769803830ULL;
  top_diag9s += 19816979103798ULL;
  right_columns += 45601033581232128ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 199284283511144448ULL;
  bottom_rows += 45598946322677760ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3D5_B7;
  flipped = true;
}

void UndoFlip_F3C6() {
  top_rows -= 1179702ULL;
  left_columns -= 1179702ULL;
  top_diag9s -= 19791209300022ULL;
  right_columns -= 45601033581232128ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 202661983231672320ULL;
  bottom_rows -= 45601033581232128ULL;
  disk_difference -= 8;
}

void Flip_F3C6() {
  top_rows += 1179702ULL;
  left_columns += 1179702ULL;
  top_diag9s += 19791209300022ULL;
  right_columns += 45601033581232128ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 202661983231672320ULL;
  bottom_rows += 45601033581232128ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3C6;
  flipped = true;
}

void UndoFlip_F3B7() {
  top_rows -= 1179702ULL;
  left_columns -= 25770983478ULL;
  top_diag9s -= 19816979103798ULL;
  right_columns -= 45601033581232128ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 204350833091936256ULL;
  bottom_rows -= 45601033676783616ULL;
  disk_difference -= 10;
}

void Flip_F3B7() {
  top_rows += 1179702ULL;
  left_columns += 25770983478ULL;
  top_diag9s += 19816979103798ULL;
  right_columns += 45601033581232128ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 204350833091936256ULL;
  bottom_rows += 45601033676783616ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3B7;
  flipped = true;
}

void UndoFlip_E4_C6() {
  top_rows -= 54ULL;
  left_columns -= 1179648ULL;
  top_diag9s -= 54ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 4608ULL;
  bottom_diag7s -= 50665495807918080ULL;
  bottom_rows -= 2087354105856ULL;
  disk_difference -= 4;
}

void Flip_E4_C6() {
  top_rows += 54ULL;
  left_columns += 1179648ULL;
  top_diag9s += 54ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 4608ULL;
  bottom_diag7s += 50665495807918080ULL;
  bottom_rows += 2087354105856ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_C6;
  flipped = true;
}

void UndoFlip_E4_C6B7() {
  top_rows -= 54ULL;
  left_columns -= 25770983424ULL;
  top_diag9s -= 25769803830ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 4608ULL;
  bottom_diag7s -= 52354345668182016ULL;
  bottom_rows -= 2087449657344ULL;
  disk_difference -= 6;
}

void Flip_E4_C6B7() {
  top_rows += 54ULL;
  left_columns += 25770983424ULL;
  top_diag9s += 25769803830ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 4608ULL;
  bottom_diag7s += 52354345668182016ULL;
  bottom_rows += 2087449657344ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_C6B7;
  flipped = true;
}

void UndoFlip_E4D5() {
  top_rows -= 54ULL;
  left_columns -= 54ULL;
  top_diag9s -= 54ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 60798594969501696ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_E4D5() {
  top_rows += 54ULL;
  left_columns += 54ULL;
  top_diag9s += 54ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 60798594969501696ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4D5;
  flipped = true;
}

void UndoFlip_E4D5_B7() {
  top_rows -= 54ULL;
  left_columns -= 25769803830ULL;
  top_diag9s -= 25769803830ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 62487444829765632ULL;
  bottom_rows -= 45598946322677760ULL;
  disk_difference -= 6;
}

void Flip_E4D5_B7() {
  top_rows += 54ULL;
  left_columns += 25769803830ULL;
  top_diag9s += 25769803830ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 62487444829765632ULL;
  bottom_rows += 45598946322677760ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4D5_B7;
  flipped = true;
}

void UndoFlip_E4C6() {
  top_rows -= 54ULL;
  left_columns -= 1179702ULL;
  top_diag9s -= 54ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 65865144550293504ULL;
  bottom_rows -= 45601033581232128ULL;
  disk_difference -= 6;
}

void Flip_E4C6() {
  top_rows += 54ULL;
  left_columns += 1179702ULL;
  top_diag9s += 54ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 65865144550293504ULL;
  bottom_rows += 45601033581232128ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4C6;
  flipped = true;
}

void UndoFlip_E4B7() {
  top_rows -= 54ULL;
  left_columns -= 25770983478ULL;
  top_diag9s -= 25769803830ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 67553994410557440ULL;
  bottom_rows -= 45601033676783616ULL;
  disk_difference -= 8;
}

void Flip_E4B7() {
  top_rows += 54ULL;
  left_columns += 25770983478ULL;
  top_diag9s += 25769803830ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 67553994410557440ULL;
  bottom_rows += 45601033676783616ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4B7;
  flipped = true;
}

void UndoFlip_D5_B7() {
  left_columns -= 25769803830ULL;
  top_diag9s -= 25769803776ULL;
  bottom_diag9s -= 231928233984ULL;
  bottom_diag7s -= 16888498602639360ULL;
  bottom_rows -= 45598946322677760ULL;
  disk_difference -= 4;
}

void Flip_D5_B7() {
  left_columns += 25769803830ULL;
  top_diag9s += 25769803776ULL;
  bottom_diag9s += 231928233984ULL;
  bottom_diag7s += 16888498602639360ULL;
  bottom_rows += 45598946322677760ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5_B7;
  flipped = true;
}

void UndoFlip_D5C6() {
  left_columns -= 1179702ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 20266198323167232ULL;
  bottom_rows -= 45601033581232128ULL;
  disk_difference -= 4;
}

void Flip_D5C6() {
  left_columns += 1179702ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 20266198323167232ULL;
  bottom_rows += 45601033581232128ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5C6;
  flipped = true;
}

void UndoFlip_D5B7() {
  left_columns -= 25770983478ULL;
  top_diag9s -= 25769803776ULL;
  bottom_diag9s -= 231928238592ULL;
  bottom_diag7s -= 21955048183431168ULL;
  bottom_rows -= 45601033676783616ULL;
  disk_difference -= 6;
}

void Flip_D5B7() {
  left_columns += 25770983478ULL;
  top_diag9s += 25769803776ULL;
  bottom_diag9s += 231928238592ULL;
  bottom_diag7s += 21955048183431168ULL;
  bottom_rows += 45601033676783616ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5B7;
  flipped = true;
}

void UndoFlip_C6B7() {
  left_columns -= 25770983424ULL;
  top_diag9s -= 25769803776ULL;
  bottom_diag9s -= 4608ULL;
  bottom_diag7s -= 6755399441055744ULL;
  bottom_rows -= 2087449657344ULL;
  disk_difference -= 4;
}

void Flip_C6B7() {
  left_columns += 25770983424ULL;
  top_diag9s += 25769803776ULL;
  bottom_diag9s += 4608ULL;
  bottom_diag7s += 6755399441055744ULL;
  bottom_rows += 2087449657344ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6B7;
  flipped = true;
}

void UndoFlip_G3_E5() {
  top_rows -= 393216ULL;
  top_diag9s -= 1688849860263936ULL;
  right_columns -= 15199648774225920ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 2319282339840ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 4;
}

void Flip_G3_E5() {
  top_rows += 393216ULL;
  top_diag9s += 1688849860263936ULL;
  right_columns += 15199648774225920ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 2319282339840ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_E5;
  flipped = true;
}

void UndoFlip_G3_E5D6() {
  top_rows -= 393216ULL;
  left_columns -= 18ULL;
  top_diag9s -= 1688849860263936ULL;
  right_columns -= 15199648774225920ULL;
  bottom_diag9s -= 15199648743555072ULL;
  bottom_diag7s -= 2396591751168ULL;
  bottom_rows -= 15200344527077376ULL;
  disk_difference -= 6;
}

void Flip_G3_E5D6() {
  top_rows += 393216ULL;
  left_columns += 18ULL;
  top_diag9s += 1688849860263936ULL;
  right_columns += 15199648774225920ULL;
  bottom_diag9s += 15199648743555072ULL;
  bottom_diag7s += 2396591751168ULL;
  bottom_rows += 15200344527077376ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_E5D6;
  flipped = true;
}

void UndoFlip_G3_E5C7() {
  top_rows -= 393216ULL;
  left_columns -= 393234ULL;
  top_diag9s -= 1688849860263936ULL;
  right_columns -= 15199648774225920ULL;
  bottom_diag9s -= 15199648743555078ULL;
  bottom_diag7s -= 2422361554944ULL;
  bottom_rows -= 15200344558927872ULL;
  disk_difference -= 8;
}

void Flip_G3_E5C7() {
  top_rows += 393216ULL;
  left_columns += 393234ULL;
  top_diag9s += 1688849860263936ULL;
  right_columns += 15199648774225920ULL;
  bottom_diag9s += 15199648743555078ULL;
  bottom_diag7s += 2422361554944ULL;
  bottom_rows += 15200344558927872ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3_E5C7;
  flipped = true;
}

void UndoFlip_G3F4() {
  top_rows -= 393234ULL;
  top_diag9s -= 1688849861443584ULL;
  right_columns -= 695816552448ULL;
  bottom_diag7s -= 2783138807808ULL;
  disk_difference -= 4;
}

void Flip_G3F4() {
  top_rows += 393234ULL;
  top_diag9s += 1688849861443584ULL;
  right_columns += 695816552448ULL;
  bottom_diag7s += 2783138807808ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3F4;
  flipped = true;
}

void UndoFlip_G3F4_D6() {
  top_rows -= 393234ULL;
  left_columns -= 18ULL;
  top_diag9s -= 1688849861443584ULL;
  right_columns -= 695816552448ULL;
  bottom_diag9s -= 1179648ULL;
  bottom_diag7s -= 2860448219136ULL;
  bottom_rows -= 695784701952ULL;
  disk_difference -= 6;
}

void Flip_G3F4_D6() {
  top_rows += 393234ULL;
  left_columns += 18ULL;
  top_diag9s += 1688849861443584ULL;
  right_columns += 695816552448ULL;
  bottom_diag9s += 1179648ULL;
  bottom_diag7s += 2860448219136ULL;
  bottom_rows += 695784701952ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3F4_D6;
  flipped = true;
}

void UndoFlip_G3F4_D6C7() {
  top_rows -= 393234ULL;
  left_columns -= 393234ULL;
  top_diag9s -= 1688849861443584ULL;
  right_columns -= 695816552448ULL;
  bottom_diag9s -= 1179654ULL;
  bottom_diag7s -= 2886218022912ULL;
  bottom_rows -= 695816552448ULL;
  disk_difference -= 8;
}

void Flip_G3F4_D6C7() {
  top_rows += 393234ULL;
  left_columns += 393234ULL;
  top_diag9s += 1688849861443584ULL;
  right_columns += 695816552448ULL;
  bottom_diag9s += 1179654ULL;
  bottom_diag7s += 2886218022912ULL;
  bottom_rows += 695816552448ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3F4_D6C7;
  flipped = true;
}

void UndoFlip_G3E5() {
  top_rows -= 393234ULL;
  top_diag9s -= 1688849861443584ULL;
  right_columns -= 15200344558927872ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 3015067041792ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 6;
}

void Flip_G3E5() {
  top_rows += 393234ULL;
  top_diag9s += 1688849861443584ULL;
  right_columns += 15200344558927872ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 3015067041792ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3E5;
  flipped = true;
}

void UndoFlip_G3E5_C7() {
  top_rows -= 393234ULL;
  left_columns -= 393216ULL;
  top_diag9s -= 1688849861443584ULL;
  right_columns -= 15200344558927872ULL;
  bottom_diag9s -= 15199648742375430ULL;
  bottom_diag7s -= 3040836845568ULL;
  bottom_rows -= 15199648774225920ULL;
  disk_difference -= 8;
}

void Flip_G3E5_C7() {
  top_rows += 393234ULL;
  left_columns += 393216ULL;
  top_diag9s += 1688849861443584ULL;
  right_columns += 15200344558927872ULL;
  bottom_diag9s += 15199648742375430ULL;
  bottom_diag7s += 3040836845568ULL;
  bottom_rows += 15199648774225920ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3E5_C7;
  flipped = true;
}

void UndoFlip_G3D6() {
  top_rows -= 393234ULL;
  left_columns -= 18ULL;
  top_diag9s -= 1688849861443584ULL;
  right_columns -= 15200344558927872ULL;
  bottom_diag9s -= 15199648743555072ULL;
  bottom_diag7s -= 3092376453120ULL;
  bottom_rows -= 15200344527077376ULL;
  disk_difference -= 8;
}

void Flip_G3D6() {
  top_rows += 393234ULL;
  left_columns += 18ULL;
  top_diag9s += 1688849861443584ULL;
  right_columns += 15200344558927872ULL;
  bottom_diag9s += 15199648743555072ULL;
  bottom_diag7s += 3092376453120ULL;
  bottom_rows += 15200344527077376ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3D6;
  flipped = true;
}

void UndoFlip_G3C7() {
  top_rows -= 393234ULL;
  left_columns -= 393234ULL;
  top_diag9s -= 1688849861443584ULL;
  right_columns -= 15200344558927872ULL;
  bottom_diag9s -= 15199648743555078ULL;
  bottom_diag7s -= 3118146256896ULL;
  bottom_rows -= 15200344558927872ULL;
  disk_difference -= 10;
}

void Flip_G3C7() {
  top_rows += 393234ULL;
  left_columns += 393234ULL;
  top_diag9s += 1688849861443584ULL;
  right_columns += 15200344558927872ULL;
  bottom_diag9s += 15199648743555078ULL;
  bottom_diag7s += 3118146256896ULL;
  bottom_rows += 15200344558927872ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G3C7;
  flipped = true;
}

void UndoFlip_F4_D6() {
  top_rows -= 18ULL;
  left_columns -= 18ULL;
  top_diag9s -= 1179648ULL;
  right_columns -= 695784701952ULL;
  bottom_diag9s -= 1179648ULL;
  bottom_diag7s -= 773094113280ULL;
  bottom_rows -= 695784701952ULL;
  disk_difference -= 4;
}

void Flip_F4_D6() {
  top_rows += 18ULL;
  left_columns += 18ULL;
  top_diag9s += 1179648ULL;
  right_columns += 695784701952ULL;
  bottom_diag9s += 1179648ULL;
  bottom_diag7s += 773094113280ULL;
  bottom_rows += 695784701952ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4_D6;
  flipped = true;
}

void UndoFlip_F4_D6C7() {
  top_rows -= 18ULL;
  left_columns -= 393234ULL;
  top_diag9s -= 1179648ULL;
  right_columns -= 695784701952ULL;
  bottom_diag9s -= 1179654ULL;
  bottom_diag7s -= 798863917056ULL;
  bottom_rows -= 695816552448ULL;
  disk_difference -= 6;
}

void Flip_F4_D6C7() {
  top_rows += 18ULL;
  left_columns += 393234ULL;
  top_diag9s += 1179648ULL;
  right_columns += 695784701952ULL;
  bottom_diag9s += 1179654ULL;
  bottom_diag7s += 798863917056ULL;
  bottom_rows += 695816552448ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4_D6C7;
  flipped = true;
}

void UndoFlip_F4E5() {
  top_rows -= 18ULL;
  top_diag9s -= 1179648ULL;
  right_columns -= 15200344527077376ULL;
  bottom_diag9s -= 15199648742375424ULL;
  bottom_diag7s -= 927712935936ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 4;
}

void Flip_F4E5() {
  top_rows += 18ULL;
  top_diag9s += 1179648ULL;
  right_columns += 15200344527077376ULL;
  bottom_diag9s += 15199648742375424ULL;
  bottom_diag7s += 927712935936ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4E5;
  flipped = true;
}

void UndoFlip_F4E5_C7() {
  top_rows -= 18ULL;
  left_columns -= 393216ULL;
  top_diag9s -= 1179648ULL;
  right_columns -= 15200344527077376ULL;
  bottom_diag9s -= 15199648742375430ULL;
  bottom_diag7s -= 953482739712ULL;
  bottom_rows -= 15199648774225920ULL;
  disk_difference -= 6;
}

void Flip_F4E5_C7() {
  top_rows += 18ULL;
  left_columns += 393216ULL;
  top_diag9s += 1179648ULL;
  right_columns += 15200344527077376ULL;
  bottom_diag9s += 15199648742375430ULL;
  bottom_diag7s += 953482739712ULL;
  bottom_rows += 15199648774225920ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4E5_C7;
  flipped = true;
}

void UndoFlip_F4D6() {
  top_rows -= 18ULL;
  left_columns -= 18ULL;
  top_diag9s -= 1179648ULL;
  right_columns -= 15200344527077376ULL;
  bottom_diag9s -= 15199648743555072ULL;
  bottom_diag7s -= 1005022347264ULL;
  bottom_rows -= 15200344527077376ULL;
  disk_difference -= 6;
}

void Flip_F4D6() {
  top_rows += 18ULL;
  left_columns += 18ULL;
  top_diag9s += 1179648ULL;
  right_columns += 15200344527077376ULL;
  bottom_diag9s += 15199648743555072ULL;
  bottom_diag7s += 1005022347264ULL;
  bottom_rows += 15200344527077376ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4D6;
  flipped = true;
}

void UndoFlip_F4C7() {
  top_rows -= 18ULL;
  left_columns -= 393234ULL;
  top_diag9s -= 1179648ULL;
  right_columns -= 15200344527077376ULL;
  bottom_diag9s -= 15199648743555078ULL;
  bottom_diag7s -= 1030792151040ULL;
  bottom_rows -= 15200344558927872ULL;
  disk_difference -= 8;
}

void Flip_F4C7() {
  top_rows += 18ULL;
  left_columns += 393234ULL;
  top_diag9s += 1179648ULL;
  right_columns += 15200344527077376ULL;
  bottom_diag9s += 15199648743555078ULL;
  bottom_diag7s += 1030792151040ULL;
  bottom_rows += 15200344558927872ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4C7;
  flipped = true;
}

void UndoFlip_E5_C7() {
  left_columns -= 393216ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 15199648742375430ULL;
  bottom_diag7s -= 257698037760ULL;
  bottom_rows -= 15199648774225920ULL;
  disk_difference -= 4;
}

void Flip_E5_C7() {
  left_columns += 393216ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 15199648742375430ULL;
  bottom_diag7s += 257698037760ULL;
  bottom_rows += 15199648774225920ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5_C7;
  flipped = true;
}

void UndoFlip_E5D6() {
  left_columns -= 18ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 15199648743555072ULL;
  bottom_diag7s -= 309237645312ULL;
  bottom_rows -= 15200344527077376ULL;
  disk_difference -= 4;
}

void Flip_E5D6() {
  left_columns += 18ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 15199648743555072ULL;
  bottom_diag7s += 309237645312ULL;
  bottom_rows += 15200344527077376ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5D6;
  flipped = true;
}

void UndoFlip_E5C7() {
  left_columns -= 393234ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 15199648743555078ULL;
  bottom_diag7s -= 335007449088ULL;
  bottom_rows -= 15200344558927872ULL;
  disk_difference -= 6;
}

void Flip_E5C7() {
  left_columns += 393234ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 15199648743555078ULL;
  bottom_diag7s += 335007449088ULL;
  bottom_rows += 15200344558927872ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5C7;
  flipped = true;
}

void UndoFlip_D6C7() {
  left_columns -= 393234ULL;
  bottom_diag9s -= 1179654ULL;
  bottom_diag7s -= 103079215104ULL;
  bottom_rows -= 695816552448ULL;
  disk_difference -= 4;
}

void Flip_D6C7() {
  left_columns += 393234ULL;
  bottom_diag9s += 1179654ULL;
  bottom_diag7s += 103079215104ULL;
  bottom_rows += 695816552448ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6C7;
  flipped = true;
}

void UndoFlip_G4_E6() {
  top_rows -= 6ULL;
  top_diag9s -= 6597069766656ULL;
  right_columns -= 5066549591408640ULL;
  bottom_diag9s -= 77309411328ULL;
  bottom_diag7s -= 11796480ULL;
  bottom_rows -= 231928233984ULL;
  disk_difference -= 4;
}

void Flip_G4_E6() {
  top_rows += 6ULL;
  top_diag9s += 6597069766656ULL;
  right_columns += 5066549591408640ULL;
  bottom_diag9s += 77309411328ULL;
  bottom_diag7s += 11796480ULL;
  bottom_rows += 231928233984ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4_E6;
  flipped = true;
}

void UndoFlip_G4_E6D7() {
  top_rows -= 6ULL;
  left_columns -= 6ULL;
  top_diag9s -= 6597069766656ULL;
  right_columns -= 5066549591408640ULL;
  bottom_diag9s -= 77309412864ULL;
  bottom_diag7s -= 12189696ULL;
  bottom_rows -= 231938850816ULL;
  disk_difference -= 6;
}

void Flip_G4_E6D7() {
  top_rows += 6ULL;
  left_columns += 6ULL;
  top_diag9s += 6597069766656ULL;
  right_columns += 5066549591408640ULL;
  bottom_diag9s += 77309412864ULL;
  bottom_diag7s += 12189696ULL;
  bottom_rows += 231938850816ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4_E6D7;
  flipped = true;
}

void UndoFlip_G4F5() {
  top_rows -= 6ULL;
  top_diag9s -= 6597069766674ULL;
  right_columns -= 231938850816ULL;
  bottom_diag7s -= 14155776ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 4;
}

void Flip_G4F5() {
  top_rows += 6ULL;
  top_diag9s += 6597069766674ULL;
  right_columns += 231938850816ULL;
  bottom_diag7s += 14155776ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4F5;
  flipped = true;
}

void UndoFlip_G4F5_D7() {
  top_rows -= 6ULL;
  left_columns -= 6ULL;
  top_diag9s -= 6597069766674ULL;
  right_columns -= 231938850816ULL;
  bottom_diag9s -= 1536ULL;
  bottom_diag7s -= 14548992ULL;
  bottom_rows -= 5066549591408640ULL;
  disk_difference -= 6;
}

void Flip_G4F5_D7() {
  top_rows += 6ULL;
  left_columns += 6ULL;
  top_diag9s += 6597069766674ULL;
  right_columns += 231938850816ULL;
  bottom_diag9s += 1536ULL;
  bottom_diag7s += 14548992ULL;
  bottom_rows += 5066549591408640ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4F5_D7;
  flipped = true;
}

void UndoFlip_G4E6() {
  top_rows -= 6ULL;
  top_diag9s -= 6597069766674ULL;
  right_columns -= 5066781519642624ULL;
  bottom_diag9s -= 77309411328ULL;
  bottom_diag7s -= 15335424ULL;
  bottom_rows -= 5066781509025792ULL;
  disk_difference -= 6;
}

void Flip_G4E6() {
  top_rows += 6ULL;
  top_diag9s += 6597069766674ULL;
  right_columns += 5066781519642624ULL;
  bottom_diag9s += 77309411328ULL;
  bottom_diag7s += 15335424ULL;
  bottom_rows += 5066781509025792ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4E6;
  flipped = true;
}

void UndoFlip_G4D7() {
  top_rows -= 6ULL;
  left_columns -= 6ULL;
  top_diag9s -= 6597069766674ULL;
  right_columns -= 5066781519642624ULL;
  bottom_diag9s -= 77309412864ULL;
  bottom_diag7s -= 15728640ULL;
  bottom_rows -= 5066781519642624ULL;
  disk_difference -= 8;
}

void Flip_G4D7() {
  top_rows += 6ULL;
  left_columns += 6ULL;
  top_diag9s += 6597069766674ULL;
  right_columns += 5066781519642624ULL;
  bottom_diag9s += 77309412864ULL;
  bottom_diag7s += 15728640ULL;
  bottom_rows += 5066781519642624ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G4D7;
  flipped = true;
}

void UndoFlip_F5_D7() {
  left_columns -= 6ULL;
  top_diag9s -= 18ULL;
  right_columns -= 231928233984ULL;
  bottom_diag9s -= 1536ULL;
  bottom_diag7s -= 3932160ULL;
  bottom_rows -= 5066549591408640ULL;
  disk_difference -= 4;
}

void Flip_F5_D7() {
  left_columns += 6ULL;
  top_diag9s += 18ULL;
  right_columns += 231928233984ULL;
  bottom_diag9s += 1536ULL;
  bottom_diag7s += 3932160ULL;
  bottom_rows += 5066549591408640ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5_D7;
  flipped = true;
}

void UndoFlip_F5E6() {
  top_diag9s -= 18ULL;
  right_columns -= 5066781509025792ULL;
  bottom_diag9s -= 77309411328ULL;
  bottom_diag7s -= 4718592ULL;
  bottom_rows -= 5066781509025792ULL;
  disk_difference -= 4;
}

void Flip_F5E6() {
  top_diag9s += 18ULL;
  right_columns += 5066781509025792ULL;
  bottom_diag9s += 77309411328ULL;
  bottom_diag7s += 4718592ULL;
  bottom_rows += 5066781509025792ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5E6;
  flipped = true;
}

void UndoFlip_F5D7() {
  left_columns -= 6ULL;
  top_diag9s -= 18ULL;
  right_columns -= 5066781509025792ULL;
  bottom_diag9s -= 77309412864ULL;
  bottom_diag7s -= 5111808ULL;
  bottom_rows -= 5066781519642624ULL;
  disk_difference -= 6;
}

void Flip_F5D7() {
  left_columns += 6ULL;
  top_diag9s += 18ULL;
  right_columns += 5066781509025792ULL;
  bottom_diag9s += 77309412864ULL;
  bottom_diag7s += 5111808ULL;
  bottom_rows += 5066781519642624ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5D7;
  flipped = true;
}

void UndoFlip_E6D7() {
  left_columns -= 6ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 77309412864ULL;
  bottom_diag7s -= 1572864ULL;
  bottom_rows -= 231938850816ULL;
  disk_difference -= 4;
}

void Flip_E6D7() {
  left_columns += 6ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 77309412864ULL;
  bottom_diag7s += 1572864ULL;
  bottom_rows += 231938850816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6D7;
  flipped = true;
}

void UndoFlip_G5_E7() {
  top_diag9s -= 393216ULL;
  right_columns -= 1688849863802880ULL;
  bottom_diag9s -= 393216ULL;
  bottom_diag7s -= 15360ULL;
  bottom_rows -= 1688849863802880ULL;
  disk_difference -= 4;
}

void Flip_G5_E7() {
  top_diag9s += 393216ULL;
  right_columns += 1688849863802880ULL;
  bottom_diag9s += 393216ULL;
  bottom_diag7s += 15360ULL;
  bottom_rows += 1688849863802880ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5_E7;
  flipped = true;
}

void UndoFlip_G5F6() {
  top_diag9s -= 393216ULL;
  right_columns -= 77312950272ULL;
  bottom_diag9s -= 5066549580791808ULL;
  bottom_diag7s -= 18432ULL;
  bottom_rows -= 1688927169675264ULL;
  disk_difference -= 4;
}

void Flip_G5F6() {
  top_diag9s += 393216ULL;
  right_columns += 77312950272ULL;
  bottom_diag9s += 5066549580791808ULL;
  bottom_diag7s += 18432ULL;
  bottom_rows += 1688927169675264ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5F6;
  flipped = true;
}

void UndoFlip_G5E7() {
  top_diag9s -= 393216ULL;
  right_columns -= 1688927173214208ULL;
  bottom_diag9s -= 5066549581185024ULL;
  bottom_diag7s -= 19968ULL;
  bottom_rows -= 1688927173214208ULL;
  disk_difference -= 6;
}

void Flip_G5E7() {
  top_diag9s += 393216ULL;
  right_columns += 1688927173214208ULL;
  bottom_diag9s += 5066549581185024ULL;
  bottom_diag7s += 19968ULL;
  bottom_rows += 1688927173214208ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G5E7;
  flipped = true;
}

void UndoFlip_F6E7() {
  right_columns -= 1688927169675264ULL;
  bottom_diag9s -= 5066549581185024ULL;
  bottom_diag7s -= 6144ULL;
  bottom_rows -= 77312950272ULL;
  disk_difference -= 4;
}

void Flip_F6E7() {
  right_columns += 1688927169675264ULL;
  bottom_diag9s += 5066549581185024ULL;
  bottom_diag7s += 6144ULL;
  bottom_rows += 77312950272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F6E7;
  flipped = true;
}

void UndoFlip_G6F7() {
  top_diag9s -= 6ULL;
  right_columns -= 25770983424ULL;
  bottom_diag9s -= 25769803776ULL;
  bottom_diag7s -= 24ULL;
  bottom_rows -= 25770983424ULL;
  disk_difference -= 4;
}

void Flip_G6F7() {
  top_diag9s += 6ULL;
  right_columns += 25770983424ULL;
  bottom_diag9s += 25769803776ULL;
  bottom_diag7s += 24ULL;
  bottom_rows += 25770983424ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_G6F7;
  flipped = true;
}

void UndoFlip_F2G3() {
  top_rows -= 77309804544ULL;
  top_diag9s -= 6755399441055744ULL;
  top_diag7s -= 486ULL;
  right_columns -= 6262094168064ULL;
  bottom_diag7s -= 2087354105856ULL;
  disk_difference -= 4;
}

void Flip_F2G3() {
  top_rows += 77309804544ULL;
  top_diag9s += 6755399441055744ULL;
  top_diag7s += 486ULL;
  right_columns += 6262094168064ULL;
  bottom_diag7s += 2087354105856ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F2G3;
  flipped = true;
}

void UndoFlip_E2_G4() {
  top_rows -= 231928233990ULL;
  top_diag9s -= 65970697666560ULL;
  top_diag7s -= 10616832ULL;
  right_columns -= 410390516054753280ULL;
  bottom_diag7s -= 10616832ULL;
  disk_difference -= 4;
}

void Flip_E2_G4() {
  top_rows += 231928233990ULL;
  top_diag9s += 65970697666560ULL;
  top_diag7s += 10616832ULL;
  right_columns += 410390516054753280ULL;
  bottom_diag7s += 10616832ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2_G4;
  flipped = true;
}

void UndoFlip_E2F3() {
  top_rows -= 231929413632ULL;
  top_diag9s -= 79164837199872ULL;
  top_diag7s -= 10616832ULL;
  right_columns -= 410392603398242304ULL;
  bottom_diag7s -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_E2F3() {
  top_rows += 231929413632ULL;
  top_diag9s += 79164837199872ULL;
  top_diag7s += 10616832ULL;
  right_columns += 410392603398242304ULL;
  bottom_diag7s += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2F3;
  flipped = true;
}

void UndoFlip_E2G4() {
  top_rows -= 231929413638ULL;
  top_diag9s -= 85761906966528ULL;
  top_diag7s -= 10616832ULL;
  right_columns -= 410392603408859136ULL;
  bottom_diag7s -= 136796838691995648ULL;
  disk_difference -= 6;
}

void Flip_E2G4() {
  top_rows += 231929413638ULL;
  top_diag9s += 85761906966528ULL;
  top_diag7s += 10616832ULL;
  right_columns += 410392603408859136ULL;
  bottom_diag7s += 136796838691995648ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E2G4;
  flipped = true;
}

void UndoFlip_F3G4() {
  top_rows -= 1179654ULL;
  top_diag9s -= 26388279066624ULL;
  right_columns -= 2087364722688ULL;
  bottom_diag7s -= 136796838691995648ULL;
  disk_difference -= 4;
}

void Flip_F3G4() {
  top_rows += 1179654ULL;
  top_diag9s += 26388279066624ULL;
  right_columns += 2087364722688ULL;
  bottom_diag7s += 136796838691995648ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F3G4;
  flipped = true;
}

void UndoFlip_D2_F4() {
  top_rows -= 695784701970ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 11796480ULL;
  top_diag7s -= 59373627899904ULL;
  right_columns -= 695784701952ULL;
  bottom_diag7s -= 695784701952ULL;
  disk_difference -= 4;
}

void Flip_D2_F4() {
  top_rows += 695784701970ULL;
  left_columns += 1458ULL;
  top_diag9s += 11796480ULL;
  top_diag7s += 59373627899904ULL;
  right_columns += 695784701952ULL;
  bottom_diag7s += 695784701952ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_F4;
  flipped = true;
}

void UndoFlip_D2_F4G5() {
  top_rows -= 695784701970ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 12189696ULL;
  top_diag7s -= 59373627899904ULL;
  right_columns -= 695788240896ULL;
  bottom_diag7s -= 695784715776ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 6;
}

void Flip_D2_F4G5() {
  top_rows += 695784701970ULL;
  left_columns += 1458ULL;
  top_diag9s += 12189696ULL;
  top_diag7s += 59373627899904ULL;
  right_columns += 695788240896ULL;
  bottom_diag7s += 695784715776ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2_F4G5;
  flipped = true;
}

void UndoFlip_D2E3() {
  top_rows -= 695788240896ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 14155776ULL;
  top_diag7s -= 59373627900066ULL;
  right_columns -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_D2E3() {
  top_rows += 695788240896ULL;
  left_columns += 1458ULL;
  top_diag9s += 14155776ULL;
  top_diag7s += 59373627900066ULL;
  right_columns += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2E3;
  flipped = true;
}

void UndoFlip_D2E3_G5() {
  top_rows -= 695788240896ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 14548992ULL;
  top_diag7s -= 59373627900066ULL;
  right_columns -= 136796838684917760ULL;
  bottom_diag7s -= 13824ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 6;
}

void Flip_D2E3_G5() {
  top_rows += 695788240896ULL;
  left_columns += 1458ULL;
  top_diag9s += 14548992ULL;
  top_diag7s += 59373627900066ULL;
  right_columns += 136796838684917760ULL;
  bottom_diag7s += 13824ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2E3_G5;
  flipped = true;
}

void UndoFlip_D2F4() {
  top_rows -= 695788240914ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 15335424ULL;
  top_diag7s -= 59373627900066ULL;
  right_columns -= 136797534466080768ULL;
  bottom_diag7s -= 695784701952ULL;
  disk_difference -= 6;
}

void Flip_D2F4() {
  top_rows += 695788240914ULL;
  left_columns += 1458ULL;
  top_diag9s += 15335424ULL;
  top_diag7s += 59373627900066ULL;
  right_columns += 136797534466080768ULL;
  bottom_diag7s += 695784701952ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2F4;
  flipped = true;
}

void UndoFlip_D2G5() {
  top_rows -= 695788240914ULL;
  left_columns -= 1458ULL;
  top_diag9s -= 15728640ULL;
  top_diag7s -= 59373627900066ULL;
  right_columns -= 136797534469619712ULL;
  bottom_diag7s -= 695784715776ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 8;
}

void Flip_D2G5() {
  top_rows += 695788240914ULL;
  left_columns += 1458ULL;
  top_diag9s += 15728640ULL;
  top_diag7s += 59373627900066ULL;
  right_columns += 136797534469619712ULL;
  bottom_diag7s += 695784715776ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D2G5;
  flipped = true;
}

void UndoFlip_E3_G5() {
  top_rows -= 3538944ULL;
  top_diag9s -= 3932160ULL;
  top_diag7s -= 162ULL;
  right_columns -= 136796838684917760ULL;
  bottom_diag7s -= 13824ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 4;
}

void Flip_E3_G5() {
  top_rows += 3538944ULL;
  top_diag9s += 3932160ULL;
  top_diag7s += 162ULL;
  right_columns += 136796838684917760ULL;
  bottom_diag7s += 13824ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3_G5;
  flipped = true;
}

void UndoFlip_E3F4() {
  top_rows -= 3538962ULL;
  top_diag9s -= 4718592ULL;
  top_diag7s -= 162ULL;
  right_columns -= 136797534466080768ULL;
  bottom_diag7s -= 695784701952ULL;
  disk_difference -= 4;
}

void Flip_E3F4() {
  top_rows += 3538962ULL;
  top_diag9s += 4718592ULL;
  top_diag7s += 162ULL;
  right_columns += 136797534466080768ULL;
  bottom_diag7s += 695784701952ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3F4;
  flipped = true;
}

void UndoFlip_E3G5() {
  top_rows -= 3538962ULL;
  top_diag9s -= 5111808ULL;
  top_diag7s -= 162ULL;
  right_columns -= 136797534469619712ULL;
  bottom_diag7s -= 695784715776ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 6;
}

void Flip_E3G5() {
  top_rows += 3538962ULL;
  top_diag9s += 5111808ULL;
  top_diag7s += 162ULL;
  right_columns += 136797534469619712ULL;
  bottom_diag7s += 695784715776ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E3G5;
  flipped = true;
}

void UndoFlip_F4G5() {
  top_rows -= 18ULL;
  top_diag9s -= 1572864ULL;
  right_columns -= 695788240896ULL;
  bottom_diag7s -= 695784715776ULL;
  bottom_rows -= 1688849860263936ULL;
  disk_difference -= 4;
}

void Flip_F4G5() {
  top_rows += 18ULL;
  top_diag9s += 1572864ULL;
  right_columns += 695788240896ULL;
  bottom_diag7s += 695784715776ULL;
  bottom_rows += 1688849860263936ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F4G5;
  flipped = true;
}

void UndoFlip_C2_E4() {
  top_rows -= 2087354105910ULL;
  left_columns -= 95551488ULL;
  top_diag9s -= 540ULL;
  top_diag7s -= 5066549580791808ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_C2_E4() {
  top_rows += 2087354105910ULL;
  left_columns += 95551488ULL;
  top_diag9s += 540ULL;
  top_diag7s += 5066549580791808ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E4;
  flipped = true;
}

void UndoFlip_C2_E4F5() {
  top_rows -= 2087354105910ULL;
  left_columns -= 95551488ULL;
  top_diag9s -= 558ULL;
  top_diag7s -= 5066549580791808ULL;
  right_columns -= 45599178155360256ULL;
  bottom_diag7s -= 45598946230665216ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 6;
}

void Flip_C2_E4F5() {
  top_rows += 2087354105910ULL;
  left_columns += 95551488ULL;
  top_diag9s += 558ULL;
  top_diag7s += 5066549580791808ULL;
  right_columns += 45599178155360256ULL;
  bottom_diag7s += 45598946230665216ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E4F5;
  flipped = true;
}

void UndoFlip_C2_E4G6() {
  top_rows -= 2087354105910ULL;
  left_columns -= 95551488ULL;
  top_diag9s -= 564ULL;
  top_diag7s -= 5066549580791808ULL;
  right_columns -= 45599178156539904ULL;
  bottom_diag7s -= 45598946230665234ULL;
  bottom_rows -= 5066575350595584ULL;
  disk_difference -= 8;
}

void Flip_C2_E4G6() {
  top_rows += 2087354105910ULL;
  left_columns += 95551488ULL;
  top_diag9s += 564ULL;
  top_diag7s += 5066549580791808ULL;
  right_columns += 45599178156539904ULL;
  bottom_diag7s += 45598946230665234ULL;
  bottom_rows += 5066575350595584ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2_E4G6;
  flipped = true;
}

void UndoFlip_C2D3() {
  top_rows -= 2087364722688ULL;
  left_columns -= 95551974ULL;
  top_diag9s -= 648ULL;
  top_diag7s -= 5066549584330752ULL;
  disk_difference -= 4;
}

void Flip_C2D3() {
  top_rows += 2087364722688ULL;
  left_columns += 95551974ULL;
  top_diag9s += 648ULL;
  top_diag7s += 5066549584330752ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D3;
  flipped = true;
}

void UndoFlip_C2D3_F5() {
  top_rows -= 2087364722688ULL;
  left_columns -= 95551974ULL;
  top_diag9s -= 666ULL;
  top_diag7s -= 5066549584330752ULL;
  right_columns -= 231928233984ULL;
  bottom_diag7s -= 3538944ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 6;
}

void Flip_C2D3_F5() {
  top_rows += 2087364722688ULL;
  left_columns += 95551974ULL;
  top_diag9s += 666ULL;
  top_diag7s += 5066549584330752ULL;
  right_columns += 231928233984ULL;
  bottom_diag7s += 3538944ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D3_F5;
  flipped = true;
}

void UndoFlip_C2D3_F5G6() {
  top_rows -= 2087364722688ULL;
  left_columns -= 95551974ULL;
  top_diag9s -= 672ULL;
  top_diag7s -= 5066549584330752ULL;
  right_columns -= 231929413632ULL;
  bottom_diag7s -= 3538962ULL;
  bottom_rows -= 5066575350595584ULL;
  disk_difference -= 8;
}

void Flip_C2D3_F5G6() {
  top_rows += 2087364722688ULL;
  left_columns += 95551974ULL;
  top_diag9s += 672ULL;
  top_diag7s += 5066549584330752ULL;
  right_columns += 231929413632ULL;
  bottom_diag7s += 3538962ULL;
  bottom_rows += 5066575350595584ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2D3_F5G6;
  flipped = true;
}

void UndoFlip_C2E4() {
  top_rows -= 2087364722742ULL;
  left_columns -= 95551974ULL;
  top_diag9s -= 702ULL;
  top_diag7s -= 5066549584330752ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_C2E4() {
  top_rows += 2087364722742ULL;
  left_columns += 95551974ULL;
  top_diag9s += 702ULL;
  top_diag7s += 5066549584330752ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2E4;
  flipped = true;
}

void UndoFlip_C2E4_G6() {
  top_rows -= 2087364722742ULL;
  left_columns -= 95551974ULL;
  top_diag9s -= 708ULL;
  top_diag7s -= 5066549584330752ULL;
  right_columns -= 45598946228305920ULL;
  bottom_diag7s -= 45598946227126290ULL;
  bottom_rows -= 25769803776ULL;
  disk_difference -= 8;
}

void Flip_C2E4_G6() {
  top_rows += 2087364722742ULL;
  left_columns += 95551974ULL;
  top_diag9s += 708ULL;
  top_diag7s += 5066549584330752ULL;
  right_columns += 45598946228305920ULL;
  bottom_diag7s += 45598946227126290ULL;
  bottom_rows += 25769803776ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2E4_G6;
  flipped = true;
}

void UndoFlip_C2F5() {
  top_rows -= 2087364722742ULL;
  left_columns -= 95551974ULL;
  top_diag9s -= 720ULL;
  top_diag7s -= 5066549584330752ULL;
  right_columns -= 45599178155360256ULL;
  bottom_diag7s -= 45598946230665216ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 8;
}

void Flip_C2F5() {
  top_rows += 2087364722742ULL;
  left_columns += 95551974ULL;
  top_diag9s += 720ULL;
  top_diag7s += 5066549584330752ULL;
  right_columns += 45599178155360256ULL;
  bottom_diag7s += 45598946230665216ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2F5;
  flipped = true;
}

void UndoFlip_C2G6() {
  top_rows -= 2087364722742ULL;
  left_columns -= 95551974ULL;
  top_diag9s -= 726ULL;
  top_diag7s -= 5066549584330752ULL;
  right_columns -= 45599178156539904ULL;
  bottom_diag7s -= 45598946230665234ULL;
  bottom_rows -= 5066575350595584ULL;
  disk_difference -= 10;
}

void Flip_C2G6() {
  top_rows += 2087364722742ULL;
  left_columns += 95551974ULL;
  top_diag9s += 726ULL;
  top_diag7s += 5066549584330752ULL;
  right_columns += 45599178156539904ULL;
  bottom_diag7s += 45598946230665234ULL;
  bottom_rows += 5066575350595584ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C2G6;
  flipped = true;
}

void UndoFlip_D3_F5() {
  top_rows -= 10616832ULL;
  left_columns -= 486ULL;
  top_diag9s -= 180ULL;
  top_diag7s -= 3538944ULL;
  right_columns -= 231928233984ULL;
  bottom_diag7s -= 3538944ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 4;
}

void Flip_D3_F5() {
  top_rows += 10616832ULL;
  left_columns += 486ULL;
  top_diag9s += 180ULL;
  top_diag7s += 3538944ULL;
  right_columns += 231928233984ULL;
  bottom_diag7s += 3538944ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_F5;
  flipped = true;
}

void UndoFlip_D3_F5G6() {
  top_rows -= 10616832ULL;
  left_columns -= 486ULL;
  top_diag9s -= 186ULL;
  top_diag7s -= 3538944ULL;
  right_columns -= 231929413632ULL;
  bottom_diag7s -= 3538962ULL;
  bottom_rows -= 5066575350595584ULL;
  disk_difference -= 6;
}

void Flip_D3_F5G6() {
  top_rows += 10616832ULL;
  left_columns += 486ULL;
  top_diag9s += 186ULL;
  top_diag7s += 3538944ULL;
  right_columns += 231929413632ULL;
  bottom_diag7s += 3538962ULL;
  bottom_rows += 5066575350595584ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3_F5G6;
  flipped = true;
}

void UndoFlip_D3E4() {
  top_rows -= 10616886ULL;
  left_columns -= 486ULL;
  top_diag9s -= 216ULL;
  top_diag7s -= 3538944ULL;
  right_columns -= 45598946227126272ULL;
  bottom_diag7s -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_D3E4() {
  top_rows += 10616886ULL;
  left_columns += 486ULL;
  top_diag9s += 216ULL;
  top_diag7s += 3538944ULL;
  right_columns += 45598946227126272ULL;
  bottom_diag7s += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3E4;
  flipped = true;
}

void UndoFlip_D3E4_G6() {
  top_rows -= 10616886ULL;
  left_columns -= 486ULL;
  top_diag9s -= 222ULL;
  top_diag7s -= 3538944ULL;
  right_columns -= 45598946228305920ULL;
  bottom_diag7s -= 45598946227126290ULL;
  bottom_rows -= 25769803776ULL;
  disk_difference -= 6;
}

void Flip_D3E4_G6() {
  top_rows += 10616886ULL;
  left_columns += 486ULL;
  top_diag9s += 222ULL;
  top_diag7s += 3538944ULL;
  right_columns += 45598946228305920ULL;
  bottom_diag7s += 45598946227126290ULL;
  bottom_rows += 25769803776ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3E4_G6;
  flipped = true;
}

void UndoFlip_D3F5() {
  top_rows -= 10616886ULL;
  left_columns -= 486ULL;
  top_diag9s -= 234ULL;
  top_diag7s -= 3538944ULL;
  right_columns -= 45599178155360256ULL;
  bottom_diag7s -= 45598946230665216ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 6;
}

void Flip_D3F5() {
  top_rows += 10616886ULL;
  left_columns += 486ULL;
  top_diag9s += 234ULL;
  top_diag7s += 3538944ULL;
  right_columns += 45599178155360256ULL;
  bottom_diag7s += 45598946230665216ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3F5;
  flipped = true;
}

void UndoFlip_D3G6() {
  top_rows -= 10616886ULL;
  left_columns -= 486ULL;
  top_diag9s -= 240ULL;
  top_diag7s -= 3538944ULL;
  right_columns -= 45599178156539904ULL;
  bottom_diag7s -= 45598946230665234ULL;
  bottom_rows -= 5066575350595584ULL;
  disk_difference -= 8;
}

void Flip_D3G6() {
  top_rows += 10616886ULL;
  left_columns += 486ULL;
  top_diag9s += 240ULL;
  top_diag7s += 3538944ULL;
  right_columns += 45599178156539904ULL;
  bottom_diag7s += 45598946230665234ULL;
  bottom_rows += 5066575350595584ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D3G6;
  flipped = true;
}

void UndoFlip_E4_G6() {
  top_rows -= 54ULL;
  top_diag9s -= 60ULL;
  right_columns -= 45598946228305920ULL;
  bottom_diag7s -= 45598946227126290ULL;
  bottom_rows -= 25769803776ULL;
  disk_difference -= 4;
}

void Flip_E4_G6() {
  top_rows += 54ULL;
  top_diag9s += 60ULL;
  right_columns += 45598946228305920ULL;
  bottom_diag7s += 45598946227126290ULL;
  bottom_rows += 25769803776ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4_G6;
  flipped = true;
}

void UndoFlip_E4F5() {
  top_rows -= 54ULL;
  top_diag9s -= 72ULL;
  right_columns -= 45599178155360256ULL;
  bottom_diag7s -= 45598946230665216ULL;
  bottom_rows -= 5066549580791808ULL;
  disk_difference -= 4;
}

void Flip_E4F5() {
  top_rows += 54ULL;
  top_diag9s += 72ULL;
  right_columns += 45599178155360256ULL;
  bottom_diag7s += 45598946230665216ULL;
  bottom_rows += 5066549580791808ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4F5;
  flipped = true;
}

void UndoFlip_E4G6() {
  top_rows -= 54ULL;
  top_diag9s -= 78ULL;
  right_columns -= 45599178156539904ULL;
  bottom_diag7s -= 45598946230665234ULL;
  bottom_rows -= 5066575350595584ULL;
  disk_difference -= 6;
}

void Flip_E4G6() {
  top_rows += 54ULL;
  top_diag9s += 78ULL;
  right_columns += 45599178156539904ULL;
  bottom_diag7s += 45598946230665234ULL;
  bottom_rows += 5066575350595584ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E4G6;
  flipped = true;
}

void UndoFlip_F5G6() {
  top_diag9s -= 24ULL;
  right_columns -= 231929413632ULL;
  bottom_diag7s -= 3538962ULL;
  bottom_rows -= 5066575350595584ULL;
  disk_difference -= 4;
}

void Flip_F5G6() {
  top_diag9s += 24ULL;
  right_columns += 231929413632ULL;
  bottom_diag7s += 3538962ULL;
  bottom_rows += 5066575350595584ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F5G6;
  flipped = true;
}

void UndoFlip_B2_D4() {
  top_rows -= 6262062317730ULL;
  left_columns -= 6262062317730ULL;
  top_diag7s -= 432345564227567670ULL;
  bottom_diag9s -= 455989462271262720ULL;
  disk_difference -= 4;
}

void Flip_B2_D4() {
  top_rows += 6262062317730ULL;
  left_columns += 6262062317730ULL;
  top_diag7s += 432345564227567670ULL;
  bottom_diag9s += 455989462271262720ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D4;
  flipped = true;
}

void UndoFlip_B2_D4E5() {
  top_rows -= 6262062317730ULL;
  left_columns -= 6262062317730ULL;
  top_diag7s -= 432345564227567670ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 471189111013638144ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 6;
}

void Flip_B2_D4E5() {
  top_rows += 6262062317730ULL;
  left_columns += 6262062317730ULL;
  top_diag7s += 432345564227567670ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 471189111013638144ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D4E5;
  flipped = true;
}

void UndoFlip_B2_D4F6() {
  top_rows -= 6262062317730ULL;
  left_columns -= 6262062317730ULL;
  top_diag7s -= 432345564227567670ULL;
  right_columns -= 15199726051786752ULL;
  bottom_diag9s -= 476255660594429952ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726051786752ULL;
  disk_difference -= 8;
}

void Flip_B2_D4F6() {
  top_rows += 6262062317730ULL;
  left_columns += 6262062317730ULL;
  top_diag7s += 432345564227567670ULL;
  right_columns += 15199726051786752ULL;
  bottom_diag9s += 476255660594429952ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726051786752ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D4F6;
  flipped = true;
}

void UndoFlip_B2_D4G7() {
  top_rows -= 6262062317730ULL;
  left_columns -= 6262062317730ULL;
  top_diag7s -= 432345589997371446ULL;
  right_columns -= 15199726052179968ULL;
  bottom_diag9s -= 477944510454693888ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726052179968ULL;
  disk_difference -= 10;
}

void Flip_B2_D4G7() {
  top_rows += 6262062317730ULL;
  left_columns += 6262062317730ULL;
  top_diag7s += 432345589997371446ULL;
  right_columns += 15199726052179968ULL;
  bottom_diag9s += 477944510454693888ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726052179968ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2_D4G7;
  flipped = true;
}

void UndoFlip_B2C3() {
  top_rows -= 6262094168064ULL;
  left_columns -= 6262094168064ULL;
  top_diag7s -= 432365355436867584ULL;
  bottom_diag9s -= 547187354725515264ULL;
  disk_difference -= 4;
}

void Flip_B2C3() {
  top_rows += 6262094168064ULL;
  left_columns += 6262094168064ULL;
  top_diag7s += 432365355436867584ULL;
  bottom_diag9s += 547187354725515264ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C3;
  flipped = true;
}

void UndoFlip_B2C3_E5() {
  top_rows -= 6262094168064ULL;
  left_columns -= 6262094168064ULL;
  top_diag7s -= 432365355436867584ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 562387003467890688ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 6;
}

void Flip_B2C3_E5() {
  top_rows += 6262094168064ULL;
  left_columns += 6262094168064ULL;
  top_diag7s += 432365355436867584ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 562387003467890688ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C3_E5;
  flipped = true;
}

void UndoFlip_B2C3_E5F6() {
  top_rows -= 6262094168064ULL;
  left_columns -= 6262094168064ULL;
  top_diag7s -= 432365355436867584ULL;
  right_columns -= 15199726051786752ULL;
  bottom_diag9s -= 567453553048682496ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726051786752ULL;
  disk_difference -= 8;
}

void Flip_B2C3_E5F6() {
  top_rows += 6262094168064ULL;
  left_columns += 6262094168064ULL;
  top_diag7s += 432365355436867584ULL;
  right_columns += 15199726051786752ULL;
  bottom_diag9s += 567453553048682496ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726051786752ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C3_E5F6;
  flipped = true;
}

void UndoFlip_B2C3_E5G7() {
  top_rows -= 6262094168064ULL;
  left_columns -= 6262094168064ULL;
  top_diag7s -= 432365381206671360ULL;
  right_columns -= 15199726052179968ULL;
  bottom_diag9s -= 569142402908946432ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726052179968ULL;
  disk_difference -= 10;
}

void Flip_B2C3_E5G7() {
  top_rows += 6262094168064ULL;
  left_columns += 6262094168064ULL;
  top_diag7s += 432365381206671360ULL;
  right_columns += 15199726052179968ULL;
  bottom_diag9s += 569142402908946432ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726052179968ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2C3_E5G7;
  flipped = true;
}

void UndoFlip_B2D4() {
  top_rows -= 6262094168226ULL;
  left_columns -= 6262094168226ULL;
  top_diag7s -= 432365355436867638ULL;
  bottom_diag9s -= 592786300952641536ULL;
  disk_difference -= 6;
}

void Flip_B2D4() {
  top_rows += 6262094168226ULL;
  left_columns += 6262094168226ULL;
  top_diag7s += 432365355436867638ULL;
  bottom_diag9s += 592786300952641536ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D4;
  flipped = true;
}

void UndoFlip_B2D4_F6() {
  top_rows -= 6262094168226ULL;
  left_columns -= 6262094168226ULL;
  top_diag7s -= 432365355436867638ULL;
  right_columns -= 77309411328ULL;
  bottom_diag9s -= 597852850533433344ULL;
  bottom_diag7s -= 4608ULL;
  bottom_rows -= 77309411328ULL;
  disk_difference -= 8;
}

void Flip_B2D4_F6() {
  top_rows += 6262094168226ULL;
  left_columns += 6262094168226ULL;
  top_diag7s += 432365355436867638ULL;
  right_columns += 77309411328ULL;
  bottom_diag9s += 597852850533433344ULL;
  bottom_diag7s += 4608ULL;
  bottom_rows += 77309411328ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D4_F6;
  flipped = true;
}

void UndoFlip_B2D4_F6G7() {
  top_rows -= 6262094168226ULL;
  left_columns -= 6262094168226ULL;
  top_diag7s -= 432365381206671414ULL;
  right_columns -= 77309804544ULL;
  bottom_diag9s -= 599541700393697280ULL;
  bottom_diag7s -= 4608ULL;
  bottom_rows -= 77309804544ULL;
  disk_difference -= 10;
}

void Flip_B2D4_F6G7() {
  top_rows += 6262094168226ULL;
  left_columns += 6262094168226ULL;
  top_diag7s += 432365381206671414ULL;
  right_columns += 77309804544ULL;
  bottom_diag9s += 599541700393697280ULL;
  bottom_diag7s += 4608ULL;
  bottom_rows += 77309804544ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2D4_F6G7;
  flipped = true;
}

void UndoFlip_B2E5() {
  top_rows -= 6262094168226ULL;
  left_columns -= 6262094168226ULL;
  top_diag7s -= 432365355436867638ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 607985949695016960ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 8;
}

void Flip_B2E5() {
  top_rows += 6262094168226ULL;
  left_columns += 6262094168226ULL;
  top_diag7s += 432365355436867638ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 607985949695016960ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2E5;
  flipped = true;
}

void UndoFlip_B2E5_G7() {
  top_rows -= 6262094168226ULL;
  left_columns -= 6262094168226ULL;
  top_diag7s -= 432365381206671414ULL;
  right_columns -= 15199648742768640ULL;
  bottom_diag9s -= 609674799555280896ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742768640ULL;
  disk_difference -= 10;
}

void Flip_B2E5_G7() {
  top_rows += 6262094168226ULL;
  left_columns += 6262094168226ULL;
  top_diag7s += 432365381206671414ULL;
  right_columns += 15199648742768640ULL;
  bottom_diag9s += 609674799555280896ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742768640ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2E5_G7;
  flipped = true;
}

void UndoFlip_B2F6() {
  top_rows -= 6262094168226ULL;
  left_columns -= 6262094168226ULL;
  top_diag7s -= 432365355436867638ULL;
  right_columns -= 15199726051786752ULL;
  bottom_diag9s -= 613052499275808768ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726051786752ULL;
  disk_difference -= 10;
}

void Flip_B2F6() {
  top_rows += 6262094168226ULL;
  left_columns += 6262094168226ULL;
  top_diag7s += 432365355436867638ULL;
  right_columns += 15199726051786752ULL;
  bottom_diag9s += 613052499275808768ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726051786752ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2F6;
  flipped = true;
}

void UndoFlip_B2G7() {
  top_rows -= 6262094168226ULL;
  left_columns -= 6262094168226ULL;
  top_diag7s -= 432365381206671414ULL;
  right_columns -= 15199726052179968ULL;
  bottom_diag9s -= 614741349136072704ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726052179968ULL;
  disk_difference -= 12;
}

void Flip_B2G7() {
  top_rows += 6262094168226ULL;
  left_columns += 6262094168226ULL;
  top_diag7s += 432365381206671414ULL;
  right_columns += 15199726052179968ULL;
  bottom_diag9s += 614741349136072704ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726052179968ULL;
  disk_difference += 12;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B2G7;
  flipped = true;
}

void UndoFlip_C3_E5() {
  top_rows -= 31850496ULL;
  left_columns -= 31850496ULL;
  top_diag7s -= 19791209299968ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 151996487423754240ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 4;
}

void Flip_C3_E5() {
  top_rows += 31850496ULL;
  left_columns += 31850496ULL;
  top_diag7s += 19791209299968ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 151996487423754240ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E5;
  flipped = true;
}

void UndoFlip_C3_E5F6() {
  top_rows -= 31850496ULL;
  left_columns -= 31850496ULL;
  top_diag7s -= 19791209299968ULL;
  right_columns -= 15199726051786752ULL;
  bottom_diag9s -= 157063037004546048ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726051786752ULL;
  disk_difference -= 6;
}

void Flip_C3_E5F6() {
  top_rows += 31850496ULL;
  left_columns += 31850496ULL;
  top_diag7s += 19791209299968ULL;
  right_columns += 15199726051786752ULL;
  bottom_diag9s += 157063037004546048ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726051786752ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E5F6;
  flipped = true;
}

void UndoFlip_C3_E5G7() {
  top_rows -= 31850496ULL;
  left_columns -= 31850496ULL;
  top_diag7s -= 19816979103744ULL;
  right_columns -= 15199726052179968ULL;
  bottom_diag9s -= 158751886864809984ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726052179968ULL;
  disk_difference -= 8;
}

void Flip_C3_E5G7() {
  top_rows += 31850496ULL;
  left_columns += 31850496ULL;
  top_diag7s += 19816979103744ULL;
  right_columns += 15199726052179968ULL;
  bottom_diag9s += 158751886864809984ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726052179968ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3_E5G7;
  flipped = true;
}

void UndoFlip_C3D4() {
  top_rows -= 31850658ULL;
  left_columns -= 31850658ULL;
  top_diag7s -= 19791209300022ULL;
  bottom_diag9s -= 182395784908505088ULL;
  disk_difference -= 4;
}

void Flip_C3D4() {
  top_rows += 31850658ULL;
  left_columns += 31850658ULL;
  top_diag7s += 19791209300022ULL;
  bottom_diag9s += 182395784908505088ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D4;
  flipped = true;
}

void UndoFlip_C3D4_F6() {
  top_rows -= 31850658ULL;
  left_columns -= 31850658ULL;
  top_diag7s -= 19791209300022ULL;
  right_columns -= 77309411328ULL;
  bottom_diag9s -= 187462334489296896ULL;
  bottom_diag7s -= 4608ULL;
  bottom_rows -= 77309411328ULL;
  disk_difference -= 6;
}

void Flip_C3D4_F6() {
  top_rows += 31850658ULL;
  left_columns += 31850658ULL;
  top_diag7s += 19791209300022ULL;
  right_columns += 77309411328ULL;
  bottom_diag9s += 187462334489296896ULL;
  bottom_diag7s += 4608ULL;
  bottom_rows += 77309411328ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D4_F6;
  flipped = true;
}

void UndoFlip_C3D4_F6G7() {
  top_rows -= 31850658ULL;
  left_columns -= 31850658ULL;
  top_diag7s -= 19816979103798ULL;
  right_columns -= 77309804544ULL;
  bottom_diag9s -= 189151184349560832ULL;
  bottom_diag7s -= 4608ULL;
  bottom_rows -= 77309804544ULL;
  disk_difference -= 8;
}

void Flip_C3D4_F6G7() {
  top_rows += 31850658ULL;
  left_columns += 31850658ULL;
  top_diag7s += 19816979103798ULL;
  right_columns += 77309804544ULL;
  bottom_diag9s += 189151184349560832ULL;
  bottom_diag7s += 4608ULL;
  bottom_rows += 77309804544ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3D4_F6G7;
  flipped = true;
}

void UndoFlip_C3E5() {
  top_rows -= 31850658ULL;
  left_columns -= 31850658ULL;
  top_diag7s -= 19791209300022ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 197595433650880512ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 6;
}

void Flip_C3E5() {
  top_rows += 31850658ULL;
  left_columns += 31850658ULL;
  top_diag7s += 19791209300022ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 197595433650880512ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3E5;
  flipped = true;
}

void UndoFlip_C3E5_G7() {
  top_rows -= 31850658ULL;
  left_columns -= 31850658ULL;
  top_diag7s -= 19816979103798ULL;
  right_columns -= 15199648742768640ULL;
  bottom_diag9s -= 199284283511144448ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742768640ULL;
  disk_difference -= 8;
}

void Flip_C3E5_G7() {
  top_rows += 31850658ULL;
  left_columns += 31850658ULL;
  top_diag7s += 19816979103798ULL;
  right_columns += 15199648742768640ULL;
  bottom_diag9s += 199284283511144448ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742768640ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3E5_G7;
  flipped = true;
}

void UndoFlip_C3F6() {
  top_rows -= 31850658ULL;
  left_columns -= 31850658ULL;
  top_diag7s -= 19791209300022ULL;
  right_columns -= 15199726051786752ULL;
  bottom_diag9s -= 202661983231672320ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726051786752ULL;
  disk_difference -= 8;
}

void Flip_C3F6() {
  top_rows += 31850658ULL;
  left_columns += 31850658ULL;
  top_diag7s += 19791209300022ULL;
  right_columns += 15199726051786752ULL;
  bottom_diag9s += 202661983231672320ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726051786752ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3F6;
  flipped = true;
}

void UndoFlip_C3G7() {
  top_rows -= 31850658ULL;
  left_columns -= 31850658ULL;
  top_diag7s -= 19816979103798ULL;
  right_columns -= 15199726052179968ULL;
  bottom_diag9s -= 204350833091936256ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726052179968ULL;
  disk_difference -= 10;
}

void Flip_C3G7() {
  top_rows += 31850658ULL;
  left_columns += 31850658ULL;
  top_diag7s += 19816979103798ULL;
  right_columns += 15199726052179968ULL;
  bottom_diag9s += 204350833091936256ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726052179968ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C3G7;
  flipped = true;
}

void UndoFlip_D4_F6() {
  top_rows -= 162ULL;
  left_columns -= 162ULL;
  top_diag7s -= 54ULL;
  right_columns -= 77309411328ULL;
  bottom_diag9s -= 50665495807918080ULL;
  bottom_diag7s -= 4608ULL;
  bottom_rows -= 77309411328ULL;
  disk_difference -= 4;
}

void Flip_D4_F6() {
  top_rows += 162ULL;
  left_columns += 162ULL;
  top_diag7s += 54ULL;
  right_columns += 77309411328ULL;
  bottom_diag9s += 50665495807918080ULL;
  bottom_diag7s += 4608ULL;
  bottom_rows += 77309411328ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_F6;
  flipped = true;
}

void UndoFlip_D4_F6G7() {
  top_rows -= 162ULL;
  left_columns -= 162ULL;
  top_diag7s -= 25769803830ULL;
  right_columns -= 77309804544ULL;
  bottom_diag9s -= 52354345668182016ULL;
  bottom_diag7s -= 4608ULL;
  bottom_rows -= 77309804544ULL;
  disk_difference -= 6;
}

void Flip_D4_F6G7() {
  top_rows += 162ULL;
  left_columns += 162ULL;
  top_diag7s += 25769803830ULL;
  right_columns += 77309804544ULL;
  bottom_diag9s += 52354345668182016ULL;
  bottom_diag7s += 4608ULL;
  bottom_rows += 77309804544ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4_F6G7;
  flipped = true;
}

void UndoFlip_D4E5() {
  top_rows -= 162ULL;
  left_columns -= 162ULL;
  top_diag7s -= 54ULL;
  right_columns -= 15199648742375424ULL;
  bottom_diag9s -= 60798594969501696ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742375424ULL;
  disk_difference -= 4;
}

void Flip_D4E5() {
  top_rows += 162ULL;
  left_columns += 162ULL;
  top_diag7s += 54ULL;
  right_columns += 15199648742375424ULL;
  bottom_diag9s += 60798594969501696ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742375424ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4E5;
  flipped = true;
}

void UndoFlip_D4E5_G7() {
  top_rows -= 162ULL;
  left_columns -= 162ULL;
  top_diag7s -= 25769803830ULL;
  right_columns -= 15199648742768640ULL;
  bottom_diag9s -= 62487444829765632ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742768640ULL;
  disk_difference -= 6;
}

void Flip_D4E5_G7() {
  top_rows += 162ULL;
  left_columns += 162ULL;
  top_diag7s += 25769803830ULL;
  right_columns += 15199648742768640ULL;
  bottom_diag9s += 62487444829765632ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742768640ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4E5_G7;
  flipped = true;
}

void UndoFlip_D4F6() {
  top_rows -= 162ULL;
  left_columns -= 162ULL;
  top_diag7s -= 54ULL;
  right_columns -= 15199726051786752ULL;
  bottom_diag9s -= 65865144550293504ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726051786752ULL;
  disk_difference -= 6;
}

void Flip_D4F6() {
  top_rows += 162ULL;
  left_columns += 162ULL;
  top_diag7s += 54ULL;
  right_columns += 15199726051786752ULL;
  bottom_diag9s += 65865144550293504ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726051786752ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4F6;
  flipped = true;
}

void UndoFlip_D4G7() {
  top_rows -= 162ULL;
  left_columns -= 162ULL;
  top_diag7s -= 25769803830ULL;
  right_columns -= 15199726052179968ULL;
  bottom_diag9s -= 67553994410557440ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726052179968ULL;
  disk_difference -= 8;
}

void Flip_D4G7() {
  top_rows += 162ULL;
  left_columns += 162ULL;
  top_diag7s += 25769803830ULL;
  right_columns += 15199726052179968ULL;
  bottom_diag9s += 67553994410557440ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726052179968ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D4G7;
  flipped = true;
}

void UndoFlip_E5_G7() {
  top_diag7s -= 25769803776ULL;
  right_columns -= 15199648742768640ULL;
  bottom_diag9s -= 16888498602639360ULL;
  bottom_diag7s -= 231928233984ULL;
  bottom_rows -= 15199648742768640ULL;
  disk_difference -= 4;
}

void Flip_E5_G7() {
  top_diag7s += 25769803776ULL;
  right_columns += 15199648742768640ULL;
  bottom_diag9s += 16888498602639360ULL;
  bottom_diag7s += 231928233984ULL;
  bottom_rows += 15199648742768640ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5_G7;
  flipped = true;
}

void UndoFlip_E5F6() {
  right_columns -= 15199726051786752ULL;
  bottom_diag9s -= 20266198323167232ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726051786752ULL;
  disk_difference -= 4;
}

void Flip_E5F6() {
  right_columns += 15199726051786752ULL;
  bottom_diag9s += 20266198323167232ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726051786752ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5F6;
  flipped = true;
}

void UndoFlip_E5G7() {
  top_diag7s -= 25769803776ULL;
  right_columns -= 15199726052179968ULL;
  bottom_diag9s -= 21955048183431168ULL;
  bottom_diag7s -= 231928238592ULL;
  bottom_rows -= 15199726052179968ULL;
  disk_difference -= 6;
}

void Flip_E5G7() {
  top_diag7s += 25769803776ULL;
  right_columns += 15199726052179968ULL;
  bottom_diag9s += 21955048183431168ULL;
  bottom_diag7s += 231928238592ULL;
  bottom_rows += 15199726052179968ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E5G7;
  flipped = true;
}

void UndoFlip_F6G7() {
  top_diag7s -= 25769803776ULL;
  right_columns -= 77309804544ULL;
  bottom_diag9s -= 6755399441055744ULL;
  bottom_diag7s -= 4608ULL;
  bottom_rows -= 77309804544ULL;
  disk_difference -= 4;
}

void Flip_F6G7() {
  top_diag7s += 25769803776ULL;
  right_columns += 77309804544ULL;
  bottom_diag9s += 6755399441055744ULL;
  bottom_diag7s += 4608ULL;
  bottom_rows += 77309804544ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_F6G7;
  flipped = true;
}

void UndoFlip_B3_D5() {
  top_rows -= 95551488ULL;
  left_columns -= 2087354105910ULL;
  top_diag7s -= 1688849860263936ULL;
  bottom_diag9s -= 2319282339840ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_B3_D5() {
  top_rows += 95551488ULL;
  left_columns += 2087354105910ULL;
  top_diag7s += 1688849860263936ULL;
  bottom_diag9s += 2319282339840ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D5;
  flipped = true;
}

void UndoFlip_B3_D5E6() {
  top_rows -= 95551488ULL;
  left_columns -= 2087354105910ULL;
  top_diag7s -= 1688849860263936ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 2396591751168ULL;
  bottom_diag7s -= 15199648743555072ULL;
  bottom_rows -= 45599178155360256ULL;
  disk_difference -= 6;
}

void Flip_B3_D5E6() {
  top_rows += 95551488ULL;
  left_columns += 2087354105910ULL;
  top_diag7s += 1688849860263936ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 2396591751168ULL;
  bottom_diag7s += 15199648743555072ULL;
  bottom_rows += 45599178155360256ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D5E6;
  flipped = true;
}

void UndoFlip_B3_D5F7() {
  top_rows -= 95551488ULL;
  left_columns -= 2087354105910ULL;
  top_diag7s -= 1688849860263936ULL;
  right_columns -= 5066575350595584ULL;
  bottom_diag9s -= 2422361554944ULL;
  bottom_diag7s -= 15199648743555078ULL;
  bottom_rows -= 45599178156539904ULL;
  disk_difference -= 8;
}

void Flip_B3_D5F7() {
  top_rows += 95551488ULL;
  left_columns += 2087354105910ULL;
  top_diag7s += 1688849860263936ULL;
  right_columns += 5066575350595584ULL;
  bottom_diag9s += 2422361554944ULL;
  bottom_diag7s += 15199648743555078ULL;
  bottom_rows += 45599178156539904ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3_D5F7;
  flipped = true;
}

void UndoFlip_B3C4() {
  top_rows -= 95551974ULL;
  left_columns -= 2087364722688ULL;
  top_diag7s -= 1688849861443584ULL;
  bottom_diag9s -= 2783138807808ULL;
  disk_difference -= 4;
}

void Flip_B3C4() {
  top_rows += 95551974ULL;
  left_columns += 2087364722688ULL;
  top_diag7s += 1688849861443584ULL;
  bottom_diag9s += 2783138807808ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C4;
  flipped = true;
}

void UndoFlip_B3C4_E6() {
  top_rows -= 95551974ULL;
  left_columns -= 2087364722688ULL;
  top_diag7s -= 1688849861443584ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 2860448219136ULL;
  bottom_diag7s -= 1179648ULL;
  bottom_rows -= 231928233984ULL;
  disk_difference -= 6;
}

void Flip_B3C4_E6() {
  top_rows += 95551974ULL;
  left_columns += 2087364722688ULL;
  top_diag7s += 1688849861443584ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 2860448219136ULL;
  bottom_diag7s += 1179648ULL;
  bottom_rows += 231928233984ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C4_E6;
  flipped = true;
}

void UndoFlip_B3C4_E6F7() {
  top_rows -= 95551974ULL;
  left_columns -= 2087364722688ULL;
  top_diag7s -= 1688849861443584ULL;
  right_columns -= 5066575350595584ULL;
  bottom_diag9s -= 2886218022912ULL;
  bottom_diag7s -= 1179654ULL;
  bottom_rows -= 231929413632ULL;
  disk_difference -= 8;
}

void Flip_B3C4_E6F7() {
  top_rows += 95551974ULL;
  left_columns += 2087364722688ULL;
  top_diag7s += 1688849861443584ULL;
  right_columns += 5066575350595584ULL;
  bottom_diag9s += 2886218022912ULL;
  bottom_diag7s += 1179654ULL;
  bottom_rows += 231929413632ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3C4_E6F7;
  flipped = true;
}

void UndoFlip_B3D5() {
  top_rows -= 95551974ULL;
  left_columns -= 2087364722742ULL;
  top_diag7s -= 1688849861443584ULL;
  bottom_diag9s -= 3015067041792ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 6;
}

void Flip_B3D5() {
  top_rows += 95551974ULL;
  left_columns += 2087364722742ULL;
  top_diag7s += 1688849861443584ULL;
  bottom_diag9s += 3015067041792ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3D5;
  flipped = true;
}

void UndoFlip_B3D5_F7() {
  top_rows -= 95551974ULL;
  left_columns -= 2087364722742ULL;
  top_diag7s -= 1688849861443584ULL;
  right_columns -= 25769803776ULL;
  bottom_diag9s -= 3040836845568ULL;
  bottom_diag7s -= 15199648742375430ULL;
  bottom_rows -= 45598946228305920ULL;
  disk_difference -= 8;
}

void Flip_B3D5_F7() {
  top_rows += 95551974ULL;
  left_columns += 2087364722742ULL;
  top_diag7s += 1688849861443584ULL;
  right_columns += 25769803776ULL;
  bottom_diag9s += 3040836845568ULL;
  bottom_diag7s += 15199648742375430ULL;
  bottom_rows += 45598946228305920ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3D5_F7;
  flipped = true;
}

void UndoFlip_B3E6() {
  top_rows -= 95551974ULL;
  left_columns -= 2087364722742ULL;
  top_diag7s -= 1688849861443584ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 3092376453120ULL;
  bottom_diag7s -= 15199648743555072ULL;
  bottom_rows -= 45599178155360256ULL;
  disk_difference -= 8;
}

void Flip_B3E6() {
  top_rows += 95551974ULL;
  left_columns += 2087364722742ULL;
  top_diag7s += 1688849861443584ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 3092376453120ULL;
  bottom_diag7s += 15199648743555072ULL;
  bottom_rows += 45599178155360256ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3E6;
  flipped = true;
}

void UndoFlip_B3F7() {
  top_rows -= 95551974ULL;
  left_columns -= 2087364722742ULL;
  top_diag7s -= 1688849861443584ULL;
  right_columns -= 5066575350595584ULL;
  bottom_diag9s -= 3118146256896ULL;
  bottom_diag7s -= 15199648743555078ULL;
  bottom_rows -= 45599178156539904ULL;
  disk_difference -= 10;
}

void Flip_B3F7() {
  top_rows += 95551974ULL;
  left_columns += 2087364722742ULL;
  top_diag7s += 1688849861443584ULL;
  right_columns += 5066575350595584ULL;
  bottom_diag9s += 3118146256896ULL;
  bottom_diag7s += 15199648743555078ULL;
  bottom_rows += 45599178156539904ULL;
  disk_difference += 10;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B3F7;
  flipped = true;
}

void UndoFlip_C4_E6() {
  top_rows -= 486ULL;
  left_columns -= 10616832ULL;
  top_diag7s -= 1179648ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 773094113280ULL;
  bottom_diag7s -= 1179648ULL;
  bottom_rows -= 231928233984ULL;
  disk_difference -= 4;
}

void Flip_C4_E6() {
  top_rows += 486ULL;
  left_columns += 10616832ULL;
  top_diag7s += 1179648ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 773094113280ULL;
  bottom_diag7s += 1179648ULL;
  bottom_rows += 231928233984ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_E6;
  flipped = true;
}

void UndoFlip_C4_E6F7() {
  top_rows -= 486ULL;
  left_columns -= 10616832ULL;
  top_diag7s -= 1179648ULL;
  right_columns -= 5066575350595584ULL;
  bottom_diag9s -= 798863917056ULL;
  bottom_diag7s -= 1179654ULL;
  bottom_rows -= 231929413632ULL;
  disk_difference -= 6;
}

void Flip_C4_E6F7() {
  top_rows += 486ULL;
  left_columns += 10616832ULL;
  top_diag7s += 1179648ULL;
  right_columns += 5066575350595584ULL;
  bottom_diag9s += 798863917056ULL;
  bottom_diag7s += 1179654ULL;
  bottom_rows += 231929413632ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4_E6F7;
  flipped = true;
}

void UndoFlip_C4D5() {
  top_rows -= 486ULL;
  left_columns -= 10616886ULL;
  top_diag7s -= 1179648ULL;
  bottom_diag9s -= 927712935936ULL;
  bottom_diag7s -= 15199648742375424ULL;
  bottom_rows -= 45598946227126272ULL;
  disk_difference -= 4;
}

void Flip_C4D5() {
  top_rows += 486ULL;
  left_columns += 10616886ULL;
  top_diag7s += 1179648ULL;
  bottom_diag9s += 927712935936ULL;
  bottom_diag7s += 15199648742375424ULL;
  bottom_rows += 45598946227126272ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4D5;
  flipped = true;
}

void UndoFlip_C4D5_F7() {
  top_rows -= 486ULL;
  left_columns -= 10616886ULL;
  top_diag7s -= 1179648ULL;
  right_columns -= 25769803776ULL;
  bottom_diag9s -= 953482739712ULL;
  bottom_diag7s -= 15199648742375430ULL;
  bottom_rows -= 45598946228305920ULL;
  disk_difference -= 6;
}

void Flip_C4D5_F7() {
  top_rows += 486ULL;
  left_columns += 10616886ULL;
  top_diag7s += 1179648ULL;
  right_columns += 25769803776ULL;
  bottom_diag9s += 953482739712ULL;
  bottom_diag7s += 15199648742375430ULL;
  bottom_rows += 45598946228305920ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4D5_F7;
  flipped = true;
}

void UndoFlip_C4E6() {
  top_rows -= 486ULL;
  left_columns -= 10616886ULL;
  top_diag7s -= 1179648ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 1005022347264ULL;
  bottom_diag7s -= 15199648743555072ULL;
  bottom_rows -= 45599178155360256ULL;
  disk_difference -= 6;
}

void Flip_C4E6() {
  top_rows += 486ULL;
  left_columns += 10616886ULL;
  top_diag7s += 1179648ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 1005022347264ULL;
  bottom_diag7s += 15199648743555072ULL;
  bottom_rows += 45599178155360256ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4E6;
  flipped = true;
}

void UndoFlip_C4F7() {
  top_rows -= 486ULL;
  left_columns -= 10616886ULL;
  top_diag7s -= 1179648ULL;
  right_columns -= 5066575350595584ULL;
  bottom_diag9s -= 1030792151040ULL;
  bottom_diag7s -= 15199648743555078ULL;
  bottom_rows -= 45599178156539904ULL;
  disk_difference -= 8;
}

void Flip_C4F7() {
  top_rows += 486ULL;
  left_columns += 10616886ULL;
  top_diag7s += 1179648ULL;
  right_columns += 5066575350595584ULL;
  bottom_diag9s += 1030792151040ULL;
  bottom_diag7s += 15199648743555078ULL;
  bottom_rows += 45599178156539904ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C4F7;
  flipped = true;
}

void UndoFlip_D5_F7() {
  left_columns -= 54ULL;
  right_columns -= 25769803776ULL;
  bottom_diag9s -= 257698037760ULL;
  bottom_diag7s -= 15199648742375430ULL;
  bottom_rows -= 45598946228305920ULL;
  disk_difference -= 4;
}

void Flip_D5_F7() {
  left_columns += 54ULL;
  right_columns += 25769803776ULL;
  bottom_diag9s += 257698037760ULL;
  bottom_diag7s += 15199648742375430ULL;
  bottom_rows += 45598946228305920ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5_F7;
  flipped = true;
}

void UndoFlip_D5E6() {
  left_columns -= 54ULL;
  right_columns -= 5066549580791808ULL;
  bottom_diag9s -= 309237645312ULL;
  bottom_diag7s -= 15199648743555072ULL;
  bottom_rows -= 45599178155360256ULL;
  disk_difference -= 4;
}

void Flip_D5E6() {
  left_columns += 54ULL;
  right_columns += 5066549580791808ULL;
  bottom_diag9s += 309237645312ULL;
  bottom_diag7s += 15199648743555072ULL;
  bottom_rows += 45599178155360256ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5E6;
  flipped = true;
}

void UndoFlip_D5F7() {
  left_columns -= 54ULL;
  right_columns -= 5066575350595584ULL;
  bottom_diag9s -= 335007449088ULL;
  bottom_diag7s -= 15199648743555078ULL;
  bottom_rows -= 45599178156539904ULL;
  disk_difference -= 6;
}

void Flip_D5F7() {
  left_columns += 54ULL;
  right_columns += 5066575350595584ULL;
  bottom_diag9s += 335007449088ULL;
  bottom_diag7s += 15199648743555078ULL;
  bottom_rows += 45599178156539904ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D5F7;
  flipped = true;
}

void UndoFlip_E6F7() {
  right_columns -= 5066575350595584ULL;
  bottom_diag9s -= 103079215104ULL;
  bottom_diag7s -= 1179654ULL;
  bottom_rows -= 231929413632ULL;
  disk_difference -= 4;
}

void Flip_E6F7() {
  right_columns += 5066575350595584ULL;
  bottom_diag9s += 103079215104ULL;
  bottom_diag7s += 1179654ULL;
  bottom_rows += 231929413632ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_E6F7;
  flipped = true;
}

void UndoFlip_B4_D6() {
  top_rows -= 1458ULL;
  left_columns -= 695784701970ULL;
  top_diag7s -= 6597069766656ULL;
  bottom_diag9s -= 11796480ULL;
  bottom_diag7s -= 77309411328ULL;
  bottom_rows -= 695784701952ULL;
  disk_difference -= 4;
}

void Flip_B4_D6() {
  top_rows += 1458ULL;
  left_columns += 695784701970ULL;
  top_diag7s += 6597069766656ULL;
  bottom_diag9s += 11796480ULL;
  bottom_diag7s += 77309411328ULL;
  bottom_rows += 695784701952ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D6;
  flipped = true;
}

void UndoFlip_B4_D6E7() {
  top_rows -= 1458ULL;
  left_columns -= 695784701970ULL;
  top_diag7s -= 6597069766656ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 12189696ULL;
  bottom_diag7s -= 77309412864ULL;
  bottom_rows -= 695788240896ULL;
  disk_difference -= 6;
}

void Flip_B4_D6E7() {
  top_rows += 1458ULL;
  left_columns += 695784701970ULL;
  top_diag7s += 6597069766656ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 12189696ULL;
  bottom_diag7s += 77309412864ULL;
  bottom_rows += 695788240896ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4_D6E7;
  flipped = true;
}

void UndoFlip_B4C5() {
  top_rows -= 1458ULL;
  left_columns -= 695788240896ULL;
  top_diag7s -= 6597069766674ULL;
  bottom_diag9s -= 14155776ULL;
  bottom_rows -= 136796838681378816ULL;
  disk_difference -= 4;
}

void Flip_B4C5() {
  top_rows += 1458ULL;
  left_columns += 695788240896ULL;
  top_diag7s += 6597069766674ULL;
  bottom_diag9s += 14155776ULL;
  bottom_rows += 136796838681378816ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C5;
  flipped = true;
}

void UndoFlip_B4C5_E7() {
  top_rows -= 1458ULL;
  left_columns -= 695788240896ULL;
  top_diag7s -= 6597069766674ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 14548992ULL;
  bottom_diag7s -= 1536ULL;
  bottom_rows -= 136796838684917760ULL;
  disk_difference -= 6;
}

void Flip_B4C5_E7() {
  top_rows += 1458ULL;
  left_columns += 695788240896ULL;
  top_diag7s += 6597069766674ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 14548992ULL;
  bottom_diag7s += 1536ULL;
  bottom_rows += 136796838684917760ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4C5_E7;
  flipped = true;
}

void UndoFlip_B4D6() {
  top_rows -= 1458ULL;
  left_columns -= 695788240914ULL;
  top_diag7s -= 6597069766674ULL;
  bottom_diag9s -= 15335424ULL;
  bottom_diag7s -= 77309411328ULL;
  bottom_rows -= 136797534466080768ULL;
  disk_difference -= 6;
}

void Flip_B4D6() {
  top_rows += 1458ULL;
  left_columns += 695788240914ULL;
  top_diag7s += 6597069766674ULL;
  bottom_diag9s += 15335424ULL;
  bottom_diag7s += 77309411328ULL;
  bottom_rows += 136797534466080768ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4D6;
  flipped = true;
}

void UndoFlip_B4E7() {
  top_rows -= 1458ULL;
  left_columns -= 695788240914ULL;
  top_diag7s -= 6597069766674ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 15728640ULL;
  bottom_diag7s -= 77309412864ULL;
  bottom_rows -= 136797534469619712ULL;
  disk_difference -= 8;
}

void Flip_B4E7() {
  top_rows += 1458ULL;
  left_columns += 695788240914ULL;
  top_diag7s += 6597069766674ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 15728640ULL;
  bottom_diag7s += 77309412864ULL;
  bottom_rows += 136797534469619712ULL;
  disk_difference += 8;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B4E7;
  flipped = true;
}

void UndoFlip_C5_E7() {
  left_columns -= 3538944ULL;
  top_diag7s -= 18ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 3932160ULL;
  bottom_diag7s -= 1536ULL;
  bottom_rows -= 136796838684917760ULL;
  disk_difference -= 4;
}

void Flip_C5_E7() {
  left_columns += 3538944ULL;
  top_diag7s += 18ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 3932160ULL;
  bottom_diag7s += 1536ULL;
  bottom_rows += 136796838684917760ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5_E7;
  flipped = true;
}

void UndoFlip_C5D6() {
  left_columns -= 3538962ULL;
  top_diag7s -= 18ULL;
  bottom_diag9s -= 4718592ULL;
  bottom_diag7s -= 77309411328ULL;
  bottom_rows -= 136797534466080768ULL;
  disk_difference -= 4;
}

void Flip_C5D6() {
  left_columns += 3538962ULL;
  top_diag7s += 18ULL;
  bottom_diag9s += 4718592ULL;
  bottom_diag7s += 77309411328ULL;
  bottom_rows += 136797534466080768ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5D6;
  flipped = true;
}

void UndoFlip_C5E7() {
  left_columns -= 3538962ULL;
  top_diag7s -= 18ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 5111808ULL;
  bottom_diag7s -= 77309412864ULL;
  bottom_rows -= 136797534469619712ULL;
  disk_difference -= 6;
}

void Flip_C5E7() {
  left_columns += 3538962ULL;
  top_diag7s += 18ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 5111808ULL;
  bottom_diag7s += 77309412864ULL;
  bottom_rows += 136797534469619712ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C5E7;
  flipped = true;
}

void UndoFlip_D6E7() {
  left_columns -= 18ULL;
  right_columns -= 1688849860263936ULL;
  bottom_diag9s -= 1572864ULL;
  bottom_diag7s -= 77309412864ULL;
  bottom_rows -= 695788240896ULL;
  disk_difference -= 4;
}

void Flip_D6E7() {
  left_columns += 18ULL;
  right_columns += 1688849860263936ULL;
  bottom_diag9s += 1572864ULL;
  bottom_diag7s += 77309412864ULL;
  bottom_rows += 695788240896ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_D6E7;
  flipped = true;
}

void UndoFlip_B5_D7() {
  left_columns -= 231928233990ULL;
  top_diag7s -= 393216ULL;
  bottom_diag9s -= 15360ULL;
  bottom_diag7s -= 393216ULL;
  bottom_rows -= 410390516054753280ULL;
  disk_difference -= 4;
}

void Flip_B5_D7() {
  left_columns += 231928233990ULL;
  top_diag7s += 393216ULL;
  bottom_diag9s += 15360ULL;
  bottom_diag7s += 393216ULL;
  bottom_rows += 410390516054753280ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5_D7;
  flipped = true;
}

void UndoFlip_B5C6() {
  left_columns -= 231929413632ULL;
  top_diag7s -= 393216ULL;
  bottom_diag9s -= 18432ULL;
  bottom_diag7s -= 5066549580791808ULL;
  bottom_rows -= 410392603398242304ULL;
  disk_difference -= 4;
}

void Flip_B5C6() {
  left_columns += 231929413632ULL;
  top_diag7s += 393216ULL;
  bottom_diag9s += 18432ULL;
  bottom_diag7s += 5066549580791808ULL;
  bottom_rows += 410392603398242304ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5C6;
  flipped = true;
}

void UndoFlip_B5D7() {
  left_columns -= 231929413638ULL;
  top_diag7s -= 393216ULL;
  bottom_diag9s -= 19968ULL;
  bottom_diag7s -= 5066549581185024ULL;
  bottom_rows -= 410392603408859136ULL;
  disk_difference -= 6;
}

void Flip_B5D7() {
  left_columns += 231929413638ULL;
  top_diag7s += 393216ULL;
  bottom_diag9s += 19968ULL;
  bottom_diag7s += 5066549581185024ULL;
  bottom_rows += 410392603408859136ULL;
  disk_difference += 6;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B5D7;
  flipped = true;
}

void UndoFlip_C6D7() {
  left_columns -= 1179654ULL;
  bottom_diag9s -= 6144ULL;
  bottom_diag7s -= 5066549581185024ULL;
  bottom_rows -= 2087364722688ULL;
  disk_difference -= 4;
}

void Flip_C6D7() {
  left_columns += 1179654ULL;
  bottom_diag9s += 6144ULL;
  bottom_diag7s += 5066549581185024ULL;
  bottom_rows += 2087364722688ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_C6D7;
  flipped = true;
}

void UndoFlip_B6C7() {
  left_columns -= 77309804544ULL;
  top_diag7s -= 6ULL;
  bottom_diag9s -= 24ULL;
  bottom_diag7s -= 25769803776ULL;
  bottom_rows -= 6262094168064ULL;
  disk_difference -= 4;
}

void Flip_B6C7() {
  left_columns += 77309804544ULL;
  top_diag7s += 6ULL;
  bottom_diag9s += 24ULL;
  bottom_diag7s += 25769803776ULL;
  bottom_rows += 6262094168064ULL;
  disk_difference += 4;
  action_stack.value[depth][action_stack_count++] = &UndoFlip_B6C7;
  flipped = true;
}

void Unset_A1() {
  top_rows -= 615585774066204672ULL;
  left_columns -= 615585774066204672ULL;
  bottom_diag9s -= 615585774066204672ULL;
  disk_difference -= 1;
}

void Set_A1() {
  top_rows += 615585774066204672ULL;
  left_columns += 615585774066204672ULL;
  bottom_diag9s += 615585774066204672ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_A1;
}

void Unset_B1() {
  top_rows -= 205195258022068224ULL;
  left_columns -= 9393093476352ULL;
  top_diag9s -= 729ULL;
  disk_difference -= 1;
}

void Set_B1() {
  top_rows += 205195258022068224ULL;
  left_columns += 9393093476352ULL;
  top_diag9s += 729ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_B1;
}

void Unset_C1() {
  top_rows -= 68398419340689408ULL;
  left_columns -= 143327232ULL;
  top_diag7s -= 648518346341351424ULL;
  top_diag9s -= 15925248ULL;
  disk_difference -= 1;
}

void Set_C1() {
  top_rows += 68398419340689408ULL;
  left_columns += 143327232ULL;
  top_diag7s += 648518346341351424ULL;
  top_diag9s += 15925248ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_C1;
}

void Unset_D1() {
  top_rows -= 22799473113563136ULL;
  left_columns -= 2187ULL;
  top_diag7s -= 7599824371187712ULL;
  top_diag9s -= 89060441849856ULL;
  disk_difference -= 1;
}

void Set_D1() {
  top_rows += 22799473113563136ULL;
  left_columns += 2187ULL;
  top_diag7s += 7599824371187712ULL;
  top_diag9s += 89060441849856ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_D1;
}

void Unset_E1() {
  top_rows -= 7599824371187712ULL;
  right_columns -= 615585774066204672ULL;
  top_diag7s -= 89060441849856ULL;
  top_diag9s -= 7599824371187712ULL;
  disk_difference -= 1;
}

void Set_E1() {
  top_rows += 7599824371187712ULL;
  right_columns += 615585774066204672ULL;
  top_diag7s += 89060441849856ULL;
  top_diag9s += 7599824371187712ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_E1;
}

void Unset_F1() {
  top_rows -= 2533274790395904ULL;
  right_columns -= 9393093476352ULL;
  top_diag7s -= 15925248ULL;
  top_diag9s -= 648518346341351424ULL;
  disk_difference -= 1;
}

void Set_F1() {
  top_rows += 2533274790395904ULL;
  right_columns += 9393093476352ULL;
  top_diag7s += 15925248ULL;
  top_diag9s += 648518346341351424ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_F1;
}

void Unset_G1() {
  top_rows -= 844424930131968ULL;
  right_columns -= 143327232ULL;
  top_diag7s -= 729ULL;
  disk_difference -= 1;
}

void Set_G1() {
  top_rows += 844424930131968ULL;
  right_columns += 143327232ULL;
  top_diag7s += 729ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_G1;
}

void Unset_H1() {
  top_rows -= 281474976710656ULL;
  right_columns -= 2187ULL;
  bottom_diag7s -= 615585774066204672ULL;
  disk_difference -= 1;
}

void Set_H1() {
  top_rows += 281474976710656ULL;
  right_columns += 2187ULL;
  bottom_diag7s += 615585774066204672ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_H1;
}

void Unset_A2() {
  top_rows -= 9393093476352ULL;
  left_columns -= 205195258022068224ULL;
  bottom_diag9s -= 3131031158784ULL;
  disk_difference -= 1;
}

void Set_A2() {
  top_rows += 9393093476352ULL;
  left_columns += 205195258022068224ULL;
  bottom_diag9s += 3131031158784ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_A2;
}

void Unset_B2() {
  top_rows -= 3131031158784ULL;
  left_columns -= 3131031158784ULL;
  top_diag7s -= 216172782113783808ULL;
  bottom_diag9s -= 205195258022068224ULL;
  disk_difference -= 1;
}

void Set_B2() {
  top_rows += 3131031158784ULL;
  left_columns += 3131031158784ULL;
  top_diag7s += 216172782113783808ULL;
  bottom_diag9s += 205195258022068224ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_B2;
}

void Unset_C2() {
  top_rows -= 1043677052928ULL;
  left_columns -= 47775744ULL;
  top_diag7s -= 2533274790395904ULL;
  top_diag9s -= 243ULL;
  disk_difference -= 1;
}

void Set_C2() {
  top_rows += 1043677052928ULL;
  left_columns += 47775744ULL;
  top_diag7s += 2533274790395904ULL;
  top_diag9s += 243ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_C2;
}

void Unset_D2() {
  top_rows -= 347892350976ULL;
  left_columns -= 729ULL;
  top_diag7s -= 29686813949952ULL;
  top_diag9s -= 5308416ULL;
  disk_difference -= 1;
}

void Set_D2() {
  top_rows += 347892350976ULL;
  left_columns += 729ULL;
  top_diag7s += 29686813949952ULL;
  top_diag9s += 5308416ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_D2;
}

void Unset_E2() {
  top_rows -= 115964116992ULL;
  right_columns -= 205195258022068224ULL;
  top_diag7s -= 5308416ULL;
  top_diag9s -= 29686813949952ULL;
  disk_difference -= 1;
}

void Set_E2() {
  top_rows += 115964116992ULL;
  right_columns += 205195258022068224ULL;
  top_diag7s += 5308416ULL;
  top_diag9s += 29686813949952ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_E2;
}

void Unset_F2() {
  top_rows -= 38654705664ULL;
  right_columns -= 3131031158784ULL;
  top_diag7s -= 243ULL;
  top_diag9s -= 2533274790395904ULL;
  disk_difference -= 1;
}

void Set_F2() {
  top_rows += 38654705664ULL;
  right_columns += 3131031158784ULL;
  top_diag7s += 243ULL;
  top_diag9s += 2533274790395904ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_F2;
}

void Unset_G2() {
  top_rows -= 12884901888ULL;
  right_columns -= 47775744ULL;
  bottom_diag7s -= 205195258022068224ULL;
  top_diag9s -= 216172782113783808ULL;
  disk_difference -= 1;
}

void Set_G2() {
  top_rows += 12884901888ULL;
  right_columns += 47775744ULL;
  bottom_diag7s += 205195258022068224ULL;
  top_diag9s += 216172782113783808ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_G2;
}

void Unset_H2() {
  top_rows -= 4294967296ULL;
  right_columns -= 729ULL;
  bottom_diag7s -= 3131031158784ULL;
  disk_difference -= 1;
}

void Set_H2() {
  top_rows += 4294967296ULL;
  right_columns += 729ULL;
  bottom_diag7s += 3131031158784ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_H2;
}

void Unset_A3() {
  top_rows -= 143327232ULL;
  left_columns -= 68398419340689408ULL;
  top_diag7s -= 72057594037927936ULL;
  bottom_diag9s -= 15925248ULL;
  disk_difference -= 1;
}

void Set_A3() {
  top_rows += 143327232ULL;
  left_columns += 68398419340689408ULL;
  top_diag7s += 72057594037927936ULL;
  bottom_diag9s += 15925248ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_A3;
}

void Unset_B3() {
  top_rows -= 47775744ULL;
  left_columns -= 1043677052928ULL;
  top_diag7s -= 844424930131968ULL;
  bottom_diag9s -= 1043677052928ULL;
  disk_difference -= 1;
}

void Set_B3() {
  top_rows += 47775744ULL;
  left_columns += 1043677052928ULL;
  top_diag7s += 844424930131968ULL;
  bottom_diag9s += 1043677052928ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_B3;
}

void Unset_C3() {
  top_rows -= 15925248ULL;
  left_columns -= 15925248ULL;
  top_diag7s -= 9895604649984ULL;
  bottom_diag9s -= 68398419340689408ULL;
  disk_difference -= 1;
}

void Set_C3() {
  top_rows += 15925248ULL;
  left_columns += 15925248ULL;
  top_diag7s += 9895604649984ULL;
  bottom_diag9s += 68398419340689408ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_C3;
}

void Unset_D3() {
  top_rows -= 5308416ULL;
  left_columns -= 243ULL;
  top_diag7s -= 1769472ULL;
  top_diag9s -= 81ULL;
  disk_difference -= 1;
}

void Set_D3() {
  top_rows += 5308416ULL;
  left_columns += 243ULL;
  top_diag7s += 1769472ULL;
  top_diag9s += 81ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_D3;
}

void Unset_E3() {
  top_rows -= 1769472ULL;
  right_columns -= 68398419340689408ULL;
  top_diag7s -= 81ULL;
  top_diag9s -= 1769472ULL;
  disk_difference -= 1;
}

void Set_E3() {
  top_rows += 1769472ULL;
  right_columns += 68398419340689408ULL;
  top_diag7s += 81ULL;
  top_diag9s += 1769472ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_E3;
}

void Unset_F3() {
  top_rows -= 589824ULL;
  right_columns -= 1043677052928ULL;
  bottom_diag7s -= 68398419340689408ULL;
  top_diag9s -= 9895604649984ULL;
  disk_difference -= 1;
}

void Set_F3() {
  top_rows += 589824ULL;
  right_columns += 1043677052928ULL;
  bottom_diag7s += 68398419340689408ULL;
  top_diag9s += 9895604649984ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_F3;
}

void Unset_G3() {
  top_rows -= 196608ULL;
  right_columns -= 15925248ULL;
  bottom_diag7s -= 1043677052928ULL;
  top_diag9s -= 844424930131968ULL;
  disk_difference -= 1;
}

void Set_G3() {
  top_rows += 196608ULL;
  right_columns += 15925248ULL;
  bottom_diag7s += 1043677052928ULL;
  top_diag9s += 844424930131968ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_G3;
}

void Unset_H3() {
  top_rows -= 65536ULL;
  right_columns -= 243ULL;
  bottom_diag7s -= 15925248ULL;
  top_diag9s -= 72057594037927936ULL;
  disk_difference -= 1;
}

void Set_H3() {
  top_rows += 65536ULL;
  right_columns += 243ULL;
  bottom_diag7s += 15925248ULL;
  top_diag9s += 72057594037927936ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_H3;
}

void Unset_A4() {
  top_rows -= 2187ULL;
  left_columns -= 22799473113563136ULL;
  top_diag7s -= 281474976710656ULL;
  bottom_diag9s -= 20736ULL;
  disk_difference -= 1;
}

void Set_A4() {
  top_rows += 2187ULL;
  left_columns += 22799473113563136ULL;
  top_diag7s += 281474976710656ULL;
  bottom_diag9s += 20736ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_A4;
}

void Unset_B4() {
  top_rows -= 729ULL;
  left_columns -= 347892350976ULL;
  top_diag7s -= 3298534883328ULL;
  bottom_diag9s -= 5308416ULL;
  disk_difference -= 1;
}

void Set_B4() {
  top_rows += 729ULL;
  left_columns += 347892350976ULL;
  top_diag7s += 3298534883328ULL;
  bottom_diag9s += 5308416ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_B4;
}

void Unset_C4() {
  top_rows -= 243ULL;
  left_columns -= 5308416ULL;
  top_diag7s -= 589824ULL;
  bottom_diag9s -= 347892350976ULL;
  disk_difference -= 1;
}

void Set_C4() {
  top_rows += 243ULL;
  left_columns += 5308416ULL;
  top_diag7s += 589824ULL;
  bottom_diag9s += 347892350976ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_C4;
}

void Unset_D4() {
  top_rows -= 81ULL;
  left_columns -= 81ULL;
  top_diag7s -= 27ULL;
  bottom_diag9s -= 22799473113563136ULL;
  disk_difference -= 1;
}

void Set_D4() {
  top_rows += 81ULL;
  left_columns += 81ULL;
  top_diag7s += 27ULL;
  bottom_diag9s += 22799473113563136ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_D4;
}

void Unset_E4() {
  top_rows -= 27ULL;
  right_columns -= 22799473113563136ULL;
  bottom_diag7s -= 22799473113563136ULL;
  top_diag9s -= 27ULL;
  disk_difference -= 1;
}

void Set_E4() {
  top_rows += 27ULL;
  right_columns += 22799473113563136ULL;
  bottom_diag7s += 22799473113563136ULL;
  top_diag9s += 27ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_E4;
}

void Unset_F4() {
  top_rows -= 9ULL;
  right_columns -= 347892350976ULL;
  bottom_diag7s -= 347892350976ULL;
  top_diag9s -= 589824ULL;
  disk_difference -= 1;
}

void Set_F4() {
  top_rows += 9ULL;
  right_columns += 347892350976ULL;
  bottom_diag7s += 347892350976ULL;
  top_diag9s += 589824ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_F4;
}

void Unset_G4() {
  top_rows -= 3ULL;
  right_columns -= 5308416ULL;
  bottom_diag7s -= 5308416ULL;
  top_diag9s -= 3298534883328ULL;
  disk_difference -= 1;
}

void Set_G4() {
  top_rows += 3ULL;
  right_columns += 5308416ULL;
  bottom_diag7s += 5308416ULL;
  top_diag9s += 3298534883328ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_G4;
}

void Unset_H4() {
  top_rows -= 1ULL;
  right_columns -= 81ULL;
  bottom_diag7s -= 20736ULL;
  top_diag9s -= 281474976710656ULL;
  disk_difference -= 1;
}

void Set_H4() {
  top_rows += 1ULL;
  right_columns += 81ULL;
  bottom_diag7s += 20736ULL;
  top_diag9s += 281474976710656ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_H4;
}

void Unset_A5() {
  bottom_rows -= 615585774066204672ULL;
  left_columns -= 7599824371187712ULL;
  top_diag7s -= 1099511627776ULL;
  bottom_diag9s -= 27ULL;
  disk_difference -= 1;
}

void Set_A5() {
  bottom_rows += 615585774066204672ULL;
  left_columns += 7599824371187712ULL;
  top_diag7s += 1099511627776ULL;
  bottom_diag9s += 27ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_A5;
}

void Unset_B5() {
  bottom_rows -= 205195258022068224ULL;
  left_columns -= 115964116992ULL;
  top_diag7s -= 196608ULL;
  bottom_diag9s -= 6912ULL;
  disk_difference -= 1;
}

void Set_B5() {
  bottom_rows += 205195258022068224ULL;
  left_columns += 115964116992ULL;
  top_diag7s += 196608ULL;
  bottom_diag9s += 6912ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_B5;
}

void Unset_C5() {
  bottom_rows -= 68398419340689408ULL;
  left_columns -= 1769472ULL;
  top_diag7s -= 9ULL;
  bottom_diag9s -= 1769472ULL;
  disk_difference -= 1;
}

void Set_C5() {
  bottom_rows += 68398419340689408ULL;
  left_columns += 1769472ULL;
  top_diag7s += 9ULL;
  bottom_diag9s += 1769472ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_C5;
}

void Unset_D5() {
  bottom_rows -= 22799473113563136ULL;
  left_columns -= 27ULL;
  bottom_diag7s -= 7599824371187712ULL;
  bottom_diag9s -= 115964116992ULL;
  disk_difference -= 1;
}

void Set_D5() {
  bottom_rows += 22799473113563136ULL;
  left_columns += 27ULL;
  bottom_diag7s += 7599824371187712ULL;
  bottom_diag9s += 115964116992ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_D5;
}

void Unset_E5() {
  bottom_rows -= 7599824371187712ULL;
  right_columns -= 7599824371187712ULL;
  bottom_diag7s -= 115964116992ULL;
  bottom_diag9s -= 7599824371187712ULL;
  disk_difference -= 1;
}

void Set_E5() {
  bottom_rows += 7599824371187712ULL;
  right_columns += 7599824371187712ULL;
  bottom_diag7s += 115964116992ULL;
  bottom_diag9s += 7599824371187712ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_E5;
}

void Unset_F5() {
  bottom_rows -= 2533274790395904ULL;
  right_columns -= 115964116992ULL;
  bottom_diag7s -= 1769472ULL;
  top_diag9s -= 9ULL;
  disk_difference -= 1;
}

void Set_F5() {
  bottom_rows += 2533274790395904ULL;
  right_columns += 115964116992ULL;
  bottom_diag7s += 1769472ULL;
  top_diag9s += 9ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_F5;
}

void Unset_G5() {
  bottom_rows -= 844424930131968ULL;
  right_columns -= 1769472ULL;
  bottom_diag7s -= 6912ULL;
  top_diag9s -= 196608ULL;
  disk_difference -= 1;
}

void Set_G5() {
  bottom_rows += 844424930131968ULL;
  right_columns += 1769472ULL;
  bottom_diag7s += 6912ULL;
  top_diag9s += 196608ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_G5;
}

void Unset_H5() {
  bottom_rows -= 281474976710656ULL;
  right_columns -= 27ULL;
  bottom_diag7s -= 27ULL;
  top_diag9s -= 1099511627776ULL;
  disk_difference -= 1;
}

void Set_H5() {
  bottom_rows += 281474976710656ULL;
  right_columns += 27ULL;
  bottom_diag7s += 27ULL;
  top_diag9s += 1099511627776ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_H5;
}

void Unset_A6() {
  bottom_rows -= 9393093476352ULL;
  left_columns -= 2533274790395904ULL;
  top_diag7s -= 65536ULL;
  top_diag9s -= 38654705664ULL;
  disk_difference -= 1;
}

void Set_A6() {
  bottom_rows += 9393093476352ULL;
  left_columns += 2533274790395904ULL;
  top_diag7s += 65536ULL;
  top_diag9s += 38654705664ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_A6;
}

void Unset_B6() {
  bottom_rows -= 3131031158784ULL;
  left_columns -= 38654705664ULL;
  top_diag7s -= 3ULL;
  bottom_diag9s -= 9ULL;
  disk_difference -= 1;
}

void Set_B6() {
  bottom_rows += 3131031158784ULL;
  left_columns += 38654705664ULL;
  top_diag7s += 3ULL;
  bottom_diag9s += 9ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_B6;
}

void Unset_C6() {
  bottom_rows -= 1043677052928ULL;
  left_columns -= 589824ULL;
  bottom_diag7s -= 2533274790395904ULL;
  bottom_diag9s -= 2304ULL;
  disk_difference -= 1;
}

void Set_C6() {
  bottom_rows += 1043677052928ULL;
  left_columns += 589824ULL;
  bottom_diag7s += 2533274790395904ULL;
  bottom_diag9s += 2304ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_C6;
}

void Unset_D6() {
  bottom_rows -= 347892350976ULL;
  left_columns -= 9ULL;
  bottom_diag7s -= 38654705664ULL;
  bottom_diag9s -= 589824ULL;
  disk_difference -= 1;
}

void Set_D6() {
  bottom_rows += 347892350976ULL;
  left_columns += 9ULL;
  bottom_diag7s += 38654705664ULL;
  bottom_diag9s += 589824ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_D6;
}

void Unset_E6() {
  bottom_rows -= 115964116992ULL;
  right_columns -= 2533274790395904ULL;
  bottom_diag7s -= 589824ULL;
  bottom_diag9s -= 38654705664ULL;
  disk_difference -= 1;
}

void Set_E6() {
  bottom_rows += 115964116992ULL;
  right_columns += 2533274790395904ULL;
  bottom_diag7s += 589824ULL;
  bottom_diag9s += 38654705664ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_E6;
}

void Unset_F6() {
  bottom_rows -= 38654705664ULL;
  right_columns -= 38654705664ULL;
  bottom_diag7s -= 2304ULL;
  bottom_diag9s -= 2533274790395904ULL;
  disk_difference -= 1;
}

void Set_F6() {
  bottom_rows += 38654705664ULL;
  right_columns += 38654705664ULL;
  bottom_diag7s += 2304ULL;
  bottom_diag9s += 2533274790395904ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_F6;
}

void Unset_G6() {
  bottom_rows -= 12884901888ULL;
  right_columns -= 589824ULL;
  bottom_diag7s -= 9ULL;
  top_diag9s -= 3ULL;
  disk_difference -= 1;
}

void Set_G6() {
  bottom_rows += 12884901888ULL;
  right_columns += 589824ULL;
  bottom_diag7s += 9ULL;
  top_diag9s += 3ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_G6;
}

void Unset_H6() {
  bottom_rows -= 4294967296ULL;
  right_columns -= 9ULL;
  top_diag7s -= 38654705664ULL;
  top_diag9s -= 65536ULL;
  disk_difference -= 1;
}

void Set_H6() {
  bottom_rows += 4294967296ULL;
  right_columns += 9ULL;
  top_diag7s += 38654705664ULL;
  top_diag9s += 65536ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_H6;
}

void Unset_A7() {
  bottom_rows -= 143327232ULL;
  left_columns -= 844424930131968ULL;
  top_diag7s -= 1ULL;
  disk_difference -= 1;
}

void Set_A7() {
  bottom_rows += 143327232ULL;
  left_columns += 844424930131968ULL;
  top_diag7s += 1ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_A7;
}

void Unset_B7() {
  bottom_rows -= 47775744ULL;
  left_columns -= 12884901888ULL;
  bottom_diag7s -= 844424930131968ULL;
  top_diag9s -= 12884901888ULL;
  disk_difference -= 1;
}

void Set_B7() {
  bottom_rows += 47775744ULL;
  left_columns += 12884901888ULL;
  bottom_diag7s += 844424930131968ULL;
  top_diag9s += 12884901888ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_B7;
}

void Unset_C7() {
  bottom_rows -= 15925248ULL;
  left_columns -= 196608ULL;
  bottom_diag7s -= 12884901888ULL;
  bottom_diag9s -= 3ULL;
  disk_difference -= 1;
}

void Set_C7() {
  bottom_rows += 15925248ULL;
  left_columns += 196608ULL;
  bottom_diag7s += 12884901888ULL;
  bottom_diag9s += 3ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_C7;
}

void Unset_D7() {
  bottom_rows -= 5308416ULL;
  left_columns -= 3ULL;
  bottom_diag7s -= 196608ULL;
  bottom_diag9s -= 768ULL;
  disk_difference -= 1;
}

void Set_D7() {
  bottom_rows += 5308416ULL;
  left_columns += 3ULL;
  bottom_diag7s += 196608ULL;
  bottom_diag9s += 768ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_D7;
}

void Unset_E7() {
  bottom_rows -= 1769472ULL;
  right_columns -= 844424930131968ULL;
  bottom_diag7s -= 768ULL;
  bottom_diag9s -= 196608ULL;
  disk_difference -= 1;
}

void Set_E7() {
  bottom_rows += 1769472ULL;
  right_columns += 844424930131968ULL;
  bottom_diag7s += 768ULL;
  bottom_diag9s += 196608ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_E7;
}

void Unset_F7() {
  bottom_rows -= 589824ULL;
  right_columns -= 12884901888ULL;
  bottom_diag7s -= 3ULL;
  bottom_diag9s -= 12884901888ULL;
  disk_difference -= 1;
}

void Set_F7() {
  bottom_rows += 589824ULL;
  right_columns += 12884901888ULL;
  bottom_diag7s += 3ULL;
  bottom_diag9s += 12884901888ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_F7;
}

void Unset_G7() {
  bottom_rows -= 196608ULL;
  right_columns -= 196608ULL;
  top_diag7s -= 12884901888ULL;
  bottom_diag9s -= 844424930131968ULL;
  disk_difference -= 1;
}

void Set_G7() {
  bottom_rows += 196608ULL;
  right_columns += 196608ULL;
  top_diag7s += 12884901888ULL;
  bottom_diag9s += 844424930131968ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_G7;
}

void Unset_H7() {
  bottom_rows -= 65536ULL;
  right_columns -= 3ULL;
  top_diag9s -= 1ULL;
  disk_difference -= 1;
}

void Set_H7() {
  bottom_rows += 65536ULL;
  right_columns += 3ULL;
  top_diag9s += 1ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_H7;
}

void Unset_A8() {
  bottom_rows -= 2187ULL;
  left_columns -= 281474976710656ULL;
  bottom_diag7s -= 281474976710656ULL;
  disk_difference -= 1;
}

void Set_A8() {
  bottom_rows += 2187ULL;
  left_columns += 281474976710656ULL;
  bottom_diag7s += 281474976710656ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_A8;
}

void Unset_B8() {
  bottom_rows -= 729ULL;
  left_columns -= 4294967296ULL;
  bottom_diag7s -= 4294967296ULL;
  disk_difference -= 1;
}

void Set_B8() {
  bottom_rows += 729ULL;
  left_columns += 4294967296ULL;
  bottom_diag7s += 4294967296ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_B8;
}

void Unset_C8() {
  bottom_rows -= 243ULL;
  left_columns -= 65536ULL;
  bottom_diag7s -= 65536ULL;
  top_diag9s -= 4294967296ULL;
  disk_difference -= 1;
}

void Set_C8() {
  bottom_rows += 243ULL;
  left_columns += 65536ULL;
  bottom_diag7s += 65536ULL;
  top_diag9s += 4294967296ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_C8;
}

void Unset_D8() {
  bottom_rows -= 81ULL;
  left_columns -= 1ULL;
  bottom_diag7s -= 256ULL;
  bottom_diag9s -= 1ULL;
  disk_difference -= 1;
}

void Set_D8() {
  bottom_rows += 81ULL;
  left_columns += 1ULL;
  bottom_diag7s += 256ULL;
  bottom_diag9s += 1ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_D8;
}

void Unset_E8() {
  bottom_rows -= 27ULL;
  right_columns -= 281474976710656ULL;
  bottom_diag7s -= 1ULL;
  bottom_diag9s -= 256ULL;
  disk_difference -= 1;
}

void Set_E8() {
  bottom_rows += 27ULL;
  right_columns += 281474976710656ULL;
  bottom_diag7s += 1ULL;
  bottom_diag9s += 256ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_E8;
}

void Unset_F8() {
  bottom_rows -= 9ULL;
  right_columns -= 4294967296ULL;
  top_diag7s -= 4294967296ULL;
  bottom_diag9s -= 65536ULL;
  disk_difference -= 1;
}

void Set_F8() {
  bottom_rows += 9ULL;
  right_columns += 4294967296ULL;
  top_diag7s += 4294967296ULL;
  bottom_diag9s += 65536ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_F8;
}

void Unset_G8() {
  bottom_rows -= 3ULL;
  right_columns -= 65536ULL;
  bottom_diag9s -= 4294967296ULL;
  disk_difference -= 1;
}

void Set_G8() {
  bottom_rows += 3ULL;
  right_columns += 65536ULL;
  bottom_diag9s += 4294967296ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_G8;
}

void Unset_H8() {
  bottom_rows -= 1ULL;
  right_columns -= 1ULL;
  bottom_diag9s -= 281474976710656ULL;
  disk_difference -= 1;
}

void Set_H8() {
  bottom_rows += 1ULL;
  right_columns += 1ULL;
  bottom_diag9s += 281474976710656ULL;
  disk_difference += 1;
  action_stack.value[depth][action_stack_count++] = &Unset_H8;
}

#endif  // SENSEI_BOARD_OPTIMIZED_FLIP_OPTIMIZED_H
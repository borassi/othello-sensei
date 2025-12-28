/*
 * Copyright 2023-2025 Michele Borassi
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
#include <stdbool.h>

#include "bindings.h"
#include "../utils/types.h"

#ifndef OTHELLO_SENSEI_UI_H
#define OTHELLO_SENSEI_UI_H

#ifdef __cplusplus
extern "C" {
#endif


EXPORT
Square PassMove();

EXPORT
Square NoMove();

EXPORT
Square SetupBoardMove();

EXPORT
void* MainInit(
    char* evals_filepath,
    char* book_filepath,
    char* thor_filepath,
    char* saved_games_filepath,
    char* xot_small_filepath,
    char* xot_large_filepath,
    SetBoard set_board,
    UpdateAnnotations update_annotations,
    SetThorMetadata set_thor_metadata,
    UpdateTimers update_timers,
    SendMessage send_message);

EXPORT
void MainDelete(void* ptr);

EXPORT
void SetEvaluateParams(void* ptr, struct EvaluateParams* params);

EXPORT
void NewGame(void* ptr);

EXPORT
bool PlayMove(void* ptr, int square, bool automatic);

EXPORT
bool SetSequence(void* ptr, char* sequence);

EXPORT
bool PasteBoard(void* ptr, char* sequence);

EXPORT
char* GetSequence(void* ptr);

EXPORT
bool Undo(void* ptr);

EXPORT
bool SetCurrentMove(void* ptr, int current_move);

EXPORT
bool Redo(void* ptr);

EXPORT
bool ToLastImportantNode(void* ptr);

EXPORT
void Evaluate(void* ptr);

EXPORT
void Analyze(void* ptr);

EXPORT
void ResetAnalyzedGame(void* ptr);

EXPORT
struct Annotations* GetCurrentAnnotations(void* ptr, int current_thread);

EXPORT
struct Annotations* GetStartAnnotations(void* ptr, int current_thread);

EXPORT
void Stop(void* ptr);

EXPORT
void RandomXOT(void* ptr, bool large);

EXPORT
void SetXOTState(void* ptr, enum XOTState xot_state);

EXPORT
enum XOTState GetXOTState(void* ptr);

EXPORT
bool IsXot(void* ptr);

EXPORT
void SetBlackSquare(void* ptr, int square);

EXPORT
void SetWhiteSquare(void* ptr, int square);

EXPORT
void SetEmptySquare(void* ptr, int square);

EXPORT
void InvertTurn(void* ptr);

EXPORT
struct SaveGameOutput* GetGameToSave(void* ptr);

EXPORT
void Open(void* ptr, char* path);

EXPORT
struct GameMetadata* MutableGameMetadata(void* ptr);

EXPORT
void PlayOneMove(void* ptr, struct ThorGame game);

EXPORT
void OpenThorGame(void* ptr, struct ThorGame game);

EXPORT
void SetFileSources(void* ptr, int num_folders, char** folders);

EXPORT
bool ReloadSourceUi(void* ptr, const char* file);

EXPORT
void SetCountingTime(void* ptr, bool value);

#ifdef __cplusplus
}
#endif
#endif // OTHELLO_SENSEI_UI_H

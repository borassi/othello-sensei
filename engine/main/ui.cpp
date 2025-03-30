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

#include "ui.h"
#include "main.h"

Square PassMove() { return kPassMove; }

void* MainInit(
    char* evals_filepath,
    char* book_filepath,
    char* thor_filepath,
    char* xot_small_filepath,
    char* xot_large_filepath,
    SetBoard set_board,
    UpdateAnnotations update_annotations) {
  return new Main(
      evals_filepath, book_filepath, thor_filepath, xot_small_filepath, xot_large_filepath,
      set_board, update_annotations);
}

ThorMetadata* MainGetThorMetadata(void* ptr) { return static_cast<Main*>(ptr)->GetThorMetadata(); }

void MainDelete(void* ptr) { delete static_cast<Main*>(ptr); }

struct EvaluateParams* MainGetEvaluateParams(void* ptr) {
  return &static_cast<Main*>(ptr)->GetEvaluateParams();
}

void NewGame(void* ptr) { static_cast<Main*>(ptr)->NewGame(); }

bool PlayMove(void* ptr, int square) { return static_cast<Main*>(ptr)->PlayMove(square); }

bool SetSequence(void* ptr, char* sequence) {
  std::string sequence_string(sequence);
  return static_cast<Main*>(ptr)->SetSequence(sequence_string);
}

char* GetSequence(void* ptr) { return static_cast<Main*>(ptr)->GetSequence(); }

bool Undo(void* ptr) { return static_cast<Main*>(ptr)->Undo(); }

bool SetCurrentMove(void* ptr, int square) { return static_cast<Main*>(ptr)->SetCurrentMove(square); }

bool Redo(void* ptr) { return static_cast<Main*>(ptr)->Redo(); }

bool ToAnalyzedGameOrLastChoice(void* ptr) {
  return static_cast<Main*>(ptr)->ToAnalyzedGameOrLastChoice();
}

void Evaluate(void* ptr) { static_cast<Main*>(ptr)->Evaluate(); }

void Analyze(void* ptr) { static_cast<Main*>(ptr)->Analyze(); }

void ResetAnalyzedGame(void* ptr) { static_cast<Main*>(ptr)->ResetAnalyzedGame(); }

void Stop(void* ptr) { static_cast<Main*>(ptr)->Stop(); }

Annotations* GetCurrentAnnotations(void* ptr, int current_thread) {
  return static_cast<Main*>(ptr)->GetCurrentAnnotations(current_thread);
}

Annotations* GetStartAnnotations(void* ptr, int current_thread) {
  return static_cast<Main*>(ptr)->GetStartAnnotations(current_thread);
}

void RandomXOT(void* ptr, bool large) { static_cast<Main*>(ptr)->RandomXOT(large); }

void SetXOTState(void* ptr, XOTState xot_state) { static_cast<Main*>(ptr)->SetXOTState(xot_state); }
XOTState GetXOTState(void* ptr) { return static_cast<Main*>(ptr)->GetXOTState(); }

bool IsXot(void* ptr) { return static_cast<Main*>(ptr)->IsXot(); }

void SetBlackSquare(void* ptr, int square) { static_cast<Main*>(ptr)->SetSquare(square, -1); }
void SetWhiteSquare(void* ptr, int square) { static_cast<Main*>(ptr)->SetSquare(square, 1); }
void SetEmptySquare(void* ptr, int square) { static_cast<Main*>(ptr)->SetSquare(square, 0); }
void InvertTurn(void* ptr) { static_cast<Main*>(ptr)->InvertTurn(); }
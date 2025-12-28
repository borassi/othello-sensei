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

Square NoMove() { return kNoMove; }
Square PassMove() { return kPassMove; }
Square SetupBoardMove() { return kSetupBoardMove; }

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
    SendMessage send_message) {
  return new Main(
      evals_filepath, book_filepath, thor_filepath, saved_games_filepath, xot_small_filepath,
      xot_large_filepath, set_board, update_annotations, set_thor_metadata, update_timers,
      send_message);
}

void MainDelete(void* ptr) { delete static_cast<Main*>(ptr); }

void SetEvaluateParams(void* ptr, struct EvaluateParams* params) {
  assert(params != nullptr);
  static_cast<Main*>(ptr)->SetEvaluateParams(*params);
}

void NewGame(void* ptr) { static_cast<Main*>(ptr)->NewGame(); }

bool PlayMove(void* ptr, int square, bool automatic) {
  return static_cast<Main*>(ptr)->PlayMove(square, automatic);
}

bool SetSequence(void* ptr, char* sequence) {
  std::string sequence_string(sequence);
  return static_cast<Main*>(ptr)->SetSequence(sequence_string);
}

bool PasteBoard(void* ptr, char* board) {
  return static_cast<Main*>(ptr)->PasteBoard(board);
}

char* GetSequence(void* ptr) { return static_cast<Main*>(ptr)->GetSequence(); }

bool Undo(void* ptr) { return static_cast<Main*>(ptr)->Undo(); }

bool SetCurrentMove(void* ptr, int depth) { return static_cast<Main*>(ptr)->SetCurrentMove(depth); }

bool Redo(void* ptr) { return static_cast<Main*>(ptr)->Redo(); }

bool ToLastImportantNode(void* ptr) {
  return static_cast<Main*>(ptr)->ToLastImportantNode();
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
struct SaveGameOutput* GetGameToSave(void* ptr) { return static_cast<Main*>(ptr)->GetGameToSave(); }
void Open(void* ptr, char* path) { static_cast<Main*>(ptr)->Open(path); }
void PlayOneMove(void* ptr, struct ThorGame game) { static_cast<Main*>(ptr)->PlayOneMove(game); }
void OpenThorGame(void* ptr, struct ThorGame game) { static_cast<Main*>(ptr)->OpenThorGame(game); }
GameMetadata* MutableGameMetadata(void* ptr) { return static_cast<Main*>(ptr)->MutableGameMetadata(); }

void SetFileSources(void* ptr, int num_folders, char** folders) {
  std::vector<std::string> folders_vector;
  folders_vector.reserve(num_folders);
  for (int i = 0; i < num_folders; ++i) {
    folders_vector.emplace_back(folders[i]);
  }
  static_cast<Main*>(ptr)->SetFileSources(folders_vector);
}

bool ReloadSourceUi(void* ptr, const char* file) { return static_cast<Main*>(ptr)->ReloadSource(file); }
void SetCountingTime(void* ptr, bool value) { static_cast<Main*>(ptr)->SetCountingTime(value); }

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

#include "bindings.h"
#include "main.h"

void* MainInit(char* evals_filepath, char* book_filepath, char* thor_filepath, SetBoard set_board, UpdateAnnotations update_annotations) {
  return new Main(evals_filepath, book_filepath, thor_filepath, set_board, update_annotations);
}

ThorMetadata MainGetThorMetadata(void* ptr) { return static_cast<Main*>(ptr)->GetThorMetadata(); }

void MainDelete(void* ptr) { delete static_cast<Main*>(ptr); }

struct EvaluateParams* MainGetEvaluateParams(void* ptr) {
  return &static_cast<Main*>(ptr)->GetEvaluateParams();
}

void NewGame(void* ptr) { static_cast<Main*>(ptr)->NewGame(); }

void PlayMove(void* ptr, int square) { static_cast<Main*>(ptr)->PlayMove(square); }

void Undo(void* ptr) { static_cast<Main*>(ptr)->Undo(); }

void Redo(void* ptr) { static_cast<Main*>(ptr)->Redo(); }

void Evaluate(void* ptr) { static_cast<Main*>(ptr)->Evaluate(); }

void Stop(void* ptr) { static_cast<Main*>(ptr)->Stop(); }

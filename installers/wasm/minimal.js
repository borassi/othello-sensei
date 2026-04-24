/*
 * Copyright 2026 Michele Borassi
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { SenseiAPI } from './sensei_api.js';

const senseiApi = new SenseiAPI();

const statusEl = document.getElementById('status');
const analyzeBtn = document.getElementById('analyze-btn');
const boardInput = document.getElementById('board-input');
const evaluationVal = document.getElementById('evaluation-value');
const stopBtn = document.getElementById('stop-btn');

(async () => {
    senseiApi.init(window.onSetBoard, window.onUpdateAnnotations).then(() => {
      statusEl.innerText = "Engine Ready";
    });
})()

// Starts analyzing.
analyzeBtn.addEventListener('click', () => {
  const input = boardInput.value.trim();
  if (!input) return;

  // 1. Try to set as a sequence (e.g., "f5f6e6...")
  let success = senseiApi.setSequence(input);

  // 2. If it fails, try to paste as a board string
  if (!success) {
    success = senseiApi.pasteBoard(input);
  }

  if (success) {
    senseiApi.evaluate();
    evaluationVal.innerText = "Thinking...";
  } else {
    evaluationVal.innerText = "Invalid input";
  }
});

// Stops the computation.
stopBtn.addEventListener('click', () => {
  senseiApi.stop();
});

// This is called every time we have a new evaluation (by default every second).
window.onUpdateAnnotations = (threadId, finished, move) => {
  // Use thread 0 for the primary evaluation
  const evaluation = senseiApi.getCurrentEvaluation(threadId);

  // Format the score (assuming it's a float/int from the engine)
  const formattedScore = (evaluation.eval > 0 ? "+" : "") + evaluation.eval.toFixed(2);
  evaluationVal.innerText = formattedScore;
  if (!finished) {
    senseiApi.evaluate();
  }
};

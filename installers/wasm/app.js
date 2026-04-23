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

const boardEl = document.getElementById('othello-board');
const consoleEl = document.getElementById('console-output');
const statusEl = document.getElementById('status');
let enginePtr = null;

// 1. Create the Board UI (0-63)// 1. Create the Board UI (0-63)
for (let i = 0; i < 64; i++) {
  const square = document.createElement('div');
  square.className = 'square';
  square.dataset.index = i;
  square.style.gridColumn = 8 - Math.floor(i / 8);
  square.style.gridRow = 8 - (i % 8);

  const disk = document.createElement('div');
  disk.className = 'disk';
  disk.id = `disk-${i}`;

  const ann = document.createElement('div');
  ann.className = 'annotation';
  ann.id = `annotation-${i}`;

  square.appendChild(disk);
  square.appendChild(ann);
  square.onclick = () => handleSquareClick(i);
  boardEl.appendChild(square);
}

document.getElementById('othello-board').addEventListener('contextmenu', (e) => {
  e.preventDefault(); // Prevent the default browser right-click menu
  handleAction('undo');
});

function handleSquareClick(index) {
  if (!enginePtr) return;
  Module.playMove(enginePtr, index, false);
  Module.evaluate(enginePtr);
}

function handleAction(action) {
  if (!enginePtr) return;
  Module[action](enginePtr);
  if (action !== 'stop') {
    Module.evaluate(enginePtr);
  }
}

// 2. Define the Thread-Safe Global Callbacks for C++
window.onSendMessage = (msg) => {};

window.onSetBoard = (data) => {
  const player = BigInt(data.player);
  const opponent = BigInt(data.opponent);

  // Map the bitboards to colors based on whose turn it is
  const blackBoard = data.black_turn ? player : opponent;
  const whiteBoard = data.black_turn ? opponent : player;

  let empties = 0;

  for (let i = 0; i < 64; i++) {
    const disk = document.getElementById(`disk-${i}`);

    // Clear existing pieces (hides the disk)
    disk.className = 'disk';
    document.getElementById(`annotation-${i}`).innerText = '';

    // Create a mask for the current square
    // Note: This assumes square 0 is the Least Significant Bit (LSB).
    // If your engine maps square 0 to the Most Significant Bit,
    // change `BigInt(i)` to `BigInt(63 - i)`.
    const bitMask = 1n << BigInt(i);

    // Apply the correct CSS class if the bit is set
    if ((blackBoard & bitMask) !== 0n) {
      disk.classList.add('black');
    } else if ((whiteBoard & bitMask) !== 0n) {
      disk.classList.add('white');
    } else {
      ++empties;
    }
  }

  // Update Empties UI
  document.getElementById('stat-empties').innerText = `Empties: ${empties}`;
};

window.onUpdateTimers = (t1, t2) => {
    statusEl.innerText = `B: ${t1.toFixed(1)}s | W: ${t2.toFixed(1)}s`;
};

// Create this formatter once outside your functions for performance
const evalFormatter = new Intl.NumberFormat('en-US', {
    minimumFractionDigits: 2,
    maximumFractionDigits: 2,
    signDisplay: 'always' // Forces + for positive and 0, - for negative
});

// Formats large numbers like Flutter's prettyPrintDouble (e.g., 1.5M, 24K)
const compactFormatter = new Intl.NumberFormat('en-US', {
    notation: "compact",
    maximumFractionDigits: 1
});

window.onUpdateAnnotations = (threadId, finished, move) => {
  // 1. Clear all old annotations from the board
  for (let i = 0; i < 64; i++) {
    const ann = document.getElementById(`annotation-${i}`);
    if (ann) {
      ann.innerText = '';
      ann.classList.remove('optimal');
    }
  }

  // 2. Fetch the new evaluation array from C++
  const children = Module.getEvaluations(enginePtr, threadId);
  if (!children || children.length === 0) return;

  // 3. First loop: compute the best evaluation
  let bestEval = -Infinity;
  let nVisited = 0n;
  let nVisitedBook = 0n;
  let seconds = 0.0;

  for (let i = 0; i < children.length; i++) {
    const child = children[i];
    const displayEval = -child.eval;
    nVisited += child.descendants;
    nVisitedBook += child.descendants_book;
    seconds += child.seconds;
    if (displayEval > bestEval) {
      bestEval = displayEval;
    }
  }

  // 4. Second loop: paint the annotations
  for (let i = 0; i < children.length; i++) {
    const child = children[i];
    const displayEval = -child.eval;
    for (let j = 0; j < child.num_moves; ++j) {
      const moveId = `annotation-${child.moves[j]}`;
      const annElement = document.getElementById(moveId);

      if (annElement) {
        annElement.innerText = evalFormatter.format(displayEval);

        if (Math.abs(displayEval - bestEval) <= 1.0) {
          annElement.classList.add('optimal');
        } else {
          annElement.classList.remove('optimal');
        }
      }
    }
  }

  // 5. Positions Text
  const totalPositions = nVisited + nVisitedBook;
  document.getElementById('stat-positions').innerText =
      totalPositions > 0n ? `Positions: ${compactFormatter.format(totalPositions)}` : '';

  // Positions Per Second Text
  document.getElementById('stat-pos-sec').innerText =
      seconds > 0 ? `Pos / sec: ${compactFormatter.format(Number(nVisited) / seconds)}` : '';

  // Time / Book Status Text
  let timeStatusText = '';
  if (nVisitedBook > 0n) {
    timeStatusText = nVisited > 0 ? 'Book + Evaluate' : 'Book';
  } else if (nVisited > 0n) {
    timeStatusText = `Time: ${seconds.toFixed(1)} sec`;
  }
  document.getElementById('stat-time').innerText = timeStatusText;

  // 6. Continue evaluating if not finished
  if (!finished) {
    Module.evaluate(enginePtr);
  }
};

// 3. Initialize Engine
Module.onRuntimeInitialized = () => {
  statusEl.innerText = "Engine Ready";

  // Initialize your engine using the uintptr_t wrapper approach
  enginePtr = Module.mainInit(
      "assets/pattern_evaluator.dat", "assets/book", "assets/archive",
      "", "assets/xot/openingssmall.txt", "assets/xot/openingslarge.txt"
  );

  Module.newGame(enginePtr);
  Module.evaluate(enginePtr);
};
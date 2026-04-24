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
import sensei_wasm_generated from './sensei_wasm_generated.js';

export class SenseiAPI {
  constructor() {
    this._engine = null;
    this._enginePtr = null;

    // The Proxy intercepts property access
    return new Proxy(this, {
      get: (target, prop) => {
        // 1. If the property exists in this class (like 'init'), return it
        if (prop in target) return target[prop];

        if (!target._engine || !target._enginePtr) return;
        // 2. Otherwise, check if it's a function on the WASM engine
        const engineFunc = target._engine?.[prop];
        if (typeof engineFunc === 'function') {
          // Return a wrapper that automatically injects the pointer
          return (...args) => engineFunc(target._enginePtr, ...args);
        }
        return undefined;
      }
    });
  }

  async init(onSetBoard, onUpdateAnnotations, onUpdateTimers) {
    // The factory returns the module instance directly when using EXPORT_ES6
    this._engine = await sensei_wasm_generated();

    this._enginePtr = this._engine.mainInit(
      "assets/pattern_evaluator.dat",
      "assets/book",
      "assets/archive",
      "",
      "assets/xot/openingssmall.txt",
      "assets/xot/openingslarge.txt"
    );

    this._engine.newGame(this._enginePtr);
    this._engine.evaluate(this._enginePtr);
  }
}
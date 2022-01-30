// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package ui;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import java.util.Arrays;

import bitpattern.BitPattern;
import board.Board;
import constants.Constants;
import evaluateposition.StoredBoard;
import helpers.Utils;

public class BoardView extends View {
  MobileUI main;
  private final Paint fillGreen;
  private final Paint drawBlack;
  private final Paint fillBlack;
  private final Paint fillWhite;
  private int cellSize;
  private Board board = new Board();
  private boolean blackTurn = false;
  private CaseAnnotations[][] annotations;
  private final RectF[][] caseBorders;

  public BoardView(Context context, AttributeSet attrs) {
    super(context, attrs);
    this.main = (MobileUI) context;
    fillGreen = new Paint();
    fillGreen.setColor(Color.GREEN);
    fillGreen.setStyle(Paint.Style.FILL);
    drawBlack = new Paint();
    drawBlack.setColor(Color.BLACK);
    drawBlack.setStyle(Paint.Style.STROKE);
    fillBlack = new Paint();
    fillBlack.setColor(Color.BLACK);
    fillBlack.setStyle(Paint.Style.FILL);
    fillWhite = new Paint();
    fillWhite.setColor(Color.WHITE);
    fillWhite.setStyle(Paint.Style.FILL);
    caseBorders = new RectF[8][8];
    for (int i = 0; i < 8; ++i) {
      Arrays.fill(caseBorders[i], new RectF());
    }
    setOnTouchListener((v, event) -> {
      int move = 8 * (7 - (int) (event.getX() / cellSize)) + (7-(int) (event.getY() / cellSize));
      main.getMove(move);
      v.performClick();
      return true;
    });
    resetAnnotations();
  }

  public void setAnnotations(CaseAnnotations annotations, int move) {
    this.annotations[BitPattern.getX(move)][BitPattern.getY(move)] = annotations;
  }

  public void resetAnnotations() {
    this.annotations = new CaseAnnotations[8][8];
  }

  @Override
  protected void onDraw(Canvas canvas) {
    cellSize = getWidth() / 8;
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        RectF rect = caseBorders[i][j];
        rect.set(i * cellSize, j * cellSize,
            (i+1) * cellSize - 1, (j+1) * cellSize - 1);
        canvas.drawRect(rect, fillGreen);
        canvas.drawRect(rect, drawBlack);
        switch (board.getCase(i, j, blackTurn)) {
          case 'X':
            canvas.drawOval(rect, fillBlack);
            break;
          case 'O':
            canvas.drawOval(rect, fillWhite);
            canvas.drawOval(rect, drawBlack);
            break;
        }
      }
    }
    drawAnnotations(canvas);
  }


  @Override
  public void onMeasure(int widthMeasureSpec, int heightMeasureSpec){
    super.onMeasure(widthMeasureSpec, heightMeasureSpec);

    int h = this.getMeasuredHeight();
    int w = this.getMeasuredWidth();

    Log.d("Board", "height " + h + ", width: " + w);

    setMeasuredDimension(w, w);
  }

  public void drawAnnotations(Canvas canvas) {
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        drawAnnotation(canvas, this.annotations[i][j], i * cellSize,
            j * cellSize);
      }
    }
  }

  public void drawAnnotation(Canvas canvas, CaseAnnotations annotation, int x, int y) {
    if (annotation == null || annotation.storedBoard == null) {
      return;
    }
    StoredBoard storedBoard = annotation.storedBoard;
    int lower = storedBoard.getPercentileLower(Constants.ZERO_PERC_FOR_WEAK);
    int upper = storedBoard.getPercentileUpper(Constants.ZERO_PERC_FOR_WEAK);
    String[] rows = {
        String.format(storedBoard.getLower() == storedBoard.getUpper() ? "%+.0f" : "%+.2f", -storedBoard.getEval() / 100.0),
        Utils.prettyPrintDouble(storedBoard.getDescendants()),
        lower == upper ?
            Utils.prettyPrintDouble(storedBoard.proofNumber(lower-100)) + " " +
                Utils.prettyPrintDouble(storedBoard.disproofNumber(lower+100)) :
            ("[" + (-lower/100) + ", " + (-upper/100) + "]")
    };

    Paint paint = new Paint();
    paint.setColor(annotation.isBestMove ? Color.RED : Color.BLACK);
    paint.setStyle(Paint.Style.FILL);
    paint.setTextSize(cellSize);

    for (int i = 0; i < rows.length; ++i) {
      String row = rows[i];
      int textSize = (i == 0) ? (cellSize / 3) : (cellSize / 5);
      paint.setTextSize(textSize);
      Rect textBounds = new Rect();
      paint.getTextBounds(row, 0, row.length(), textBounds);
      y += textSize;
      canvas.drawText(row, (int) (x + cellSize / 2 - textBounds.exactCenterX()), y, paint);
      y += (int) (textSize * 0.2);
    }
  }

  public void invalidate() {
    main.runOnUiThread(BoardView.super::invalidate);
  }
  public void setCases(Board board, boolean blackTurn) {
    this.board = board.deepCopy();
    this.blackTurn = blackTurn;
    this.invalidate();
  }
}

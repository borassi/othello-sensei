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
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;

import bitpattern.PositionIJ;
import board.Board;
import evaluateposition.StoredBoard;
import helpers.Utils;
import main.Main;
import ui.CaseAnnotations;

public class BoardView extends View {
  MobileUI main;
  private Canvas canvas;
  private Paint fillGreen;
  private Paint drawBlack;
  private Paint fillBlack;
  private Paint fillRed;
  private Paint fillWhite;
  private Bitmap bitmap;
  private ImageView imageView;
  private int cellSize;
  private Board board = new Board();
  private boolean blackTurn = false;
  private CaseAnnotations annotations[][];

  private OnTouchListener clickListener = new View.OnTouchListener() {
    @Override
    public boolean onTouch(View v, MotionEvent event) {
      PositionIJ move = new PositionIJ((int) (event.getX() / cellSize), (int) (event.getY() / cellSize));
      main.getMove(move);
      return true;
    }
  };

  public void setAnnotations(CaseAnnotations annotations, PositionIJ position) {
    this.annotations[position.i][position.j] = annotations;
  }

  public void resetAnnotations() {
    this.annotations = new CaseAnnotations[8][8];
  }

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
    this.setOnTouchListener(clickListener);
    resetAnnotations();
  }

  @Override
  protected void onDraw(Canvas canvas)
  {
    this.canvas = canvas;

    cellSize = getWidth() / 8;
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        RectF rect = new RectF(i * cellSize, j * cellSize,
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
    if (annotation == null) {
      return;
    }
    StoredBoard storedBoard = annotation.storedBoard;
    String rows[] = {
        String.format(storedBoard.getLower() == storedBoard.getUpper() ? "%+.0f" : "%+.2f", -storedBoard.getEval() / 100.0),
//                String.format("%s %s", Utils.prettyPrintDouble(annotation.pro), Utils.prettyPrintDouble(annotation.costUntilLeafDefense)),
        Utils.prettyPrintDouble(storedBoard.getDescendants()),
        Utils.prettyPrintDouble(storedBoard.getProofNumberGreaterEqual()) + " " + Utils.prettyPrintDouble(storedBoard.getDisproofNumberStrictlyGreater())
    };

    Paint paint = new Paint();
    paint.setColor(annotation.isBestMove ? Color.RED : Color.BLACK);
    paint.setStyle(Paint.Style.FILL);
    paint.setTextSize(cellSize);

    for (int i = 0; i < rows.length; ++i) {
      String row = rows[i];
      paint.setTextSize((i == 0) ? (int) (cellSize / 3) : (int) (cellSize / 6));

      Rect textBounds = new Rect();
      paint.getTextBounds(row, 0, row.length(), textBounds);
      y += (int) (textBounds.height() * 1.5);
      canvas.drawText(row, (int) (x + cellSize / 2 - textBounds.exactCenterX()), y, paint);
    }
  }

  public void setCases(Board board, boolean blackTurn) {
    this.board = board.deepCopy();
    this.blackTurn = blackTurn;
    this.invalidate();
  }
}

// Copyright 2019 Google LLC
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

import bitpattern.PositionIJ;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JPanel;

public class Case extends JPanel implements MouseListener, ComponentListener {
  private static final long serialVersionUID = 1L;
  
  /**
   * The size of the case.
   */
  private int size = 100;
  /**
   * The state of this case: 'X' for black, 'O' for white, '-' for empty.
   */
  private char state;
  /**
   * The UI this case belongs to. Used to propagate move events.
   */
  private final DesktopUI ui;
  /**
   * The row and the column (from 0 to 7).
   */
  private final PositionIJ ij;
  /**
   * The annotations to be printed in this case (first line).
   */
  private String annotations = "";
  /**
   * The color of the annotations.
   */
  private Color annotationsColor = Color.BLACK;
  
  /**
   * Constructor.
   * @param ui the UI that uses this case
   * @param ij: the row and the column (from 0 to 7).
   */
  public Case(DesktopUI ui, PositionIJ ij) {
    this.ij = ij;
    this.ui = ui;
    super.addMouseListener(this);
    setState('-');
  }
  
  /**
   * Sets the annotations of this case (e.g., the evaluation).
   * @param annotations the new annotations.
   */
  public void setAnnotations(String annotations) {
    this.annotations = annotations;
  }
  
  /**
   * Gets the annotations of this case (e.g., the evaluation).
   * @returns the annotations.
   */
  public String getAnnotations() {
    return annotations;
  }
  
  /**
   * Sets the color of this case (e.g., the evaluation).
   * @param color The new color
   */
  public void setAnnotationsColor(Color color) {
    this.annotationsColor = color;
  }

  @Override
  public void paintComponent(Graphics g) {
    this.size = this.getHeight() + 1;
    Font font = new Font("Serif", Font.PLAIN, size / 9);
    g.setFont(font); 
    
    g.setColor(Color.BLACK);
    g.fillRect(0, 0, size, size);
    g.setColor(new Color(50, 200, 50));
    g.fillRect(1, 1, size - 2, size - 2);

    FontMetrics metrics = g.getFontMetrics();
    g.setColor(annotationsColor);
    
    String annotationLines[] = annotations.split("\n");
    
    int position = Math.max(metrics.getHeight(),
      size / 2 - (annotationLines.length - 2) * metrics.getHeight() / 2);

    for (String annotationLine : annotationLines) {
      g.drawString(
        annotationLine, (size - metrics.stringWidth(annotationLine)) / 2, 
              position);
      position += g.getFontMetrics().getHeight();
    }
    
    switch (state) {
      case 'X':
        g.setColor(Color.BLACK);
        break;
      case 'O':
        g.setColor(Color.WHITE);
        break;
      default:
        return;
    }
    g.fillOval(8, 8, size - 16, size - 16);
    g.setColor(Color.BLACK);
    g.drawOval(8, 8, size - 16, size - 16);
  }
  
  /**
   * Sets the state of this case.
   * @param newState the new state: 'X' for black, 'O' for white, '-' for empty.
   */
  public final void setState(char newState) {
    this.state = newState;
  }
  
  /**
   * Sets the state of this case.
   * @param newState the new state: 'X' for black, 'O' for white, '-' for empty.
   */
  public final char getState() {
    return state;
  }

  public void setSize(int size) {
    this.size = size;
  }

  @Override
  public void mousePressed(MouseEvent e) {
    ui.getClick(ij, e);
  }

  @Override
  public void mouseEntered(MouseEvent e) {}

  @Override
  public void mouseExited(MouseEvent e) {}

  @Override
  public void mouseClicked(MouseEvent e) {}

  @Override
  public void mouseReleased(MouseEvent e) {}
  
  @Override
  public void componentResized(ComponentEvent e) {
  }

  @Override
  public void componentMoved(ComponentEvent arg0) {}

  @Override
  public void componentShown(ComponentEvent arg0) {}

  @Override
  public void componentHidden(ComponentEvent arg0) {}
}

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
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.MouseEvent;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.JToolBar;
import javax.swing.SpinnerModel;

import board.Board;
import constants.Constants;
import evaluateposition.StoredBoard;
import helpers.Utils;
import java.util.Arrays;
import javax.swing.Box;
import javax.swing.JLabel;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SpinnerListModel;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import main.Main;

public class DesktopUI extends JFrame implements ComponentListener, UI {

  private static final long serialVersionUID = 1L;
  private Case[][] cases = new Case[8][8];
  private JPanel casesContainer = new JPanel(new GridLayout(0, 8)) {
    /**
     * Needed to silence a warning.
     */
    private static final long serialVersionUID = 1L;

    /**
     * Override the preferred size to return the largest it can, in
     * a square shape.  Must (must, must) be added to a GridBagLayout
     * as the only component (it uses the parent as a guide to size)
     * with no GridBagConstaint (so it is centered).
     */
    @Override
    public final Dimension getPreferredSize() {
      Component c = getParent();
      if (c != null) {
        return new Dimension(c.getHeight(), c.getHeight());
      }
      return new Dimension((int) 800, (int) 800);
    }
  };

  private final JToolBar commands = new JToolBar(null, JToolBar.VERTICAL);
  private final Main main;
  private final JCheckBox playBlackMoves = new JCheckBox("Play black moves");
  private final JCheckBox playWhiteMoves = new JCheckBox("Play white moves");
  private final JCheckBox debugMode = new JCheckBox("Debug mode", true);
  private final JButton newGame = new JButton("New game");
  private final JButton stop = new JButton("Stop");
  private final JButton resetHashMaps = new JButton("Reset hash maps");
  private final JTextField depth;
  private final JSpinner delta;  
  private final JSpinner ffoPositions;
  private final JSpinner lower;
  private final JSpinner upper;
  private final JLabel empties;
  private final JTextArea extras;
  
  public void getClick(PositionIJ ij, MouseEvent e) {
    if (SwingUtilities.isLeftMouseButton(e)) {
      main.play(ij);
    } else if (SwingUtilities.isRightMouseButton(e)) {
      main.undo();
    }
  }
  
  @Override
  public void setAnnotations(CaseAnnotations annotations, PositionIJ ij) {
    if (debugMode.isSelected()) {
      setAnnotationsDebug(annotations, ij);
    } else {
      setAnnotationsLarge(annotations, ij);
    }
  }
  private void setAnnotationsLarge(CaseAnnotations annotations, PositionIJ ij) {
    String annotationsString = String.format("%+.1f", annotations.eval);
    if (annotations.lower == annotations.eval && annotations.eval == annotations.upper) {
      annotationsString = String.format("%+.0f", annotations.eval);
    }
    annotationsString += "\n" + Utils.prettyPrintDouble(annotations.nVisited);
    annotationsString += "\n" + Utils.prettyPrintDouble(annotations.proofNumberCurEval + annotations.disproofNumberCurEval);
    cases[ij.i][ij.j].setAnnotations(annotationsString);
    cases[ij.i][ij.j].setFontSizes(new double[] {0.3, 0.16});
    cases[ij.i][ij.j].setAnnotationsColor(annotations.isBestMove ? new Color(210, 30, 30) : Color.BLACK);
    cases[ij.i][ij.j].repaint();    
  }
  
  private void setAnnotationsDebug(CaseAnnotations annotations, PositionIJ ij) {
    cases[ij.i][ij.j].setFontSizes(new double[] {0.13});
    
    String annotationsString = "";
    if (annotations.eval != Float.NEGATIVE_INFINITY) {
      if (annotations.lower == annotations.eval && annotations.eval == annotations.upper) {
        annotationsString = String.format("%+.0f", annotations.eval);
      } else {
        annotationsString += String.format("%+.2f", annotations.eval);
      }
    }

    if (annotations.lower != Float.NEGATIVE_INFINITY && annotations.upper != Float.NEGATIVE_INFINITY) {
      annotationsString += String.format("\n%.1f  %.1f", annotations.lower, annotations.upper);
    }

    if (annotations.proofNumberCurEval != Double.NEGATIVE_INFINITY && annotations.proofNumberNextEval != Double.NEGATIVE_INFINITY) {
      annotationsString += "\n" + Utils.prettyPrintDouble(annotations.proofNumberCurEval) + " " + Utils.prettyPrintDouble(annotations.proofNumberNextEval);
      annotationsString += "\n" + Utils.prettyPrintDouble(annotations.disproofNumberNextEval) + " " + Utils.prettyPrintDouble(annotations.disproofNumberCurEval);
    }
    if (annotations.nVisited != 0) {
      annotationsString += "\n" + Utils.prettyPrintDouble(annotations.nVisited);
    }
    if (annotations.otherAnnotations != "") {
      annotationsString += "\n" + annotations.otherAnnotations;
    }
    cases[ij.i][ij.j].setAnnotations(annotationsString);
    cases[ij.i][ij.j].setAnnotationsColor(annotations.isBestMove ? Color.RED : Color.BLACK);
    cases[ij.i][ij.j].repaint();    
  }
  
//  @Override
//  public void setBestMove(PositionIJ ij) {
//    cases[ij.i][ij.j].setAnnotationsColor(Color.RED);
//    cases[ij.i][ij.j].update(cases[ij.i][ij.j].getGraphics());
//  }
  private boolean lastBlackTurn = true;
  
  @Override
  public void setCases(Board board, boolean blackTurn) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        if (cases[i][j].getState() == board.getCase(i, j, blackTurn) &&
            "".equals(cases[i][j].getAnnotations())) {
          continue;
        }
        cases[i][j].setState(board.getCase(i, j, blackTurn));
        cases[i][j].setAnnotations("");
        cases[i][j].update(cases[i][j].getGraphics());
      }
    }
    if (lastBlackTurn != blackTurn) {
      lastBlackTurn = blackTurn;
      int lowerValue = (int) lower.getValue();
      lower.setValue(-(int) upper.getValue());
      upper.setValue(-lowerValue);
    }
    empties.setText("Empties: " + board.getEmptySquares());
  }
  
  
  
  public DesktopUI(Main main) {
    super("Othello");
    
    this.main = main;

    setDefaultCloseOperation(EXIT_ON_CLOSE);

    casesContainer.setLayout(new GridLayout(0, 8));

    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        PositionIJ ij = new PositionIJ(i, j);
        cases[i][j] = new Case(this, ij);
        casesContainer.add(cases[i][j]);
      }
    }
    
    JPanel boardConstrain = new JPanel(new GridBagLayout());
    boardConstrain.add(casesContainer);
    add(boardConstrain);

    commands.add(newGame);
    commands.add(playBlackMoves);
    commands.add(playWhiteMoves);
    commands.add(debugMode);
    commands.add(stop);
    commands.add(resetHashMaps);
    
    newGame.addActionListener((ActionEvent e) -> {
      main.newGame();
    });
    
    stop.addActionListener((ActionEvent e) -> {
      main.stop();
    });
    
    resetHashMaps.addActionListener((ActionEvent e) -> {
      main.resetHashMaps();
    });
    depth = new JTextField();
    depth.setText("100000000000000");
    depth.setMaximumSize(new Dimension(Short.MAX_VALUE, 2 * depth.getPreferredSize().height));
    commands.add(depth);
    
    SpinnerModel allowedDeltas = new SpinnerNumberModel(0, 0, 64, 1);
    delta = new JSpinner(allowedDeltas);
    delta.setMaximumSize(new Dimension(Short.MAX_VALUE, 2 * delta.getPreferredSize().height));
    commands.add(delta);
    add(commands, BorderLayout.LINE_END);
    
    SpinnerModel allowedFFOPositions = new SpinnerNumberModel(46, 40, 60, 1);
    ffoPositions = new JSpinner(allowedFFOPositions);
    commands.add(ffoPositions);
    ffoPositions.addChangeListener((ChangeEvent e) -> {
      main.setEndgameBoard((int) ffoPositions.getValue());
    });
    
    SpinnerModel allowedLower = new SpinnerNumberModel(-63, -63, 63, 2);
    SpinnerModel allowedUpper = new SpinnerNumberModel(63, -63, 63, 2);
    lower = new JSpinner(allowedLower);
    upper = new JSpinner(allowedUpper);
    lower.addChangeListener((ChangeEvent e) -> {
      upper.setValue(Math.max((int) upper.getValue(), (int) lower.getValue()));
    });
    upper.addChangeListener((ChangeEvent e) -> {
      lower.setValue(Math.min((int) upper.getValue(), (int) lower.getValue()));
    });
    commands.add(lower);
    commands.add(upper);

    empties = new JLabel();
    commands.add(empties);
    extras = new JTextArea();
    commands.add(extras);
    
    setSize(1200, 800);

    setVisible(true);
  }

  @Override
  public boolean playBlackMoves() {
    return playBlackMoves.isSelected();
  }

  @Override
  public boolean playWhiteMoves() {
    return playWhiteMoves.isSelected();
  }

  @Override
  public long depth() {
    return Long.parseLong((String) depth.getText());
  }
  
  @Override
  public void componentHidden(ComponentEvent arg0) {}

  @Override
  public void componentMoved(ComponentEvent e) {}

  @Override
  public void componentResized(ComponentEvent e) {
    casesContainer.setSize(this.getHeight() / 2, this.getHeight() / 2);
    casesContainer.setVisible(true);
  }

  @Override
  public void componentShown(ComponentEvent e) {}

  @Override
  public int delta() {
    return (int) delta.getValue();
  }

  @Override
  public void setExtras(StoredBoard firstPosition, double milliseconds) {
    Runnable tmp = new Runnable() {
      public void run() {
        String text = 
            "Positions: " + Utils.prettyPrintDouble(firstPosition.getDescendants()) + "\n" +
            "Positions/s: " + Utils.prettyPrintDouble(firstPosition.getDescendants() * 1000 / milliseconds) + "\n" +
            "Missing: " + Utils.prettyPrintDouble(firstPosition.getProofNumberGreaterEqual()) + " + " +
                Utils.prettyPrintDouble(firstPosition.getDisproofNumberStrictlyGreater());

        if (Constants.FIND_BEST_PROOF_AFTER_EVAL) {
          text += "\nProof: " +
              Utils.prettyPrintDouble(firstPosition.getDescendants())
              + "\nBest proof: " +
              Utils.prettyPrintDouble(firstPosition.extraInfo.minProofGreaterEqual + firstPosition.extraInfo.minDisproofStrictlyGreater)
              + " = " + 
              Utils.prettyPrintDouble(firstPosition.extraInfo.minProofGreaterEqual) + " + " + 
              Utils.prettyPrintDouble(firstPosition.extraInfo.minDisproofStrictlyGreater);
        }
        extras.setText(text);
      }
    };
    SwingUtilities.invokeLater(tmp);
  }

  @Override
  public int lower() {
    return (int) lower.getValue() * 100;
  }

  @Override
  public int upper() {
    return (int) upper.getValue() * 100;
  }
}

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
import java.awt.event.ActionListener;
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
import java.util.Arrays;
import javax.swing.SpinnerListModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import main.Main;

public class UI extends JFrame implements ComponentListener {
  
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
//      Dimension d = super.getPreferredSize();
//      Dimension prefSize = null;
//      Component c = getParent();
//      if (c == null) {
//          prefSize = new Dimension(
//                  (int)d.getWidth(), (int)d.getHeight());
//      } else if (c.getWidth() > d.getWidth() &&
//          c.getHeight() > d.getHeight()) {
//          prefSize = c.getSize();
//      } else {
//          prefSize = d;
//      }
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
  private final JButton newGame = new JButton("New game");
  private final JButton retrain = new JButton("Retrain");
  private final JButton selftrain = new JButton("Self-train");
  private final JButton improveDataset = new JButton("Improve dataset");
  private final JSpinner spinner;
  
  public void getClick(PositionIJ ij, MouseEvent e) {
    main.getClick(ij, e);
  }
  
  public void setAnnotations(String annotations, PositionIJ ij) {
    cases[ij.i][ij.j].setAnnotations(annotations);
    cases[ij.i][ij.j].repaint();
  }
  
  public void setAnnotationsColor(Color color, PositionIJ ij) {
    cases[ij.i][ij.j].setAnnotationsColor(color);
    cases[ij.i][ij.j].update(cases[ij.i][ij.j].getGraphics());
  }
  
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
  }
  
  
  
  public UI(Main main) {
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
    commands.add(retrain);
    commands.add(selftrain);
    commands.add(improveDataset);
    
    newGame.addActionListener((ActionEvent e) -> {
      main.newGame();
    });
    
    improveDataset.addActionListener((ActionEvent e) -> {
      main.improveDataset();
    });
    
    selftrain.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        main.selfTrain();
      }
    });
    
    retrain.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        main.retrain();
      }
    });
    
    SpinnerModel depth = new SpinnerListModel(Arrays.asList(new String[] 
      {"0", "10", "100", "1000", "2000", "10000", "20000", "50000", "100000", "200000", "500000", "1000000", "2000000", "4000000"}));
    depth.setValue("1000");
    spinner = new JSpinner(depth);
    
    spinner.addChangeListener(new ChangeListener() {
      @Override
      public void stateChanged(ChangeEvent e) {
        System.out.println(spinner.getValue());
        main.changeDepth(depth());
      }
    });
    commands.add(spinner);
    add(commands, BorderLayout.LINE_END);

    setSize(1200, 800);

    setVisible(true);
  }

  public boolean playBlackMoves() {
    return playBlackMoves.isSelected();
  }

  public boolean playWhiteMoves() {
    return playWhiteMoves.isSelected();
  }

  public int depth() {
    return Integer.parseInt((String) spinner.getValue());
  }
  
  @Override
  public void componentHidden(ComponentEvent arg0) {}

  @Override
  public void componentMoved(ComponentEvent e) {}

  @Override
  public void componentResized(ComponentEvent e) {
    System.out.println(this.getHeight() + " " + this.getWidth());
    casesContainer.setSize(this.getHeight() / 2, this.getHeight() / 2);
    casesContainer.setVisible(true);
  }

  @Override
  public void componentShown(ComponentEvent e) {
    // TODO Auto-generated method stub
    
  }
}

// Copyright 2022 Google LLC
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

package ui_desktop;

import bitpattern.BitPattern;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Locale;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.JToolBar;
import javax.swing.SpinnerListModel;
import javax.swing.SpinnerModel;

import board.Board;
import constants.Constants;
import evaluateposition.TreeNodeInterface;
import helpers.FileAccessor;
import helpers.Utils;

import javax.swing.JLabel;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;

import jni.JNI;
import main.Main;
import main.UI;
import thor.Game;

public class DesktopUI extends JFrame implements ComponentListener, UI {

  private static final long serialVersionUID = 1L;
  private final Case[][] cases = new Case[8][8];
  private final JPanel casesContainer = new JPanel(new GridLayout(0, 8)) {
    /**
     * Needed to silence a warning.
     */
    private static final long serialVersionUID = 1L;

    /**
     * Override the preferred size to return the largest it can, in
     * a square shape.  Must (must, must) be added to a GridBagLayout
     * as the only component (it uses the parent as a guide to size)
     * with no GridBagConstraint (so it is centered).
     */
    @Override
    public Dimension getPreferredSize() {
      Component c = getParent();
      if (c != null) {
        return new Dimension(c.getHeight(), c.getHeight());
      }
      return new Dimension(800, 800);
    }
  };
  private final Main main;
  private boolean lastBlackTurn = true;

  private final JCheckBox playBlackMoves = new JCheckBox("Play black moves");
  private final JCheckBox playWhiteMoves = new JCheckBox("Play white moves");
  private final JCheckBox debugMode = new JCheckBox("Debug mode", true);
  private final JCheckBox active = new JCheckBox("Active", true);
  private final JTextField depth;
  private final JTextField delta;
  private final JSpinner ffoPositions;
  private final JSpinner lower;
  private final JSpinner upper;
  private final JLabel empties;
  private final JTextArea extras;
  private final JSpinner thorWhite;
  private final JTextArea thorWhiteAll;
  private final JSpinner thorBlack;
  private final JTextArea thorBlackAll;
  private final JTextArea extrasPosition;
  private final JButton lookupThor;
  private final JCheckBox thorActive;
  private final ThorGamesWindow thorGamesWindow;


  public DesktopUI() {
    super("Othello");

    addWindowListener(new java.awt.event.WindowAdapter() {
      @Override
      public void windowClosing(java.awt.event.WindowEvent windowEvent) {
        main.stop();
      }
    });
    setDefaultCloseOperation(EXIT_ON_CLOSE);

    casesContainer.setLayout(new GridLayout(0, 8));

    for (int i = 0; i < 64; i++) {
      int x = BitPattern.getX(i);
      int y = BitPattern.getY(i);

      cases[x][y] = new Case(this, i);
    }
    for (int x = 0; x < 8; ++x) {
      for (int y = 0; y < 8; ++y) {
        casesContainer.add(cases[x][y]);
      }
    }

    JPanel boardConstrain = new JPanel(new GridBagLayout());
    boardConstrain.add(casesContainer);
    add(boardConstrain);
    JToolBar commands = new JToolBar(null, JToolBar.VERTICAL);
    JButton newGame = new JButton("New game");
    JButton stop = new JButton("Stop");
    JButton resetHashMaps = new JButton("Reset hash maps");
    JButton copy = new JButton("Copy");
    commands.add(newGame);
    commands.add(playBlackMoves);
    commands.add(playWhiteMoves);
    commands.add(debugMode);
    commands.add(active);
    commands.add(stop);
    commands.add(resetHashMaps);
    commands.add(copy);

    main = new Main(this);

    newGame.addActionListener((ActionEvent e) -> main.newGame());

    stop.addActionListener((ActionEvent e) -> main.stop());

    resetHashMaps.addActionListener((ActionEvent e) -> main.resetHashMaps());
    copy.addActionListener((ActionEvent e) -> main.copy());
    depth = new JTextField();
    depth.setText("10000000000");
    depth.setMaximumSize(new Dimension(Short.MAX_VALUE, 2 * depth.getPreferredSize().height));
    commands.add(depth);

    delta = new JTextField();
    delta.setText("0");
    delta.setMaximumSize(new Dimension(Short.MAX_VALUE, 2 * delta.getPreferredSize().height));
    commands.add(delta);
    add(commands, BorderLayout.LINE_END);

    SpinnerModel allowedFFOPositions = new SpinnerNumberModel(40, 40, 61, 1);
    ffoPositions = new JSpinner(allowedFFOPositions);
    commands.add(ffoPositions);
    ffoPositions.addChangeListener((ChangeEvent e) -> main.setEndgameBoard((int) ffoPositions.getValue()));

    SpinnerModel allowedLower = new SpinnerNumberModel(-63, -63, 63, 2);
    SpinnerModel allowedUpper = new SpinnerNumberModel(63, -63, 63, 2);
    lower = new JSpinner(allowedLower);
    upper = new JSpinner(allowedUpper);
    lower.addChangeListener((ChangeEvent e) -> upper.setValue(Math.max((int) upper.getValue(), (int) lower.getValue())));
    upper.addChangeListener((ChangeEvent e) -> lower.setValue(Math.min((int) upper.getValue(), (int) lower.getValue())));
    commands.add(lower);
    commands.add(upper);

    empties = new JLabel();
    commands.add(empties);
    extras = new JTextArea();
    extras.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 14));
    commands.add(extras);
    commands.add(new JLabel("Current position eval:"));
    extrasPosition = new JTextArea();
    extrasPosition.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 14));
    commands.add(extrasPosition);

    JLabel thorBlackLabel = new JLabel("Thor black player:");
    commands.add(thorBlackLabel);
    thorBlackAll = new JTextArea();
    thorBlackAll.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 14));
    thorBlackAll.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent me) {
        thorBlackAll.setText("");
      }
    });

    SpinnerModel thorBlackModel = new SpinnerListModel(new ArrayList<>(main.getThorPlayers()));
    thorBlack = new JSpinner(thorBlackModel);
    thorBlack.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 14));
    thorBlack.addChangeListener((ChangeEvent e) -> thorBlackAll.setText(thorBlackAll.getText() + "\n" + thorBlack.getValue()));
    commands.add(thorBlack);
    commands.add(thorBlackAll);

    JLabel thorWhiteLabel = new JLabel("Thor white player:");
    commands.add(thorWhiteLabel);
    thorWhiteAll = new JTextArea();
    thorWhiteAll.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 14));
    thorWhiteAll.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent me) {
        thorWhiteAll.setText("");
      }
    });

    SpinnerModel thorWhiteModel = new SpinnerListModel(new ArrayList<>(main.getThorPlayers()));
    thorWhite = new JSpinner(thorWhiteModel);
    thorWhite.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 14));
    thorWhite.addChangeListener((ChangeEvent e) -> thorWhiteAll.setText(thorWhiteAll.getText() + "\n" + thorWhite.getValue()));
    commands.add(thorWhite);
    commands.add(thorWhiteAll);
    lookupThor = new JButton("Lookup Thor");
    lookupThor.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent me) {
        HashSet<String> black = new HashSet<>(Arrays.asList(thorBlackAll.getText().split("\n")));
        black.remove("");
        HashSet<String> white = new HashSet<>(Arrays.asList(thorWhiteAll.getText().split("\n")));
        white.remove("");
        main.thorLookup(black, white, new HashSet<>());
        thorActive.setSelected(true);
      }
    });
    thorGamesWindow = new ThorGamesWindow();
    commands.add(lookupThor);
    thorActive = new JCheckBox("Thor mode");
    thorActive.addChangeListener((ChangeEvent e) -> thorGamesWindow.setVisible(thorActive.isSelected()));
    commands.add(thorActive);

    setSize(1200, 800);

    setVisible(true);
    main.newGame();
  }

  public void getClick(int move, MouseEvent e) {
    if (SwingUtilities.isLeftMouseButton(e)) {
      main.play(move);
    } else if (SwingUtilities.isRightMouseButton(e)) {
      main.undo();
    }
  }

  @Override
  public void setAnnotations(CaseAnnotations annotations, int move) {
    if (debugMode.isSelected()) {
      setAnnotationsDebug(annotations, move);
    } else {
      setAnnotationsLarge(annotations, move);
    }
  }
  private void setAnnotationsLarge(CaseAnnotations annotations, int move) {
    TreeNodeInterface treeNode = annotations.treeNode;
    int lower = treeNode.getPercentileLower(Constants.PROB_INCREASE_WEAK_EVAL);
    int upper = treeNode.getPercentileUpper(Constants.PROB_INCREASE_WEAK_EVAL);
    String rows;
    String evalFormatter;
    if (treeNode.isSolved()) {
      evalFormatter = "%+.0f";
    } else if (treeNode.isPartiallySolved()) {
      evalFormatter = "%+.1f";
    } else {
      evalFormatter = "%+.2f";
    }
    if (!thorActive.isSelected() || annotations.thorGames < 0) {
      rows =
          String.format(evalFormatter, -treeNode.getEval() / 100.0) + "\n" +
          Utils.prettyPrintDouble(treeNode.getDescendants()) + "\n" + (
              lower == upper ?
                  Utils.prettyPrintDouble(treeNode.proofNumber(lower-100)) + " " +
                      Utils.prettyPrintDouble(treeNode.disproofNumber(lower+100)) :
                  ("[" + (-upper/100) + ", " + (-lower/100) + "]")
          );
    } else {
      rows = (annotations.thorGames == 0 ? "" : annotations.thorGames) + "\n" +
          String.format(evalFormatter, -treeNode.getEval() / 100.0) + "\n"
          + Utils.prettyPrintDouble(treeNode.getDescendants());
    }

    int x = BitPattern.getX(move);
    int y = BitPattern.getY(move);
    cases[x][y].setFontSizes(new double[] {0.25, 0.16});
    cases[x][y].setAnnotations(rows);
    cases[x][y].setAnnotationsColor(annotations.isBestMove ? new Color(210, 30, 30) : Color.BLACK);
    cases[x][y].repaint();
  }

  private void setAnnotationsDebug(CaseAnnotations annotations, int move) {
    TreeNodeInterface board = annotations.treeNode;
    if (board == null) {
      return;
    }
    StringBuilder rows;

    String formatter = "%+.2f ";
    if (board.isSolved()) {
      formatter = "%+.0f ";
    } else if (board.isPartiallySolved()) {
      formatter = "%+.1f ";
    }
    String evalStr = String.format(Locale.US, formatter, -board.getEval() / 100.0);
    if (board.getProb(board.getWeakLower()) > 1 - Constants.PROB_INCREASE_WEAK_EVAL) {
      if (board.getProb(board.getWeakUpper()) < Constants.PROB_INCREASE_WEAK_EVAL) {
        rows = new StringBuilder(evalStr);
      } else {
        rows = new StringBuilder("≤" + evalStr);
      }
    } else {
      if (board.getProb(board.getWeakUpper()) < Constants.PROB_INCREASE_WEAK_EVAL) {
        rows = new StringBuilder("≥" + evalStr);
      } else {
        rows = new StringBuilder("?");
      }
    }
    rows.append(" ").append(Utils.prettyPrintDouble(board.getDescendants()));

    for (int evalGoal = board.getWeakUpper(); evalGoal >= board.getWeakLower(); evalGoal -= 200) {
      float prob = 1 - board.getProb(evalGoal);
      rows.append(String.format(Locale.US, "\n%+3d %3.0f%% ", -evalGoal / 100, (float) Math.round((0.0049 + (1-2*0.0049) * prob) * 100)));

      if (prob == 1) {
        rows.append(Utils.prettyPrintDouble(board.disproofNumber(evalGoal)));
      } else if (prob == 0) {
        rows.append(Utils.prettyPrintDouble(board.proofNumber(evalGoal)));
      } else {
        rows.append(Utils.prettyPrintDouble(annotations.father.childLogDerivative(board, -evalGoal)));
      }
    }

    int x = BitPattern.getX(move);
    int y = BitPattern.getY(move);
    cases[x][y].setFontSizes(new double[] {0.125});
    cases[x][y].setAnnotations(rows.toString());
    cases[x][y].setAnnotationsColor(annotations.isBestMove ? Color.RED : Color.BLACK);
  }

  public void repaint() {
    for (Case[] row : cases) {
      for (Case c : row) {
        c.repaint();
      }
    }
  }


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

  @Override
  public boolean playBlackMoves() {
    return playBlackMoves.isSelected();
  }

  @Override
  public boolean playWhiteMoves() {
    return playWhiteMoves.isSelected();
  }

  @Override
  public long maxVisited() {
    try {
      return Long.parseLong(depth.getText());
    } catch (NumberFormatException e) {
      return 0;
    }
  }

  @Override
  public void componentHidden(ComponentEvent arg0) {}

  @Override
  public void componentMoved(ComponentEvent e) {}

  @Override
  public void componentResized(ComponentEvent e) {
    casesContainer.setSize(this.getHeight() / 2, this.getHeight() / 2);
    casesContainer.setVisible(true);
    thorGamesWindow.setLocation(this.getX() + this.getWidth(), this.getY());
  }

  @Override
  public void componentShown(ComponentEvent e) {}

  @Override
  public double delta() {
    return Double.parseDouble(delta.getText());
  }

  @Override
  public boolean wantThorGames() {
    return this.thorActive.isSelected();
  }

  @Override
  public void setThorGames(Board b, ArrayList<Game> thorGames) {
    thorGamesWindow.setGames(b, thorGames);
  }

  @Override
  public FileAccessor fileAccessor() {
    return new FileAccessor();
  }

  @Override
  public boolean active() {
    return active.isSelected();
  }

  public static short roundEval(int eval) {
    return (short) Math.max(-6300, Math.min(6300, ((eval + 20000) / 200) * 200 - 19900));
  }

  @Override
  public void setExtras(long nVisited, double milliseconds, CaseAnnotations annotations) {
    String text =
        "Positions: " + Utils.prettyPrintDouble(nVisited) + "\n" +
            "Positions/s: " + Utils.prettyPrintDouble(nVisited * 1000 / milliseconds) + "\n";
    SwingUtilities.invokeLater(() -> extras.setText(text));
    if (annotations == null) {
      return;
    }
    TreeNodeInterface board = annotations.treeNode;
    StringBuilder firstPositionText = new StringBuilder(board.getEval() + " " + board.getLower() + " " + board.getUpper() + "\n");

    int eval = board.getEval();
    for (int evalGoal = roundEval(eval + 1200); evalGoal >= roundEval(eval - 1200); evalGoal -= 200) {
      firstPositionText.append(String.format(Locale.US, "\n%+3d %3.0f%% %4s %4s %4s", evalGoal / 100, (board.getProb(evalGoal)) * 100,
          Utils.prettyPrintDouble(board.maxLogDerivative(evalGoal)),
          Utils.prettyPrintDouble(board.proofNumber(evalGoal)), Utils.prettyPrintDouble(board.disproofNumber(evalGoal))));
    }

    String tmp = firstPositionText.toString();
    SwingUtilities.invokeLater(() -> extrasPosition.setText(tmp));
  }

  @Override
  public int lower() {
    return (int) lower.getValue() * 100;
  }

  @Override
  public int upper() {
    return (int) upper.getValue() * 100;
  }

  public static void main(String[] args) {
    new DesktopUI();
  }
}
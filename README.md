This is not an officially supported Google product.

For more information on this work, contact borassi@google.com.

# Othello Sensei

This repository contains a program to play the
[Othello/Reversi](https://en.wikipedia.org/wiki/Reversi) board game.

## How to run the program

TBD

## How to change the source code

TBD

## How does the program work

### Evaluating at depth 1

Similarly to existing programs, we use a
[pattern-based evaluation](https://skatgame.net/mburo/log.html), but we use
different subsets of the board to define patterns:

*	3x3 corners;
*	5x2 corners;
*	main diagonals;
*	some "edge-based" pattern: a list of patterns describing the edge, of which
  we always choose the outermost.

Then, we use linear regression to evaluate a position using these features.

### Evaluating at larger depths

Differently from previous programs, this program uses a variation of Monte
Carlo Tree Search algorithm. We keep in memory a tree of possible
configurations (actually, a DAG, because the same configuration can occur with
different sequences), and we update the evaluations of this tree. At each step,
we choose a "promising" leaf of the tree and we expand it.

# Othello Sensei

A strong, responsive, and user-friendly [Othello](https://en.wikipedia.org/wiki/Reversi) engine
for advanced players.

## Installing the app

### Android

Download it from the Play Store
([link](https://play.google.com/store/apps/details?id=com.othellosensei.app)).

### iOS

Coming soon.

### Windows

Download it from the Microsoft Store
([link](https://apps.microsoft.com/detail/9n54cs9f3jgp?hl=en-us&gl=US)).

### Mac

#### Using the app store

Coming soon.

#### Using the dmg file

1. Open this [link](https://github.com/borassi/othello-sensei/blob/main/Othello%20Sensei.dmg).
2. Click cmd+shift+S at the same time or click on "..." > "Download" (it might take a few seconds
   before the download actually starts).
3. Install the app from the `.dmg` file, following these
   [instructions](https://support.apple.com/en-gb/guide/mac-help/mh40616/mac) to ignore the
   unidentified developer error.

WARNING: Select the right operating system in the webpage with the instructions to ignore the
unidentified developer error.

WARNING: You must ignore the error TWICE, once to open the .dmg, once to open the .app file from the
applications.

### Linux

1. Install Snap if it's not already installed
   ([instructions](https://snapcraft.io/docs/installing-snapd)).
2. Run in a terminal: `sudo snap install othello-sensei`.

## After the installation

1. Download the latest book from the menu.
2. Download the latest archive from the menu.
3. Open the settings and customize the app as you prefer. Examples:
   - Reduce or increase margins.
   - Round the evaluations.
   - Hide extra evaluation information.
   - Enable undo or redo when clicking on illegal moves.

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

Sensei uses a new tree search algorithm, developed by myself and currently not published (I might
publish it in the future, but I currently don't have enough time to write a paper). The algorithm
provides the following advantages over standard alpha-beta pruning:

- Provides continuous updates to the value of a position (while alpha-beta can update the value only
  after finishing the search at a given depth).
- Estimates how long it takes before solving the position.
- Provides a natural extension to expand the most "interesting" positions in an opening book.

## Contributing to the app

At the moment, the code is not in a state which allows multiple people to collaborate. If you want
to contribute, please contact michele.borassi@gmail.com and we can discuss.

## Planned features

I'm considering adding the following features in the future (but I cannot guarantee a timeline):

- Download smaller opening books, for phones with little storage.
- Save and load games.
- Better support for the archive (click on a game to play a move, winning percentages, etc).
- Support for XOT.
- Play against the engine (maybe a weaker version).

If one of these features is really important for you, please write an email to
michele.borassi@gmail.com to help me prioritize. Also, if you want a different feature, please write
me and I'll consider it!

## FAQ

### Is the app free?

Yes. Every feature in the app is and will always be free. There are no ads, and there never will be.

### Can I make a donation?

Thank you very much for thinking of making a donation!

However, this app is my pet project, and I don't want to earn money from it. But I'd be really happy
if you could make a donation to the World Othello Federation
([website](https://www.worldothello.org)) and cc me (michele.borassi@gmail.com).

### Can I make the analysis faster?

Yes, edit the setting "Seconds/positions in game analysis" to less than 1 (you can open the
settings from the three dots menu in the top right corner). The values in the analysis will be less
precise, though.

### Can I make the values in the analysis more precise?

Yes, edit the setting "Seconds/positions in game analysis" to more than 1 (you can open the
settings from the three dots menu in the top right corner). The analysis will take more time,
though.

### The app consumes a lot of battery on my phone. What can I do?

Evaluating a position is hard and costs a lot of energy, which translates to high battery
consumption. You can make the program use less energy by reducing the "Number of threads" in the
settings (but the evaluation will be slower).

### The fan on my computer is very loud when using Sensei. What can I do?

Evaluating a position is hard and costs a lot of energy, which increases the CPU temperature and
starts the fan. You can make the program use less CPU by reducing the "Number of threads" in the
settings (but the evaluation will be slower).

### Can we increase the font size?

You can enlarge the evaluations by enabling the setting "Round evaluations" and disabling the
setting "Show extra data in evaluate mode". You can also make the board and text slightly larger by
setting "Margin size" to "None".

### The undo and redo buttons are too small. What can I do?

There are various options:
- Undo and redo by clicking on the number of disks (already enabled).
- Undo and redo by clicking on illegal moves (you need to enable it in the settings).
- Change setting "Controls position" to "Side bar" to have bigger buttons in the side bar (below or
  on the side of the board).

### What does the setting "Spend half time on positions worse by" mean?

Imagine you have multiple moves: a +0, a -2, and a -22. Probably, you care more about having a good
evaluation for the +0 and the -2, and you don't care if the -22 is a -20 or -24. For this reason,
Sensei spends more time on the best moves.

This parameter determines by how much: for example, if the parameter is 6 and there are just a +0
and a -6 move, Sensei will spend 67% of the time on the +0 and 33% of the time on the -6 (half of
the time spent on the +0). Instead, if the parameter is 3, it will spend 80% of the time on the +0
and 20% of the time on the -6 (1/4 of the time spent on the +0).

If the math is too complex, you can just follow the following rules:
- If you care only about the best move, set the parameter around 2.
- If you want a good balance, leave the default 6.
- If you want to spend the same time on each move, set the parameter to 64 or higher.

Pentobi Source Code Overview
============================

Libboardgame Modules
--------------------

The Libboardgame modules contain code that is not specific to Blokus and
could be reused for other board games.

* libboardgame_util --
  General utilities not specific to board games
* libboardgame_base --
  Utility classes and functions specific to board games
* libboardgame_gtp --
  Implementation of the
  [Go Text Protocol](https://en.wikipedia.org/wiki/Go_Text_Protocol)
  (GTP)
* libboardgame_sgf --
  Implementation of the
  [Smart Game Format](https://en.wikipedia.org/wiki/Smart_Game_Format)
* libboardgame_mcts --
  Monte-Carlo tree search
* libboardgame_test --
  Functionality for unit tests

Pentobi Engine Modules
----------------------

The engine modules contain code that is specific to Blokus and the
computer player used in Pentobi.

* libpentobi_base --
  General Blokus-specific functionality.
* libpentobi_mcts --
  Blokus player based on Monte-Carlo tree search (using
  libboardgame_mcts)
* pentobi_gtp --
  GTP interface to the player in libpentobi_mcts.
* twogtp --
  Tool for playing games between two GTP engines (currently only
  supported on Linux/GCC)
* learn_tool --
  Tool for learning the weights used for move priors in libpentobi_mcts

Pentobi GUI Modules
-------------------

The GUI modules implement the user interface. They depend on the
[Qt](https://www.qt.io/) libraries.

* pentobi --
  Main program that provides a GUI for the player in libpentobi_mcts
* convert --
  Helper program to convert SVG icons to bitmaps at build time
* libpentobi_paint --
  Common functionality for painting board and pieces both used in the
  QtQuick-based GUI in pentobi and the QtGui-based painting in file
  preview generators (thumbnailers).
* libpentobi_thumbnail --
  Common functionality for the thumbnailers
* pentobi_thumbnailer --
  Thumbnailer for the [Gnome](http://www.gnome.org) desktop
* pentobi_kde_thumbnailer --
  Theumbnailer plugin the [KDE](http://www.kde.org) desktop

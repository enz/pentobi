Pentobi Source Code Overview
============================

Git Repository
--------------

The source code repository is at https://github.com/enz/pentobi.


Libboardgame Modules
--------------------

The Libboardgame modules contain code that is not specific to Blokus and
could be reused for other board games.

* libboardgame_base --
  General utilities and functionality for board games
* libboardgame_gtp --
  Implementation of the
  [Go Text Protocol](https://en.wikipedia.org/wiki/Go_Text_Protocol) (GTP)
* libboardgame_test --
  Functionality for unit tests
* libboardgame_mcts --
  Abstract Monte-Carlo tree search

Pentobi Engine Modules
----------------------

The engine modules contain code that is specific to Blokus and the
computer player used in Pentobi.

* libpentobi_base --
  General Blokus-specific functionality.
* libpentobi_gtp --
  General Blokus-specific GTP interface based on libboardgame_gtp and
  libpentobi_base.
* libpentobi_mcts --
  Main Blokus computer player used in Pentobi based on libboardgame_mcts
* learn_tool --
  Tool for learning the move priors used in libpentobi_mcts
* pentobi_gtp --
  GTP interface to the player in libpentobi_mcts.
  See pentobi_gtp/Pentobi-GTP.html for more information.
* twogtp --
  Tool for playing Blokus games between two GTP engines (currently only
  supported on Unix)

Pentobi GUI Modules
-------------------

The GUI modules implement the user interface. They depend on the
[Qt](https://www.qt.io/) libraries.

* pentobi --
  Main program that provides a GUI for the player in libpentobi_mcts
* pentobi_thumbnailer --
  File preview generator for the [Gnome](http://www.gnome.org) desktop
* pentobi_kde_thumbnailer --
  File preview generator for the [KDE](http://www.kde.org) desktop
* libpentobi_paint --
  Common functionality for pentobi and libpentobi_thumbnail
* libpentobi_thumbnail --
  Common functionality for pentobi_thumbnailer and pentobi_kde_thumbnailer

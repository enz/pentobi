Contributing to Pentobi
=======================

Report Bugs
-----------

Bugs can be reported at the [issue tracker](https://github.com/enz/pentobi/issues)
at GitHub. There are no plans to add more game variants or new features to
Pentobi, so please use the issue tracker only for bugs.

The only officially supported platforms is Linux, releases are done only as
source code. The Windows version is maintained by an external maintainer,
please report Windows-related bugs [here](https://github.com/lwchkg/pentobi/issues).

Translations
------------

Translations can be contributed at
[Transifex](https://app.transifex.com/markus-enzenberger/pentobi/).

Source Code Overview
--------------------

The Libboardgame modules contain code that is not specific to Blokus and
could be reused for other board games.

* __[libboardgame_base](libboardgame_base)__
  General utilities and functionality for board games
* __[libboardgame_gtp](libboardgame_gtp)__
  Implementation of the [Go Text Protocol](https://en.wikipedia.org/wiki/Go_Text_Protocol) (GTP)
* __[libboardgame_test](libboardgame_test)__
  Functionality for unit tests
* __[libboardgame_mcts](libboardgame_mcts)__
  Abstract Monte-Carlo tree search (MCTS)

The engine modules contain code that is specific to Blokus and the
computer player used in Pentobi.

* __[libpentobi_base](libpentobi_base)__
  General Blokus-specific functionality. The board implementation is
  optimized for fast move generation needed in MCTS. For a definition
  of the game file format, see [Pentobi-SGF](libpentobi_base/Pentobi-SGF.md)
* __[libpentobi_gtp](libpentobi_gtp)__
  General Blokus-specific GTP interface based on libboardgame_gtp and
  libpentobi_base.
* __[libpentobi_mcts](libpentobi_mcts)__
  Main Blokus computer player used in Pentobi based on libboardgame_mcts
* __[opening_books](opening_books)__
  Opening moves in SGF format used by libpentobi_mcts for fast move
  generation without search in early positions
* __[learn_tool](learn_tool)__
  Tool for learning the move priors used in libpentobi_mcts
* __[pentobi_gtp](pentobi_gtp)__
  GTP interface to the player in libpentobi_mcts.
  See [Pentobi-GTP](pentobi_gtp/Pentobi-GTP.md) for more information.
* __[twogtp](twogtp)__
  Tool for playing Blokus games between two GTP engines (currently only
  supported on Unix)

The GUI module implements the user interface. It depends on the
[Qt](https://www.qt.io/) libraries.

* __[pentobi](pentobi)__
  Main program that provides a GUI for the player in libpentobi_mcts

Contributing to Pentobi
=======================

Report Bugs
-----------

Bugs can be reported at the [issue tracker](https://github.com/enz/pentobi/issues)
at GitHub. There are no plans to add more game variants or new features to
Pentobi, so please use the issue tracker only for bugs or issues with compiling
or running Pentobi on newer operating system versions.

The Windows version is maintained by an external maintainer, please report
Windows-related bugs [here](https://github.com/lwchkg/pentobi/issues).

Translations
------------

Translations can be contributed at [Transifex](https://www.transifex.com/markus-enzenberger/pentobi/).
Check the announcements at Transifex for information about what branch
the translations currently correspond to. Translation components will
be included in releases if at least 75 percent of the strings are
translated. The translation components are:

* __Pentobi__ ([QT format](https://doc.qt.io/qt-5/linguist-ts-file-format.html))
  User interface of Pentobi main program
* __Pentobi Manual__ ([PO format](https://www.gnu.org/software/gettext/manual/html_node/PO-Files.html))
  User manual
* __Pentobi Unix Files__ (PO format)
  Additional Pentobi files installed on Unix (desktop entry, appstream
  file, manpage, etc.)
* __Pentobi Thumbnailer__ (QT format)
  Messages used in Gnome thumbnailer
* __Pentobi Thumbnailer Unix Files__ (PO format)
  Additional Pentobi Thumbnailer files installed on Unix (manpage)

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

The GUI modules implement the user interface. They depend on the
[Qt](https://www.qt.io/) libraries.

* __[pentobi](pentobi)__
  Main program that provides a GUI for the player in libpentobi_mcts
* __[pentobi_thumbnailer](pentobi_thumbnailer)__
  File preview generator for the [Gnome](http://www.gnome.org) desktop
* __[libpentobi_paint](libpentobi_paint)__
  Common functionality for pentobi and pentobi_thumbnailer

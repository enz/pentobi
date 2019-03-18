Pentobi Source Code Overview
============================

Libboardgame Modules
--------------------

The Libboardgame modules contain code that is not specific to Blokus and
could be reused for other board games.

* __libboardgame_base__
  General utilities and functionality for board games
* __libboardgame_gtp__
  Implementation of the [Go Text Protocol](https://en.wikipedia.org/wiki/Go_Text_Protocol) (GTP)
* __libboardgame_test__
  Functionality for unit tests
* __libboardgame_mcts__
  Abstract Monte-Carlo tree search (MCTS)

Pentobi Engine Modules
----------------------

The engine modules contain code that is specific to Blokus and the
computer player used in Pentobi.

* __libpentobi_base__
  General Blokus-specific functionality. The board implementation is
  optimized for fast move generation needed in MCTS. For a definition
  of the game file format, see [Pentobi-SGF](libpentobi_base/Pentobi-SGF.md)
* __libpentobi_gtp__
  General Blokus-specific GTP interface based on libboardgame_gtp and
  libpentobi_base.
* __libpentobi_mcts__
  Main Blokus computer player used in Pentobi based on libboardgame_mcts
* __opening_books__
  Opening moves in SGF format used by libpentobi_mcts for fast move
  generation without search in early positions
* __learn_tool__
  Tool for learning the move priors used in libpentobi_mcts
* __pentobi_gtp__
  GTP interface to the player in libpentobi_mcts.
  See [Pentobi-GTP](pentobi_gtp/Pentobi-GTP.md) for more information.
* __twogtp__
  Tool for playing Blokus games between two GTP engines (currently only
  supported on Unix)

Pentobi GUI Modules
-------------------

The GUI modules implement the user interface. They depend on the
[Qt](https://www.qt.io/) libraries.

* __pentobi__
  Main program that provides a GUI for the player in libpentobi_mcts
* __pentobi_thumbnailer__
  File preview generator for the [Gnome](http://www.gnome.org) desktop
* __pentobi_kde_thumbnailer__
  File preview generator for the [KDE](http://www.kde.org) desktop
* __libpentobi_paint__
  Common functionality for pentobi and libpentobi_thumbnail
* __libpentobi_thumbnail__
  Common functionality for pentobi_thumbnailer and
  pentobi_kde_thumbnailer
* __libpentobi_kde_thumbnailer__
  Only needed for technical reasons during compilation, see comment in
  libpentobi_kde_thumbnailer/CMakeLists.txt

Translations
============

Translations can be contributed at [Transifex](https://www.transifex.com/markus-enzenberger/pentobi/).
Translation components will be included in Pentobi if at least 75
percent of the strings are translated. The current translation
components are:

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

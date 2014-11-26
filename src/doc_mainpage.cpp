/** @mainpage notitle

    @section mainpage_libboardgame LibBoardGame Modules

    The LibBoardGame modules contain code that is not specific to the board
    game Blokus and could be reused for other projects:

    - libboardgame_gtp -
      Implementation of the Go Text Protocol GTP (@ref libboardgame_doc_gtp)
    - libboardgame_sys -
      Platform-dependent functionality
    - libboardgame_util -
      General utilities not specific to board games
    - libboardgame_sgf -
      Implementation of the Smart Game Format (@ref libboardgame_doc_sgf)
    - libboardgame_base -
      Utility classes and functions specific to board games
    - libboardgame_mcts -
      Monte-Carlo tree search
    - libboardgame_test -
      Functionality for unit tests similar to Boost::Test

    @section mainpage_pentobi Pentobi Modules

    The Pentobi modules are specific to the board game Blokus:

    - libpentobi_base -
      General Blokus-specific functionality
    - libpentobi_mcts -
      Blokus player based on Monte-Carlo tree search
    - pentobi_gtp -
      GTP interface to the player in libpentobi_mcts
    - twogtp -
      Tool for playing games between two GTP engines
      (currently only supported on Linux/GCC)

    @section mainpage_gui Pentobi GUI Modules

    The Pentobi GUI modules implement the user interface.
    They have a dependency on the following
    <a href="http://qt.digia.com/">Qt</a> libraries: QtCore4, QtGui4.

    - convert -
      Small helper program to convert SVG icons to bitmaps at build time
    - libpentobi_gui -
      GUI functionality that could be reused for other projects
    - libpentobi_thumbnail -
      Common functionality for file preview thumbnailers
    - pentobi -
      Main program that provides a GUI for the player in libpentobi_mcts
    - pentobi_thumbnailer -
      Generates file preview thumbnails for the
      <a href="http://www.gnome.org/">Gnome</a> desktop
    - pentobi_kde_thumbnailer -
      Plugin for file preview thumbnails for the
      <a href="http://www.kde.org/">KDE</a> desktop

    @section mainpage_gui Pentobi Mobile GUI Modules

    The Pentobi Mobile GUI modules implement a user interface targeted
    at mobile devices based on Qt Quick / QML.

    - pentobi_qml -
      Main program that provides a GUI for the player in libpentobi_mcts
*/

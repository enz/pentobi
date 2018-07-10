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
    - learn_tool -
      Tool for learning the weights used for move priors in the
      Monte-Carlo tree search in libpentobi_mcts

    @section mainpage_gui Pentobi QWidgets GUI Modules

    The Pentobi QtWidgets GUI modules implement a user interface based on
    <a href="http://qt.digia.com/">Qt</a>/QtWidgets.
    They are currently used for the desktop versions of Pentobi.
    They may become obsolete in the future, once the QML GUI modules
    (@ref mainpage_gui_qml) provide the same functionality.

    - convert -
      Small helper program to convert SVG icons to bitmaps at build time
    - libpentobi_gui -
      GUI functionality that could be reused for other projects
    - pentobi -
      Main program that provides a GUI for the player in libpentobi_mcts

    @section mainpage_gui_qml Pentobi QML GUI Modules

    The Pentobi QML GUI modules implement a user interface based on
    Qt Quick / QML. They currently support only a subset of the features
    of the QWidgets-based GUI (@ref mainpage_gui) but provide fluid
    animations and are usable on touch-screens. They are currently
    used for the Android version of Pentobi.

    - pentobi_qml -
      Main program that provides a GUI for the player in libpentobi_mcts
*/

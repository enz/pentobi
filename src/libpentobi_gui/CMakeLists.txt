include(${QT_USE_FILE})

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(pentobi_gui_STAT_SRCS
  BoardPainter.cpp
  ComputerColorDialog.cpp
  GameInfoDialog.cpp
  GuiBoard.cpp
  GuiBoardUtil.cpp
  HelpWindow.cpp
  InitialRatingDialog.cpp
  LineEdit.cpp
  OrientationDisplay.cpp
  PieceSelector.cpp
  SameHeightLayout.cpp
  ScoreDisplay.cpp
  StatusBarButton.cpp
  Util.cpp
  )

set(pentobi_gui_MOC_HDRS
  ComputerColorDialog.h
  GameInfoDialog.h
  GuiBoard.h
  HelpWindow.h
  InitialRatingDialog.h
  LineEdit.h
  OrientationDisplay.h
  PieceSelector.h
  SameHeightLayout.h
  ScoreDisplay.h
  StatusBarButton.h
  )

set(pentobi_gui_RCS
  libpentobi_gui_resources.qrc
  )

set(pentobi_gui_TS
  libpentobi_gui_de.ts
  libpentobi_gui_en_CA.ts
  libpentobi_gui_en_GB.ts
  )

qt4_add_resources(pentobi_gui_RC_SRCS ${pentobi_gui_RCS})

qt4_add_translation(pentobi_gui_QM_SRCS ${pentobi_gui_TS})

qt4_wrap_cpp(pentobi_gui_MOC_SRCS ${pentobi_gui_MOC_HDRS})

add_library(pentobi_gui STATIC
  ${pentobi_gui_STAT_SRCS}
  ${pentobi_gui_MOC_SRCS}
  ${pentobi_gui_RC_SRCS}
  ${pentobi_gui_QM_SRCS})

# Install translation files. If you change the destination, you need to
# update the directory used in
# src/pentobi/Main.cpp:loadLibPentobiGuiTranslator()
install(FILES ${pentobi_gui_QM_SRCS} DESTINATION share/games/pentobi/translations)

set(CMAKE_AUTOMOC TRUE)

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
  LeaveFullscreenButton.cpp
  LineEdit.cpp
  OrientationDisplay.cpp
  PieceSelector.cpp
  SameHeightLayout.cpp
  ScoreDisplay.cpp
  Util.cpp
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

add_library(pentobi_gui STATIC
  ${pentobi_gui_STAT_SRCS}
  ${pentobi_gui_MOC_SRCS}
  ${pentobi_gui_RC_SRCS}
  ${pentobi_gui_QM_SRCS})

# Install translation files. If you change the destination, you need to
# update the default for PENTOBI_TRANSLATIONS in the main CMakeLists.txt
install(FILES ${pentobi_gui_QM_SRCS}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/translations)

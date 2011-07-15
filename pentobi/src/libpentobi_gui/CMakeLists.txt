include(${QT_USE_FILE})

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(pentobi_gui_STAT_SRCS
  BoardPainter.cpp
  ComputerColorDialog.cpp
  GuiBoard.cpp
  GuiBoardUtil.cpp
  HelpWindow.cpp
  OrientationDisplay.cpp
  PieceSelector.cpp
  SameHeightLayout.cpp
  ScoreDisplay.cpp
  Util.cpp
  )

set(pentobi_gui_MOC_HDRS
  ComputerColorDialog.h
  GuiBoard.h
  HelpWindow.h
  OrientationDisplay.h
  PieceSelector.h
  SameHeightLayout.h
  ScoreDisplay.h
  )

set(pentobi_gui_RCS
  libpentobi_gui_resources.qrc
  )

qt4_add_resources(pentobi_gui_RC_SRCS ${pentobi_gui_RCS})

qt4_wrap_cpp(pentobi_gui_MOC_SRCS ${pentobi_gui_MOC_HDRS})

add_library(pentobi_gui STATIC
  ${pentobi_gui_STAT_SRCS}
  ${pentobi_gui_MOC_SRCS}
  ${pentobi_gui_RC_SRCS})

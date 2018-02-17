set(CMAKE_AUTOMOC TRUE)

set(pentobi_gui_STAT_SRCS
  BoardPainter.h
  BoardPainter.cpp
  ComputerColorDialog.h
  ComputerColorDialog.cpp
  GameInfoDialog.h
  GameInfoDialog.cpp
  GuiBoard.h
  GuiBoard.cpp
  GuiBoardUtil.h
  GuiBoardUtil.cpp
  HelpWindow.h
  HelpWindow.cpp
  InitialRatingDialog.h
  InitialRatingDialog.cpp
  LeaveFullscreenButton.h
  LeaveFullscreenButton.cpp
  LineEdit.h
  LineEdit.cpp
  OrientationDisplay.h
  OrientationDisplay.cpp
  PieceSelector.h
  PieceSelector.cpp
  SameHeightLayout.h
  SameHeightLayout.cpp
  ScoreDisplay.h
  ScoreDisplay.cpp
  Util.h
  Util.cpp
)

set(pentobi_gui_TS
  translations/libpentobi_gui_de.ts
  translations/libpentobi_gui_fr.ts
  translations/libpentobi_gui_fr.ts
  translations/libpentobi_gui_nb_NO.ts
  translations/libpentobi_gui_ru.ts
  )

qt5_add_translation(pentobi_gui_QM_SRCS ${pentobi_gui_TS})

add_library(pentobi_gui STATIC
  ${pentobi_gui_STAT_SRCS}
  ${pentobi_gui_QM_SRCS})

target_compile_definitions(pentobi_gui PRIVATE
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x051000)

target_link_libraries(pentobi_gui Qt5::Widgets)

# Install translation files. If you change the destination, you need to
# update the default for PENTOBI_TRANSLATIONS in the main CMakeLists.txt
install(FILES ${pentobi_gui_QM_SRCS}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/translations)

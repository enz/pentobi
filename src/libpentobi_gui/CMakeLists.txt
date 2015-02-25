set(CMAKE_AUTOMOC TRUE)

if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

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

set(pentobi_gui_ICNS
  go-home.png
  go-next.png
  go-previous.png
)

set(pentobi_gui_TS
  libpentobi_gui_de.ts
  libpentobi_gui_en_CA.ts
  libpentobi_gui_en_GB.ts
  )

# Create PNG icons from SVG icons using the helper program src/convert
foreach(icon ${pentobi_gui_ICNS})
  string(REPLACE ".png" ".svg" svgicon ${icon})
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/icons/${icon}"
    COMMAND convert ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon} ${CMAKE_CURRENT_BINARY_DIR}/icons/${icon}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon}
    )
endforeach()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/icons)
configure_file(
  libpentobi_gui_resources.qrc libpentobi_gui_resources.qrc COPYONLY)

if (USE_QT5)
  qt5_add_resources(pentobi_gui_RC_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/libpentobi_gui_resources.qrc)
else()
  qt4_add_resources(pentobi_gui_RC_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/libpentobi_gui_resources.qrc)
endif()

# We don't use qt4_add_translation because it doesn't use option
# -removeidentical (see QTBUG-44323)
if (USE_QT5)
  set(LRELEASE_EXECUTABLE ${Qt5_LRELEASE_EXECUTABLE})
else()
  set(LRELEASE_EXECUTABLE ${QT_LRELEASE_EXECUTABLE})
endif()
set(pentobi_gui_QM_SRCS)
foreach(ts ${pentobi_gui_TS})
  get_filename_component(qm ${ts} NAME_WE)
  set(qm "${CMAKE_CURRENT_BINARY_DIR}/${qm}.qm")
  add_custom_command(OUTPUT ${qm}
    COMMAND ${LRELEASE_EXECUTABLE}
    ARGS -removeidentical -nounfinished
      ${CMAKE_CURRENT_SOURCE_DIR}/translations/${ts} -qm ${qm}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/translations/${ts} VERBATIM
    )
  set(pentobi_gui_QM_SRCS ${pentobi_gui_QM_SRCS} ${qm})
endforeach()

add_library(pentobi_gui STATIC
  ${pentobi_gui_STAT_SRCS}
  ${pentobi_gui_RC_SRCS}
  ${pentobi_gui_QM_SRCS})

if (USE_QT5)
  target_link_libraries(pentobi_gui Qt5::Widgets)
endif()

# Install translation files. If you change the destination, you need to
# update the default for PENTOBI_TRANSLATIONS in the main CMakeLists.txt
install(FILES ${pentobi_gui_QM_SRCS}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/translations)

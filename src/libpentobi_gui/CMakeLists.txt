set(CMAKE_AUTOMOC TRUE)

if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

set(pentobi_gui_STAT_SRCS
  BoardPainter.cpp
  BoardPainter.h
  ComputerColorDialog.cpp
  ComputerColorDialog.h
  GameInfoDialog.cpp
  GameInfoDialog.h
  GuiBoard.cpp
  GuiBoard.h
  GuiBoardUtil.cpp
  GuiBoardUtil.h
  HelpWindow.cpp
  HelpWindow.h
  InitialRatingDialog.cpp
  InitialRatingDialog.h
  LeaveFullscreenButton.cpp
  LeaveFullscreenButton.h
  LineEdit.cpp
  LineEdit.h
  OrientationDisplay.cpp
  OrientationDisplay.h
  PieceSelector.cpp
  PieceSelector.h
  SameHeightLayout.cpp
  SameHeightLayout.h
  ScoreDisplay.cpp
  ScoreDisplay.h
  Util.cpp
  Util.h
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
# -removeidentical with lrelease, which causes larger sizes of the generated qm
# files
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
  qt5_use_modules(pentobi_gui Widgets)
endif()

# Install translation files. If you change the destination, you need to
# update the default for PENTOBI_TRANSLATIONS in the main CMakeLists.txt
install(FILES ${pentobi_gui_QM_SRCS}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/translations)

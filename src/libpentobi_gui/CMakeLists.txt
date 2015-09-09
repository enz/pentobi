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

set(pentobi_gui_ICNS
  go-home.png
  go-next.png
  go-previous.png
)

set(pentobi_gui_TS
  libpentobi_gui_de.ts
  )

# Create PNG icons from SVG icons using the helper program src/convert
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/icons)
file(COPY libpentobi_gui_resources.qrc DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
foreach(icon ${pentobi_gui_ICNS})
  string(REPLACE ".png" ".svg" svgicon ${icon})
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/icons/${icon}"
    COMMAND convert ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon}
      ${CMAKE_CURRENT_BINARY_DIR}/icons/${icon}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon}
  )
endforeach()
qt5_add_resources(pentobi_gui_RC_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/libpentobi_gui_resources.qrc
    OPTIONS -no-compress)
file(COPY libpentobi_gui_resources_2x.qrc DESTINATION
  ${CMAKE_CURRENT_BINARY_DIR})
foreach(icon ${pentobi_gui_ICNS})
string(REPLACE ".png" ".svg" svgicon ${icon})
string(REPLACE ".png" "@2x.png" hdpiicon ${icon})
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/icons/${hdpiicon}"
  COMMAND convert --hdpi ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon}
    ${CMAKE_CURRENT_BINARY_DIR}/icons/${hdpiicon}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon}
)
endforeach()
qt5_add_resources(pentobi_gui_RC_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/libpentobi_gui_resources_2x.qrc
  OPTIONS -no-compress)

# We don't use qt_add_translation because it doesn't use option
# -removeidentical (see QTBUG-44323)
set(LRELEASE_EXECUTABLE ${Qt5_LRELEASE_EXECUTABLE})
set(pentobi_gui_QM_SRCS)
foreach(ts ${pentobi_gui_TS})
  get_filename_component(qm ${ts} NAME_WE)
  set(qm "${CMAKE_CURRENT_BINARY_DIR}/${qm}.qm")
  add_custom_command(OUTPUT ${qm}
    COMMAND ${LRELEASE_EXECUTABLE} -removeidentical -nounfinished
      ${CMAKE_CURRENT_SOURCE_DIR}/translations/${ts} -qm ${qm}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/translations/${ts} VERBATIM
    )
  set(pentobi_gui_QM_SRCS ${pentobi_gui_QM_SRCS} ${qm})
endforeach()

add_library(pentobi_gui STATIC
  ${pentobi_gui_STAT_SRCS}
  ${pentobi_gui_RC_SRCS}
  ${pentobi_gui_QM_SRCS})

target_link_libraries(pentobi_gui Qt5::Widgets)

# Install translation files. If you change the destination, you need to
# update the default for PENTOBI_TRANSLATIONS in the main CMakeLists.txt
install(FILES ${pentobi_gui_QM_SRCS}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/translations)

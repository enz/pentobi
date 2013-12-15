set(CMAKE_AUTOMOC TRUE)

if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

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

if (USE_QT5)
  qt5_add_resources(pentobi_gui_RC_SRCS ${pentobi_gui_RCS})
  qt5_add_translation(pentobi_gui_QM_SRCS ${pentobi_gui_TS})
else()
  qt4_add_resources(pentobi_gui_RC_SRCS ${pentobi_gui_RCS})
  # We don't use qt4_add_translation because it doesn't use option -removeidentical
  # with lrelease, which causes larger sizes of the generated qm files
  foreach(ts ${pentobi_gui_TS})
    get_filename_component(qm ${ts} NAME_WE)
    set(qm "${CMAKE_CURRENT_BINARY_DIR}/${qm}.qm")
    add_custom_command(OUTPUT ${qm}
      COMMAND ${QT_LRELEASE_EXECUTABLE}
      ARGS -removeidentical -nounfinished ${CMAKE_CURRENT_SOURCE_DIR}/${ts} -qm ${qm}
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ts} VERBATIM
      )
    set(pentobi_gui_QM_SRCS ${pentobi_gui_QM_SRCS} ${qm})
  endforeach()
endif()

add_library(pentobi_gui STATIC
  ${pentobi_gui_STAT_SRCS}
  ${pentobi_gui_MOC_SRCS}
  ${pentobi_gui_RC_SRCS}
  ${pentobi_gui_QM_SRCS})

if (USE_QT5)
  qt5_use_modules(pentobi_gui Widgets)
endif()

# Install translation files. If you change the destination, you need to
# update the default for PENTOBI_TRANSLATIONS in the main CMakeLists.txt
install(FILES ${pentobi_gui_QM_SRCS}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/translations)

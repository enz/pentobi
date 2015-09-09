set(CMAKE_AUTOMOC TRUE)

set(pentobi_SRCS
  AnalyzeGameWidget.h
  AnalyzeGameWidget.cpp
  AnalyzeGameWindow.h
  AnalyzeGameWindow.cpp
  AnalyzeSpeedDialog.h
  AnalyzeSpeedDialog.cpp
  Application.h
  Application.cpp
  Main.cpp
  MainWindow.h
  MainWindow.cpp
  RatedGamesList.h
  RatedGamesList.cpp
  RatingDialog.h
  RatingDialog.cpp
  RatingGraph.h
  RatingGraph.cpp
  RatingHistory.h
  RatingHistory.cpp
  ShowMessage.h
  ShowMessage.cpp
  Util.h
  Util.cpp
  pentobi.rc
)

set(pentobi_ICNS
  pentobi.png
  pentobi-16.png
  pentobi-32.png
  pentobi-backward.png
  pentobi-backward-16.png
  pentobi-beginning.png
  pentobi-beginning-16.png
  pentobi-computer-colors.png
  pentobi-computer-colors-16.png
  pentobi-end.png
  pentobi-end-16.png
  pentobi-flip-horizontal.png
  pentobi-flip-vertical.png
  pentobi-forward.png
  pentobi-forward-16.png
  pentobi-newgame.png
  pentobi-newgame-16.png
  pentobi-next-piece.png
  pentobi-next-variation.png
  pentobi-next-variation-16.png
  pentobi-piece-clear.png
  pentobi-play.png
  pentobi-play-16.png
  pentobi-previous-piece.png
  pentobi-previous-variation.png
  pentobi-previous-variation-16.png
  pentobi-rated-game.png
  pentobi-rated-game-16.png
  pentobi-rotate-left.png
  pentobi-rotate-right.png
  )

set(pentobi_TS
  pentobi.ts
  pentobi_de.ts
  )

# Create PNG icons from SVG icons using the helper program src/convert
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/icons)
file(COPY resources.qrc DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
foreach(icon ${pentobi_ICNS})
  string(REPLACE ".png" ".svg" svgicon ${icon})
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/icons/${icon}"
    COMMAND convert  ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon}
      ${CMAKE_CURRENT_BINARY_DIR}/icons/${icon}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon}
  )
endforeach()
qt5_add_resources(pentobi_RC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/resources.qrc
    OPTIONS -no-compress)
file(COPY resources_2x.qrc DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
foreach(icon ${pentobi_ICNS})
string(REPLACE ".png" ".svg" svgicon ${icon})
string(REPLACE ".png" "@2x.png" hdpiicon ${icon})
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/icons/${hdpiicon}"
  COMMAND convert --hdpi ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon}
    ${CMAKE_CURRENT_BINARY_DIR}/icons/${hdpiicon}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon}
)
endforeach()
qt5_add_resources(pentobi_RC_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/resources_2x.qrc OPTIONS -no-compress)

# We don't use qt_add_translation because it doesn't use option
# -removeidentical (see QTBUG-44323)
set(LRELEASE_EXECUTABLE ${Qt5_LRELEASE_EXECUTABLE})
set(pentobi_QM_SRCS)
foreach(ts ${pentobi_TS})
  get_filename_component(qm ${ts} NAME_WE)
  set(qm "${CMAKE_CURRENT_BINARY_DIR}/${qm}.qm")
  add_custom_command(OUTPUT ${qm}
    COMMAND ${LRELEASE_EXECUTABLE} -removeidentical -nounfinished
      ${CMAKE_CURRENT_SOURCE_DIR}/translations/${ts} -qm ${qm}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/translations/${ts} VERBATIM
    )
  set(pentobi_QM_SRCS ${pentobi_QM_SRCS} ${qm})
endforeach()

add_executable(pentobi WIN32
  ${pentobi_SRCS}
  ${pentobi_QM_SRCS}
  ${pentobi_RC_SRCS}
  )


if(MINGW AND (CMAKE_SIZEOF_VOID_P EQUAL "4"))
  set_target_properties(pentobi PROPERTIES LINK_FLAGS -Wl,--large-address-aware)
endif()

target_link_libraries(pentobi
  pentobi_gui
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  )

target_link_libraries(pentobi Qt5::Widgets Qt5::Concurrent)

if (DEFINED CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(pentobi ${CMAKE_THREAD_LIBS_INIT})
endif()

install(TARGETS pentobi DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install translation files. If you change the destination, you need to
# update the default for PENTOBI_TRANSLATIONS in the main CMakeLists.txt
install(FILES ${pentobi_QM_SRCS}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/translations)

install(DIRECTORY help DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  FILES_MATCHING PATTERN "*.css" PATTERN "*.html" PATTERN "*.png" PATTERN "*.jpg")

if(MSVC)
  configure_file(pentobi.conf.in Debug/pentobi.conf @ONLY)
  configure_file(pentobi.conf.in Release/pentobi.conf @ONLY)
else()
  configure_file(pentobi.conf.in pentobi.conf @ONLY)
endif()

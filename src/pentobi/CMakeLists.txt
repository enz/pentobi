set(CMAKE_AUTOMOC TRUE)

set(pentobi_SRCS
  AnalyzeGameWidget.h
  AnalyzeGameWidget.cpp
  AnalyzeGameWindow.h
  AnalyzeGameWindow.cpp
  AnalyzeSpeedDialog.h
  AnalyzeSpeedDialog.cpp
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
  pentobi-undo.png
  pentobi-undo-16.png
  )

set(pentobi_TS
  translations/pentobi.ts
  translations/pentobi_de.ts
  translations/pentobi_fr.ts
  translations/pentobi_nb_NO.ts
  translations/qtbase_minimal_nb_NO.ts
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

qt5_add_translation(pentobi_QM_SRCS ${pentobi_TS})

add_executable(pentobi WIN32
  ${pentobi_SRCS}
  ${pentobi_QM_SRCS}
  ${pentobi_RC_SRCS}
  )


if(MINGW AND (CMAKE_SIZEOF_VOID_P EQUAL "4"))
  set_target_properties(pentobi PROPERTIES LINK_FLAGS -Wl,--large-address-aware)
endif()

target_compile_definitions(pentobi PRIVATE
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x051100)

target_link_libraries(pentobi
  pentobi_gui
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  Qt5::Widgets
  Qt5::Concurrent
  Threads::Threads
  )

install(TARGETS pentobi DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install translation files. If you change the destination, you need to
# update the default for PENTOBI_TRANSLATIONS in the main CMakeLists.txt
install(FILES ${pentobi_QM_SRCS}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/translations)

install(DIRECTORY ../../doc/help DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  FILES_MATCHING PATTERN "*.css" PATTERN "*.html" PATTERN "*.png" PATTERN "*.jpg")

install(DIRECTORY ../books DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi
  FILES_MATCHING PATTERN "*.blksgf")

if(MSVC)
  configure_file(pentobi.conf.in Debug/pentobi.conf @ONLY)
  configure_file(pentobi.conf.in Release/pentobi.conf @ONLY)
else()
  configure_file(pentobi.conf.in pentobi.conf @ONLY)
endif()

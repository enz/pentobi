set(CMAKE_AUTOMOC TRUE)

if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

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
)

set(pentobi_ICNS
  pentobi.png
  pentobi-16.png
  pentobi-32.png
  pentobi-backward.png
  pentobi-backward-16.png
  pentobi-backward10.png
  pentobi-backward10-16.png
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
  pentobi-forward10.png
  pentobi-forward10-16.png
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
  pentobi_en_CA.ts
  pentobi_en_GB.ts
  )

# Create PNG icons from SVG icons using the helper program src/convert
foreach(icon ${pentobi_ICNS})
  string(REPLACE ".png" ".svg" svgicon ${icon})
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/icons/${icon}"
    COMMAND convert ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon} ${CMAKE_CURRENT_BINARY_DIR}/icons/${icon}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/icons/${svgicon}
    )
endforeach()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/icons)
configure_file(resources.qrc resources.qrc COPYONLY)

if (USE_QT5)
  qt5_add_resources(pentobi_RC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/resources.qrc)
else()
  qt4_add_resources(pentobi_RC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/resources.qrc)
endif()

# We don't use qt4_add_translation because it doesn't use option
# -removeidentical with lrelease, which causes larger sizes of the generated qm
# files
if (USE_QT5)
  set(LRELEASE_EXECUTABLE ${Qt5_LRELEASE_EXECUTABLE})
else()
  set(LRELEASE_EXECUTABLE ${QT_LRELEASE_EXECUTABLE})
endif()
set(pentobi_QM_SRCS)
foreach(ts ${pentobi_TS})
  get_filename_component(qm ${ts} NAME_WE)
  set(qm "${CMAKE_CURRENT_BINARY_DIR}/${qm}.qm")
  add_custom_command(OUTPUT ${qm}
    COMMAND ${LRELEASE_EXECUTABLE}
    ARGS -removeidentical -nounfinished
      ${CMAKE_CURRENT_SOURCE_DIR}/translations/${ts} -qm ${qm}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/translations/${ts} VERBATIM
    )
  set(pentobi_QM_SRCS ${pentobi_QM_SRCS} ${qm})
endforeach()

if (WIN32)
  # Note: CMake's support of adding rc files directly to the sources is
  # broken because it invokes windres with the current CXXFLAGS but some
  # of them might be compiler-specific and cause windres to fail (e.g.
  # -std=c++11 of MinGW/Cygwin GCC; last tested with CMake 2.8.12.1).
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pentobi_res.o
    COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
    -i${CMAKE_CURRENT_SOURCE_DIR}/pentobi.rc
    -o ${CMAKE_CURRENT_BINARY_DIR}/pentobi_res.o)
  set(pentobi_SRCS ${pentobi_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/pentobi_res.o)
endif()

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

# Remove once we require CMake 2.8.11
if (WIN32)
  if (USE_QT5)
    target_link_libraries(pentobi Qt5::WinMain)
  else()
    target_link_libraries(pentobi ${QT_QTMAIN_LIBRARY})
  endif()
endif()

if (USE_QT5)
  qt5_use_modules(pentobi Widgets Concurrent)
else()
  target_link_libraries(pentobi
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIB_DEPENDENCIES}
    ${QT_QTCORE_LIB_DEPENDENCIES}
    )
endif()

if (DEFINED CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(pentobi ${CMAKE_THREAD_LIBS_INIT})
endif()

install(TARGETS pentobi
DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install translation files. If you change the destination, you need to
# update the default for PENTOBI_TRANSLATIONS in the main CMakeLists.txt
install(FILES ${pentobi_QM_SRCS}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/translations)

install(DIRECTORY help DESTINATION ${CMAKE_INSTALL_DATADIR}
  FILES_MATCHING PATTERN "*.css" PATTERN "*.html" PATTERN "*.png" PATTERN "*.jpg")

if(MSVC)
  configure_file(pentobi.conf.in Debug/pentobi.conf @ONLY)
  configure_file(pentobi.conf.in Release/pentobi.conf @ONLY)
else()
  configure_file(pentobi.conf.in pentobi.conf @ONLY)
endif()

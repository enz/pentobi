
include(${QT_USE_FILE})

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(pentobi_SRCS
  AnalyzeGameWidget.cpp
  AnalyzeGameWindow.cpp
  AnalyzeSpeedDialog.cpp
  Application.cpp
  ExportImage.cpp
  MainWindow.cpp
  Main.cpp
  RatedGamesList.cpp
  RatingDialog.cpp
  RatingHistory.cpp
  RatingGraph.cpp
  ShowMessage.cpp
  Util.cpp
  )

set(pentobi_MOC_HDRS
  AnalyzeGameWidget.h
  AnalyzeGameWindow.h
  AnalyzeSpeedDialog.h
  Application.h
  MainWindow.h
  RatedGamesList.h
  RatingDialog.h
  RatingGraph.h
  )
  
set(pentobi_RCS
  resources.qrc
  )

set(pentobi_TS
  pentobi.ts
  pentobi_de.ts
  pentobi_en_CA.ts
  pentobi_en_GB.ts
  )

link_directories(${Boost_LIBRARY_DIRS})

qt4_add_resources(pentobi_RC_SRCS ${pentobi_RCS})

qt4_add_translation(pentobi_QM_SRCS ${pentobi_TS})

qt4_wrap_cpp(pentobi_MOC_SRCS ${pentobi_MOC_HDRS})

if (MINGW)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icon.o
    COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
    -i${CMAKE_CURRENT_SOURCE_DIR}/icon.rc
    -o ${CMAKE_CURRENT_BINARY_DIR}/icon.o)
  set(pentobi_SRCS ${pentobi_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/icon.o)
else()
  set(pentobi_SRCS ${pentobi_SRCS} icon.rc)
endif()

add_executable(pentobi WIN32
  ${pentobi_SRCS}
  ${pentobi_MOC_SRCS}
  ${pentobi_QM_SRCS}
  ${pentobi_RC_SRCS}
  )

target_link_libraries(pentobi
  pentobi_gui
  ${QT_LIBRARIES}
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_CHRONO_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  )

if (WIN32)
target_link_libraries(pentobi
  ${QT_QTMAIN_LIBRARY}
  )
endif()

install(TARGETS pentobi DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install translation files. If you change the destination, you need to
# update the default for PENTOBI_TRANSLATIONS in the main CMakeLists.txt
install(FILES ${pentobi_QM_SRCS}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/translations)

install(DIRECTORY manual DESTINATION ${CMAKE_INSTALL_DOCDIR}
  FILES_MATCHING PATTERN "*.css" PATTERN "*.html" PATTERN "*.png")

if(MSVC)
  configure_file(pentobi.conf.in Debug/pentobi.conf @ONLY)
  configure_file(pentobi.conf.in Release/pentobi.conf @ONLY)
else()
  configure_file(pentobi.conf.in pentobi.conf @ONLY)
endif()
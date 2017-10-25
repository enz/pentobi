set(CMAKE_AUTOMOC TRUE)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(pentobi_qml_SRCS
  AnalyzeGameModel.h
  AnalyzeGameModel.cpp
  GameModel.h
  GameModel.cpp
  HelpFileExtractor.h
  HelpFileExtractor.cpp
  Main.cpp
  PieceModel.h
  PieceModel.cpp
  PlayerModel.h
  PlayerModel.cpp
  RatingModel.h
  RatingModel.cpp
)

set(pentobi_qml_QRC
    resources.qrc
    qml/themes/theme_dark.qrc
    qml/themes/theme_light.qrc
    qml/themes/theme_shared.qrc
    ../books/pentobi_books.qrc
    )
if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(pentobi_qml_QRC ${pentobi_qml_QRC}
        ../pentobi/help.qrc
        )
endif()

qt5_add_resources(pentobi_qml_RC_SRCS ${pentobi_qml_QRC})

add_executable(pentobi_qml WIN32
  ${pentobi_qml_SRCS}
  ${pentobi_qml_RC_SRCS}
)

target_compile_definitions(pentobi_qml PRIVATE
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050900)

target_link_libraries(pentobi_qml
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  Qt5::Concurrent
  Qt5::Gui
  Qt5::Qml
  Qt5::Svg
  Qt5::WebView
  Threads::Threads
)

if(MSVC)
  configure_file(pentobi.conf.in Debug/pentobi.conf @ONLY)
  configure_file(pentobi.conf.in Release/pentobi.conf @ONLY)
else()
  configure_file(pentobi.conf.in pentobi.conf @ONLY)
endif()

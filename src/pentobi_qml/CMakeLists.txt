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

qt5_add_resources(pentobi_qml_RC_SRCS
  resources.qrc
  qml/themes/theme_dark.qrc
  qml/themes/theme_light.qrc
  qml/themes/theme_shared.qrc
  ../books/pentobi_books.qrc
  ../pentobi/help.qrc
)

add_executable(pentobi_qml WIN32
  ${pentobi_qml_SRCS}
  ${pentobi_qml_RC_SRCS}
)

target_compile_definitions(pentobi_qml PRIVATE
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050800)

target_link_libraries(pentobi_qml
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
)

qt5_use_modules(pentobi_qml Concurrent Gui Qml Svg WebView)

if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(pentobi_qml ${CMAKE_THREAD_LIBS_INIT})
endif()

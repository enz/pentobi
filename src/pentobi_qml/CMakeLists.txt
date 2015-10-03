set(CMAKE_AUTOMOC TRUE)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(pentobi_qml_SRCS
  GameModel.h
  GameModel.cpp
  Main.cpp
  PieceModel.h
  PieceModel.cpp
  PlayerModel.h
  PlayerModel.cpp
)

qt5_add_resources(pentobi_qml_RC_SRCS
  resources.qrc
  qml/themes/light/theme_light.qrc
  ../books/pentobi_books.qrc
  ../pentobi/icons_base.qrc
)

add_executable(pentobi_qml WIN32
  ${pentobi_qml_SRCS}
  ${pentobi_qml_RC_SRCS}
)

target_link_libraries(pentobi_qml
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
)

qt5_use_modules(pentobi_qml Concurrent Gui Qml Svg)

if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(pentobi_qml ${CMAKE_THREAD_LIBS_INIT})
endif()

set(CMAKE_AUTOMOC TRUE)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(pentobi_qml_SRCS
  BoardModel.h
  BoardModel.cpp
  Main.cpp
  PieceModel.h
  PieceModel.cpp
  PlayerModel.h
  PlayerModel.cpp
)

qt5_add_resources(pentobi_qml_RC_SRCS
  resources.qrc
  ../books/pentobi_books.qrc
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

# Remove once we require CMake 2.8.11
if (WIN32)
  target_link_libraries(pentobi_qml Qt5::WinMain)
endif()

qt5_use_modules(pentobi_qml Concurrent Gui Qml Svg)

if (DEFINED CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(pentobi_qml ${CMAKE_THREAD_LIBS_INIT})
endif()

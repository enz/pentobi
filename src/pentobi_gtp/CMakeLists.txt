include_directories(${CMAKE_SOURCE_DIR}/src)

set(pentobi_gtp_SRCS
  Engine.cpp
  Main.cpp
  )

add_executable(pentobi-gtp ${pentobi_gtp_SRCS})

target_link_libraries(pentobi-gtp
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  boardgame_gtp
  ${CMAKE_THREAD_LIBS_INIT}
  )

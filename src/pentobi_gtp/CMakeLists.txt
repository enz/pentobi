include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

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
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  )

configure_file(libpentobi_mcts.conf.in libpentobi_mcts.conf @ONLY)

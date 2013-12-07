include_directories(${CMAKE_SOURCE_DIR}/src)

set(unittest_libpentobi_mcts_STAT_SRCS
  SearchTest.cpp
  )

add_executable(unittest_libpentobi_mcts
  ${unittest_libpentobi_mcts_STAT_SRCS})

target_link_libraries(unittest_libpentobi_mcts
  boardgame_test_main
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_test
  boardgame_util
  boardgame_sys
  ${CMAKE_THREAD_LIBS_INIT}
  )

add_test(libpentobi_mcts unittest_libpentobi_mcts)
